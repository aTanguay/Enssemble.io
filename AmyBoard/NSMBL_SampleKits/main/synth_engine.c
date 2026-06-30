#include "synth_engine.h"
#include "ble_midi.h"
#include "sample_player.h"
#include "config.h"

#include "esp_log.h"

static const char *TAG = "synth";

static void synth_task(void *param)
{
    QueueHandle_t midi_queue = (QueueHandle_t)param;
    midi_event_t event;

    ESP_LOGI(TAG, "Synth engine running on core %d", xPortGetCoreID());

    while (1) {
        if (xQueueReceive(midi_queue, &event, portMAX_DELAY) == pdTRUE) {
            switch (event.type) {
            case MIDI_NOTE_ON:
                ESP_LOGI(TAG, "Note ON: %d vel=%d", event.data1, event.data2);
                sample_player_note_on(event.data1, event.data2);
                break;
            case MIDI_NOTE_OFF:
                ESP_LOGI(TAG, "Note OFF: %d", event.data1);
                sample_player_note_off(event.data1);
                break;
            case MIDI_CC:
                ESP_LOGI(TAG, "CC %d = %d", event.data1, event.data2);
                break;
            case MIDI_PROGRAM_CHANGE:
                ESP_LOGI(TAG, "Program Change: %d", event.data1);
                break;
            case MIDI_PITCH_BEND:
                ESP_LOGI(TAG, "Pitch Bend: %d", event.pitch_bend);
                break;
            }
        }
    }
}

void synth_engine_init(QueueHandle_t midi_queue)
{
    xTaskCreatePinnedToCore(synth_task, "synth", 4096, midi_queue, 5, NULL, SYNTH_CORE);
    ESP_LOGI(TAG, "Synth engine initialized (pinned to core %d)", SYNTH_CORE);
}
