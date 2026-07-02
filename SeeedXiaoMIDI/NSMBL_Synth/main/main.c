#include "config.h"
#include "ble_midi.h"
#include "sam2695.h"
#include "buttons.h"

#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

static const char *TAG = "nsmbl";

// --- Voice state ---
static uint8_t  current_program = DEFAULT_PATCH;
static uint8_t  current_bank    = DEFAULT_BANK;
static uint8_t  current_volume  = 100;
static uint32_t last_note_ms    = 0;   // when the last incoming note arrived

// A Program Change is silent on its own, so preview the new voice with a short
// chord when a patch button is pressed while nothing is being played.
#define AUDITION_IDLE_MS 1200

static uint32_t now_ms(void)
{
    return (uint32_t)(esp_timer_get_time() / 1000);
}

// --- GM Patch names (first 10 of each family for display) ---
static const char *GM_FAMILIES[] = {
    "Piano", "Chrom Perc", "Organ", "Guitar",
    "Bass", "Strings", "Ensemble", "Brass",
    "Reed", "Pipe", "Synth Lead", "Synth Pad",
    "Synth FX", "Ethnic", "Percussive", "SFX"
};

// --- MIDI event processing ---
// The parser only delivers events on VOICE_CHANNEL (melody, MIDI ch1) and
// DRUM_CHANNEL (GM drums, MIDI ch10), so each message just plays on its own
// channel. Program/bank/volume state is tracked for the melodic voice only.
static void process_midi_event(const midi_event_t *event)
{
    uint8_t ch    = event->channel - 1;              // VOICE_CHANNEL or DRUM_CHANNEL
    bool    drums = (ch == DRUM_CHANNEL);

    switch (event->type) {
    case MIDI_NOTE_ON:
        last_note_ms = now_ms();
        sam2695_note_on(ch, event->data1, event->data2);
        break;

    case MIDI_NOTE_OFF:
        sam2695_note_off(ch, event->data1);
        break;

    case MIDI_CC:
        if (!drums && event->data1 == 0) {
            // Bank Select MSB — SAM2695 has only bank 0 (GM) and 127 (MT-32).
            // Clamp and track it so the next Program Change uses the same bank.
            current_bank = (event->data2 == 0) ? 0 : 127;
            sam2695_control_change(VOICE_CHANNEL, 0, current_bank);
        } else {
            sam2695_control_change(ch, event->data1, event->data2);
            if (!drums && event->data1 == 7) {
                current_volume = event->data2;
            }
        }
        break;

    case MIDI_PROGRAM_CHANGE:
        if (drums) {
            // On channel 10 a Program Change selects the drum kit (bank 0).
            sam2695_program_change(DRUM_CHANNEL, 0, event->data1);
        } else {
            sam2695_program_change(VOICE_CHANNEL, current_bank, event->data1);
            current_program = event->data1;
            ESP_LOGI(TAG, "Patch: [%d] %s", event->data1,
                     GM_FAMILIES[event->data1 / 8]);
        }
        break;

    case MIDI_PITCH_BEND: {
        uint16_t bend = (uint16_t)(event->pitch_bend + 8192);
        sam2695_pitch_bend(ch, bend);
        break;
    }

    case MIDI_AFTERTOUCH:
        sam2695_aftertouch(ch, event->data1);
        break;
    }
}

// --- Voice audition ---
// Play a short C-major chord on the voice channel so a patch-button press is
// audible. Skipped if a note arrived recently, so it won't interrupt playing.
static void audition_voice(void)
{
    if (last_note_ms != 0 && (now_ms() - last_note_ms) < AUDITION_IDLE_MS) {
        return;
    }
    // The SAM2695 needs time to load the new patch's wavetable after a Program
    // Change; a note played too soon (<~150ms) still uses the OLD patch. 250ms
    // is a safe margin so the preview always reflects the selected voice.
    vTaskDelay(pdMS_TO_TICKS(250));
    sam2695_note_on(VOICE_CHANNEL, 60, 110);   // single C4 preview note
    vTaskDelay(pdMS_TO_TICKS(500));
    sam2695_note_off(VOICE_CHANNEL, 60);
}

