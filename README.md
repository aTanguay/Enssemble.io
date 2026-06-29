# Enssemble — BLE MIDI Spatial Synth Firmware
### "The Band"

Custom ESP-IDF firmware for wireless BLE MIDI synthesizer units, designed for **spatial audio deployment** — multiple synth units placed around a room, each a different instrument, all driven wirelessly from an iPhone.

The project supports two hardware platforms:

| Platform | MCU | Synth Engine | Notes |
|----------|-----|-------------|-------|
| [Seeed Studio XIAO MIDI Synthesizer](https://wiki.seeedstudio.com/xiao_midi_synthesizer/) | ESP32-C3 | SAM2695 (hardware GM chip) | Original platform, ~$15/unit |
| [AMYboard](https://github.com/shorepine/amyboard) | ESP32-S3 | AMY (software synth engine) | Open-source, more flexible |

Both platforms use **ESP-IDF** (not Arduino) and share the same BLE MIDI transport layer.

## What It Does

- Advertises as a BLE MIDI 1.0 peripheral — connects to iOS with no pairing
- Receives Note On/Off, Program Change, Pitch Bend, and all key CCs
- Per-unit MIDI channel filtering — each band member responds only to its assigned channel
- Boot sounds — each unit plays a signature phrase on power-up
- Onboard button navigation for voice selection
- Designed for multi-unit spatial deployment

## The Concept

Multiple units deployed around a room. Each one is a band member — a different instrument, a different voice, placed wherever sounds right spatially. The iPhone is the bandleader. Sound comes from physically located objects rather than stereo speakers.

Units are named like band members. One name. No last name needed.

**EEnoo. Jami. Kneel. Mowss. Bowee. Ryngo.**

## The Band Roster

| Name | Channel | Default Patch | Role | Boot Sound |
|------|---------|---------------|------|------------|
| **EEnoo** | 1 | 0 (Grand Piano) | Keys | C major arpeggio |
| **Ryngo** | 10 | 0 (Standard Kit) | Drums | Kick-hat-snare beat |
| Jami | 2 | 80 (Lead 1 Square) | Lead | *planned* |
| Kneel | 3 | 32 (Acoustic Bass) | Bass | *planned* |
| Mowss | 4 | 88 (Pad 1 Fantasia) | Pads | *planned* |
| Bowee | 5 | 48 (String Ensemble) | Strings | *planned* |
| **SmplCty** | TBD | — | Sample Player | *in progress* |

## Building

Requires [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/) (v5.x recommended).

```bash
idf.py set-target esp32c3   # or esp32s3 for AMYboard
idf.py build
idf.py -p <PORT> flash monitor
```

## Hardware

### XIAO MIDI Synthesizer (ESP32-C3 + SAM2695)
- SAM2695 hardware GM synth chip — 64-voice polyphony, 128 GM programs, MT-32 bank
- Button pins (active LOW, internal pullup): GPIO 5, 4, 3, 2
- SAM2695 on UART at 31250 baud
- LED on GPIO 10, active LOW
- 3.5mm stereo out + onboard Class-D amp and speaker

### AMYboard (ESP32-S3 + AMY)
- AMY software synth engine — open-source, extensible
- ESP32-S3 with more RAM and processing headroom
- I2S audio output

## iOS Setup (AUM)

1. Power on units — each plays its boot sound
2. In AUM, tap MIDI plug icon, connect via Bluetooth MIDI
3. Create a track per band member, route MIDI to the corresponding unit and channel
4. Use a sequencer (Helium, etc.) to drive the ensemble

iOS handles 13+ simultaneous BLE connections — 4-6 band members is well within range.

## CC Reference

| CC | Parameter | Range |
|----|-----------|-------|
| 0 | Bank Select | 0=GM, 127=MT-32 |
| 1 | Modulation | 0-127 |
| 7 | Channel Volume | 0-127 |
| 10 | Pan | 0=L, 64=C, 127=R |
| 11 | Expression | 0-127 |
| 64 | Sustain Pedal | 0=off, 127=on |
| 80 | Reverb Type | 0-7 |
| 81 | Chorus Type | 0-7 |
| 91 | Reverb Send | 0-127 |
| 93 | Chorus Send | 0-127 |
| 121 | Reset All Controllers | 0 |
| 123 | All Notes Off | 0 |

## License

MIT
