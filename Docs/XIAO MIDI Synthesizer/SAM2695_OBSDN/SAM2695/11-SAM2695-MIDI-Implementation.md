---
tags: [firmware, sam2695, midi, sysex]
---

# SAM2695 — Detailed MIDI Implementation

⬅ [[00-SAM2695-Index]]

MIDI messages are received by the built-in wavetable synth from either the serial MIDI IN pin (serial mode) or the 8-bit parallel data port (parallel mode) — see [[08-SAM2695-Serial-Parallel-Modes]].

## Standard channel messages

| Message | Hex | Description | Compat |
|---|---|---|---|
| Note On | `9nh kk vv` | Channel n(0-15), note kk(1-127), velocity vv(1-127). vv=0 means Note Off | MIDI |
| Note Off | `8nh kk vv` | Channel n(0-15), note kk(1-127), vv don't-care | MIDI |
| Pitch Bend | `Enh bl bh` | 14-bit bend. Max swing ±1 tone (power-up), adjustable via pitch bend sensitivity RPN. Center = 00h 40h | GM |
| Program Change | `Cnh pp` | Patch change. Channel 10 (n=9) selects drumset — see [[13-SAM2695-Drum-Sets]] | GM/GS |
| Channel Aftertouch | `Dnh vv` | Pressure value; effect set via SysEx `40h 2nh 20h-26h` | MIDI |
| MIDI Reset | `FFh` | Reset to power-up condition | GM |

## Control changes (CC)

| CC | Hex | Description | Compat |
|---|---|---|---|
| 0 | `Bnh 00h cc` | Bank select (no action on drumset) | GS |
| 1 | `Bnh 01h cc` | Modulation wheel | MIDI |
| 5 | `Bnh 05h cc` | Portamento time | MIDI |
| 6 | `Bnh 06h cc` | Data entry (RPN/NRPN value) | MIDI |
| 7 | `Bnh 07h cc` | Volume (default=100) | MIDI |
| 10 | `Bnh 0Ah cc` | Pan (default=64 center) | MIDI |
| 11 | `Bnh 0Bh cc` | Expression (default=127) | MIDI/GM |
| 64 | `Bnh 40h cc` | Sustain (damper) pedal | MIDI |
| 65 | `Bnh 41h cc` | Portamento on/off | MIDI |
| 66 | `Bnh 42h cc` | Sostenuto pedal | MIDI |
| 67 | `Bnh 43h cc` | Soft pedal | MIDI |
| 80 | `Bnh 50h vv` | **Reverb program** (see table below) | DREAM |
| 81 | `Bnh 51h vv` | **Chorus program** (see table below) | DREAM |
| 91 | `Bnh 5Bh vv` | Reverb send level | GS |
| 93 | `Bnh 5Dh vv` | Chorus send level | GS |
| 120 | `Bnh 78h 00h` | All sound off (abrupt) | MIDI |
| 121 | `Bnh 79h 00h` | Reset all controllers | MIDI |
| 123 | `Bnh 7Bh 00h` | All notes off | MIDI |
| 126 | `Bnh 7Eh 00h` | Mono on | MIDI |
| 127 | `Bnh 7Fh 00h` | Poly on (default power-up) | MIDI |
| CC1 | `Bnh cch vvh` | Assignable controller 1 — number set via SysEx `40 1x 1F`, function via `40 2x 40-4A` | GS |
| CC2 | `Bnh cch vvh` | Assignable controller 2 — number set via SysEx `40 1x 20`, function via `40 2x 50-5A` | GS |

### Reverb program values (CC80)
`00h`=Room1 `01h`=Room2 `02h`=Room3 `03h`=Hall1 `04h`=**Hall2 (default)** `05h`=Plate `06h`=Delay `07h`=Pan delay

### Chorus program values (CC81)
`00h`=Chorus1 `01h`=Chorus2 `02h`=**Chorus3 (default)** `03h`=Chorus4 `04h`=Feedback `05h`=Flanger `06h`=Short delay `07h`=FB delay

