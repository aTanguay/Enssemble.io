#pragma once

#include <stdint.h>
#include <stdbool.h>

// Runtime channel routing for the bridge, shared between the BLE-parse task
// (reads IN filter), the forward task (reads OUT remap), and the UI task
// (sets both). Seeded from the config BRIDGE_IN/BRIDGE_OUT defaults at boot.
void    bridge_state_init(uint8_t in_default, uint8_t out_default);

uint8_t bridge_in(void);          // 0 = ALL channels, 1-16 = only that channel
void    bridge_set_in(uint8_t v);
uint8_t bridge_out(void);         // 0 = passthrough, 1-16 = remap target
void    bridge_set_out(uint8_t v);

// Activity monitor. Recorded by the parse task (received) and the forward task
// (forwarded); read by the UI. `type` is a midi_msg_type_t (see ble_midi.h).
void bridge_note_in(uint8_t type, uint8_t note, uint8_t vel);
void bridge_note_out(uint8_t type, uint8_t note, uint8_t vel);

typedef struct {
    uint8_t  last_note;      // most recent forwarded note-on
    uint8_t  last_vel;
    bool     have_note;
    uint32_t out_count;      // forwarded note-ons
    uint32_t last_in_ms;     // activity timestamps (for the IN/OUT blinkers)
    uint32_t last_out_ms;
} bridge_snap_t;

void bridge_snapshot(bridge_snap_t *out);
