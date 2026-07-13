#include "ui.h"
#include "config.h"
#include "modulino_knob.h"
#include "oled.h"
#include "ble_midi.h"   // midi_event_t + MIDI_* enums

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <stdio.h>

static const char *TAG = "ui";

// Menu rows. Patch is special (loads via bank+PC); the rest are 0-127 CC params.
enum { ROW_PATCH = 0, ROW_FILTER, ROW_RESO, ROW_REVERB, ROW_CHORUS, ROW_VOLUME, ROW_PAN, ROW_COUNT };

static const char *const s_labels[ROW_COUNT] = {
    "Patch", "Filter", "Reso", "Reverb", "Chorus", "Volume", "Pan"
};
// CC number per row (matches nsmbl_midi_cc_handler); ROW_PATCH has none.
static const int s_cc[ROW_COUNT] = { -1, 70, 71, 91, 93, 7, 10 };

static QueueHandle_t s_queue;
static int  s_val[ROW_COUNT];   // 0-127 param values (ROW_PATCH slot unused)
static int  s_patch;            // 0-255
static int  s_sel;              // selected row
static bool s_edit;             // false = NAVIGATE, true = EDIT

static void enqueue(uint8_t type, uint8_t d1, uint8_t d2)
{
    midi_event_t e = {0};
    e.type = type;
    e.channel = UI_CHANNEL;
    e.data1 = d1;
    e.data2 = d2;
    xQueueSend(s_queue, &e, 0);
}

static void send_patch(void)
{
    // AMY: patch = program + 128*bank. Bank-select (CC0) then Program Change.
    enqueue(MIDI_CC, 0, (s_patch >> 7) & 0x7F);
    enqueue(MIDI_PROGRAM_CHANGE, s_patch & 0x7F, 0);
}

static void send_param(int row)
{
    enqueue(MIDI_CC, (uint8_t)s_cc[row], (uint8_t)s_val[row]);
}

static void fmt_patch(char *buf, size_t n)
{
    if (s_patch < 128) snprintf(buf, n, "Juno %03d", s_patch);
    else               snprintf(buf, n, "DX7 %03d", s_patch - 128);
}

static void redraw(void)
{
    char vbuf[ROW_COUNT][12];
    const char *vals[ROW_COUNT];
    int bars[ROW_COUNT];

    for (int i = 0; i < ROW_COUNT; i++) {
        if (i == ROW_PATCH) {
            fmt_patch(vbuf[i], sizeof(vbuf[i]));
            bars[i] = -1;                 // name, no bar
        } else {
            snprintf(vbuf[i], sizeof(vbuf[i]), "%d", s_val[i]);
            bars[i] = s_val[i];
        }
        vals[i] = vbuf[i];
    }

    if (s_edit) {
        oled_draw_edit(s_labels[s_sel], vals[s_sel], bars[s_sel]);
    } else {
        oled_draw_menu(s_labels, vals, bars, ROW_COUNT, s_sel);
    }
}

static void ui_task(void *param)
{
    i2c_master_bus_handle_t bus = (i2c_master_bus_handle_t)param;

    modulino_knob_init(bus);
    oled_init(bus);

    // Nominal defaults for display; params aren't pushed at boot (let the patch
    // define its own sound — first edit sends the real CC). Patch IS pushed so
    // the screen and the audio agree.
    s_val[ROW_FILTER] = 127;
    s_val[ROW_RESO]   = 0;
    s_val[ROW_REVERB] = 0;
    s_val[ROW_CHORUS] = 0;
    s_val[ROW_VOLUME] = 100;
    s_val[ROW_PAN]    = 64;
    s_patch = DEFAULT_PATCH;
    send_patch();

    ESP_LOGI(TAG, "UI on core %d (knob=%d oled=%d ch=%d)",
             xPortGetCoreID(), modulino_knob_present(), oled_present(), UI_CHANNEL);

    redraw();

    while (1) {
        int delta = 0;
        bool pressed = false;
        modulino_knob_poll(&delta, &pressed);

        if (delta != 0) {
            if (!s_edit) {                          // NAVIGATE: move selection
                int n = s_sel + delta;
                if (n < 0) n = 0;
                if (n >= ROW_COUNT) n = ROW_COUNT - 1;
                if (n != s_sel) { s_sel = n; redraw(); }
            } else if (s_sel == ROW_PATCH) {        // EDIT patch: load live
                int n = s_patch + delta;
                if (n < 0) n = 0;
                if (n > 255) n = 255;
                if (n != s_patch) { s_patch = n; send_patch(); redraw(); }
            } else {                                // EDIT param: send CC live
                int n = s_val[s_sel] + delta;
                if (n < 0) n = 0;
                if (n > 127) n = 127;
                if (n != s_val[s_sel]) { s_val[s_sel] = n; send_param(s_sel); redraw(); }
            }
        }

        if (pressed) {                              // toggle NAVIGATE <-> EDIT
            s_edit = !s_edit;
            redraw();
        }

        vTaskDelay(pdMS_TO_TICKS(20)); // ~50 Hz
    }
}

void ui_init(QueueHandle_t midi_queue, i2c_master_bus_handle_t bus)
{
    s_queue = midi_queue;
    // Low priority on the BLE/control core so it never disturbs audio on core 1.
    xTaskCreatePinnedToCore(ui_task, "ui", 4096, bus, 3, NULL, BLE_CORE);
}
