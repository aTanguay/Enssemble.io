#pragma once

#include "esp_err.h"
#include "driver/i2c_master.h"
#include <stdbool.h>

// Probe + initialize the SH1107 128x128 OLED on the shared I2C bus.
// Non-fatal: returns ESP_ERR_NOT_FOUND if absent (UI still runs without a screen).
esp_err_t oled_init(i2c_master_bus_handle_t bus);

bool oled_present(void);

// Scrollable kit list. `selected` is the highlighted row; `loaded` is marked '*'.
void oled_draw_kit_list(const char *const *names, int count, int selected, int loaded);

// Full-screen "Loading <name>..." shown during the ~5 s kit load.
void oled_draw_loading(const char *name);

// Two centered lines — boot splash / "No kits found" style status.
void oled_draw_message(const char *line1, const char *line2);
