#pragma once

// --- Bridge identity (generic default) ---
// Copy a named config from ../configs/ before building, e.g.:
//   cp configs/config_Garee.h main/config.h
#define DEVICE_NAME     "NSMBL_Bridge"

// --- Bridge channel routing ---
//   BRIDGE_IN:  BRIDGE_ALL (0) = forward every channel, or 1-16 = only that channel
//   BRIDGE_OUT: BRIDGE_PASSTHROUGH (0) = keep each message's original channel,
//               or 1-16 = remap ALL forwarded messages onto that one channel
//               (handy because most hardware is fixed to channel 1).
#define BRIDGE_ALL          0
#define BRIDGE_PASSTHROUGH  0
#define BRIDGE_IN   BRIDGE_ALL          // generic default: forward everything...
#define BRIDGE_OUT  BRIDGE_PASSTHROUGH  // ...unchanged

// --- AMYboard DIN MIDI out (UART1) ---
// AMYboard: MIDI OUT Type A = GPIO14 (Type B = GPIO15), MIDI IN = GPIO21, UART1.
#define MIDI_UART_NUM   UART_NUM_1
#define MIDI_BAUD       31250
#define MIDI_TX_PIN     14      // MIDI OUT (Type A); use 15 for Type B
#define MIDI_RX_PIN     21      // MIDI IN — reserved for future thru/merge

// --- MIDI Event Queue ---
#define MIDI_QUEUE_SIZE 128
