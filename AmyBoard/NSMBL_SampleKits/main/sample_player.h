#pragma once

#include "esp_err.h"
#include <stdint.h>

esp_err_t sample_player_init(void);

// Load a kit WAV. Safe to call at runtime: the currently-playing kit keeps
// sounding during the (~5 s) load and is swapped in atomically at the end. On
// failure the previous kit is left intact.
esp_err_t sample_player_load_kit(const char *path);

void sample_player_note_on(uint8_t note, uint8_t velocity);
void sample_player_note_off(uint8_t note);

// Basename (no dir/extension) of the loaded kit, or "" if none. For the OLED.
const char *sample_player_current_kit(void);
