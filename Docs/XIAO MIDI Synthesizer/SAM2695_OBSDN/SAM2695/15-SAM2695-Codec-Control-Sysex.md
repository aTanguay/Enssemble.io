---
tags: [firmware, sam2695, codec, sysex]
---

# SAM2695 — Codec Control System Exclusive

⬅ [[00-SAM2695-Index]]

A raw SysEx form exists for writing a 16-bit value into **any** internal port register.

## Message format

```
F0h 00h 20h 00h 00h 00h 12h 33h 77h pp vv3 vv2 vv1 vv0 xx F7h
```
- `pp` = port number (0–7Fh)
- `vv3..vv0` = 16-bit value as four 4-bit nibbles (vv3=highest, vv0=lowest)

Example: writing `0D35Ah` to port `56h`:
```
F0h 00h 20h 00h 00h 00h 12h 33h 77h 56h 0Dh 03h 05h 0Ah xx F7h
```

> ⚠️ **Not recommended to write to any port except the two documented below** — everything else is undocumented internal state.

## Port 18 (12h): CODEC CONTROL 0

| Bit 15 | 14 | 13-8 | 7 | 6 | 5-0 |
|---|---|---|---|---|---|
| ADC MUTE | IN MUTE | ING[5:0] | DAC MUTE | DAC SEL | OUTG[5:0] |

Reset default: `1B79h` = `0001 1011 0111 1001`

**Restore to default:**
```
F0h 00h 20h 00h 00h 00h 12h 33h 77h 12h 01h 0Bh 07h 09h xx F7h
```

## Port 20 (14h): CODEC CONTROL 1

| Bit 15 | 14 | 13-0 |
|---|---|---|
| HPF | MICBT | xxx / RSV[12:0] (reserved for test — don't change) |

Reset default: `077Dh`

**Enable mic boost (+20dB):**
```
F0h 00h 20h 00h 00h 00h 12h 33h 77h 14h 04h 07h 07h 0Dh xx F7h
```

**Restore default (mic boost off):**
```
F0h 00h 20h 00h 00h 00h 12h 33h 77h 14h 00h 07h 07h 0Dh xx F7h
```

## Codec control bit description

### OUTG[5:0] — audio output gain
Adjusts DAC amplifier output logarithmically, **+6dB to -40dB in 1dB steps**, plus two extra low gains -43.5dB and -58.5dB. Default 0dB.

| OUTG5 | OUTG4 | OUTG3 | OUTG2 | OUTG1 | OUTG0 | Gain |
|---|---|---|---|---|---|---|
| 1 | 1 | 1 | 1 | 1 | 1 | +6 dB |
| 1 | 1 | 1 | 1 | 1 | 0 | +5 dB |
| 1 | 1 | 1 | 1 | 0 | 1 | +4 dB |
| 1 | 1 | 1 | 1 | 0 | 0 | +3 dB |
| 1 | 1 | 1 | 0 | 1 | 1 | +2 dB |
| 1 | 1 | 1 | 0 | 1 | 0 | +1 dB |
| **1** | **1** | **1** | **0** | **0** | **1** | **+0 dB (default)** |
| 1 | 1 | 1 | 0 | 0 | 0 | -1 dB |
| 1 | 1 | 0 | 1 | 1 | 1 | -2 dB |
| 1 | 1 | 0 | 1 | 1 | 0 | -3 dB |
| 1 | 1 | 0 | 1 | 0 | 1 | -4 dB |
| 1 | 1 | 0 | 1 | 0 | 0 | -5 dB |
| 1 | 1 | 0 | 0 | 1 | 1 | -6 dB |
| 1 | 1 | 0 | 0 | 1 | 0 | -7 dB |
| 1 | 1 | 0 | 0 | 0 | 1 | -8 dB |
| 1 | 1 | 0 | 0 | 0 | 0 | -9 dB |
| ... | ... | ... | ... | ... | ... | (1dB steps continue) |
| 0 | 1 | 0 | 0 | 1 | 1 | -38 dB |
| 0 | 1 | 0 | 0 | 1 | 0 | -39 dB |
| 0 | 1 | 0 | 0 | 0 | 1 | -40 dB |
| 0 | 1 | 0 | 0 | 0 | 0 | -43.5 dB |
| 0 | 0 | x | x | x | x | -58.5 dB |

**DACSEL:** 0 = DAC not routed to output (muted); 1 = routed (default)
**DACMUTE:** 0 = digital mute disabled (default); 1 = enabled
> ⚠️ When using DACMUTE, ensure OUTG[5:0] ≤ `039h` (≤0dB gain) first — using DACMUTE with gain >0dB can produce a **loud pop** when DACMUTE transitions 1→0.

### ING[5:0] — audio input gain
Adjusts IPGA gain in ADC path, **+36dB to -27dB in 1dB steps**. Default 0dB.

| ING5 | ING4 | ING3 | ING2 | ING1 | ING0 | Gain |
|---|---|---|---|---|---|---|
| 1 | 1 | 1 | 1 | 1 | 1 | +36 dB |
| 1 | 1 | 1 | 1 | 1 | 0 | +35 dB |
| ... | | | | | | (1dB steps) |
| 1 | 1 | 0 | 0 | 0 | 0 | +21 dB |
| ... | | | | | | ... |
| **0** | **1** | **1** | **0** | **1** | **1** | **0 dB (default)** |
| ... | | | | | | ... |
| 0 | 0 | 1 | 1 | 1 | 0 | -13 dB |
| 0 | 0 | 0 | 0 | 0 | 1 | -26 dB |
| 0 | 0 | 0 | 0 | 0 | 0 | -27 dB |

**INMUTE:** 0=normal (default), 1=muted
**ADCMUTE:** 0=disable ADC digital mute (default), 1=enable ADC digital mute + input mute
**RSV[12:0]:** reserved for test — do not change
**MICBT:** microphone boost. 0=off/0dB (default), 1=on/+20dB
**HPF:** ADC high-pass filter. 0=disabled (default), 1=enabled

## Codec block diagram (signal path)

```
Digital Audio Bus → Digital Audio Interface
   → [R channel] Digital Interpolation Filter → Digital Sigma-Delta Mod → SCF DAC
        → DACSEL mux → HPAMP (OUTG[5:0], DACMUTE) → AOUTR
   → [L channel] (same chain) → AOUTL
   VCM buffer → VCMHPOUT
   MICIN → MICBT (0/+20dB) → IPGA (ING[5:0]) → Sigma-Delta Mod (ADCMUTE)
        → Digital Decimation Filter → High-Pass Filter (HPF) → Digital Audio Interface
   Voltage Reference → VCM
```

## Related
- [[03-SAM2695-Electrical-Characteristics]] — DAC/ADC specs these gains operate within
- [[06-SAM2695-Reference-Circuits]]
- [[11-SAM2695-MIDI-Implementation]] — the generic port-write SysEx form
