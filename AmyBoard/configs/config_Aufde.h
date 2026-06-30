#pragma once

// --- Band Member: Aufde (Melissa Auf der Maur) ---
// Role: Bass
// Firmware: NSMBL_EEnoo (AMY synth)

#define DEVICE_NAME     "NSMBL_Aufde"
#define MIDI_CHANNEL    3       // Dedicated channel for bass
#define DEFAULT_PATCH   31      // Juno A48 Synth Bass I

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
