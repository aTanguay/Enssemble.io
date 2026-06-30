#pragma once

// --- Band Member: Pryns (Prince) ---
// Role: Lead
// Firmware: NSMBL_EEnoo (AMY synth)

#define DEVICE_NAME     "NSMBL_Pryns"
#define MIDI_CHANNEL    2       // Dedicated channel for lead
#define DEFAULT_PATCH   32      // Juno A51 Lead I

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
