#pragma once

// ============================================================
//  PRYNZ — Lead (Prince)
//  Copy to main/config.h before building:
//    cp configs/config_Prynz.h main/config.h
// ============================================================

// --- Band Member Identity ---
#define DEVICE_NAME     "NSMBL_Prynz"
#define DEFAULT_PATCH   81      // Lead 2 (sawtooth) — GM
#define DEFAULT_BANK    0

// --- Channel behavior ---
// Listens on VOICE_CHANNEL (melody) + DRUM_CHANNEL (GM drums), ignores the rest.
// Roster channel for Prynz is MIDI ch2 -> VOICE_CHANNEL 1.
#define VOICE_CHANNEL   1       // melody on MIDI channel 2
#define DRUM_CHANNEL    9       // GM drums on MIDI channel 10

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
