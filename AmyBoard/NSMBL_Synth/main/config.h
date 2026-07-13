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

// --- I2C bus (Qwiic accessories: Modulino Knob + SH1107 OLED) ---
#define I2C_SDA             17
#define I2C_SCL             18
#define I2C_FREQ_HZ         400000
#define KNOB_ADDR_PRIMARY   0x3B    // Modulino Knob 7-bit (Arduino 8-bit 0x76 / 2)
#define KNOB_ADDR_ALT       0x3A    // Modulino Knob 7-bit (Arduino 8-bit 0x74 / 2)
#define OLED_ADDR           0x3C    // SH1107 128x128
#define OLED_SETUP          u8g2_Setup_sh1107_i2c_seeed_128x128_f  // seeed = x_offset 0

// --- Local UI ---
// Channel the encoder/OLED edits patch + CCs on. All-channel units (MIDI_CHANNEL 0)
// still need one target, so default to channel 1.
#define UI_CHANNEL          (MIDI_CHANNEL != 0 ? MIDI_CHANNEL : 1)
