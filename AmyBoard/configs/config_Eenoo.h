#pragma once

// --- Band Member: Eenoo (Brian Eno) ---
// Role: Pads / Ambient
// Firmware: NSMBL_Synth (AMY synth)

#define DEVICE_NAME     "NSMBL_Eenoo"
#define MIDI_CHANNEL    1       // Dedicated channel for pads
#define DEFAULT_PATCH   47      // Juno A68 Synth Pad

// --- Core Assignment ---
#define BLE_CORE        0
#define SYNTH_CORE      1

// --- MIDI Event Queue ---
#define MIDI_QUEUE_SIZE 64

// --- I2S Audio Output ---
#define I2S_MCLK        3
#define I2S_BCLK        8
#define I2S_WS          2
#define I2S_DOUT        6
