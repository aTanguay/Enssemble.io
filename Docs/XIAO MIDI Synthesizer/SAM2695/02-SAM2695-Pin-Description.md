---
tags: [hardware, sam2695, pinout]
---

# SAM2695 — Pin Description

⬅ [[00-SAM2695-Index]]

Notation: `5VT` = 5V-tolerant input/IO pin. `DR4`/`DR8` = driving capability at VOL/VOH (see [[03-SAM2695-Electrical-Characteristics]]).

## Power supply group

| Pin name | Pin # | Type | Description |
|---|---|---|---|
| GND | 9, 13, 18, 25, 36, 45, 48 | PWR | Digital ground — connect all to a ground plane |
| GND | exposed die pad | PWR | Digital ground, down-bonded to heatsink pad; recommended (not obligatory) to connect to ground plane |
| VD33 | 8, 11, 20, 31, 35, 41, 46 | PWR | Periphery power supply, +2.7V–3.6V, nominal 3.3V |
| AGND | 4 | PWR | Analog ground — connect to analog ground plane |
| VA33 | 6 | PWR | CODEC periphery analog supply — nominal 3.3V through a series inductor filter (preferred) or 10Ω resistor |

## Serial MIDI / parallel MIDI

| Pin name | Pin # | Type | Description |
|---|---|---|---|
| MIDI_IN | 16 | IN-5VT | Serial TTL MIDI IN, connected to built-in synth at power-up. Built-in pull-up; leave open or tie HIGH if unused |
| D0–D7 | 24, 26-30, 32, 33 | I/O-5VT-DR8 | 8-bit host data bus (parallel MIDI) |
| A0 | 10 | IN-5VT | Select DATA(0)/CONTROL(1) for write; DATA(0)/STATUS(1) for read |
| CS/ | 14 | IN-5VT | Chip select, active low, built-in pull-up. Leave unconnected if unused |
| RD/ | 15 | IN-5VT | Read, active low. With CS/ low, reads DATA (A0=0) or STATUS (A0=1), acknowledged on RD/ rising edge |
| WR/ | 12 | IN-5VT | Write, active low. With CS/ low, writes DATA (A0=0) or CONTROL (A0=1), on WR/ rising edge |
| IRQ | 42 | OUT-DR4 | Rising edge = a MIDI byte is available to read on D0-D7. Acknowledged by reading the byte |

## Analog audio group

| Pin name | Pin # | Type | Description |
|---|---|---|---|
| MICIN | 7 | IN | Analog microphone input |
| AOUTL | 1 | OUT | Left channel audio line/headphone output |
| AOUTR | 2 | OUT | Right channel audio line/headphone output |
| VCM | 5 | OUT | Analog common-mode voltage; stabilize with 10µF // 100nF to AGND |
| VCMHPOUT | 3 | OUT | Analog headphone common-mode voltage buffer output |

## Miscellaneous group

| Pin name | Pin # | Type | Description |
|---|---|---|---|
| RST/PD/ | 38 | IN-5VT | Master reset & power-down, Schmitt trigger. Hold low ≥10ms after power-up. Rising edge → init routine. Low → power-down (D-bus Hi-Z, IRQ=0, PLL stopped, core power removed). To exit power-down, drive HIGH |
| X1–X2 | 39, 40 | – | 9.6 MHz or 12 MHz crystal connection; external clock can also be used on X1 |
| XDIV | 47 | IN | System clock divider select: grounded = 9.6MHz crystal, tied to VD33 = 12MHz crystal |
| OUTVC12 | 34 | PWR | 3.3V→1.2V regulator output (internal core supply only). Decouple with 4.7µF or 10µF to GND |
| TEST | 37 | IN | Test pin, built-in pull-down. Ground or leave open for normal operation |
| NC | 17, 19, 21-23, 43, 44 | – | Not connected |

## Pinout by pin number (48-lead QFN)

| Pin# | Signal | Pin# | Signal | Pin# | Signal | Pin# | Signal |
|---|---|---|---|---|---|---|---|
| 1 | AOUTL | 13 | GND | 25 | GND | 37 | TEST |
| 2 | AOUTR | 14 | CS/ | 26 | D1 | 38 | RST/PD/ |
| 3 | VCMHPOUT | 15 | RD/ | 27 | D2 | 39 | X1 |
| 4 | AGND | 16 | MIDI_IN | 28 | D3 | 40 | X2 |
| 5 | VCM | 17 | NC | 29 | D4 | 41 | VD33 |
| 6 | VA33 | 18 | GND | 30 | D5 | 42 | IRQ |
| 7 | MICIN | 19 | NC | 31 | VD33 | 43 | NC |
| 8 | VD33 | 20 | VD33 | 32 | D6 | 44 | NC |
| 9 | GND | 21 | NC | 33 | D7 | 45 | GND |
| 10 | A0 | 22 | NC | 34 | OUTVC12 | 46 | VD33 |
| 11 | VD33 | 23 | NC | 35 | VD33 | 47 | XDIV |
| 12 | WR/ | 24 | D0 | 36 | GND | 48 | GND |

## Mechanical package

48-lead MQFN, 6×6mm, JEDEC MO-220 reference. Body 5.90–6.10mm (D/E), pitch 0.40mm BSC (e), terminal pitch/pad dims per JEDEC. Full dimension table available in original PDF if needed for CAD footprint creation — flag if you want it transcribed in full.

## Marking

```
dream FRANCE
SAM2695
XXXXX-XXX
• YYWW      (Pin 1 marked by dot, adjacent to marking)
```

## Related
- [[03-SAM2695-Electrical-Characteristics]]
- [[04-SAM2695-Timings]]
- [[06-SAM2695-Reference-Circuits]]
- [[07-SAM2695-Board-Layout]]
