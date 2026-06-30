# PLANNING.md — Enssemble / "The Band"

## Project Vision

A set of wireless BLE MIDI synthesizer units deployed spatially around a room — each one a band member with its own voice, driven wirelessly from an iPhone. Sound comes from physically located objects rather than stereo speakers.

Each unit is a band member with a single-name identity: EEnoo, Jami, Kneel, Mowss, Bowee, Ryngo.

The firmware is built with **ESP-IDF** and targets two hardware platforms:
- **XIAO MIDI Synthesizer** (ESP32-C3 + SAM2695 hardware GM chip)
- **AMYboard** (ESP32-S3 + AMY software synth engine)

---

## Current Status

ESP-IDF firmware working on AMYboard. Two firmware variants ready. Three AMYboards available for spatial deployment. Mozaic iOS control surface replaces planned TouchOSC phase. Approaching MVP for first spatial composition.

### Firmware Variants
| Firmware | Directory | Purpose | Status |
|----------|-----------|---------|--------|
| NSMBL_Synth | `AmyBoard/NSMBL_Synth/` | AMY synth engine (Juno, DX7, drums) | ✅ Verified |
| NSMBL_SampleKits | `AmyBoard/NSMBL_SampleKits/` | 16-slice WAV sample player (SD card) | ✅ Verified |

Band member identity is set via `config.h` — copy from `AmyBoard/configs/` before building.

### iOS Control Surface (Mozaic)
| Script | Purpose | Status |
|--------|---------|--------|
| `NSMBL_EEnoo_Controller.moz` | Unified patch browser + sound shaper + panic | ✅ Working |
| `NSMBL_EEnoo_PatchSelect.moz` | Simple patch bank selector (superseded) | ✅ Working |
| `NSMBL_EEnoo_SoundShaper.moz` | CC knobs only (superseded) | ✅ Working |

### NSMBL CC Map (firmware, all channels)
| CC | Function | Range |
|----|----------|-------|
| 7 | Volume | 0-127 linear |
| 10 | Pan | 0=L, 64=center, 127=R |
| 70 | Filter Cutoff | Exponential ~13Hz-6400Hz |
| 71 | Resonance | Exponential Q 0.5-16 |
| 75/93 | Chorus Level | 0=dry, 127=full |
| 91 | Reverb Level | 0=dry, 127=full |

AMY handles internally: CC 0 (bank select), CC 64 (sustain), CC 123 (all notes off).

### The Band — Full Roster
*Names riff on famous musicians. Configs in `AmyBoard/configs/`.*

| Name | Inspiration | Channel | Default Patch | Role | Firmware | Status |
|------|-------------|---------|---------------|------|----------|--------|
| EEnoo | Brian Eno | 1 | Juno #47 Synth Pad | Pads/Ambient | NSMBL_Synth | ✅ Ready |
| Pryns | Prince | 2 | Juno #32 Lead I | Lead | NSMBL_Synth | ✅ Ready |
| Aufde | Melissa Auf der Maur | 3 | Juno #31 Synth Bass I | Bass | NSMBL_Synth | ✅ Ready |
| Kneel | Neal Peart | 10 | SD card kits | Drums | NSMBL_SampleKits | ✅ Ready |
| Dapht | Daft Punk | TBD | SD card samples | Samples/Vocal | TBD | 🔮 Future |

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

### Phase 3 — ~~TouchOSC~~ Mozaic Control Surface ✅
*Replaced TouchOSC plan with Mozaic iOS scripts — simpler, more flexible, already working.*

- [x] Patch browsing (prev/next per channel)
- [x] Sound shaping knobs (filter, resonance, reverb, chorus)
- [x] Volume/Pan mode toggle
- [x] Channel selector (Juno/DX7/Drums)
- [x] Sustain toggle
- [x] CC reset
- [x] MIDI Panic (all sound off, all channels)
- [ ] Multi-unit page — one Mozaic instance per band member

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
- AMY software synth engine (250 oscillators, dual-core rendering)
- I2S audio output (32-bit stereo via onboard DAC)
- 8MB octal PSRAM for samples and synth data
- SD card (SPI: MOSI=11, MISO=13, CLK=12, CS=10)
- I2S pins: MCLK=3, BCLK=8, WS=2, DOUT=6
- I2C port: SDA=17, SCL=18 (for Modulino accessories)
- MIDI out: GPIO 14 (Type A) / GPIO 15 (Type B)
- MIDI in: GPIO 21
- Line out: 3.5mm TRS stereo
- S/PDIF in/out, CV in/out (2x each)

### iOS BLE Limits
- iOS handles 13+ simultaneous BLE connections
- iPad Pro tested to 15 concurrent
- 4-5 band members is comfortably within limits
- Each unit is independent — no bandwidth competition

---

## Session Log

### 2026-06-30 — NSMBL_EEnoo + Mozaic Control Surface
- Integrated AMY synth engine as ESP-IDF component (250 oscillators)
- AMY handles I2S, dual-core rendering, and voice management internally
- Default synths: Juno-6 (ch1), DX7 (ch2), GM drums (ch10)
- All-channel MIDI pass-through (no channel filter)
- Reorganized project: `AmyBoard/NSMBL_SmplCty/` and `AmyBoard/NSMBL_EEnoo/`
- Custom NSMBL CC handler replacing AMY's Juno-only example — filter, resonance, reverb, chorus, volume, pan on all channels
- Mozaic iOS control surface: learned API from scratch, built 3 scripts
- Unified Controller script: patch browsing, sound shaping, sustain, panic
- Phase 3 (TouchOSC) rendered unnecessary by Mozaic approach
- Three AMYboards available for first spatial composition test

### 2026-06-29 — NSMBL_SmplCty (Sample Player) Working
- First ESP-IDF firmware from scratch — BLE MIDI + sample playback
- WAV cue point parser for 16-slice drum kits
- 4-voice polyphony with velocity scaling, I2S output
- SD card with FAT32 long filename support
- End-to-end verified: iPhone → AUM → BLE → 808 drums

### 2026-06-29 — ESP-IDF Migration Started
- Decision: move from Arduino to ESP-IDF for both XIAO and AMYboard platforms
- Updated all project documentation for new framework
- Arduino-era prototype code preserved in git history

### 2026-06-26 — Multi-Unit Success (Arduino era)
- Dual-unit operation achieved: EEnoo + Ryngo both connected to AUM
- Drums sequenced via Helium on channel 10
- This validated the spatial multi-unit concept
