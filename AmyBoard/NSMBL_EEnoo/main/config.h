#pragma once

// --- Band Member Identity ---
#define DEVICE_NAME     "NSMBL_EEnoo"
#define MIDI_CHANNEL    0       // 0 = all channels, 1-16 = single channel filter
#define DEFAULT_PATCH   0       // AMY patch number (0 = piano-like)

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

// --- AMY Synth ---
// AMY defines its own sample rate (44100) and block size (256)
