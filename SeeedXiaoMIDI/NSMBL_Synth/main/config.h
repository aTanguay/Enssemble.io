#pragma once

// --- Band Member Identity ---
// Copy a named config from configs/ before building, e.g.:
//   cp configs/config_Eenoo.h  main/config.h   (pads,  ch1)
//   cp configs/config_Prynz.h  main/config.h   (lead,  ch2)
//   cp configs/config_Botsee.h main/config.h   (bass,  ch3)
//   cp configs/config_8OhAte.h main/config.h   (drums, ch10)
#define DEVICE_NAME     "NSMBL_Synth"
#define DEFAULT_PATCH   0
#define DEFAULT_BANK    0

// --- Channel behavior ---
// The unit listens on exactly two MIDI channels and ignores all others, like a
// normal hardware synth — you route in your host (AUM). Send melody to MIDI
// channel 1 and GM drums to channel 10; anything else is dropped by the parser.
#define VOICE_CHANNEL   0       // melodic voice (internal 0 = MIDI channel 1)
#define DRUM_CHANNEL    9       // GM drums   (internal 9 = MIDI channel 10)

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
