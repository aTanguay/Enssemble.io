# PLANNING.md — XIAO BLE MIDI Bridge / "The Band"

## Project Vision

Transform the Seeed Studio XIAO MIDI Synthesizer from a standalone novelty into a
serious wireless GM sound module ecosystem. The end goal is **"The Band"** — a set of
3D printed character figures, each housing a XIAO board, a battery, and a speaker,
deployed around a room as a spatial wireless orchestra conducted from an iPhone.

Each unit is a band member with a single-name identity: EEnoo, Jami, Kneel, Mowss, Bowee, Ryngo.
No last name needed. Everyone knows who you mean.

---

## Current Status

**Two-unit band operational!** EEnoo (keys) and Ryngo (drums) both connected simultaneously
to AUM on iPhone, sequenced via Helium on channel 10 for drums.

### Sketch Structure
```
XIAO_BLE_MIDI_Bridge/   ← basic template (for reference)
XIAO_BLE_MIDI_EEnoo/    ← EEnoo (keys, ch 1, patch 0)
XIAO_BLE_MIDI_Ryngo/    ← Ryngo (drums, ch 10, patch 0)
```

### Active Band Members
| Name | Channel | Patch | Role | Boot Sound |
|------|---------|-------|------|------------|
| EEnoo | 1 | 0 | Keys/Piano | C major arpeggio |
| Ryngo | 10 | 0 | Drums | Kick-hat-snare-hat-kick-kick-crash |

### Planned Band Members (awaiting hardware)
| Name | Channel | Patch | Role |
|------|---------|-------|------|
| Jami | 2 | 80 | Lead |
| Kneel | 3 | 32 | Bass |
| Mowss | 4 | 88 | Pads |
| Bowee | 5 | 48 | Strings |

---

## What We Learned Getting Here

### Library Version Matrix (CRITICAL)
The working combination that actually compiles and runs on XIAO ESP32-C3:

| Component | Version | Notes |
|-----------|---------|-------|
| esp32 by Espressif | **2.0.17** | 3.x core crashes on BLE init with NimBLE 1.4.2 |
| NimBLE-Arduino | **1.4.2** | 2.x restructured internals, incompatible with lathoub |
| BLE-MIDI by lathoub | latest | Use lathoub, NOT max22/ESP32-BLE-MIDI |
| MIDI Library by lathoub | latest | Required companion to BLE-MIDI |

### Library Pitfalls Encountered
- **max22/ESP32-BLE-MIDI** uses `BLEMidiServer` API — crashes on ESP32 core 3.x
- **lathoub/BLE-MIDI** uses `BLEMIDI_Transport.h` + `BLEMIDI_ESP32_NimBLE.h` — correct choice
- **NimBLE 2.x** completely restructured internals — incompatible with lathoub library
- **ESP32 core 3.x** changed BLE stack defaults — causes null pointer crash with older NimBLE
- **LED_BUILTIN** not defined on XIAO ESP32-C3 — must `#define LED_BUILTIN 10`
- **Both .ino files in same folder** — Arduino compiles all .ino files together, causes redefinition errors
- **PitchBend callback signature** differs between libraries — lathoub passes combined `int`, max22 passes LSB/MSB bytes

### Future Library Option
**ESP32_Host_MIDI by sauloverissimo** (Feb 2026) — designed for ESP32 core 3.x, clean API,
supports BLE MIDI with iOS/AUM. Worth investigating to eliminate version downgrade requirement
for new users.

### BLE MIDI API (lathoub style)
```cpp
#include <BLEMIDI_Transport.h>
#include <hardware/BLEMIDI_ESP32_NimBLE.h>
BLEMIDI_CREATE_INSTANCE("Device Name", MIDI)

// In setup():
MIDI.begin(MIDI_CHANNEL);  // 1-16 for channel filtering, or MIDI_CHANNEL_OMNI
MIDI.setHandleNoteOn(handleNoteOn);
MIDI.setHandleNoteOff(handleNoteOff);
MIDI.setHandleProgramChange(handleProgramChange);
MIDI.setHandleControlChange(handleControlChange);
MIDI.setHandlePitchBend(handlePitchBend);
MIDI.setHandleAfterTouchChannel(handleAfterTouch);
BLEMIDI.setHandleConnected([]() { ... });
BLEMIDI.setHandleDisconnected([]() { ... });

// In loop():
MIDI.read(); // REQUIRED every loop
```

### Per-Unit Configuration
Each unit has these defines at the top of its sketch:
```cpp
#define DEVICE_NAME    "EEnoo"      // BLE name shown in AUM
#define MIDI_CHANNEL   1            // 1-16, unit only responds to this channel
#define DEFAULT_PATCH  0            // GM program number
#define DEFAULT_BANK   0            // 0=GM, 127=MT-32
```

### Drum Channel Notes
- MIDI channel 10 = SAM2695 channel 9 (0-indexed) = always drums in GM
- Program changes on channel 10 select drum kits, not melodic instruments
- Ryngo routes ALL incoming notes to LOCAL_CH (9) regardless of incoming MIDI channel
- Drum volume was very low by default — explicitly set to 127 in setup

