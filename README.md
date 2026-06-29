# XIAO MIDI Synthesizer — BLE MIDI Bridge
### "The Band"

A custom firmware for the [Seeed Studio XIAO MIDI Synthesizer](https://wiki.seeedstudio.com/xiao_midi_synthesizer/) (ESP32-C3 + SAM2695) that enables **wireless BLE MIDI input from iOS**, transforming the board from a standalone toy into a fully controllable GM sound module — driveable from AUM, Helium, TouchOSC, GarageBand, or any BLE MIDI-capable app on iPhone/iPad.

Out of the box the XIAO MIDI Synthesizer has no MIDI input at all, despite containing a fully-featured 64-voice GM synth chip. This firmware fixes that — and nobody had done it before.

## Current Status

**Two-unit band operational!** EEnoo (keys, channel 1) and Ryngo (drums, channel 10) both connected simultaneously to AUM on iPhone.

## What It Does

- Advertises as a BLE MIDI 1.0 peripheral — connects to iOS with no pairing ritual
- Receives Note On/Off, Program Change, Pitch Bend, and all key CCs
- **Per-unit MIDI channel filtering** — each band member responds only to its assigned channel
- Full SAM2695 effects control: reverb type/send, chorus type/send, pan, expression, sustain, mod
- MT-32 bank switching (128 GM sounds + MT-32 soundset)
- Onboard button voice navigation — jump by GM category or step individual voices
- **Boot sounds** — each unit plays a signature phrase on power-up
- Long-press buttons for volume and MIDI panic
- Serial Monitor feedback — instrument names, CC values, connection status
- Designed for **multi-unit spatial deployment** — run several units around a room, each a different instrument, all driven from a single iPhone

## The Concept

Multiple units deployed around a room. Each one is a band member — a different instrument, a different voice, placed wherever sounds right spatially. The iPhone is the bandleader. Sound comes from physically located objects rather than stereo speakers.

Units are named like band members. One name. No last name needed.

**EEnoo. Jami. Kneel. Mowss. Bowee. Ryngo.**

Each one is a 3D printed figure housing the board, a battery pack, and an upgraded speaker. Completely wireless. Place them anywhere. Power on. Connect in AUM. Play.

## The Band Roster

| Name | Channel | Default Patch | Role | Boot Sound |
|------|---------|---------------|------|------------|
| **EEnoo** | 1 | 0 (Grand Piano) | Keys | C major arpeggio |
| **Ryngo** | 10 | 0 (Standard Kit) | Drums | Kick-hat-snare beat |
| Jami | 2 | 80 (Lead 1 Square) | Lead | *planned* |
| Kneel | 3 | 32 (Acoustic Bass) | Bass | *planned* |
| Mowss | 4 | 88 (Pad 1 Fantasia) | Pads | *planned* |
| Bowee | 5 | 48 (String Ensemble) | Strings | *planned* |

## Sketch Structure

Each band member has their own sketch folder:

```
XIAO_BLE_MIDI_Bridge/   ← basic template
XIAO_BLE_MIDI_EEnoo/    ← EEnoo (keys, ch 1)
XIAO_BLE_MIDI_Ryngo/    ← Ryngo (drums, ch 10)
```

## Hardware

- [Seeed Studio XIAO MIDI Synthesizer](https://www.seeedstudio.com/) (~$15 per unit)
- USB-C cable for flashing
- iPhone/iPad with AUM, Helium, TouchOSC, GarageBand, or any BLE MIDI app
- Optional: small LiPo battery + USB-C charge board for wireless power
- Optional: 2-3" 4-8Ω speaker to replace onboard speaker

## Quick Start

1. Install required libraries (see Critical Library Versions below)
2. Open the sketch folder for your band member (e.g., `XIAO_BLE_MIDI_EEnoo/`)
3. Select board: Tools → Board → esp32 → XIAO_ESP32C3
4. Flash
5. Open AUM, tap MIDI plug icon → Bluetooth MIDI → connect

Each unit will play its boot sound when powered on.

## Per-Unit Configuration

At the top of each sketch:

```cpp
#define DEVICE_NAME    "EEnoo"      // BLE name shown in AUM
#define MIDI_CHANNEL   1            // 1-16, unit only responds to this channel
#define DEFAULT_PATCH  0            // GM program number
#define DEFAULT_BANK   0            // 0=General MIDI, 127=MT-32
```

## Critical Library Versions

This firmware requires specific library versions — do not use latest:

| Library | Version | Source |
|---------|---------|--------|
| esp32 by Espressif | **2.0.17** | Arduino Boards Manager |
| NimBLE-Arduino by h2zero | **1.4.2** | Arduino Library Manager |
| BLE-MIDI by lathoub | latest | Arduino Library Manager |
| MIDI Library by lathoub | latest | Arduino Library Manager |
| Seeed_Arduino_MIDIMaster | latest | Manual ZIP install |

⚠️ ESP32 core 3.x + NimBLE 1.4.2 causes a crash on boot. Stay on core **2.0.17**.

## Button Map

### Melodic Units (EEnoo, Jami, etc.)

| Button | Short Press | Long Press |
|--------|------------|------------|
| 0 | Next category | Toggle GM/MT-32 bank |
| 1 | Previous category | Volume up (+10) |
| 2 | Next instrument (+1) | Volume down (-10) |
| 3 | Previous instrument (-1) | MIDI panic |

### Drum Unit (Ryngo)

| Button | Short Press | Long Press |
|--------|------------|------------|
| 0 | Next drum kit | (reserved) |
| 1 | Previous drum kit | Volume up (+10) |
| 2 | Next kit (+1) | Volume down (-10) |
| 3 | Previous kit (-1) | MIDI panic |

## CC Reference (send from AUM / TouchOSC)

| CC | Parameter | Range |
|----|-----------|-------|
| 0 | Bank Select | 0=GM, 127=MT-32 |
| 1 | Modulation | 0–127 |
| 7 | Channel Volume | 0–127 |
| 10 | Pan | 0=L, 64=C, 127=R |
| 11 | Expression | 0–127 |
| 64 | Sustain Pedal | 0=off, 127=on |
| 80 | Reverb Type | 0–7 (Room1–3, Hall1–2, Plate, Delay, Pan Delay) |
| 81 | Chorus Type | 0–7 (Chorus1–4, FB Chorus, Flanger, Short Delay, FB Delay) |
| 91 | Reverb Send | 0–127 |
| 93 | Chorus Send | 0–127 |
| 121 | Reset All Controllers | 0 |
| 123 | All Notes Off | 0 |

## GM Instrument Categories

Each category contains 8 programs. Jump between them with button navigation.

| # | Category | Programs |
|---|----------|---------|
| 0 | Piano | 0–7 |
| 1 | Chromatic Perc | 8–15 |
| 2 | Organ | 16–23 |
| 3 | Guitar | 24–31 |
| 4 | Bass | 32–39 |
| 5 | Strings | 40–47 |
| 6 | Ensemble | 48–55 |
| 7 | Brass | 56–63 |
| 8 | Reed | 64–71 |
| 9 | Pipe | 72–79 |
| 10 | Synth Lead | 80–87 |
| 11 | Synth Pad | 88–95 |
| 12 | Synth FX | 96–103 |
| 13 | Ethnic | 104–111 |
| 14 | Percussive | 112–119 |
| 15 | Sound FX | 120–127 |

## Drum Kits (Channel 10)

Program changes on channel 10 select drum kits:

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

## Multi-Unit iOS BLE Limits

Real-world testing confirms iOS handles 13+ simultaneous BLE connections. 4–5 band members is well within range. Each unit appears as a separate MIDI destination in AUM automatically.

## AUM Setup

1. Connect all units via Bluetooth MIDI
2. Create a track for each band member
3. Set each track's MIDI output to the corresponding unit and channel:
   - EEnoo → Channel 1
   - Ryngo → Channel 10
   - etc.
4. Use Helium or another sequencer to drive the drums on channel 10

## License

MIT
