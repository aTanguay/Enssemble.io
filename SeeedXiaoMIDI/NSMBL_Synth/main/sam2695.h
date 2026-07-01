#pragma once

#include <stdint.h>
#include "esp_err.h"

esp_err_t sam2695_init(void);

void sam2695_note_on(uint8_t channel, uint8_t note, uint8_t velocity);
void sam2695_note_off(uint8_t channel, uint8_t note);
void sam2695_program_change(uint8_t channel, uint8_t bank, uint8_t program);
void sam2695_control_change(uint8_t channel, uint8_t cc, uint8_t value);
void sam2695_pitch_bend(uint8_t channel, uint16_t bend);
void sam2695_aftertouch(uint8_t channel, uint8_t pressure);
void sam2695_all_notes_off(uint8_t channel);
void sam2695_set_volume(uint8_t channel, uint8_t volume);

void sam2695_boot_sound(uint8_t channel);
