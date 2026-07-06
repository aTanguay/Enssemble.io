#pragma once

// --- Band Member Identity ---
// Copy a named config from AmyBoard/configs/ to customize:
//   config_Eenoo.h  — Pads/Ambient (ch1, Juno Synth Pad)
//   config_Prynz.h  — Lead (ch2, Juno Lead I)
//   config_Botsee.h — Bass (ch3, Juno Synth Bass I)
//   config_Kneel.h  — Drums (ch10, sample kits)
#define DEVICE_NAME     "NSMBL_Synth"
#define MIDI_CHANNEL    0       // 0 = all channels, 1-16 = single channel filter
#define DEFAULT_PATCH   0       // AMY patch number

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
