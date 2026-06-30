#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

void synth_engine_init(QueueHandle_t midi_queue);
