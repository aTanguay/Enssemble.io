---
tags: [firmware, sam2695, midi, nrpn]
---

# SAM2695 — Special NRPN Messages

⬅ [[00-SAM2695-Index]]

Works in **either** serial or parallel mode (received on serial MIDI in serial mode, or on the 8-bit data port in parallel mode).

## How the NRPN encoding works
Special messages use standard MIDI NRPN. Mostly under NRPN high byte `037h`. Example — set master volume via NRPN `3707h`:

```
NRPN high = 037h:  CC 99 (63h) = 55 (37h)  → 0B0h 063h 037h
NRPN low  = 07h:   CC 98 (62h) = 07 (07h)  → 0B0h 062h 07h
NRPN value = vv:   CC 6  (06h) = vv        → 0B0h 006h vv
```
`vv` = master volume, range 0–127 (0–7Fh).

For reverb/chorus, use the standard GM SysEx messages in [[11-SAM2695-MIDI-Implementation]] instead of NRPN where noted.

## Full NRPN table

| NRPN # | Description | Range/notes | Power-up default |
|---|---|---|---|
| 3700h | EQ low band (bass) | 0=-12dB, 40h=0dB, 7Fh=+12dB | 60h |
| 3701h | EQ med-low band | 0=-12dB, 40h=0dB, 7Fh=+12dB | 40h |
| 3702h | EQ med-high band | 0=-12dB, 40h=0dB, 7Fh=+12dB | 40h |
| 3703h | EQ high band (treble) | 0=-12dB, 40h=0dB, 7Fh=+12dB | 60h |
| 3707h | **Master volume** | 0–7Fh | 7Fh |
| 3708h | EQ low cutoff freq | 0=0Hz, 7Fh=4.7kHz | 0Ch |
| 3709h | EQ med-low cutoff freq | 0=0Hz, 7Fh=4.2kHz | 1Bh |
| 370Ah | EQ med-high cutoff freq | 0=0Hz, 7Fh=4.2kHz | 72h |
| 370Bh | EQ high cutoff freq | 0=0Hz, 7Fh=18.75kHz | 40h |
| 3713h | Clipping mode select | 0=soft clip, 7Fh=hard clip | 00h |
| 3715h | GM reverb send | 0=no send, 40h=default, 7Fh=max | 40h |
| 3716h | GM chorus send | 0=no send, 40h=default, 7Fh=max | 40h |
| 3718h | Post effects applied on GM | 0=off, 7Fh=on¹ | 7Fh |
| 3719h | Post effects applied on Mike | 0=off, 7Fh=on¹ | 00h |
| 371Ah | Post effects applied on Reverb/Chorus | 0=off, 7Fh=on¹ | 7Fh |
| 3720h | Spatial effect volume² | 0=no effect, 7Fh=max | 00h |
| 3722h | GM volume | 0–7Fh | 7Fh |
| 3723h | GM pan | 0=left, 40h=center, 7Fh=right | 40h |
| 3724h | Mike volume | 0–7Fh | 40h |
| 3726h | Mike pan | 0=left, 40h=center, 7Fh=right | 40h |
| 3728h | Mike echo level | 0–7Fh | 7Fh |
| 3729h | Mike echo time | 0=shortest, 7Fh=longest (380ms) | 2Bh |
| 372Ah | Mike echo feedback | 0=none, 7Fh=max | 42h |
| 372Ch | Spatial effect delay² | 0=shortest, 7Fh=longest | 1Dh |
| 372Dh | Spatial effect input² | 0=stereo, 7Fh=mono | 00h |
| 3730h/3731h | Slave1 echo vol right/left | 0–7Fh (see [[16-SAM2695-Appendix]]) | 00h |
| 3732h/3733h | Slave2 echo vol right/left | 0–7Fh | 00h |
| 3734h/3735h | Master echo vol right/left | 0–7Fh | 7Fh |
| 3751h | Auto-test | see [[16-SAM2695-Appendix]]#auto-test | – |
| 3757h | System exclusive device ID | 0–1Fh, 20h=all accepted | 20h |
| 375Fh | **Effect ON/OFF + polyphony select** | bitfield — see [[14-SAM2695-Polyphony-Effects-Select]] | – |

¹ Post effects = Spatial Effect + Equalizer, applied per-module.
² See spatial effect block diagram in [[16-SAM2695-Appendix]].

## Notable detail groups

**MASTER_VOL** (3707h): data 0–7Fh, default 7Fh.

**GM_VOL/GM_PAN** (3722h/3723h): GM_PAN 0=hard left, 40h=center, 7Fh=hard right — same as GM SysEx `40h 00h 06h`.

**SUR_VOL/SUR_DEL/SUR_INP** (spatial effect, 3720h/372Ch/372Dh): SUR_INP=0 → stereo/stereo-wide (delay line input = left−right); SUR_INP=7Fh → mono/pseudo-stereo (delay line input = left+right).

**xxx_POST routing** (3718h/3719h/371Ah): post effects = Spatial Effect + Equalizer, toggled independently per module, but shared parameters (EQ_xxx, EQF_xxx, SUR_VOL, SUR_DEL, SUR_INP) apply globally once post-effect is enabled on a module.

**Mike & Echo device** — requires Mike/Echo first switched ON via NRPN `375Fh` (see [[14-SAM2695-Polyphony-Effects-Select]]):
- ECH_LEV (3728h): 0–7Fh, default 7Fh
- ECH_TIM (3729h): 0=shortest → 7Fh=longest (380ms), default 2Bh
- ECH_FEED (372Ah): 0=no feedback → 7Fh=max, default 42h
- ECHx_LEFT/RIGHT pan controls: default is stereophonic **triple echo** — heard successively left, center, right. To force a mono echo in center: mute both slave echoes (`ECHS1_RIGHT=0, ECHS1_LEFT=0, ECHS2_RIGHT=0, ECHS2_LEFT=0`) and set `ECHM_RIGHT=7Fh, ECHM_LEFT=7Fh`. Full timing diagram in [[16-SAM2695-Appendix]].

## Related
- [[10-SAM2695-Parallel-Controls]] — same functions, parallel-mode-only byte form
- [[11-SAM2695-MIDI-Implementation]] — SysEx equivalents for reverb/chorus
- [[14-SAM2695-Polyphony-Effects-Select]]
