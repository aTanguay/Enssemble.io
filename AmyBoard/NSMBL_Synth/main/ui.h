#pragma once

#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

// Start the local UI (encoder + OLED). The UI injects patch/CC events into the
// same `midi_queue` that BLE feeds, so the synth task stays the single AMY
// consumer (no cross-core AMY access).
void ui_init(QueueHandle_t midi_queue, i2c_master_bus_handle_t bus);
