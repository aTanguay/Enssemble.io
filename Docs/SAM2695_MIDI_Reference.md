# SAM2695 MIDI Reference

Quick reference for all useful MIDI controls on the Dream SAM2695 wavetable synth chip.
Extracted from [SAM2695.pdf](SAM2695.pdf).

---

## Standard MIDI CCs

| CC | Hex | Function | Range | Default |
|----|-----|----------|-------|---------|
| 0 | 00h | Bank Select MSB | 0-127 | 0 |
| 1 | 01h | Modulation Wheel | 0-127 | — |
| 5 | 05h | Portamento Time | 0-127 | — |
| 6 | 06h | Data Entry (for RPN/NRPN) | 0-127 | — |
| 7 | 07h | **Channel Volume** | 0-127 | 100 |
| 10 | 0Ah | **Pan** | 0-127 | 64 (center) |
| 11 | 0Bh | **Expression** | 0-127 | 127 |
| 64 | 40h | **Sustain Pedal** | 0/127 | — |
| 65 | 41h | Portamento On/Off | 0/127 | — |
| 66 | 42h | Sostenuto Pedal | 0/127 | — |
| 67 | 43h | Soft Pedal | 0/127 | — |
| 80 | 50h | **Reverb Type** (DREAM-specific) | 0-7 | 4 (Hall2) |
| 81 | 51h | **Chorus Type** (DREAM-specific) | 0-7 | 2 (Chorus3) |
| 91 | 5Bh | **Reverb Send Level** | 0-127 | — |
| 93 | 5Dh | **Chorus Send Level** | 0-127 | — |
| 120 | 78h | All Sound Off | 0 | — |
| 121 | 79h | Reset All Controllers | 0 | — |
| 123 | 7Bh | All Notes Off | 0 | — |
| 126 | 7Eh | Mono On | 0 | — |
| 127 | 7Fh | Poly On (default) | 0 | — |

---

## Reverb Types (CC 80)

| Value | Type |
|-------|------|
| 0 | Room 1 |
| 1 | Room 2 |
| 2 | Room 3 |
| 3 | Hall 1 |
| **4** | **Hall 2 (default)** |
| 5 | Plate |
| 6 | Delay |
| 7 | Pan Delay |

---

## Chorus Types (CC 81)

| Value | Type |
|-------|------|
| 0 | Chorus 1 |
| 1 | Chorus 2 |
| **2** | **Chorus 3 (default)** |
| 3 | Chorus 4 |
| 4 | FB Chorus |
| 5 | Flanger |
| 6 | Short Delay |
| 7 | FB Delay |

---

## NRPNs — Synth Parameter Tweaking

These modify the current patch in real time. All values are relative: **64 (0x40) = no change** from the patch default. Lower values decrease, higher values increase.

### How to send an NRPN (3 CC messages):

```
CC 99 = NRPN MSB
CC 98 = NRPN LSB
CC 6  = Data Entry (the value)
```

ESP-IDF example:
```c
sam2695_control_change(ch, 99, 1);    // NRPN MSB
sam2695_control_change(ch, 98, 32);   // NRPN LSB (0x20 = TVF cutoff)
sam2695_control_change(ch, 6, value); // 0-127, 64=neutral
```

### Filter

| NRPN | MSB | LSB | Function |
|------|-----|-----|----------|
| 0120h | 1 | 32 | **TVF Cutoff Frequency** |
| 0121h | 1 | 33 | **TVF Resonance** |

### Envelope

| NRPN | MSB | LSB | Function |
|------|-----|-----|----------|
| 0163h | 1 | 99 | Attack Time |
| 0164h | 1 | 100 | Decay Time |
| 0166h | 1 | 102 | Release Time |

### Vibrato

| NRPN | MSB | LSB | Function |
|------|-----|-----|----------|
| 0108h | 1 | 8 | Vibrato Rate |
| 0109h | 1 | 9 | Vibrato Depth |
| 010Ah | 1 | 10 | Vibrato Delay |

---

## RPNs — Registered Parameter Numbers

