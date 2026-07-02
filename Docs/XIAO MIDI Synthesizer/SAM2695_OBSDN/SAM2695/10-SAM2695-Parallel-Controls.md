---
tags: [firmware, sam2695, midi, parallel]
---

# SAM2695 — Parallel CONTROL Register Commands

⬅ [[00-SAM2695-Index]]

Received on the parallel CONTROL register (A0=1 write). Normally used in **parallel mode**; individual controls can be sent in serial mode too, if preceded by control `BEh` (see [[08-SAM2695-Serial-Parallel-Modes]]).

A control message = one CONTROL byte + one DATA8 byte (parameter).

## Full control table

| Ctrl # | Description | Range | Power-up default | Equivalent NRPN/SysEx |
|---|---|---|---|---|
| 07h | Master Volume | 0–FFh | FFh | Nrpn 3707h |
| 10h | EQ low band left (bass) | 0=-12dB,40h=0dB,7Fh=+12dB | 60h | Nrpn 3700h |
| 11h | EQ med-low band left | same scale | 40h | Nrpn 3701h |
| 12h | EQ med-high band left | same scale | 40h | Nrpn 3702h |
| 13h | EQ high band left (treble) | same scale | 60h | Nrpn 3703h |
| 14h | EQ low band right (bass) | same scale | 60h | Nrpn 3700h |
| 15h | EQ med-low band right | same scale | 40h | Nrpn 3701h |
| 16h | EQ med-high band right | same scale | 40h | Nrpn 3702h |
| 17h | EQ high band right (treble) | same scale | 60h | Nrpn 3703h |
| 18h | EQ low cutoff freq | 0=0Hz,7Fh=4.7kHz | 0Ch | Nrpn 3708h |
| 19h | EQ med-low cutoff freq | 0=0Hz,7Fh=4.2kHz | 1Bh | Nrpn 3709h |
| 1Ah | EQ med-high cutoff freq | 0=0Hz,7Fh=4.2kHz | 72h | Nrpn 370Ah |
| 1Bh | EQ high cutoff freq | 0=0Hz,7Fh=18.75kHz | 40h | Nrpn 370Bh |
| 1Ch | Main echo right volume | 0–7Fh | 7Fh | Nrpn 3734h |
| 1Dh | Main echo left volume | 0–7Fh | 7Fh | Nrpn 3735h |
| 25h | GM reverb send | 0=none,80h=default,FFh=max | 80h | Nrpn 3715h |
| 26h | GM chorus send | 0=none,80h=default,FFh=max | 80h | Nrpn 3716h |
| 28h | Mike echo level | 0–7Fh | 7Fh | Nrpn 3728h |
| 29h | Mike echo time | 0–7Fh | 2Bh | Nrpn 3729h |
| 2Ah | Mike echo feedback | 0–7Fh | 42h | Nrpn 372Ah |
| 2Bh | Slave1 echo right vol | 0–7Fh | 00h | Nrpn 3730h |
| 2Ch | Slave1 echo left vol | 0–7Fh | 00h | Nrpn 3731h |
| 2Dh | Slave2 echo right vol | 0–7Fh | 00h | Nrpn 3732h |
| 2Eh | Slave2 echo left vol | 0–7Fh | 00h | Nrpn 3733h |
| 30h | Spatial effect volume | 0=none,FFh=max | 00h | Nrpn 3720h |
| 31h | Spatial effect delay | 0=shortest,7Fh=longest | 1Dh | Nrpn 372Ch |
| 32h | Spatial effect input | 0=stereo,7Fh=mono | 00h | Nrpn 372Dh |
| 34h | Mike volume | 0–FFh | 80h | Nrpn 3724h |
| 36h | Mike pan | 0=left,40h=center,7Fh=right | 40h | Nrpn 3726h |
| 38h | GM volume | 0–FFh | FFh | Nrpn 3722h |
| 39h | GM pan | 0=left,40h=center,7Fh=right | 40h | Nrpn 3723h |
| 3Ah | Reverb general volume | 0–FFh | – | SysEx 40h 01h 33h |
| 3Bh | Chorus general volume | 0–FFh | – | SysEx 40h 01h 3Ah |
| 3Fh | Switch to UART mode | – | – | – |
| 62h | Post effects on GM | 0=off,7Fh=on | 7Fh | Nrpn 3718h |
| 65h | Post effects on Mike/Echo | 0=off,7Fh=on | 00h | Nrpn 3719h |
| 66h | Post effects on Reverb/Chorus | 0=off,7Fh=on | 7Fh | Nrpn 371Ah |
| 69h | Reverb program select | 0–7h | 04h | SysEx 40h 01h 30h |
| 6Ah | Chorus program select | 0–7h | 02h | SysEx 40h 01h 38h |
| 74h | Chorus delay | – | – | SysEx 40h 01h 3Ch |
| 75h | Chorus feedback | – | – | SysEx 40h 01h 3Bh |
| 76h | Chorus rate | – | – | SysEx 40h 01h 3Dh |
| 77h | Chorus depth | – | – | SysEx 40h 01h 3Eh |
| 78h | Reverb time | – | – | SysEx 40h 01h 34h |
| 79h | Reverb feedback (reverb # 6 or 7 only) | – | – | SysEx 40h 01h 35h |
| 7Eh | Clipping mode select | 0=soft,7Fh=hard | 00h | Nrpn 3713h |
| BEh | Enable Dream control in stand-alone mode | – | – | – |
| FFh | Reset UART mode | – | – | – |

## System messages

| Ctrl # | Name | Params | Action | Answer |
|---|---|---|---|---|
| 07h | MASTER_VOL | 0-FFh, default FFh | Master volume | – |
| BEh | EN_CONTROL | none | Enable sending one parallel control in serial mode (must prefix each control) | – |
| FFh | RESET | none | Switch SAM2695 to serial mode | – |
| 3Fh | UART_MOD | none | Switch SAM2695 to parallel mode | Data=FEh |

## Spatial effect device

| Ctrl # | Name | Params | Default |
|---|---|---|---|
| 30h | SUR_VOL | 0-FFh | 0 |
| 31h | SUR_DEL | 0-7Fh | 1Dh |
| 32h | SUR_INP | 0/7Fh (0=stereo, 7Fh=mono) | 0 |

## Routing (post-effects)

| Ctrl # | Name | Params | Action |
|---|---|---|---|
| 62h | GM_POST | 0/7Fh | Post effects on general MIDI |
| 65h | MIKECH_POST | 0/7Fh | Post effects on Mike and Echo |
| 66h | EFF_POST | 0/7Fh | Post effects on Reverb-chorus |

## MIDI-level

| Ctrl # | Name | Params | Default |
|---|---|---|---|
| 38h | GM_VOL | 0-FFh, linear | 0FFh |
| 39h | GM_PAN | 0=left, 40h=center, 7Fh=right | 40h |

## Mike & echo device
(requires Mike/Echo enabled via NRPN `375Fh` — see [[14-SAM2695-Polyphony-Effects-Select]])

| Ctrl # | Name | Params | Default |
|---|---|---|---|
| 34h | MIKE_VOL | 0-FFh | 80h |
| 36h | MIKE_PAN | 0-7Fh | 40h |
| 28h | ECH_LEV | 0-7Fh | 7Fh |
| 29h | ECH_TIM | 0-7Fh (7Fh=380ms) | 2Bh |
| 2Ah | ECH_FEED | 0-7Fh | 42h |
| 1Ch/1Dh | ECHM_RIGHT/LEFT | 0-7Fh | 7Fh |
| 2Bh/2Ch | ECHS1_RIGHT/LEFT | 0-7Fh | 00h |
| 2Dh/2Eh | ECHS2_RIGHT/LEFT | 0-7Fh | 00h |

## Reverb device

| Ctrl # | Name | Params | Notes |
|---|---|---|---|
| 69h | REV_TYPE | 0-7, default 4 | room1/room2/room3/hall1/**hall2**/plate/delay/pan-delay = 0-7 |
| 3Ah | REV_VOL | 0-FFh | per-program defaults differ (delay/pan-delay=FFh, hall1=C0h, rest=90h) |
| 78h | REV_TIME | 0-7Fh | per-program defaults: 7Fh for most, delay=18h |
| 79h | REV_FEED | 0-7Fh | only if REV_TYPE=6 or 7 (delays); defaults delay=22h, pan-delay=26h |
| 25h | GMREV_SEND | 0-FFh, 80h=unmodified | reverb send level scaling |

## Chorus device

| Ctrl # | Name | Params | Notes |
|---|---|---|---|
| 6Ah | CHR_TYPE | 0-7, default 2 (chorus3) | chorus1-4/FBchorus/flanger/shortdel/FBdelay = 0-7 |
| 3Bh | CHR_VOL | 0-FFh | – |
| 74h | CHR_DEL | 0-7Fh | – |
| 75h | CHR_FEED | 0-7Fh | – |
| 76h | CHR_RATE | 0-7Fh | – |
| 77h | CHR_DEPTH | 0-7Fh | – |
| 26h | GMCHR_SEND | 0-FFh, 80h=unmodified | chorus send level scaling |

**Chorus per-program defaults:**

| | chorus1 | chorus2 | chorus3 | chorus4 | FBchorus | flanger | shortdel | FBdelay |
|---|---|---|---|---|---|---|---|---|
| CHR_VOL | 90h | 90h | 90h | 90h | 90h | 90h | FFh | FFh |
| CHR_DEL | 4Bh | 40h | 40h | 2Bh | 7Fh | 56h | 7Fh | 7Fh |
| CHR_FEED | 00h | 07h | 09h | 0Ch | 48h | 7Fh | 00h | 50h |
| CHR_RATE | 03h | 09h | 03h | 09h | 02h | 01h | 00h | 00h |
| CHR_DEPTH | 05h | 13h | 13h | 10h | 0Ch | 03h | 00h | 00h |

## Equalizer device (stereo, separate L/R)

| Ctrl # | Name | Params | Default |
|---|---|---|---|
| 10h/14h | EQ_LBL / EQ_LBR | 0-7Fh (-12/0/+12dB @ 00/40/7Fh) | 60h |
| 11h/15h | EQ_MLBL / EQ_MLBR | same scale | 40h |
| 12h/16h | EQ_MHBL / EQ_MHBR | same scale | 40h |
| 13h/17h | EQ_HBL / EQ_HBR | same scale | 60h |
| 18h | EQF_LB | 0-7Fh (0-4.7kHz) | 0Ch |
| 19h | EQF_MLB | 0-7Fh (0-4.2kHz) | 1Bh |
| 1Ah | EQF_MHB | 0-7Fh (0-4.2kHz) | 72h |
| 1Bh | EQF_HB | 0-7Fh (0-18.75kHz) | 40h |

## Related
- [[09-SAM2695-NRPN-Messages]] — same functions in NRPN form, usable in either serial or parallel mode
- [[08-SAM2695-Serial-Parallel-Modes]]
