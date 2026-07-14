---
tags: [hardware, midi, synth, sam2695, dream, moc]
chip: SAM2695
manufacturer: Dream S.A.S. France
package: QFN48 6x6mm
datasheet_date: 2015-06-22
---

# SAM2695 — Index

Low power single-chip GM synthesizer with effects and built-in CODEC, from Dream S.A.S. France. Used as the synth chip in [[AMYboard]] and similar Eurorack/MIDI projects.

Split out of the monolithic PDF into linked notes:

## Hardware
- [[01-SAM2695-Overview]] — features, block diagram, typical config
- [[02-SAM2695-Pin-Description]] — pinout by function & by pin number, mechanical package
- [[03-SAM2695-Electrical-Characteristics]] — absolute max ratings, DC/DAC/ADC specs
- [[04-SAM2695-Timings]] — 8-bit parallel interface read/write timing
- [[05-SAM2695-Reset-Power-Down]] — reset sequence, pin states in power-down
- [[06-SAM2695-Reference-Circuits]] — crystal, line out, headphone out, mic in
- [[07-SAM2695-Board-Layout]] — layout rules (GND/decoupling/crystal/analog)

## Firmware / MIDI control
- [[08-SAM2695-Serial-Parallel-Modes]] — UART vs 8-bit parallel host interface
- [[09-SAM2695-NRPN-Messages]] — full NRPN control list (EQ, reverb/chorus routing, echo, spatial)
- [[10-SAM2695-Parallel-Controls]] — CONTROL-register byte commands (parallel mode)
- [[11-SAM2695-MIDI-Implementation]] — standard MIDI messages, RPN/NRPN/SysEx table
- [[12-SAM2695-Sounds-GM-MT32]] — GM program list + MT-32 variation bank
- [[13-SAM2695-Drum-Sets]] — channel 10 drum maps (Standard/Power/Brush/Orchestra/CM-64)
- [[14-SAM2695-Polyphony-Effects-Select]] — NRPN 375Fh bitfield, polyphony cost table
- [[15-SAM2695-Codec-Control-Sysex]] — raw port-write SysEx, codec control registers, gain tables
- [[16-SAM2695-Appendix]] — 2-voice instrument list, signal flow, spatial effect & echo diagrams

## Software / porting
- [[17-SAM2695-basicino-ESP-IDF-Breakdown]] — Seeed `SAM2695Synth` Arduino example dissected line-by-line, with raw MIDI byte equivalents and an ESP-IDF UART driver skeleton for porting off Arduino

## Quick reference
- Host interface: 8-bit parallel (D0-D7, A0, CS/, RD/, WR/, IRQ) **or** serial MIDI IN — see [[08-SAM2695-Serial-Parallel-Modes]]
- Master volume NRPN: `3707h` (0-7Fh) — see [[09-SAM2695-NRPN-Messages]]
- Reset all to power-up defaults: NRPN `375Fh` = `45h` — see [[14-SAM2695-Polyphony-Effects-Select]]
- Crystal: 12 MHz (XDIV tied to VD33) or 9.6 MHz (XDIV grounded), 22pF loading caps — see [[06-SAM2695-Reference-Circuits]]
- Write cycle time must be ≥ 3.5 µs — see [[04-SAM2695-Timings]]

## Source
Dream SAM2695 datasheet + user's manual, rev. June 22 2015, © Dream S.A.S. France. `info@dream.fr` / http://www.dream.fr
