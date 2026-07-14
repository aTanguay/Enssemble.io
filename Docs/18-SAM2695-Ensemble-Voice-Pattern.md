---
tags: [firmware, sam2695, midi, esp-idf, ensemble, agent-instructions]
---

# SAM2695 — "Ensemble Voice" Pattern (Broadcast Program Change, Omni Note Passthrough)

⬅ [[00-SAM2695-Index]]

**Context:** Each unit is a standalone SAM2695 module (see [[17-SAM2695-basicino-ESP-IDF-Breakdown]] for the base ESP-IDF UART wiring) receiving MIDI over BLE from an iOS sequencer (AUM). Several units are deployed around a room as an ensemble ("The Band"). Goal: physical buttons on each unit pick one instrument voice for that unit; the unit should then play that voice regardless of which MIDI channel the sequencer happens to send notes on, while channel 10 remains dedicated to drums.

## Background an agent needs before touching this code

The SAM2695 is a **16-channel multitimbral GM synth** — every MIDI channel (1–16) has its *own independent* instrument assignment, set via Bank Select (CC0) + Program Change, and that assignment persists until changed again. **Channel 10 is hardwired by GM/GS convention to be the drum/rhythm channel** — Program Change on channel 10 selects a drum *kit*, not a melodic instrument, and this is fixed firmware behavior in the chip (see [[13-SAM2695-Drum-Sets]]), not something caused by app/host config.

This means: if your sequencer (AUM) assigns different tracks to different MIDI channels — which is normal, expected multitimbral behavior — and your firmware only ever sends Program Change to one channel while notes arrive on others, **those other channels are still sitting on whatever patch they last had (GM default = Grand Piano at power-up)**. This looks like "buttons don't work" or "wrong instrument depending on channel," but it isn't a bug in the SAM2695 or the MIDI stack — it's a mismatch between the chip's per-channel model and the single "one voice per box" behavior this project wants.

## The fix: broadcast + omni passthrough

Don't try to remap/funnel incoming note channels (that was a false start — see below). Instead:

1. **On patch change (button press):** send the same Bank Select + Program Change to **all 15 melodic channels** (everything except channel 10). This makes every channel's stored instrument identical, so it no longer matters which channel a given note arrives on.
2. **On incoming Note On/Off, CC, pitch bend, aftertouch:** pass through **completely unmodified**, on whatever channel they arrived on. No remapping needed once step 1 is done.
3. **On incoming Program Change from the host (optional but recommended):** drop it. If AUM ever sends its own PC messages per track, letting them through would desync that one channel from the other 14 and reintroduce the original bug. The hardware buttons should be the sole source of truth for patch selection.
4. **Channel 10** is left alone by all of the above — it never gets touched by the broadcast, and (depending on your ensemble design, see "Decisions" below) may also be filtered out of note passthrough on units that shouldn't play drums.

### Why not "channel funnel" (remap every note to one fixed channel)?
That's a different, also-valid pattern (documented for comparison below) but it collapses all incoming tracks onto a single MIDI channel, which throws away the channel identity of notes. **Broadcast** is the better fit here because it keeps each channel's notes native (simpler passthrough, no rewriting per-message) and simply keeps all 15 channels tuned to the same patch. Use funnel only if you specifically need to force polyphony/voice-stealing behavior onto one literal channel for some other reason.

## Reference implementation (ESP-IDF, C)

```c
#include "driver/uart.h"

#define SAM_UART       UART_NUM_1   // whichever UART is wired to SAM2695 MIDI_IN
#define DRUM_CHANNEL   9            // 0-indexed MIDI channel 10

// --- Patch selection: call this from button handlers ---
// Sets the same instrument on all 15 melodic channels (channel 10 untouched).
void sam2695_set_ensemble_voice(uint8_t bank, uint8_t program) {
    for (uint8_t ch = 0; ch < 16; ch++) {
        if (ch == DRUM_CHANNEL) continue;

        uint8_t bank_msg[3] = { (uint8_t)(0xB0 | ch), 0x00, (uint8_t)(bank & 0x7F) };
        uint8_t pc_msg[2]   = { (uint8_t)(0xC0 | ch), (uint8_t)(program & 0x7F) };

        uart_write_bytes(SAM_UART, (const char*)bank_msg, 3);
        uart_write_bytes(SAM_UART, (const char*)pc_msg, 2);
    }
}

// --- Incoming MIDI from BLE (AUM) ---
// Notes/CC/pitchbend/aftertouch: pass through unmodified.
// Program Change from host: dropped, so it can't fight the button-selected voice.
// SysEx / system messages: pass through unmodified.
void handle_incoming_midi(const uint8_t *msg, size_t len) {
    if (len == 0) return;

    uint8_t status = msg[0];

    // System messages (0xF0-0xFF) have no channel nibble - always pass through
    if ((status & 0xF0) == 0xF0) {
        uart_write_bytes(SAM_UART, (const char*)msg, len);
        return;
    }

    uint8_t msg_type = status & 0xF0;

    // Swallow host-originated Program Change - buttons are the only patch source
    if (msg_type == 0xC0) return;

    // Everything else (Note On/Off, CC, Pitch Bend, Poly/Channel Aftertouch): passthrough as-is
    uart_write_bytes(SAM_UART, (const char*)msg, len);
}

// --- Call once at boot with your default/last-saved patch ---
// GM power-on default is Grand Piano on every channel; without this call,
// a freshly booted unit sits on piano until a button is pressed.
void sam2695_init_default_voice(void) {
    sam2695_set_ensemble_voice(0 /*bank*/, 0 /*GrandPiano_1*/);
}
```

## Decisions to make explicit per-unit (don't leave these implicit)

- **Should this unit play drums at all?** If a given box in the ensemble is melodic-only, filter out channel 10 entirely in `handle_incoming_midi` (`if ((status & 0x0F) == DRUM_CHANNEL) return;` after confirming it's a channel-voice message). If it should be a dedicated drum unit, consider the inverse — only pass channel 10 through, ignore everything else.
- **Persisting the selected voice across power cycles.** If you want a unit to remember its last-selected patch after a reboot (NVS/flash), store just the `(bank, program)` pair and call `sam2695_set_ensemble_voice()` with it in `sam2695_init_default_voice()` instead of hardcoding Grand Piano.
- **Button debounce.** Not shown above — make sure button presses are debounced before calling `sam2695_set_ensemble_voice()`, since a bouncing GPIO firing this function rapidly sends 15×2 = 30 MIDI messages per bounce.
- **Bank value.** All examples above use bank `0` (standard GM). If you want MT-32 variation sounds ([[12-SAM2695-Sounds-GM-MT32]]), that's selected differently (CC0=127 then PC) — not a bank-select in the GM sense — so don't conflate the two if you add that feature later.

## Related
- [[17-SAM2695-basicino-ESP-IDF-Breakdown]] — base UART setup this builds on
- [[11-SAM2695-MIDI-Implementation]] — raw message reference (Note On/Off, CC0, Program Change)
- [[13-SAM2695-Drum-Sets]] — why channel 10 is special
- [[12-SAM2695-Sounds-GM-MT32]] — instrument/program numbers for `sam2695_set_ensemble_voice()`
