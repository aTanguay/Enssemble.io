#pragma once

#include "esp_err.h"
#include "driver/i2c_master.h"
#include <stdint.h>
#include <stdbool.h>

// Probe + initialize the SH1107 128x128 OLED on the shared I2C bus.
// Non-fatal: returns ESP_ERR_NOT_FOUND if absent (UI still runs without a screen).
esp_err_t oled_init(i2c_master_bus_handle_t bus);

bool oled_present(void);

// Bridge monitor screen. in: 0=ALL else 1-16. out: 0=passthrough (THRU) else 1-16.
// active_field: 0 = IN box highlighted, 1 = OUT box highlighted. note_str is the
// big last-note readout (e.g. "C3 v100" or "--"). in_lit/out_lit flash the
// activity dots; count is the forwarded note-on counter.
void oled_draw_bridge(uint8_t in, uint8_t out, int active_field,
                      const char *note_str, bool in_lit, bool out_lit, uint32_t count);

// Two centered lines — boot splash / status.
void oled_draw_message(const char *line1, const char *line2);
