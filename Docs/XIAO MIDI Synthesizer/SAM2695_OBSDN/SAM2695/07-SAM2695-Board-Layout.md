---
tags: [hardware, sam2695, layout]
---

# SAM2695 — Recommended Board Layout

⬅ [[00-SAM2695-Index]]

Like all HCMOS high-integration ICs, following these simple layout rules is mandatory for reliable operation:

## GND / VD33 / VA33 distribution and decoupling
- Connect **all** GND, VD33, VA33 pins.
- Strongly recommended: a GND plane directly below the SAM2695.
- Board GND/VD33 distribution should be in **grid form**.
- Decoupling: **4.7µF or 10µF close to OUTVC12** pin.
- Each VD33 and VA33 pin needs its own **0.1µF** decoupling cap.
- Additional **10µF-T** cap on VD33, placed close to the crystal.
- Keep all decoupling caps close to the IC.

## Crystal
- Keep crystal + compensation-cap traces short and shielded.
- Ground return from the compensation caps should be the SAM2695 GND plane.

## D0–D7 bus
- Implement a ground plane beneath the D0-D7 bus, connecting both to the host and to SAM2695 GND.

## Analog section
- Provide a dedicated AGND ground plane, connected to main GND by a **single trace only**.
- **No digital signals should cross the AGND plane.**

## Related
- [[06-SAM2695-Reference-Circuits]]
- [[02-SAM2695-Pin-Description]]