Sent the same way as NRPNs but using CC 101 (RPN MSB) + CC 100 (RPN LSB) + CC 6.

| RPN | MSB | LSB | Function | Default |
|-----|-----|-----|----------|---------|
| 0000h | 0 | 0 | Pitch Bend Sensitivity (semitones) | 2 |
| 0001h | 0 | 1 | Fine Tuning (cents, -100 to +100) | 64 (0) |
| 0002h | 0 | 2 | Coarse Tuning (half-tones, -64 to +64) | 64 (0) |

---

## Reverb Deep Parameters (via SysEx)

Accessible via SysEx messages (`F0 41 00 42 12 40 01 3xh vv xx F7`):

| Parameter | SysEx ID | Range | Notes |
|-----------|----------|-------|-------|
| Reverb Volume | 01 33h | 0-127 | Per-type defaults vary (~0x90) |
| Reverb Time | 01 34h | 0-127 | Default 0x7F for most types |
| Reverb Feedback | 01 35h | 0-127 | Only for Delay (6) and Pan Delay (7) |

---

## Chorus Deep Parameters (via SysEx)

Accessible via SysEx messages (`F0 41 00 42 12 40 01 3xh vv xx F7`):

| Parameter | SysEx ID | Range | Notes |
|-----------|----------|-------|-------|
| Chorus Volume | 01 3Ah | 0-127 | Default ~0x90 |
| Chorus Feedback | 01 3Bh | 0-127 | Varies by type |
| Chorus Delay | 01 3Ch | 0-127 | Varies by type |
| Chorus Rate | 01 3Dh | 0-127 | Varies by type |
| Chorus Depth | 01 3Eh | 0-127 | Varies by type |

---

## 4-Band Stereo Equalizer (via NRPN)

| NRPN | Function | Default Level | Freq Range | Default Freq |
|------|----------|---------------|------------|--------------|
| 3700h | EQ Low Band | 0x60 (+6dB) | 0-4.7 kHz | 0x0C |
| 3701h | EQ Med Low Band | 0x40 (0dB) | 0-4.2 kHz | 0x1B |
| 3702h | EQ Med High Band | 0x40 (0dB) | 0-4.2 kHz | 0x72 |
| 3703h | EQ High Band | 0x60 (+6dB) | 0-18.75 kHz | 0x40 |
| 3708h-370Bh | EQ Band Frequencies | — | see above | see above |

EQ Level scale: 0x00=-12dB, 0x20=-6dB, 0x40=0dB, 0x60=+6dB, 0x7F=+12dB

---

## Spatial Effect (via NRPN)

| NRPN | Function | Default |
|------|----------|---------|
| 3720h | Spatial Effect Volume | 0 (off) |
| 372Ch | Spatial Effect Delay | 0x1D |
| 372Dh | Spatial Input Select | 0=Stereo, 0x7F=Mono/Pseudo-stereo |

---

## Key Quirks

- **Bank Select before Program Change**: Always send CC 0 (Bank MSB) before the Program Change message
- **Boot delay**: Chip needs ~500-600ms after reset before it accepts MIDI
- **CC 80/81 are DREAM-specific**: Not standard GM controller numbers
- **Channel 10 = Drums**: Channel index 9 (zero-indexed) is drums by default
- **Polyphony**: 64 voices without effects, 38 voices with effects enabled
- **Clipping mode**: Soft clip (default) vs hard clip — controllable via NRPN 3713h

---

## GM Patch Families (for reference)

| PC Range | Family |
|----------|--------|
| 1-8 | Piano |
| 9-16 | Chromatic Percussion |
| 17-24 | Organ |
| 25-32 | Guitar |
| 33-40 | Bass |
| 41-48 | Strings |
| 49-56 | Ensemble |
| 57-64 | Brass |
| 65-72 | Reed |
| 73-80 | Pipe |
| 81-88 | Synth Lead |
| 89-96 | Synth Pad |
| 97-104 | Synth FX |
| 105-112 | Ethnic |
| 113-120 | Percussive |
| 121-128 | SFX |
