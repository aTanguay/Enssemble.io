#pragma once

#include "esp_err.h"
#include "driver/i2c_master.h"
#include <stdbool.h>

// Probe + initialize the SH1107 128x128 OLED on the shared I2C bus.
// Non-fatal: returns ESP_ERR_NOT_FOUND if absent (UI still runs without a screen).
esp_err_t oled_init(i2c_master_bus_handle_t bus);

bool oled_present(void);

// NAVIGATE screen: a menu of rows. `vals[i]` is the right-side text for row i.
// `bars[i]` in 0..127 draws a value bar for that row; bars[i] < 0 draws none
// (used for the Patch row, which shows a name instead). `selected` is highlighted.
void oled_draw_menu(const char *const *labels, const char *const *vals,
                    const int *bars, int count, int selected);

// EDIT screen: one row full-screen. `bar` in 0..127 draws a wide value bar; < 0
// shows just `val` big (Patch name).
void oled_draw_edit(const char *label, const char *val, int bar);

// Two centered lines — boot splash / status.
void oled_draw_message(const char *line1, const char *line2);
