#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

typedef enum {
    MIDI_NOTE_ON,
    MIDI_NOTE_OFF,
    MIDI_CC,
    MIDI_PROGRAM_CHANGE,
    MIDI_PITCH_BEND,
} midi_msg_type_t;

typedef struct {
    midi_msg_type_t type;
    uint8_t channel;
    uint8_t data1;      // note number, CC number, or program number
    uint8_t data2;      // velocity, CC value
    int16_t pitch_bend; // -8192 to 8191
} midi_event_t;

void ble_midi_init(QueueHandle_t midi_queue);
