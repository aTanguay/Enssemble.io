#pragma once

#include "driver/i2c_master.h"

// Start the local UI (encoder + OLED): live IN/OUT channel routing + a MIDI
// activity monitor. Non-fatal if the knob/OLED are absent.
void ui_init(i2c_master_bus_handle_t bus);
