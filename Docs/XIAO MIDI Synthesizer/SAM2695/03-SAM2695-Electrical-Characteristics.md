---
tags: [hardware, sam2695, electrical]
---

# SAM2695 — Electrical Characteristics

⬅ [[00-SAM2695-Index]]

## Absolute maximum ratings (all voltages vs GND=0V)

| Parameter | Symbol | Min | Max | Unit |
|---|---|---|---|---|
| Temperature under bias | – | -55 | +125 | °C |
| Storage temperature | – | -65 | +150 | °C |
| Voltage on any 5V-tolerant pin | – | -0.3 | 5.5 | V |
| Voltage on any non-5V-tolerant pin | – | -0.3 | VD33+0.3 | V |
| Supply voltage (I/O) | VD33 | -0.3 | 3.6 | V |
| Supply voltage (CODEC analog) | VA33 | -0.3 | 3.6 | V |

> Stress beyond these ratings may cause permanent damage; not a functional operating spec.

## Recommended operating conditions

| Parameter | Symbol | Min | Typ | Max | Unit |
|---|---|---|---|---|---|
| Digital supply voltage | VD33 | 2.7 | 3.3 | 3.6 | V |
| Analog supply voltage (CODEC) | VA33 | 2.7 | 3.3 | 3.6 | V |
| Operating ambient temperature | tA | -25 | – | +70 | °C |

## D.C. characteristics
(TA=25°C, VD33=3.3V, X1=12MHz, AOUTL/AOUTR load = 47kΩ)

| Parameter | Symbol | Min | Typ | Max | Unit |
|---|---|---|---|---|---|
| Low level input voltage | VIL | – | – | 0.8 | V |
| High level input voltage, 5VT pins | VIH | 2 | – | – | V |
| High level input voltage, non-5VT pins | VIH | 2 | – | – | V |
| Low level output voltage @ IOL=IOHL min | VOL | – | – | 0.4 | V |
| High level output voltage @ IOH=IOHL min | VOH | 2.4 | – | – | V |
| Schmitt-trigger negative threshold | VTN | 0.8 | 1.1 | – | V |
| Schmitt-trigger positive threshold | VTP | – | 1.6 | 2 | V |
| Driving capability (DR4 pins) | IOHL | – | – | 4 | mA |
| Driving capability (DR8 pins) | IOHL | – | 8 | – | mA |
| Input leakage current | IIN | -10 | ±1 | 10 | µA |
| OUTVC12 output voltage | VD12 | 1.14 | 1.2 | 1.26 | V |
| Digital power supply current | ID33 | – | 10 | – | mA |
| Analog power supply current | IA33 | – | 8 | – | mA |
| Power down supply current | – | – | 17 | – | µA |
| Pull-up/down/keeper resistor | Rudk | 30 | 75 | 190 | kΩ |

## DAC characteristics
(TA=25°C, VA33=3.3V)

| Parameter | Symbol | Typ | Max | Unit |
|---|---|---|---|---|
| THD+N (at -6dB) | THD+N | -75 | – | dB |
| Dynamic range (-60dBFS, A-weighted) | DR | 86 | – | dB |
| Inter-channel isolation (1kHz) | – | 89 | – | dB |
| Full-scale output voltage | – | 0.85×VA33 | – | Vpp |
| VCM max allowable DC current source | – | – | 0.1 | mA |
| VCM reference voltage (10µF+100nF caps) | – | VA33÷2 ±50mV | – | V |
| Output gain control, 1dB steps | – | -40 to +6 | – | dB |
| Output gain, additional setting 1 | – | -43.5 | – | dB |
| Output gain, additional setting 2 | – | -58.5 | – | dB |
| Max output power, headphone mode | – | 30 | – | mW |

## ADC characteristics
(TA=25°C, VA33=3.3V)

| Parameter | Symbol | Min | Typ | Max | Unit |
|---|---|---|---|---|---|
| THD+N (at -6dB) | THD+N | – | -75 | -70 | dB |
| Dynamic range (-60dBFS, A-weighted) | DR | – | 86 | – | dB |
| Full-scale input voltage | – | – | 0.6×VA33 | – | Vpp |
| Input gain without boost | – | – | +4 | – | dB |
| Input gain with boost | – | +22 | – | +24 | dB |
| Input programmable gain amp, 1dB steps | – | -27 | – | +36 | dB |
| Frequency response (-3dB bandwidth) | – | 0.41 | – | – | Fs |
| MICIN input resistance, boost disabled | – | – | 35 | – | kΩ |
| MICIN input resistance, boost enabled | – | – | 3.5 | – | kΩ |

## Related
- [[15-SAM2695-Codec-Control-Sysex]] — for the actual gain-control bitfields (OUTG, ING, MICBT) used to hit these ranges in firmware
- [[02-SAM2695-Pin-Description]]
