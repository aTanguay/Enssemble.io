#pragma once

#include "esp_err.h"
#include "driver/i2c_master.h"
#include <stdbool.h>

// Probe the Modulino Knob (0x76, then 0x74) on the given bus and register it.
// Returns ESP_OK if found; ESP_ERR_NOT_FOUND if absent (UI degrades gracefully).
esp_err_t modulino_knob_init(i2c_master_bus_handle_t bus);

bool modulino_knob_present(void);

// Poll the knob. On success:
//   *delta        = signed encoder change since the previous poll (wrap-safe)
//   *pressed_edge = true on a button press (rising edge)
// The first successful poll only latches state and reports zero/false.
esp_err_t modulino_knob_poll(int *delta, bool *pressed_edge);
