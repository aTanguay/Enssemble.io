---
tags: [hardware, sam2695, reset, power]
---

# SAM2695 — Reset and Power Down

⬅ [[00-SAM2695-Index]]

## Power-up / reset sequence
1. Hold `RST/PD/` low for **≥10ms** during power-up (RC/diode power-up network is fine).
2. On low→high transition, chip enters initialization routine.
3. **~50ms** before it can process a MIDI IN or MPU message.
4. **Audio begins within 500ms max**.

## Power-down
If `RST/PD/` held low: crystal oscillator and PLL stop, chip enters deep power-down (core power removed). To exit, drive `RST/PD/` high again.

## Pin status: Normal vs Power-down

| Pin | Normal mode | Power-down mode |
|---|---|---|
| MIDI IN | IN with pull-up | IN with keeper resistor |
| CS/ | IN | IN (floating) |
| RD/ | IN | IN (floating) |
| WR/ | IN | IN (floating) |
| IRQ | OUT | OUT — low level |
| A0 | IN | IN with keeper resistor |
| D[7:0] | I/O | IN (floating) |
| LHPOUT / RHOUT | ANA OUT | ANA OUT — VCM level |
| VCMHPOUT / VCM | ANA OUT | ANA OUT — VCM level |
| MICIN | ANA IN | ANA IN |
| RST/PD/ | IN | IN driven low |
| X1–X2 | Oscillator | Power-down |
| XDIV | IN | IN with keeper resistor |
| TEST | IN with pull-down | IN with pull-down |

### Notes
- **Keeper resistor** = pull-up or pull-down depending on the logic level present at that pin the moment power-down engages (low state in → pull-down keeper; high state in → pull-up keeper).
- If you plan to use power-down mode, add external pull-up/pull-down (e.g. 1MΩ) on any pin listed "IN (floating)" that isn't otherwise driven in power-down — keeps normal-mode consumption low while avoiding floating inputs.

## Related
- [[02-SAM2695-Pin-Description]]
- [[07-SAM2695-Board-Layout]]
