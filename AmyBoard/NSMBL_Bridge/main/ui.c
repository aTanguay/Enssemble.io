#include "ui.h"
#include "config.h"
#include "modulino_knob.h"
#include "oled.h"
#include "bridge_state.h"

#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <stdio.h>
#include <string.h>

static const char *TAG = "ui";

#define BLINK_MS  120   // a dot stays lit this long after activity

static const char *const NOTE_NAMES[12] = {
    "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
};

static uint8_t clamp16(int v)
{
    if (v < 0) return 0;
    if (v > 16) return 16;
    return (uint8_t)v;
}

static void fmt_note(char *buf, size_t n, const bridge_snap_t *s)
{
    if (!s->have_note) { snprintf(buf, n, "--"); return; }
    // octave = note/12 - 1 so MIDI 36 = C2 (project convention)
    snprintf(buf, n, "%s%d v%u",
             NOTE_NAMES[s->last_note % 12], s->last_note / 12 - 1, s->last_vel);
}

static void ui_task(void *param)
{
    i2c_master_bus_handle_t bus = (i2c_master_bus_handle_t)param;

    modulino_knob_init(bus);
    oled_init(bus);

    int active = 1; // 0 = IN, 1 = OUT (start on OUT — the common tweak)

    ESP_LOGI(TAG, "UI on core %d (knob=%d oled=%d)",
             xPortGetCoreID(), modulino_knob_present(), oled_present());

    // Track last-drawn state so we only flush the OLED on change.
    char last_note[16] = "";
    uint8_t last_in = 255, last_out = 255;
    int last_active = -1;
    bool last_inlit = false, last_outlit = false;
    uint32_t last_count = 0xFFFFFFFF;
    bool first = true;

    while (1) {
        int delta = 0;
        bool pressed = false;
        modulino_knob_poll(&delta, &pressed);

        if (pressed) active ^= 1;

        if (delta != 0) {
            if (active == 0) bridge_set_in(clamp16((int)bridge_in() + delta));
            else             bridge_set_out(clamp16((int)bridge_out() + delta));
        }

        bridge_snap_t s;
        bridge_snapshot(&s);

        uint32_t now = (uint32_t)(esp_timer_get_time() / 1000);
        bool in_lit  = (now - s.last_in_ms)  < BLINK_MS;
        bool out_lit = (now - s.last_out_ms) < BLINK_MS;

        char note[16];
        fmt_note(note, sizeof(note), &s);

        uint8_t in = bridge_in(), out = bridge_out();

        // Redraw only when something visible changed (keeps I2C quiet at idle).
        if (first || in != last_in || out != last_out || active != last_active ||
            in_lit != last_inlit || out_lit != last_outlit ||
            s.out_count != last_count || strcmp(note, last_note) != 0) {

            oled_draw_bridge(in, out, active, note, in_lit, out_lit, s.out_count);

            first = false;
            last_in = in; last_out = out; last_active = active;
            last_inlit = in_lit; last_outlit = out_lit;
            last_count = s.out_count;
            strncpy(last_note, note, sizeof(last_note) - 1);
        }

        vTaskDelay(pdMS_TO_TICKS(30)); // ~33 Hz
    }
}

void ui_init(i2c_master_bus_handle_t bus)
{
    xTaskCreate(ui_task, "ui", 4096, bus, 3, NULL);
}
