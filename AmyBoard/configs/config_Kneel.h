#pragma once

// --- Band Member Identity ---
// Copy a named config from AmyBoard/configs/ to customize:
//   config_Kneel.h  — Drums (ch10, 808 kits)
#define DEVICE_NAME     "NSMBL_Kneel"
#define MIDI_CHANNEL    10      // 1-16, unit only responds to this channel
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

// --- I2C bus (Qwiic accessories: Modulino Knob + SH1107 OLED) ---
#define I2C_SDA             17
#define I2C_SCL             18
#define I2C_FREQ_HZ         400000
#define KNOB_ADDR_PRIMARY   0x3B    // Modulino Knob 7-bit (Arduino's 8-bit 0x76 / 2)
#define KNOB_ADDR_ALT       0x3A    // Modulino Knob 7-bit (Arduino's 8-bit 0x74 / 2)
#define OLED_ADDR           0x3C    // SH1107 128x128
// SH1107 panel variant. The generic _128x128_ descriptor applies x_offset=96,
// which wraps this module horizontally. The _seeed_ variant is the same init
// sequence with x_offset=0 — fixes the wrap, keeps the (correct) vertical start.
#define OLED_SETUP          u8g2_Setup_sh1107_i2c_seeed_128x128_f

// --- Sample Player ---
#define KITS_DIR        "/sdcard/Kits"                      // browsed by the encoder UI
#define KIT_PATH        "/sdcard/Kits/808_Mars_01_16kit.wav" // default kit loaded at boot
#define MAX_KITS        96      // most kits the UI will list (MARS packs run well past 32)
#define MAX_SLICES      16
#define BASE_NOTE       36      // C2 — first slice maps here
#define MAX_VOICES      4       // polyphony for simultaneous hits
#define SAMPLE_RATE     44100
