---
tags: [firmware, sam2695, appendix]
---

# SAM2695 — Appendix

⬅ [[00-SAM2695-Index]]

## 8-1: Instruments requiring 2 voices (2-layer instrument)
These GM program numbers use two internal voices each — factor this into polyphony budgeting alongside [[14-SAM2695-Polyphony-Effects-Select]].

PC: 4, 5, 6, 9, 10, 11, 15, 19, 20, 22, 24, 31, 37, 40, 52, 54, 55, 56, 61, 63, 64, 73, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 91, 92, 93, 94, 95, 97, 98, 99, 100, 101, 102, 104, 113, 115, 118, 127

(Cross-reference names in [[12-SAM2695-Sounds-GM-MT32]] — e.g. PC4=Honky-tonk Piano, PC81-88=all Lead synths, PC89-95=most Pads, PC97-104=most FX sounds.)

## 8-2: Signal processing synoptic

```
Mike ──echo──→ [Echo] ─┐
                         ├─(output selector)─┐
MIDI ──main out─────────┤                     ├─→ [EQ] → [3D] → Output (L/R)
MIDI ──reverb/chorus──→ [Reverb/Chorus] ─────┘
```
Thick line = stereo bus, thin line = mono bus. The output selector determines whether main/reverb/echo paths are summed pre- or post-EQ+3D stage.

## 8-3: Spatial effect block diagram

```
Left In  ──┬────────────────────────────────→ (+) → Left Out
           │                                    ↑
Input ─────┤+/- mux (NRPN 372Dh) → Delay (372Ch) → Volume (3720h) ──┬──→ ×(-1) ──→ (+) → Right Out
           │                                                          │
Right In ──┴──────────────────────────────────────────────────────────┘
```
Input mux: stereo mode feeds `Left − Right` into the delay line; mono mode feeds `Left + Right`. Delay output is scaled by volume, then added directly to Left and inverted before adding to Right — this is what creates the stereo widening.

## 8-4: Microphone echo structure
Dream's echo is **3 taps**: one master echo plus two "slave" echoes derived from it.

- Master echo time **T** set by NRPN `3729h`; feedback by NRPN `372Ah`
- **Slave1 = Master − 2T/3**
- **Slave2 = Master − T/3**

Timeline (arrows = echo pulses, height = relative volume within that repeat cycle):
```
0 ─── Slave1 ── Slave2 ── Master(T) ────────── 2T ────────── 3T
      (T/3 apart from each other, converging toward Master at T)
```

Volume of Master/Slave1/Slave2 set independently (L/R) via NRPN `3730h`–`3735h` (see [[09-SAM2695-NRPN-Messages]]). For **mono echo** (preset 0): mute both slaves (`ECHS1_RIGHT=0, ECHS1_LEFT=0, ECHS2_RIGHT=0, ECHS2_LEFT=0`), then set `ECHM_RIGHT=7Fh, ECHM_LEFT=7Fh`.

## 7: Auto-test
Built-in self-test for board production testing.

**Start:** send NRPN `3751h = 23h`

Sine waveforms output to the DAC indicate test progress:

| Test in progress | Output frequency |
|---|---|
| On-chip RAM | 1.18 kHz |
| On-chip ROM | 876 Hz |
| **PASS** | **295 Hz** |

If you measure 295 Hz on the DAC output, the part is OK.

## Related
- [[09-SAM2695-NRPN-Messages]]
- [[12-SAM2695-Sounds-GM-MT32]]
- [[14-SAM2695-Polyphony-Effects-Select]]

---
*Source: Dream SAM2695 datasheet & user's manual, rev. June 22 2015. © 2013-2014 Dream S.A.S. France. info@dream.fr / dream.fr*
