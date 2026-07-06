#pragma once

// ============================================================
//  MOROH — Arps / Sequences (Giorgio Moroder)
//  Copy to main/config.h before building:
//    cp configs/config_Moroh.h main/config.h
// ============================================================

// --- Band Member Identity ---
#define DEVICE_NAME     "NSMBL_Moroh"
#define DEFAULT_PATCH   80      // Lead 1 (Square) — top of the GM Synth Lead area
#define DEFAULT_BANK    0

// --- Channel behavior ---
// Listens ONLY on its roster channel (MIDI ch5). DRUM_CHANNEL disabled (> 15).
#define VOICE_CHANNEL   4       // Moroh listens only on MIDI channel 5
#define DRUM_CHANNEL    0xFF    // no drums — single-channel synth

// --- Hardware Pins (XIAO ESP32-C3) ---
#define BUTTON_0_PIN    5
#define BUTTON_1_PIN    4
#define BUTTON_2_PIN    3
#define BUTTON_3_PIN    2
#define LED_PIN         10      // Active LOW

// --- SAM2695 UART ---
#define SAM_UART_NUM    UART_NUM_0
#define SAM_UART_BAUD   31250
#define SAM_TX_PIN      21      // XIAO Serial0 TX
#define SAM_RX_PIN      20      // XIAO Serial0 RX

// --- MIDI Event Queue ---
#define MIDI_QUEUE_SIZE 64

// --- Button Timing ---
#define DEBOUNCE_MS     50
#define LONG_PRESS_MS   600
