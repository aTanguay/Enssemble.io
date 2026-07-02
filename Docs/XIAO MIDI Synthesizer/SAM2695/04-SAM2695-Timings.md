---
tags: [hardware, sam2695, timing]
---

# SAM2695 — Timings (8-bit parallel interface)

⬅ [[00-SAM2695-Index]]

Slave 8-bit parallel interface, typically used to connect the chip to a host processor (A0, CS/, RD/, WR/, D0-D7, IRQ).

## Read cycle signal order
`A0` valid → `CS/` low → `RD/` low → data valid on `D0-D7` → `RD/` high → `CS/` high

## Write cycle signal order
`A0` valid → `CS/` low → `WR/` low, data setup → `WR/` high → `CS/` high

## Timing table

| Parameter | Symbol | Min | Max | Unit |
|---|---|---|---|---|
| Address valid to chip select low | tavcs | 0 | – | ns |
| Chip select low to RD/ low | tcslrdl | 5 | – | ns |
| RD/ high to CS/ high | trdhcsh | 5 | – | ns |
| RD/ pulse width | tprd | 50 | – | ns |
| Data out valid from RD/ | trdldv | – | 20 | ns |
| Data out hold from RD/ | tdrh | 5 | 10 | ns |
| Chip select low to WR/ low | tcslwrl | 5 | – | ns |
| WR/ high to CS/ high | twrhcsh | 5 | – | ns |
| WR/ pulse width | tpwr | 50 | – | ns |
| Write data setup time | tdws | 10 | – | ns |
| Write data hold time | tdwh | 0 | – | ns |
| Write cycle | twrcyc | 3.5 | – | µs |

## Notes
- **When data is pending** on the parallel port, the host must read it within **1 ms**, or the parallel port is deactivated. Reactivate with control sequence: `0FFh` (close port) then `03Fh` (open port).
- **For safe operation, write cycle time must not be lower than 3.5µs** — this is the hard floor for back-to-back parallel writes.

## Related
- [[08-SAM2695-Serial-Parallel-Modes]] — status register (TE/RF bits) referenced during these cycles
- [[02-SAM2695-Pin-Description]]