## RPN / NRPN

| Message | Hex | Description | Compat |
|---|---|---|---|
| RPN 0000h | `Bnh 65h00h 64h00h 06h vv` | Pitch bend sensitivity, semitones (default=2) | MIDI/GM |
| RPN 0001h | `... 64h01h 06h vv` | Fine tuning, cents (00=-100, 40h=0, 7Fh=+100) | MIDI |
| RPN 0002h | `... 64h02h 06h vv` | Coarse tuning, half-tones (00=-64, 40h=0, 7Fh=+64) | MIDI |
| NRPN 0108h | `Bnh 63h01h 62h08h 06h vv` | Vibrato rate modify (40h=no modif) | GS |
| NRPN 0109h | | Vibrato depth modify (40h=no modif) | GS |
| NRPN 010Ah | | Vibrato delay modify (40h=no modif) | GS |
| NRPN 0120h | | TVF cutoff freq modify (40h=no modif) | GS |
| NRPN 0121h | | TVF resonance modify (40h=no modif) | GS |
| NRPN 0163h | | Envelope attack time modify | GS |
| NRPN 0164h | | Envelope decay time modify | GS |
| NRPN 0166h | | Envelope release time modify | GS |
| NRPN 18rrh | | Pitch coarse of drum note rr, semitones | GS |
| NRPN 1Arrh | | Level of drum note rr (0-7Fh) | GS |
| NRPN 1Crrh | | Pan of drum note rr (40h=middle) | GS |
| NRPN 1Drrh | | Reverb send level of drum note rr | GS |
| NRPN 1Errh | | Chorus send level of drum note rr | GS |
| NRPN 37xxh | | Special synth features — see [[09-SAM2695-NRPN-Messages]] | DREAM |

NRPN sending convention: `CTRL#99`=high byte, `CTRL#98`=low byte, `CTRL#6`=value. E.g. NRPN 0108h=40h → CTRL#99=1, CTRL#98=8, CTRL#6=64.

## System Exclusive

