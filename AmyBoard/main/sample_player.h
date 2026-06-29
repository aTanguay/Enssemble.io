#pragma once

#include "esp_err.h"
#include <stdint.h>

esp_err_t sample_player_init(void);
esp_err_t sample_player_load_kit(const char *path);
void sample_player_note_on(uint8_t note, uint8_t velocity);
void sample_player_note_off(uint8_t note);
