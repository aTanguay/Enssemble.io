#pragma once

#include "driver/i2c_master.h"

// Start the local UI task (encoder + OLED) on the shared I2C bus.
void ui_init(i2c_master_bus_handle_t bus);
