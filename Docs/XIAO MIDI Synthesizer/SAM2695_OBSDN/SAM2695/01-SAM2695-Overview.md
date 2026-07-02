---
tags: [hardware, midi, synth, sam2695]
---

# SAM2695 — Overview

⬅ [[00-SAM2695-Index]]

Low power single chip synthesizer with effects and built-in CODEC.

## Key features

- Single chip all-in-one design:
  - MIDI control processor, serial and parallel interface
  - Synthesis, General MIDI wavetable implementation
  - GM-compatible effects: reverb + chorus
  - Spatial effect
  - 4-band stereo equalizer
  - Stereo DAC — DR 86dB typ, THD+N -75dB typ
  - Mic input — DR 86dB typ, THD+N -75dB typ
  - Mic echo
- Polyphony: **64 voices** without effects, **38 voices** with effects
- On-chip CleanWave™ wavetable data, firmware, RAM delay lines
- Stereo line audio output
- Typical applications: battery-operated musical keyboards, portable phones, karaoke machines
- Package: **QFN48, 6×6mm** — small footprint, small pin count
- Low power: 18 mA typ operating / 17 µA typ power-down, single 3.3V supply

## Typical hardware configuration

```
MIDI IN ──────┐
              │→ [ SAM2695 ] → Audio Out (stereo)
Parallel MIDI ┘
```

See [[02-SAM2695-Pin-Description]] for full pinout, [[08-SAM2695-Serial-Parallel-Modes]] for how the two input paths (serial MIDI vs 8-bit parallel bus) interact.

## Related
- [[02-SAM2695-Pin-Description]]
- [[03-SAM2695-Electrical-Characteristics]]
- [[06-SAM2695-Reference-Circuits]]
