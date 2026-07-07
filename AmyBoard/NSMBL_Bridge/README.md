# NSMBL_Bridge — AMYboard BLE → DIN MIDI bridge (a.k.a. "Garee")

Takes BLE MIDI in (from AUM / an iPhone) and re-transmits it out the AMYboard's
**hardware DIN MIDI OUT** jack, so the wireless Enssemble rig can drive real
hardware synths, drum machines, and modular gear.

- No synthesis, no AMY engine, no PSRAM — a pure forwarder.
- Reuses the proven BLE MIDI parser from the XIAO firmware.
- Named member: **Garee** (Gary Numan).

## Channel routing (two knobs in `config.h` / `configs/config_*.h`)

| Define        | Values                                   | Meaning                                             |
|---------------|------------------------------------------|-----------------------------------------------------|
| `BRIDGE_IN`   | `BRIDGE_ALL` (0) or a channel 1–16       | Forward every channel, or only one                  |
| `BRIDGE_OUT`  | `BRIDGE_PASSTHROUGH` (0) or a channel 1–16| Keep each message's channel, or remap ALL onto one  |

`config_Garee.h` default = **forward ALL → remap to ch1**, so fixed-channel
hardware (most gear defaults to ch1) just plays whatever you route to Garee.

## Hardware / pin map (AMYboard, ESP32-S3)

Confirmed from `tulipcc` `tulip/amyboard/pins.h` and the v1.4 schematic:

| Signal        | GPIO | Notes                                             |
|---------------|------|---------------------------------------------------|
| MIDI OUT data | 14   | UART1 TX, "Type A" data leg                        |
| MIDI OUT src  | 15   | "Type B" data leg — held HIGH here (see below)     |
| MIDI IN       | 21   | UART1 RX (not used yet)                            |
| UART          | 1    | 31250 baud, 8N1                                    |

Console is on **USB-Serial-JTAG** (native USB-C), off the MIDI UART. View logs
with `idf.py -p <port> monitor` from a real terminal.

## ⚠️ The critical finding: the OUT jack needs a firmware-driven current source

**This cost hours to find and is not obvious. Read this before touching MIDI OUT.**

The AMYboard MIDI OUT is a **3.5mm TRS jack (PJ-3410)** wired, per the v1.4
schematic, as:

```
TIP    → R37 (22Ω) → GPIO14   (data leg,  "Type A")
RING   → R38 (22Ω) → GPIO15   (other leg, "Type B")
SLEEVE → GND
```

There is **no fixed 3.3V rail on the jack.** A standard DIN MIDI input is an
**optocoupler that needs a current source** (classically DIN pin 4 held at V+).
On this board that source must be provided by **firmware holding the non-data TRS
leg HIGH.** If you only drive the UART TX pin (GPIO14) and leave GPIO15 floating,
**no current flows through the receiver's opto and you get complete silence — on
any adapter, Type A or Type B.** The MIDI is transmitting perfectly on GPIO14;
the receiver just can't "see" it without the source leg.

### The fix (in `midi_out_init()`):

```c
uart_set_pin(UART_NUM_1, MIDI_TX_PIN /*14*/, MIDI_RX_PIN /*21*/, ...);
// Hold the OTHER TRS leg high = the MIDI current source (MIDI idle level).
gpio_reset_pin(MIDI_SRC_PIN /*15*/);
gpio_set_direction(MIDI_SRC_PIN, GPIO_MODE_OUTPUT);
gpio_set_level(MIDI_SRC_PIN, 1);
```

This matches exactly what the stock tulipcc firmware does in
`tulip/shared/amy_connector.c` → `amyboard_set_midi_out()`:

> `gpio_set_level(other_pin, 1);  // hold it high (MIDI idle/source)`

### Type A vs Type B on this board

TRS type is chosen by **which leg carries data vs. source**, in firmware — not
just by the cable/adapter:

- **Type A:** `MIDI_TX_PIN = 14`, `MIDI_SRC_PIN = 15`  (default)
- **Type B:** `MIDI_TX_PIN = 15`, `MIDI_SRC_PIN = 14`

A TRS-A vs TRS-B *adapter* only swaps tip/ring downstream — it cannot substitute
for the missing source leg. Get the source leg driven first; then match the
adapter/cable to your gear's type.

## Build & flash

```bash
cp configs/config_Garee.h main/config.h        # pick the member/personality
idf.py set-target esp32s3                       # first time only
idf.py build
idf.py -p /dev/cu.usbmodem101 flash monitor     # AMYboard enumerates as native USB
```

## Verified

- ✅ BLE MIDI in → remap → DIN MIDI OUT, driving an Akai MPC over a Type-A TRS
  adapter (2026-07-07). Multi-channel source (e.g. Riffer on ch1/2/3) collapses
  to ch1 out.

## Known limits / future

- Forwards channel-voice messages only (note/CC/PC/pitch-bend/aftertouch).
  **SysEx, clock, and real-time are not passed yet.**
- MIDI **thru/merge** from the DIN IN (GPIO21) is stubbed but not wired up.
- Future: single-channel Garee variants; a Type-B variant.
