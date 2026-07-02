---
tags: [firmware, sam2695, midi]
---

# SAM2695 — Serial & Parallel MIDI Modes

⬅ [[00-SAM2695-Index]]

The SAM2695 can be controlled from **either** the parallel interface (D0-D7, CS/, WR/, RD/, IRQ) **or** the serial MIDI interface (MIDI IN).

## Parallel interface registers

| I/O address | Write from host | Read to host |
|---|---|---|
| A0=0 | DATA8 | DATA8 |
| A0=1 | CONTROL | STATUS |

### Status register bits

| 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|---|---|---|---|---|---|---|---|
| TE | RF | X | X | X | X | X | X |

- **TE (Transmit Empty):** 0 = data pending from SAM2695 to host, IRQ is high. Reading DATA8 (A0=0) sets TE→1, IRQ→low.
- **RF (Receiver Full):** 0 = SAM2695 ready to accept CONTROL or DATA. Minimum time between two consecutive writes must still be **3.5µs** even if RF is not set (see [[04-SAM2695-Timings]]).

## Serial mode
Default after power-up, hardware reset, or parallel reset control. Parallel MIDI interface inactive, IRQ line floating. Serial MIDI IN drives the synth directly.

In serial mode the 8-bit parallel interface still accepts two controls:
- **`3Fh`** → switch to parallel mode. SAM2695 acks by sending `FEh` back in DATA8.
- **`BEh`** → "enable control", allows sending exactly **one** parallel control while in serial mode. Each control sent this way must be prefixed with `BEh` again.

## Parallel mode
- All data on the serial MIDI IN pin is passed through to the host via DATA8, but **not** sent to the synth.
- MIDI data written by host to DATA8 **is** sent to the synth.
- Accepts:
  - **`FFh`** (parallel mode reset) → switches back to serial mode.
  - Additional controls (see [[10-SAM2695-Parallel-Controls]]) — independent of the MIDI data stream, so they can be interleaved mid-flow.

## Diagram (data flow)

```
Serial mode:
  DATA8 IN (host) ──────────┐
                              → SYNTHESIS
  Serial MIDI IN ────────────┘
  (DATA8 OUT to host: unused)

Parallel mode:
  DATA8 IN (host) ──────────→ SYNTHESIS
  Serial MIDI IN ────────────→ DATA8 OUT (to host)
```

## Related
- [[09-SAM2695-NRPN-Messages]] — usable in either mode
- [[10-SAM2695-Parallel-Controls]] — parallel-mode-only control bytes
- [[04-SAM2695-Timings]]
