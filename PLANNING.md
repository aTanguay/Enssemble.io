# PLANNING.md — Enssemble / "The Band"

## Project Vision

A set of wireless BLE MIDI synthesizer units deployed spatially around a room — each one a band member with its own voice, driven wirelessly from an iPhone. Sound comes from physically located objects rather than stereo speakers.

Each unit is a band member with a single-name identity: EEnoo, Jami, Kneel, Mowss, Bowee, Ryngo.

The firmware is built with **ESP-IDF** and targets two hardware platforms:
- **XIAO MIDI Synthesizer** (ESP32-C3 + SAM2695 hardware GM chip)
- **AMYboard** (ESP32-S3 + AMY software synth engine)

---

## Current Status

Rewriting firmware from Arduino to ESP-IDF. Prior Arduino-based prototype achieved two-unit band operation (EEnoo + Ryngo) — see git history for the Arduino-era codebase.

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
| **SmplCty** | TBD | — | Sample Player (AMYboard) |

---

## Architecture

```
iPhone (AUM + Helium / TouchOSC)
        |
        | BLE MIDI 1.0 — one connection per unit
        | iOS confirmed: 13+ simultaneous connections possible
        | Latency: 3-15ms per unit
        |
        +-- Channel 1 --> EEnoo (Keys)
        +-- Channel 2 --> Jami (Lead) [planned]
        +-- Channel 3 --> Kneel (Bass) [planned]
        +-- Channel 4 --> Mowss (Pads) [planned]
        +-- Channel 5 --> Bowee (Strings) [planned]
        +-- Channel 10 -> Ryngo (Drums)
                |
                v
AMYboard (ESP32-S3, dual-core):
  Core 0:
    +-- NimBLE BLE MIDI stack
    +-- MIDI message parsing & channel filter
    +-- Connection management
    +-- Button handler
  Core 1:
    +-- AMY synth engine (audio rendering)
    +-- I2S DAC output
  Shared (FreeRTOS queue between cores):
    +-- MIDI events from Core 0 -> Core 1
    +-- Per-unit config (device name, channel, patch)
    +-- Boot sound on startup

XIAO (ESP32-C3, single-core — future):
  +-- BLE MIDI stack (NimBLE)
  +-- SAM2695 via UART @ 31250 baud
  +-- 3.5mm stereo + onboard Class-D amp
```

---

## Phases

### Phase 1 — AMYboard BLE MIDI + Sample Player
*AMYboard first. Get BLE MIDI receiving on one core, AMY playing on the other.*

- [ ] ESP-IDF project structure with CMake (target: ESP32-S3)
- [ ] NimBLE BLE MIDI peripheral (advertise, connect, receive MIDI) — pinned to Core 0
- [ ] MIDI message parsing (Note On/Off, CC, Program Change, Pitch Bend)
- [ ] AMY synth engine integration — pinned to Core 1
- [ ] FreeRTOS queue for MIDI events between cores
- [ ] Simple sample player exposing AMY's PCM playback via MIDI notes
- [ ] I2S audio output configuration
- [ ] Per-unit configuration (device name, MIDI channel, default patch)
- [ ] Boot sound on startup
- [ ] Verified working: AUM connects, notes trigger samples, audio out confirmed

### Phase 2 — Multi-Unit & UX (AMYboard)
*Multiple AMYboard units with proper channel separation and standalone usability*

- [ ] Per-unit MIDI channel filtering
- [ ] Full CC passthrough (volume, pan, expression, sustain, mod, effects)
- [ ] Pitch Bend and Aftertouch passthrough
- [ ] Voice/patch selection via AMY
- [ ] Curated preset system per band role
- [ ] Audition note on voice change
- [ ] Favorite voice slots stored in NVS flash
- [ ] Reconnection handling — auto re-advertise after iOS disconnects
- [ ] Button input handler
- [ ] Multi-unit simultaneous operation verified

### Phase 2b — XIAO Platform Port
*Bring the proven BLE MIDI layer to the XIAO ESP32-C3 + SAM2695*

- [ ] SAM2695 UART driver component
- [ ] Adapt single-core architecture (C3 has one core)
- [ ] Validate same BLE MIDI layer works on both platforms

### Phase 3 — TouchOSC Control Surface
*A dedicated iOS editor/performance panel*

- [ ] Design TouchOSC layout for full control
- [ ] Instrument picker — category grid -> instrument grid -> sends PC
- [ ] Per-unit channel strip: volume, pan, expression, instrument name
- [ ] Effects panel: reverb type/send, chorus type/send
- [ ] Multi-unit page — one tab per band member

### Phase 4 — Deep Synth Control
*Platform-specific advanced features*

- [ ] XIAO/SAM2695: SysEx for 4-band stereo EQ, master volume/tuning
- [ ] AMYboard/AMY: Custom patches, FM synthesis, filter control
- [ ] I2C encoder/OLED UI for physical control (AMYboard)

### Phase 5 — "The Band" Physical System
*3D printed band members for room-scale deployment*

- Enclosure design — abstract sculptural forms, textile/fabric aesthetic
- LiPo battery integration for fully wireless operation
- Upgraded speakers housed in acoustic chambers
- Power on -> boot sound -> advertise -> AUM connects -> play

---

## Technical Notes

### BLE MIDI
- BLE MIDI 1.0 spec over NimBLE (ESP-IDF component, not Arduino library)
- Each unit is an independent BLE peripheral
- iOS handles 13+ simultaneous connections
- Latency: 3-15ms per unit

### SAM2695 (XIAO platform)
- Needs ~500-600ms after power-up before accepting MIDI
- Pitch Bend must be sent as raw MIDI bytes
- Bank Select (CC 0) must precede Program Change
- Channel indexing: BLE MIDI uses 1-16, SAM2695 uses 0-15
- Drum channel (10) volume defaults low — set explicitly

### Hardware — XIAO ESP32-C3
- Button pins (active LOW, internal pullup): GPIO 5, 4, 3, 2
- SAM2695 on Serial UART at 31250 baud
- LED = GPIO 10, active LOW

### Hardware — AMYboard ESP32-S3
- AMY software synth engine
- I2S audio output
- More RAM and processing headroom than C3

### iOS BLE Limits
- iOS handles 13+ simultaneous BLE connections
- iPad Pro tested to 15 concurrent
- 4-5 band members is comfortably within limits
- Each unit is independent — no bandwidth competition

---

## Session Log

### 2026-06-29 — ESP-IDF Migration Started
- Decision: move from Arduino to ESP-IDF for both XIAO and AMYboard platforms
- Updated all project documentation for new framework
- Arduino-era prototype code preserved in git history

### 2026-06-26 — Multi-Unit Success (Arduino era)
- Dual-unit operation achieved: EEnoo + Ryngo both connected to AUM
- Drums sequenced via Helium on channel 10
- This validated the spatial multi-unit concept
