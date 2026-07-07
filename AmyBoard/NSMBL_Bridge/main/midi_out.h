#pragma once

#include <stdint.h>
#include "ble_midi.h"   // midi_event_t

// Initialize the DIN MIDI output UART (AMYboard MIDI OUT, 31250 baud).
void midi_out_init(void);

// Re-serialize a parsed MIDI event to the DIN output on the given 0-indexed
// channel (0-15). Used by the bridge after applying its channel remap.
void midi_out_send_event(const midi_event_t *event, uint8_t channel);
