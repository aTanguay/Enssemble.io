#include "synth_engine.h"
#include "config.h"
#include "ble_midi.h"
#include "amy.h"
#include "amy_midi.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <math.h>

static const char *TAG = "synth";

// --- MIDI CC Handler ---
// Replaces AMY's built-in juno_filter_midi_handler with a more complete
// mapping that works on all channels. Any MIDI controller can use these.
//
//   CC 1  — Mod Wheel (placeholder, logged only)
//   CC 7  — Channel Volume
//   CC 10 — Pan (0=left, 64=center, 127=right)
//   CC 70 — Filter Cutoff (exponential, ~13Hz to ~6400Hz)
//   CC 71 — Filter Resonance (Q 0.5 to 16)
//   CC 75 — Chorus Level (0=dry, 127=full)
//   CC 91 — Reverb Level (0=dry, 127=full)
//   CC 93 — Chorus Level (alternate standard CC)
//
// CCs handled by AMY internally (do not duplicate):
//   CC 0  — Bank Select
//   CC 64 — Sustain Pedal
//   CC 123 — All Notes Off

static void nsmbl_midi_cc_handler(uint8_t *bytes, uint16_t len, uint8_t is_sysex)
{
    uint8_t status = bytes[0] & 0xF0;
    if (status != 0xB0) return;

    uint8_t channel = (bytes[0] & 0x0F) + 1;  // AMY synths are 1-indexed
    uint8_t cc = bytes[1];
    uint8_t val = bytes[2];
    float normalized = (float)val / 127.0f;
    amy_event e;

    switch (cc) {
    case 7:  // Volume — scale amplitude
        e = amy_default_event();
        e.synth = channel;
        e.amp_coefs[COEF_CONST] = normalized;
        amy_add_event(&e);
        break;

    case 10:  // Pan — 0.0 left, 0.5 center, 1.0 right
        e = amy_default_event();
        e.synth = channel;
        e.pan_coefs[COEF_CONST] = normalized;
        amy_add_event(&e);
        break;

    case 70:  // Filter Cutoff — exponential mapping from AMY's example
        e = amy_default_event();
        e.synth = channel;
        e.filter_freq_coefs[COEF_CONST] = exp2f(0.0938f * (float)val);
        amy_add_event(&e);
        break;

    case 71:  // Resonance — exponential Q 0.5-16
        e = amy_default_event();
        e.synth = channel;
        e.resonance = 0.7f * exp2f(0.03125f * (float)val);
        amy_add_event(&e);
        break;

    case 75:  // Chorus Level
    case 93:  // Chorus Send (standard GM CC)
        e = amy_default_event();
        e.chorus_level = normalized;
        amy_add_event(&e);
        break;

    case 91:  // Reverb Level
        e = amy_default_event();
        e.reverb_level = normalized;
        amy_add_event(&e);
        break;
    }
}

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

    // Replace AMY's built-in Juno-only CC handler with our expanded version
    amy_global.config.amy_external_midi_input_hook = nsmbl_midi_cc_handler;

    ESP_LOGI(TAG, "AMY synth engine initialized (%d oscs, NSMBL CC handler active)", AMY_OSCS);

    xTaskCreatePinnedToCore(synth_task, "synth", 4096, midi_queue, 5, NULL, SYNTH_CORE);
}
