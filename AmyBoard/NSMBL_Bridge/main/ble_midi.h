#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

typedef enum {
    MIDI_NOTE_ON,
    MIDI_NOTE_OFF,
    MIDI_CC,
    MIDI_PROGRAM_CHANGE,
    MIDI_PITCH_BEND,
    MIDI_AFTERTOUCH,
} midi_msg_type_t;

typedef struct {
    midi_msg_type_t type;
    uint8_t channel;
    uint8_t data1;
    uint8_t data2;
    int16_t pitch_bend;
} midi_event_t;

void ble_midi_init(QueueHandle_t midi_queue);
