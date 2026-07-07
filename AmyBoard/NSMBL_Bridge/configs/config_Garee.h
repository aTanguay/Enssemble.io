#pragma once

// ============================================================
//  GAREE — BLE-to-DIN MIDI bridge (Gary Numan)
//  Copy to main/config.h before building:
//    cp configs/config_Garee.h main/config.h
//
//  Default personality: forward ALL channels and remap them onto MIDI ch1, so
//  a hardware synth (usually fixed to ch1) just plays whatever you route to
//  Garee — no menu-diving on the gear.
//
//  Variants (edit the two #defines below):
//    Multi-channel hub : BRIDGE_IN=ALL,  BRIDGE_OUT=PASSTHROUGH
//    Dedicated bridge  : BRIDGE_IN=<ch>, BRIDGE_OUT=<ch>  (band-member style)
// ============================================================

// --- Band Member Identity ---
#define DEVICE_NAME     "NSMBL_Garee"

// --- Bridge channel routing ---
#define BRIDGE_ALL          0
#define BRIDGE_PASSTHROUGH  0
#define BRIDGE_IN   BRIDGE_ALL   // listen on every channel
#define BRIDGE_OUT  1            // remap everything to MIDI channel 1

// --- AMYboard DIN MIDI out (UART1) ---
#define MIDI_UART_NUM   UART_NUM_1
#define MIDI_BAUD       31250
#define MIDI_TX_PIN     14      // MIDI OUT data leg (Type A); use 15 for Type B
#define MIDI_RX_PIN     21      // MIDI IN — reserved for future thru/merge
// Hold the other TRS leg HIGH as the MIDI current source (see config.h note).
// Type A: TX=14, SRC=15. Type B: swap to TX=15, SRC=14.
#define MIDI_SRC_PIN    15

// --- MIDI Event Queue ---
#define MIDI_QUEUE_SIZE 128
