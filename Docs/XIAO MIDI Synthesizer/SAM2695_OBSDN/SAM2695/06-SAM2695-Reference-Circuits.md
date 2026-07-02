---
tags: [hardware, sam2695, schematic]
---

# SAM2695 — Reference Circuits

⬅ [[00-SAM2695-Index]]

Conformity with these reference designs ensures best performance. All are the datasheet's recommended designs — deviate at your own risk on audio quality.

## Crystal compensation
- 12 MHz crystal between X1 (pin 39) and X2 (pin 40)
- 22pF loading caps on each leg to GND
- Keep crystal + compensation-cap traces short and shielded; return ground for the caps should be the SAM2695 GND plane (see [[07-SAM2695-Board-Layout]])
- `XDIV` (pin 47): grounded = 9.6MHz crystal mode, tied to VD33 = 12MHz crystal mode

## Line output
- AOUTR/AOUTL → 1µF series cap (C1/C2) → 100Ω series resistor (R1/R2) → line-out jack
- 47kΩ resistor (R3/R4) from the post-cap node to AGND on each channel

## Headphones output (dedicated)
- AOUTR/AOUTL direct to headphone jack pins 1/4
- VCMHPOUT → jack pin 5 (common-mode reference for headphone ground)

## Headphones OR line output (combined, more filtering)
- AOUTR/AOUTL → 100µF series caps (C1/C2) → jack
- 47kΩ resistors (R1/R2) from post-cap node to AGND
- Use this version if you want a single output serving either headphones or line-level, rather than the simpler dedicated designs above

## Microphone input
- Mic capsule (5-pin jack, e.g. electret) biased through 4.7kΩ (R1) to AGND
- 220pF cap (C2) across the bias node for RF filtering
- 1µF series cap (C1) from bias node to `MICIN` (pin 7)

## Related
- [[02-SAM2695-Pin-Description]]
- [[07-SAM2695-Board-Layout]]
- [[15-SAM2695-Codec-Control-Sysex]] — mic boost, ADC/DAC gain and mute bits