| SysEx | Description | Compat |
|---|---|---|
| `F0h 00h20h00h00h00h 12h33h77h pp vv3vv2vv1vv0 xx F7h` | Write 16-bit value to internal port pp (see [[15-SAM2695-Codec-Control-Sysex]]) | DREAM |
| `F0h 7Eh7Fh 09h01h F7h` | General MIDI reset | GM |
| `F0h 7Fh7Fh 04h01h00h ll F7h` | Master volume (ll=0-127, default 127) | GM |
| `F0h 41h00h42h12h40h 00h00h dddddddd xx F7h` | Master tune, nibblized 4-byte, default 00h04h00h00h (-100 to +100 cents) | GS |
| `... 00h04h vv xx F7h` | Master volume (default 7Fh) | GS |
| `... 00h05h vv xx F7h` | Master key-shift (default 40h, no transpose) | GS |
| `... 00h06h vv xx F7h` | Master pan (default 40h, center) | GS |
| `... 00h7Fh 00h xx F7h` | GS reset | GS |
| `... 01h10h vv1..vv16 xx F7h` | Voice reserve per part (vv1=Part10 default 2, vv2-10=Part1-9 default 2, vv11-16=Part11-16 default 0) | GS |
| `... 01h30h vv xx F7h` | Reverb type (0-7, default 04h) — same mapping as CC80 | GS |
| `... 01h31h vv xx F7h` | Reverb character (default 04h) | GS |
| `... 01h33h vv xx F7h` | Reverb master level | GS |
| `... 01h34h vv xx F7h` | Reverb time | GS |
| `... 01h35h vv xx F7h` | Reverb delay feedback (only if reverb# 6 or 7) | GS |
| `... 01h38h vv xx F7h` | Chorus type (0-7, default 02h) — same mapping as CC81 | GS |
| `... 01h3Ah vv xx F7h` | Chorus master level | GS |
| `... 01h3Bh vv xx F7h` | Chorus feedback | GS |
| `... 01h3Ch vv xx F7h` | Chorus delay | GS |
| `... 01h3Dh vv xx F7h` | Chorus rate | GS |
| `... 01h3Eh vv xx F7h` | Chorus depth | GS |
| `... 1phh 02h nn xx F7h` | MIDI channel → part assign (p=part 0-15, nn=channel 0-15, 16=OFF). Default: part 0→ch9(drums), parts1-9→ch0-8, parts10-15→ch10-15 | GS |
| `... 1phh 15h vv xx F7h` | Part-to-rhythm allocation (vv=00 sound / 01 rhythm). Default: part 0 plays drums, rest play sound | GS |
| `... 1phh 40h v1..v12 xx F7h` | Scale tuning per channel, 12 semitone offsets, -64(00h)/0(40h)/+63(7Fh) cents. No effect on rhythm parts or non-chromatic sounds | GS |
| `... 1phh 1Ah vv xx F7h` | Velocity slope (default 40h) | GS |
| `... 1phh 1Bh vv xx F7h` | Velocity offset (default 40h) | GS |
| `... 1phh 1Fh vv xx F7h` | CC1 controller number (00-5Fh, default 10h) | GS |
| `... 1phh 20h vv xx F7h` | CC2 controller number (00-5Fh, default 11h) | GS |
| `... 2phh 00h vv xx F7h` | Mod pitch control (±24 semitone, default 40h) | GS |
| `... 2phh 01h vv xx F7h` | Mod TVF cutoff control (default 40h) | GS |
| `... 2phh 02h vv xx F7h` | Mod amplitude control (±100%, default 40h) | GS |
| `... 2phh 03h vv xx F7h` | Mod LFO1 rate control (default 40h, common to all channels) | GS |
| `... 2phh 04h vv xx F7h` | Mod LFO1 pitch depth (0-600 cents, default 0Ah) | GS |
| `... 2phh 05h vv xx F7h` | Mod LFO1 TVF depth (default 00h) | GS |
| `... 2phh 06h vv xx F7h` | Mod LFO1 TVA depth (0-100%, default 00h) | GS |
| `... 2phh 10h/11h/12h vv xx F7h` | Pitch-bend pitch/TVF-cutoff/amplitude control (defaults 42h/40h/40h) | GS |
| `... 2phh 14h/15h/16h vv xx F7h` | Pitch-bend LFO1 pitch/TVF/TVA depth (defaults 00h/00h/0h) | GS |
| `... 2phh 20h/21h/22h vv xx F7h` | Channel aftertouch pitch/TVF-cutoff/amplitude (defaults 40h/40h/40h) | GS |
| `... 2phh 24h/25h/26h vv xx F7h` | Channel aftertouch LFO1 pitch/TVF/TVA depth (defaults 00h/00h/00h) | GS |
| `... 2phh 40h/41h/42h vv xx F7h` | CC1 pitch/TVF-cutoff/amplitude control (defaults 40h/40h/40h) | GS |
| `... 2phh 44h/45h/46h vv xx F7h` | CC1 LFO1 pitch/TVF/TVA depth (defaults 00h/00h/00h) | GS |
| `... 2phh 50h/51h/52h vv xx F7h` | CC2 pitch/TVF-cutoff/amplitude control (defaults 40h/40h/40h) | GS |
| `... 2phh 54h/55h/56h vv xx F7h` | CC2 LFO1 pitch/TVF/TVA depth (defaults 00h/00h/00h) | GS |

`x`/`xx` = don't-care byte (typically checksum-related but not enforced per the spec text).

## Related
- [[09-SAM2695-NRPN-Messages]]
- [[10-SAM2695-Parallel-Controls]]
- [[15-SAM2695-Codec-Control-Sysex]]
- [[12-SAM2695-Sounds-GM-MT32]]
- [[13-SAM2695-Drum-Sets]]
