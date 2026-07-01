# Enssemble — BLE MIDI Spatial Synth Firmware
### "The Band"

Custom ESP-IDF firmware for wireless BLE MIDI synthesizer units, designed for **spatial audio deployment** — multiple synth units placed around a room, each a different instrument, all driven wirelessly from an iPhone.

## What It Does

- Each unit advertises as a **BLE MIDI 1.0 peripheral** — connects to iOS with no pairing
- Receives Note On/Off, Program Change, Pitch Bend, and CCs
- Per-unit MIDI channel filtering — each band member responds only to its assigned channel
- Real-time sound shaping via standard MIDI CCs (filter, reverb, chorus, volume, pan)
- Two platforms: **AMYboard** (ESP32-S3 + AMY software synth) and **XIAO** (ESP32-C3 + SAM2695 hardware GM wavetable)
- AMYboard firmware variants: **AMY synth** (256 patches) and **sample player** (WAV kits from SD)
- iOS control surface via **Mozaic** scripts

## The Concept

Multiple units deployed around a room. Each one is a band member — a different instrument, a different voice, placed wherever sounds right spatially. The iPhone is the bandleader. Sound comes from physically located objects rather than stereo speakers.

Units are named after famous musicians. One name. No last name needed.

## The Band

| Name | Inspiration | Ch | Role | Default Patch | Firmware |
|------|-------------|-----|------|---------------|----------|
| **EEnoo** | Brian Eno | 1 | Pads/Ambient | Juno Synth Pad | AMY synth |
| **Pryns** | Prince | 2 | Lead | Juno Lead I | AMY synth |
| **Aufde** | Melissa Auf der Maur | 3 | Bass | Juno Synth Bass I | AMY synth |
| **Kneel** | Neal Peart | 10 | Drums | 808 kit (SD card) | Sample player |
| **Dapht** | Daft Punk | TBD | Samples | TBD | Future |

Named configs in `AmyBoard/configs/` — copy to `main/config.h` before flashing each board.

## Hardware

### AMYboard (ESP32-S3 + AMY)
- [AMYboard](https://github.com/shorepine/amyboard) — open-source, $29
- AMY software synth engine — 250 oscillators, 128 Juno-6 + 128 DX7 patches, GM drums
- ESP32-S3 dual-core: BLE MIDI on core 0, audio rendering on core 1
- 8MB octal PSRAM, SD card, I2S line out (3.5mm TRS stereo)
- I2C port for accessories, MIDI in/out (TRS), CV in/out, S/PDIF

### Seeed XIAO ESP32-C3 + SAM2695
- XIAO ESP32-C3 — $5, tiny form factor, single-core RISC-V
- Dream SAM2695 — hardware GM wavetable synth, 128 GM patches + drum kits, 64-voice polyphony
- UART MIDI at 31250 baud, full CC/NRPN passthrough (filter, envelope, vibrato, reverb/chorus types)
- 3.5mm stereo line out + onboard Class-D amplifier

## Firmware Variants

### NSMBL_Synth — AMY Synth (`AmyBoard/NSMBL_Synth/`)
Full software synthesizer using the AMY engine. Default synths: Juno-6 (ch1), DX7 (ch2), GM drums (ch10). Supports program change to browse all 256 patches. Custom NSMBL CC handler for real-time sound shaping. Used by EEnoo, Pryns, and Aufde.

### NSMBL_SampleKits — Sample Player (`AmyBoard/NSMBL_SampleKits/`)
16-slice drum sample player. Reads WAV files with cue point markers from SD card. 4-voice polyphony, velocity-sensitive, chromatic mapping from C2. Used by Kneel.

### NSMBL_Synth — SAM2695 (`SeeedXiaoMIDI/NSMBL_Synth/`)
Hardware GM wavetable synth via SAM2695 chip. 128 GM patches, full CC passthrough, NRPN support for filter cutoff/resonance, envelope, and vibrato. BLE MIDI parser with running status support. See `Docs/SAM2695_MIDI_Reference.md` for full CC/NRPN map.

## Building

Requires [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/) v5.x.

```bash
# Source ESP-IDF
source ~/esp/esp-idf/export.sh

# Copy band member config, then build and flash
cp AmyBoard/configs/config_Pryns.h AmyBoard/NSMBL_Synth/main/config.h
cd AmyBoard/NSMBL_Synth
idf.py build
idf.py -p /dev/cu.usbmodem* flash monitor
```

## iOS Setup

1. Power on units — each plays a startup bleep
2. Open **AUM**, tap MIDI plug icon, connect each unit via Bluetooth MIDI
3. Route MIDI channels to the corresponding units
4. Play with a keyboard app, sequencer (Helium), or Mozaic control surface

iOS handles 13+ simultaneous BLE connections — the full band is well within range.

## Mozaic Control Surface

iOS scripts in `Mozaic/` for the [Mozaic](https://ruismaker.com) MIDI scripting app:

| Script | Purpose |
|--------|---------|
| **NSMBL_EEnoo_Controller.moz** | AMYboard: patch browsing, filter/reso/reverb/chorus knobs, vol/pan mode, sustain, MIDI panic |
| **NSMBL_XIAO_Controller.moz** | XIAO/SAM2695: Mix layout — XY filter pad, NRPNs, named reverb/chorus types, envelope, vibrato |
| NSMBL_EEnoo_PatchSelect.moz | Simple patch bank selector |
| NSMBL_EEnoo_SoundShaper.moz | CC knobs only |

## NSMBL CC Map

Standard MIDI CCs handled by the firmware on all channels. Works with any MIDI controller.

| CC | Parameter | Mapping |
|----|-----------|---------|
| 7 | Volume | Linear 0-127 |
| 10 | Pan | 0=Left, 64=Center, 127=Right |
| 70 | Filter Cutoff | Exponential ~13Hz-6400Hz |
| 71 | Resonance | Exponential Q 0.5-16 |
| 75/93 | Chorus Level | Linear 0=dry, 127=full |
| 91 | Reverb Level | Linear 0=dry, 127=full |

Handled by AMY internally: CC 0 (bank select), CC 64 (sustain), CC 123 (all notes off).

## License

MIT