### iOS BLE Limits
- iOS handles 13+ simultaneous BLE connections in real-world testing
- iPad Pro tested to 15 concurrent connections
- 4–5 band members is comfortably within limits
- Each unit is an independent BLE peripheral — no bandwidth competition
- Latency: 3–15ms per unit — fine for performance

---

## Phases

### Phase 1 — Core BLE MIDI Bridge ✅ COMPLETE
*Proof of concept: wireless BLE MIDI from iPhone to SAM2695*

- [x] BLE MIDI peripheral firmware (lathoub + NimBLE 1.4.2 + ESP32 core 2.0.17)
- [x] Note On / Off → SAM2695
- [x] Program Change → SAM2695
- [x] Pitch Bend → SAM2695 (raw MIDI bytes via Serial0)
- [x] CC passthrough (volume, pan, expression, sustain, mod, portamento)
- [x] Effects CC: reverb type/send, chorus type/send (CC 80, 81, 91, 93)
- [x] Onboard button voice navigation (category jump + fine step)
- [x] Long-press buttons for volume control
- [x] Per-unit DEVICE_NAME / DEFAULT_PATCH config at top of sketch
- [x] Serial Monitor feedback with instrument names
- [x] BLE connected/disconnected callbacks with LED feedback
- [x] Confirmed working: AUM connects, sequencer drives it, audio confirmed

---

### Phase 2a — Multi-Unit Foundation ✅ COMPLETE
*Multiple units with proper MIDI channel separation*

- [x] Per-unit MIDI channel filtering (`#define MIDI_CHANNEL`)
- [x] Per-unit device naming (`#define DEVICE_NAME`)
- [x] Separate sketch folders per band member
- [x] Boot sounds — each unit announces itself on power-up
- [x] Full CC passthrough (effects, pan, expression, bank select)
- [x] Aftertouch passthrough
- [x] MT-32 bank toggle (long-press button 0)
- [x] MIDI panic (long-press button 3)
- [x] Dual-unit test successful — EEnoo + Ryngo connected simultaneously
- [x] Drums working on channel 10 with Helium sequencer in AUM

---

### Phase 2b — Button UX & Standalone Playability
*Make the unit genuinely usable without a phone — plug in, press buttons, make music*

**Audio feedback on voice change**
- Play a short audition note (middle C, ~200ms, medium velocity) whenever voice changes
- The instrument announces itself by playing — no screen, no Serial Monitor needed
- This is the key UX insight: sound IS the feedback

**Curated preset system**
- Instead of raw 128-voice GM navigation, define 8 band-appropriate presets per unit type
- Bass unit presets: Acoustic Bass, Finger Bass, Fretless Bass, Slap Bass, Synth Bass 1/2, Pick Bass, Contrabass
- Pad unit presets: Pad 1-8 (all eight SAM2695 pad voices)
- Lead unit presets: Lead 1-8 (all eight synth lead voices)
- Strings unit: Violin, Viola, Cello, String Ensemble 1/2, Synth Strings 1/2, Tremolo
- Short press cycles through curated list — long press jumps GM categories for power users

**Favorite slots**
- Hold button 0: save current voice to slot A
- Hold button 1: save current voice to slot B
- Short press 0/1: recall slot A/B instantly
- Stored in NVS flash — persists across power cycles

**Stability**
- [x] MIDI channel lock — `#define MIDI_CHANNEL` implemented
- [x] Startup boot sequence — signature phrase on power-on
- [ ] Reconnection handling — auto re-advertise after iOS disconnects
- [ ] Watchdog timer — recover from BLE stack hangs
- [ ] Velocity curve — SOFT / LINEAR / HARD options

---

### Phase 3 — TouchOSC Control Surface
*A dedicated iOS editor/performance panel — the advanced level*

- [ ] Design TouchOSC layout for full SAM2695 control
- [ ] Instrument picker — category grid → instrument grid → sends PC
- [ ] Per-unit channel strip: volume, pan, expression, instrument name
- [ ] Effects panel: reverb type (8 buttons), reverb send, chorus type (8 buttons), chorus send
- [ ] Performance pads — chord triggers, velocity layers
- [ ] Multi-unit page — one tab per band member, each routes to different BLE destination
- [ ] AUM session template documentation

---

### Phase 4 — SysEx & Deep SAM2695 Control
*Unlock chip features not reachable via standard MIDI CC*

- [ ] Research SAM2695 SysEx (Dream Audio "port write" commands)
- [ ] 4-band stereo EQ (L/R separate, ±12dB per band)
- [ ] Master volume / master tuning via SysEx
- [ ] SysEx passthrough handler in firmware

---

### Phase 5 — "The Band" Physical System
*The room as an instrument — 3D printed band members*

