#pragma once

// --- Band Member: Kneel (Neal Peart) ---
// Role: Drums
// Firmware: NSMBL_SmplCty (sample player)

#define DEVICE_NAME     "NSMBL_Kneel"
#define MIDI_CHANNEL    10      // Standard drum channel
#define DEFAULT_PATCH   0

// --- Core Assignment ---
#define BLE_CORE        0
#define SYNTH_CORE      1

// --- MIDI Event Queue ---
#define MIDI_QUEUE_SIZE 64

// --- SD Card (SPI) ---
#define SD_MOSI         11
#define SD_MISO         13
#define SD_CLK          12
#define SD_CS           10

// --- I2S Audio Output ---
#define I2S_MCLK        3
#define I2S_BCLK        8
#define I2S_WS          2
#define I2S_DOUT        6

// --- Sample Player ---
#define KIT_PATH        "/sdcard/Kits/808_Mars_01_16kit.wav"
#define MAX_SLICES      16
#define BASE_NOTE       36      // C2 — first slice maps here
#define MAX_VOICES      4       // polyphony for simultaneous hits
#define SAMPLE_RATE     44100