// --- Button callbacks ---
// Patch navigation: button 0 steps the voice down, button 1 steps it up, one
// patch at a time through all 128 voices. Each press auditions the new voice.
// Buttons 2/3 are unused for now.
static void on_short_press(uint8_t idx)
{
    switch (idx) {
    case 0: {  // Family down — jump to the previous instrument family
        uint8_t fam = (current_program / 8 + 15) & 0x0F;   // -1 family, wraps 0..15
        current_program = fam * 8;
        sam2695_program_change(VOICE_CHANNEL, current_bank, current_program);
        ESP_LOGI(TAG, "Family down -> [%d] %s", current_program,
                 GM_FAMILIES[current_program / 8]);
        audition_voice();
        break;
    }
    case 1: {  // Family up — jump to the next instrument family
        uint8_t fam = (current_program / 8 + 1) & 0x0F;    // +1 family, wraps 0..15
        current_program = fam * 8;
        sam2695_program_change(VOICE_CHANNEL, current_bank, current_program);
        ESP_LOGI(TAG, "Family up   -> [%d] %s", current_program,
                 GM_FAMILIES[current_program / 8]);
        audition_voice();
        break;
    }
    case 2:
    case 3:
        // unused for now
        break;
    }
}

static void on_long_press(uint8_t idx)
{
    switch (idx) {
    case 0:  // (reserved)
        break;
    case 1:  // Volume up
        current_volume = (current_volume + 10 > 127) ? 127 : current_volume + 10;
        sam2695_set_volume(VOICE_CHANNEL, current_volume);
        ESP_LOGI(TAG, "Volume: %d", current_volume);
        break;
    case 2:  // Volume down
        current_volume = (current_volume < 10) ? 0 : current_volume - 10;
        sam2695_set_volume(VOICE_CHANNEL, current_volume);
        ESP_LOGI(TAG, "Volume: %d", current_volume);
        break;
    case 3:  // MIDI panic
        for (uint8_t ch = 0; ch < 16; ch++) {
            sam2695_all_notes_off(ch);
        }
        ESP_LOGI(TAG, "PANIC: All notes off");
        break;
    }
}

// --- LED blink task ---
static void led_task(void *arg)
{
    gpio_config_t led_cfg = {
        .pin_bit_mask = (1ULL << LED_PIN),
        .mode         = GPIO_MODE_OUTPUT,
    };
    gpio_config(&led_cfg);
    gpio_set_level(LED_PIN, 1);  // off (active LOW)

    extern bool ble_midi_is_connected(void);

    while (1) {
        if (ble_midi_is_connected()) {
            gpio_set_level(LED_PIN, 0);  // solid on
        } else {
            // slow blink
            int64_t ms = esp_timer_get_time() / 1000;
            gpio_set_level(LED_PIN, (ms % 1000 < 100) ? 0 : 1);
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

// --- Main event loop ---
static void midi_task(void *arg)
{
    QueueHandle_t midi_queue = (QueueHandle_t)arg;
    midi_event_t event;

    while (1) {
        if (xQueueReceive(midi_queue, &event, pdMS_TO_TICKS(10))) {
            process_midi_event(&event);
        }
        buttons_poll();
        vTaskDelay(1);
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "=================================");
    ESP_LOGI(TAG, "  %s", DEVICE_NAME);
    ESP_LOGI(TAG, "  XIAO BLE MIDI + SAM2695");
    ESP_LOGI(TAG, "=================================");

    // NVS (required for BLE)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // SAM2695 init (UART + boot delay)
    sam2695_init();

    // Init all 16 channels to default volume
    for (uint8_t ch = 0; ch < 16; ch++) {
        sam2695_set_volume(ch, 100);
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    // Set default melodic voice on the voice channel
    sam2695_program_change(VOICE_CHANNEL, current_bank, current_program);
    ESP_LOGI(TAG, "Boot voice -> patch %d [%s] bank %d",
             current_program, GM_FAMILIES[current_program / 8], current_bank);
    vTaskDelay(pdMS_TO_TICKS(50));

    // Default reverb/chorus on the voice channel
    sam2695_control_change(VOICE_CHANNEL, 0x50, 1);   // Reverb type: Room 2
    sam2695_control_change(VOICE_CHANNEL, 0x5B, 30);  // Reverb send: 30
    sam2695_control_change(VOICE_CHANNEL, 0x51, 0);   // Chorus type: Chorus 1
    sam2695_control_change(VOICE_CHANNEL, 0x5D, 0);   // Chorus send: 0

    // Boot sound — the signature beat plays on the drum channel
    sam2695_boot_sound(DRUM_CHANNEL);

    // Buttons
    buttons_init(on_short_press, on_long_press);

    // MIDI queue + BLE
    QueueHandle_t midi_queue = xQueueCreate(MIDI_QUEUE_SIZE, sizeof(midi_event_t));
    ble_midi_init(midi_queue);

    // Start tasks
    xTaskCreate(midi_task, "midi", 4096, midi_queue, 5, NULL);
    xTaskCreate(led_task, "led", 2048, NULL, 2, NULL);

    ESP_LOGI(TAG, "=== %s ready ===", DEVICE_NAME);
}
