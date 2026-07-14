#include "bridge_state.h"
#include "ble_midi.h"   // midi_msg_type_t (MIDI_NOTE_ON, ...)

#include "esp_timer.h"
#include "freertos/FreeRTOS.h"

// Guards the monitor struct (written from the BLE-parse task AND the forward
// task, read from the UI task). Short critical sections only.
static portMUX_TYPE s_mux = portMUX_INITIALIZER_UNLOCKED;

// Single-byte routing values: aligned byte access is atomic, so no lock needed.
static volatile uint8_t s_in;
static volatile uint8_t s_out;

static struct {
    uint8_t  last_note, last_vel;
    bool     have_note;
    uint32_t out_count;
    uint32_t last_in_ms, last_out_ms;
} s_mon;

static inline uint32_t now_ms(void) { return (uint32_t)(esp_timer_get_time() / 1000); }

void bridge_state_init(uint8_t in_default, uint8_t out_default)
{
    s_in = in_default;
    s_out = out_default;
}

uint8_t bridge_in(void)        { return s_in; }
void    bridge_set_in(uint8_t v)  { s_in = v; }
uint8_t bridge_out(void)       { return s_out; }
void    bridge_set_out(uint8_t v) { s_out = v; }

void bridge_note_in(uint8_t type, uint8_t note, uint8_t vel)
{
    (void)note; (void)vel;
    uint32_t t = now_ms();
    portENTER_CRITICAL(&s_mux);
    s_mon.last_in_ms = t;
    portEXIT_CRITICAL(&s_mux);
}

void bridge_note_out(uint8_t type, uint8_t note, uint8_t vel)
{
    uint32_t t = now_ms();
    portENTER_CRITICAL(&s_mux);
    s_mon.last_out_ms = t;
    if (type == MIDI_NOTE_ON && vel > 0) {
        s_mon.out_count++;
        s_mon.last_note = note;
        s_mon.last_vel = vel;
        s_mon.have_note = true;
    }
    portEXIT_CRITICAL(&s_mux);
}

void bridge_snapshot(bridge_snap_t *o)
{
    portENTER_CRITICAL(&s_mux);
    o->last_note   = s_mon.last_note;
    o->last_vel    = s_mon.last_vel;
    o->have_note   = s_mon.have_note;
    o->out_count   = s_mon.out_count;
    o->last_in_ms  = s_mon.last_in_ms;
    o->last_out_ms = s_mon.last_out_ms;
    portEXIT_CRITICAL(&s_mux);
}
