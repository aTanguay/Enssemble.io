#pragma once

// --- Bridge identity (generic default) ---
// Copy a named config from ../configs/ before building, e.g.:
//   cp configs/config_Garee.h main/config.h
#define DEVICE_NAME     "NSMBL_Garee"

// --- Bridge channel routing ---
//   BRIDGE_IN:  BRIDGE_ALL (0) = forward every channel, or 1-16 = only that channel
//   BRIDGE_OUT: BRIDGE_PASSTHROUGH (0) = keep each message's original channel,
//               or 1-16 = remap ALL forwarded messages onto that one channel
//               (handy because most hardware is fixed to channel 1).
#define BRIDGE_ALL          0
#define BRIDGE_PASSTHROUGH  0
#define BRIDGE_IN   BRIDGE_ALL          // generic default: forward everything...
#define BRIDGE_OUT  1            // remap everything to MIDI channel 1

// --- AMYboard DIN MIDI out (UART1) ---
// AMYboard: MIDI OUT Type A = GPIO14 (Type B = GPIO15), MIDI IN = GPIO21, UART1.
#define MIDI_UART_NUM   UART_NUM_1
#define MIDI_BAUD       31250
#define MIDI_TX_PIN     14      // MIDI OUT data leg (Type A); use 15 for Type B
#define MIDI_RX_PIN     21      // MIDI IN — reserved for future thru/merge
// The OUT jack is TRS tip=GPIO14, ring=GPIO15, sleeve=GND with NO fixed 3.3V rail.
// A DIN MIDI opto needs a current source, so the *other* leg must be held HIGH
// (MIDI idle/source). Type A: TX=14, SRC=15. Type B: swap to TX=15, SRC=14.
#define MIDI_SRC_PIN    15

// --- I2C bus (Qwiic accessories: Modulino Knob + SH1107 OLED) ---
#define I2C_SDA             17
#define I2C_SCL             18
#define I2C_FREQ_HZ         400000
#define KNOB_ADDR_PRIMARY   0x3B    // Modulino Knob 7-bit (Arduino 8-bit 0x76 / 2)
#define KNOB_ADDR_ALT       0x3A    // Modulino Knob 7-bit (Arduino 8-bit 0x74 / 2)
#define OLED_ADDR           0x3C    // SH1107 128x128
#define OLED_SETUP          u8g2_Setup_sh1107_i2c_seeed_128x128_f  // seeed = x_offset 0

// --- MIDI Event Queue ---
#define MIDI_QUEUE_SIZE 128
