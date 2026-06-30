#include "synth_engine.h"
#include "config.h"
#include "ble_midi.h"
#include "amy.h"
#include "amy_midi.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "synth";

static void midi_to_amy(const midi_event_t *event)
{
    uint8_t bytes[3];
    uint16_t len = 0;
    uint8_t status_base = 0;

    switch (event->type) {
    case MIDI_NOTE_ON:
        status_base = 0x90;
        bytes[0] = status_base | (event->channel - 1);
        bytes[1] = event->data1;
        bytes[2] = event->data2;
        len = 3;
        break;
    case MIDI_NOTE_OFF:
        status_base = 0x80;
        bytes[0] = status_base | (event->channel - 1);
        bytes[1] = event->data1;
        bytes[2] = 0;
        len = 3;
        break;
    case MIDI_CC:
        status_base = 0xB0;
        bytes[0] = status_base | (event->channel - 1);
        bytes[1] = event->data1;
        bytes[2] = event->data2;
        len = 3;
        break;
    case MIDI_PROGRAM_CHANGE:
        status_base = 0xC0;
        bytes[0] = status_base | (event->channel - 1);
        bytes[1] = event->data1;
        len = 2;
        break;
    case MIDI_PITCH_BEND: {
        status_base = 0xE0;
        uint16_t bend_val = (uint16_t)(event->pitch_bend + 8192);
        bytes[0] = status_base | (event->channel - 1);
        bytes[1] = bend_val & 0x7F;
        bytes[2] = (bend_val >> 7) & 0x7F;
        len = 3;
        break;
    }
    }

    if (len > 0) {
        convert_midi_bytes_to_messages(bytes, len, 0);
    }
}

static void synth_task(void *param)
{
    QueueHandle_t midi_queue = (QueueHandle_t)param;
    midi_event_t event;

    ESP_LOGI(TAG, "synth task started on core %d", xPortGetCoreID());

    while (1) {
        if (xQueueReceive(midi_queue, &event, portMAX_DELAY) == pdTRUE) {
            midi_to_amy(&event);
        }
    }
}

void synth_engine_init(QueueHandle_t midi_queue)
{
    amy_config_t config = amy_default_config();
    config.audio = AMY_AUDIO_IS_I2S;
    config.midi = AMY_MIDI_IS_NONE;
    config.i2s_lrc = I2S_WS;
    config.i2s_bclk = I2S_BCLK;
    config.i2s_dout = I2S_DOUT;
    config.i2s_din = -1;
    config.i2s_mclk = I2S_MCLK;
    config.features.default_synths = 1;
    config.features.startup_bleep = 1;
    config.ram_caps_events = MALLOC_CAP_SPIRAM;
    config.ram_caps_synth = MALLOC_CAP_SPIRAM;
    config.ram_caps_delay = MALLOC_CAP_SPIRAM;
    config.ram_caps_sample = MALLOC_CAP_SPIRAM;
    config.ram_caps_sysex = MALLOC_CAP_SPIRAM;
    amy_start(config);

    ESP_LOGI(TAG, "AMY synth engine initialized (%d oscs)", AMY_OSCS);

    xTaskCreatePinnedToCore(synth_task, "synth", 4096, midi_queue, 5, NULL, SYNTH_CORE);
}
