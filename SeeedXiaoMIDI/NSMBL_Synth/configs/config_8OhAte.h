#pragma once

// ============================================================
//  8OHATE — Drums (The 808)
//  Copy to main/config.h before building:
//    cp configs/config_8OhAte.h main/config.h
//
//  GM drums: sequence on MIDI channel 10 (note = drum piece,
//  Program Change = kit). The melodic voice on ch1 is still
//  available if wanted.
// ============================================================

// --- Band Member Identity ---
#define DEVICE_NAME     "NSMBL_8OhAte"
#define DEFAULT_PATCH   0       // melodic ch1 default (drums live on ch10)
#define DEFAULT_BANK    0

// --- Channel behavior ---
// Listens ONLY on MIDI channel 10 (drums). Both channels = 9 so the parser
// passes only ch10, and MIDI ch10 is always GM drums on the SAM2695.
#define VOICE_CHANNEL   9       // 8OhAte listens only on MIDI channel 10
#define DRUM_CHANNEL    9       // MIDI channel 10 = GM drums

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
