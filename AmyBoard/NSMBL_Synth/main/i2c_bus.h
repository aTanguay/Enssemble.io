#pragma once

#include "esp_err.h"
#include "driver/i2c_master.h"

// Initialize the shared I2C master bus (Modulino Knob + SH1107 OLED live here).
esp_err_t i2c_bus_init(void);

// Handle for adding devices; valid only after a successful i2c_bus_init().
i2c_master_bus_handle_t i2c_bus_get(void);
