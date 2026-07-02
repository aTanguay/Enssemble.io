---
tags: [firmware, sam2695, midi, polyphony]
---

# SAM2695 — NRPN 375Fh: Effect On/Off & Polyphony Selection

⬅ [[00-SAM2695-Index]]

MIDI message: `B0h 63h 37h`, `B0h 62h 5Fh`, `B0h 06h vv`

Each bit of `vv` toggles an effect ON/OFF. Every enabled effect **decreases available polyphony** (from a max of 64).

## Bit layout

| bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0 |
|---|---|---|---|---|---|---|---|
| 0 | ECH | REV | CHR | OM | MIC | EQ2 | EQ1 |

- **REV**=1 → Reverb ON, polyphony −13
- **CHR**=1 → Chorus ON, polyphony −3
- **MIC**=1 → Mike ON, polyphony −1
- **ECH**=1 → Mike Echo ON, polyphony −3
- **OM** (output mode): 0=Spatial effect OFF; 1=Spatial effect ON, polyphony −1 (parameters via NRPN 3720h volume, 372Ch delay, 372Dh stereo/mono — see [[09-SAM2695-NRPN-Messages]])
- **EQ2,EQ1**: `00`=EQ off; `10`=2-band EQ, polyphony −4; `11`=4-band EQ, polyphony −8

## Common presets

| vv (binary\|hex) | REV | CHR | Spatial | EQ | MIC | ECH | Polyphony | Comment |
|---|---|---|---|---|---|---|---|---|
| 0000\|0000 – 00h | OFF | OFF | OFF | OFF | OFF | OFF | **64** | Max poly |
| 0000\|0010 – 02h | OFF | OFF | OFF | 2-Band | OFF | OFF | 59 | |
| 0000\|0011 – 03h | OFF | OFF | OFF | 4-Band | OFF | OFF | 55 | |
| 0000\|1000 – 08h | OFF | OFF | ON | OFF | OFF | OFF | 62 | |
| 0000\|1010 – 0Ah | OFF | OFF | ON | 2-Band | OFF | OFF | 58 | |
| 0000\|1011 – 0Bh | OFF | OFF | ON | 4-Band | OFF | OFF | 54 | |
| 0000\|1110 – 0Eh | OFF | OFF | ON | 2-Band | ON | OFF | 57 | |
| 0010\|0000 – 20h | ON | OFF | OFF | OFF | OFF | OFF | 50 | |
| 0010\|0010 – 22h | ON | OFF | OFF | 2-Band | OFF | OFF | 46 | |
| 0010\|0011 – 23h | ON | OFF | OFF | 4-Band | OFF | OFF | 42 | |
| 0010\|1000 – 28h | ON | OFF | ON | OFF | OFF | OFF | 49 | |
| 0010\|1010 – 2Ah | ON | OFF | ON | 2-Band | OFF | OFF | 45 | |
| 0010\|1011 – 2Bh | ON | OFF | ON | 4-Band | OFF | OFF | 41 | |
| 0011\|0000 – 30h | ON | ON | OFF | OFF | OFF | OFF | 48 | |
| 0011\|0011 – 33h | ON | ON | OFF | 4-Band | OFF | OFF | 39 | |
| 0011\|0111 – 37h | ON | ON | OFF | 4-Band | ON | OFF | 38 | |
| **0011\|1011 – 3Bh** | ON | ON | ON | 4-Band | OFF | OFF | **38** | **Default** |
| **0100\|0101 – 45h** | ON | ON | ON | 4-Band | OFF | OFF | **38** | **Reset All** |
| 0100\|1110 – 4Eh | OFF | OFF | ON | 2-Band | ON | ON | 55 | |
| 0111\|0100 – 74h | ON | ON | OFF | OFF | ON | ON | 44 | |
| 0111\|0110 – 76h | ON | ON | OFF | 2-Band | ON | ON | 40 | |
| 0111\|0111 – 77h | ON | ON | OFF | 4-Band | ON | ON | 36 | |
| 0111\|1110 – 7Eh | ON | ON | ON | 2-Band | ON | ON | 39 | |
| 0111\|1111 – 7Fh | ON | ON | ON | 4-Band | ON | ON | 35 | |

## Important: value 45h is special
For SAM2195 compatibility, `45h` is reserved as **"reset all"**. Sending it:
- Restores power-up status (Reverb/Chorus ON, Spatial Effect OFF, 4-Band EQ, Mike/Echo OFF)
- Re-initializes all MIDI parameters to default
- **Stalls firmware for ~50ms** during the reset procedure before it's ready for more MIDI

## Related
- [[09-SAM2695-NRPN-Messages]]
- [[10-SAM2695-Parallel-Controls]]