**Enclosure design**
- Abstract sculptural forms — NOT literal instrument shapes
- Textile/fabric aesthetic — cloth sim draped forms, knit texture displacement, pillow-like geometry
- Workflow: Blender cloth sim → freeze → export → Bambu P1P
- CR-Scan Otter can scan actual fabric for real displacement data
- Silk PLA reads like draped satin; matte PLA reads like wool/felt
- Each band member a different draped form — tall/narrow vs short/wide etc.

**Electronics integration**
- Small LiPo inside figure or flat puck under base
- USB-C port for charging, hidden discreetly in design
- Power switch in base or tactile button that doubles as on/off
- 2-3" 4-8Ω speaker replacing onboard speaker, housed in acoustic chamber
- Speaker grille as design feature — geometry tuned per instrument type
- Bass unit: larger port, bass-forward chamber
- Lead unit: smaller, brighter chamber

**Speaker upgrade**
- Unsolder onboard speaker, wire to larger driver
- Enclosure shape IS the speaker cabinet — design acoustics intentionally
- Onboard Class-D amp can drive a larger speaker directly

**Deployment**
- Power on → boots → plays signature phrase → advertises → AUM connects
- No setup required after initial flash
- Place anywhere in room — battery = fully wireless
- AUM session template pre-routed for N band members

---

## Architecture

```
iPhone (AUM + Helium / TouchOSC)
        │
        │ BLE MIDI 1.0 — one connection per unit
        │ iOS confirmed: 13+ simultaneous connections possible
        │ Latency: 3–15ms per unit
        │
        ├── Channel 1 ──→ EEnoo (Keys)
        ├── Channel 2 ──→ Jami (Lead) [planned]
        ├── Channel 3 ──→ Kneel (Bass) [planned]
        ├── Channel 4 ──→ Mowss (Pads) [planned]
        ├── Channel 5 ──→ Bowee (Strings) [planned]
        └── Channel 10 ─→ Ryngo (Drums)
                │
                ▼
ESP32-C3 (XIAO MIDI Synthesizer)
  ├── BLE MIDI stack
  │     lathoub BLEMIDI_Transport
  │     + BLEMIDI_ESP32_NimBLE
  │     + NimBLE-Arduino 1.4.2
  │     + ESP32 core 2.0.17
  ├── MIDI channel filter (MIDI.begin(MIDI_CHANNEL))
  ├── Boot sound on startup
  ├── Button handler (4 buttons, short + long press)
  │     ├── Voice/kit navigation
  │     ├── Volume control
  │     ├── MT-32 bank toggle
  │     └── MIDI panic
  ├── CC router → SAM2695 via Serial0 @ 31250 baud
  └── SAM2695 synth chip
        ├── 64-voice polyphony
        ├── 128 GM programs (Bank 0)
        ├── MT-32 soundset (Bank 127)
        ├── Drums on channel 10 (9 in 0-indexed)
        ├── Reverb (8 types, CC 80/91)
        ├── Chorus (8 types, CC 81/93)
        ├── 4-band stereo EQ (SysEx — Phase 4)
        ├── 3.5mm stereo out
        └── Onboard Class-D amp + speaker
```

---

## Technical Notes

### SAM2695 Quirks
- Needs ~500-600ms after power-up before accepting MIDI — handled by startup delay
- Pitch Bend must be sent as raw MIDI bytes — Seeed library doesn't expose it
- Bank Select (CC 0) must precede Program Change to take effect
- Channel indexing: BLE MIDI uses 1–16, SAM2695 library uses 0–15
- Drum channel volume defaults low — explicitly set to 127

### Hardware Notes
- Button pins (active LOW, internal pullup): GPIO 5, 4, 3, 2
- SAM2695 on Serial0 (hardware UART) at 31250 baud
- LED_BUILTIN = GPIO 10, active LOW
- USB-C for power and flashing
- 3.5mm stereo out for external audio

### GM Drum Map (Channel 10)
| Note | Sound |
|------|-------|
| 36 (C2) | Bass Drum 1 |
| 38 (D2) | Acoustic Snare |
| 42 (F#2) | Closed Hi-Hat |
| 46 (A#2) | Open Hi-Hat |
| 49 (C#3) | Crash Cymbal |
| 51 (D#3) | Ride Cymbal |

### Drum Kits (Program Change on Channel 10)
| Patch | Kit |
|-------|-----|
| 0 | Standard |
| 8 | Room |
| 16 | Power |
| 24 | Electronic |
| 25 | TR-808 |
| 32 | Jazz |
| 40 | Brush |
| 48 | Orchestra |
| 56 | SFX |

---

## Session Log

### 2026-06-26 — Multi-Unit Success
- Fixed "Full" version that was using wrong BLE library (max22 instead of lathoub)
- Added per-unit MIDI channel filtering
- Created separate sketch folders: EEnoo, Ryngo
- Added boot sounds (piano arpeggio for EEnoo, drum beat for Ryngo)
- Fixed drum channel volume issue (was too low)
- Successfully tested dual-unit operation: EEnoo + Ryngo both connected to AUM
- Drums sequenced via Helium on channel 10
- More units ordered!
