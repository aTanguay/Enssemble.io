---
tags: [firmware, sam2695, arduino, esp-idf, midi, port]
source: Seeed_Arduino_MIDIMaster / basic.ino
---

# SAM2695Synth `basic.ino` — Breakdown for ESP-IDF Porting

⬅ [[00-SAM2695-Index]]

This note dissects the Seeed `Seeed_Arduino_MIDIMaster` example (`basic.ino`) that talks to a SAM2695 chip (as used on the XIAO MIDI Synthesizer board), so it can be handed to an agent rewriting it against raw ESP-IDF instead of the Arduino framework. Byte-level MIDI details cross-reference [[11-SAM2695-MIDI-Implementation]] and [[09-SAM2695-NRPN-Messages]] in this vault.

## What the code actually does, in one sentence

It opens a **hardware UART at 31250 baud** (standard MIDI speed) wired to the SAM2695's `MIDI_IN` pin, and writes raw 2–3 byte MIDI messages to it — there is no other protocol magic. The `SAM2695Synth` class is a thin C++ wrapper that builds those bytes for you and calls `Serial.write()`.

## Line-by-line

```cpp
#include "SAM2695Synth.h"
```
Pulls in the Seeed library — a templated singleton class `SAM2695Synth<T>` where `T` is whatever serial class you're using (`HardwareSerial`, `SoftwareSerial`, `Uart`). The template exists purely so the same library source works across AVR/SAMD/RP2040/ESP32 boards with different serial implementations. **This whole templating concern disappears in ESP-IDF** — you'll just use the UART driver directly.

### The `#if defined(...)` block (lines 13–53)
This is board-detection boilerplate, not SAM2695-specific logic. It picks:
1. **Which serial peripheral** to hand to the synth (`COM_SERIAL`) — software serial on AVR/RP2040, hardware `Serial0` on ESP32-C3/C6/S3, `Serial1` on SAMD/nRF52.
2. **Which serial to use for debug/USB print** (`SHOW_SERIAL`).

For your ESP32 target, the relevant branch is:
```cpp
#if defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32C6) || defined(CONFIG_IDF_TARGET_ESP32S3)
    #define COM_SERIAL Serial0
    #define SHOW_SERIAL Serial
    SAM2695Synth<HardwareSerial> synth = SAM2695Synth<HardwareSerial>::getInstance();
#endif
```
`Serial0` = ESP32's hardware UART0. On the XIAO MIDI Synthesizer board, UART0 TX is hard-wired on the PCB to the SAM2695's `MIDI_IN` pin (pin 16, per [[02-SAM2695-Pin-Description]]). **Note:** on ESP32-S3, Arduino's `Serial` (USB-CDC) and `Serial0` (real UART0) are separate peripherals — that's *why* this split exists. If you're on a custom board (not the XIAO Synth), you need to confirm which GPIO is physically wired to `MIDI_IN` and use that as your UART TX pin in ESP-IDF; don't assume it matches the XIAO's default.

Only **TX** matters here — this example never reads from the SAM2695 (no IRQ/status handling), so RX is unused/don't-care.

### `setup()`
```cpp
SHOW_SERIAL.begin(USB_SERIAL_BAUD_RATE);       // USB-CDC debug console, irrelevant to the synth
synth.begin(COM_SERIAL, MIDI_SERIAL_BAUD_RATE); // Opens UART0 at 31250 baud, 8N1 (standard MIDI)
synth.setInstrument(0, CHANNEL_0, unit_synth_instrument_t::GrandPiano_1);
delay(1000);
```
- `MIDI_SERIAL_BAUD_RATE` is a library constant = **31250** — the fixed MIDI spec baud rate. Non-negotiable; the SAM2695's serial MIDI IN expects exactly this.
- `synth.begin()` internally just does `serial.begin(baud, SERIAL_8N1)` (or equivalent) on the given port — 8 data bits, no parity, 1 stop bit.
- `synth.setInstrument(bank, channel, value)` sends **two** raw MIDI messages back-to-back:
  1. **Bank Select MSB**: `Bnh 00h bank` (Control Change 0)
  2. **Program Change**: `Cnh value` (where `value` = `GrandPiano_1` = program 0 in the library's 0-indexed enum, i.e. GM PC 1 — see [[12-SAM2695-Sounds-GM-MT32]])
  
  `n` = channel nibble. `CHANNEL_0` is a library constant = `0x00`.
- The 1000ms `delay()` just gives the SAM2695 time to finish its power-up init routine (datasheet: **≤500ms max** before audio is ready — see [[05-SAM2695-Reset-Power-Down]]) before the first note is sent. If your firmware brings up UART late enough after chip power-on, you may be able to shorten or drop this.

### `loop()`
```cpp
synth.setNoteOn(CHANNEL_0, NOTE_E4, VELOCITY_DEFAULT);
delay(1000);
synth.setNoteOff(CHANNEL_0, NOTE_E4);
delay(1000);
```
- `setNoteOn(channel, pitch, velocity)` → raw bytes `[0x90 | channel, pitch, velocity]` (Note On — see [[11-SAM2695-MIDI-Implementation]])
- `setNoteOff(channel, pitch)` → raw bytes `[0x80 | channel, pitch, 0x00]` (Note Off, velocity is don't-care per spec but the library sends 0)
- `NOTE_E4` is a library constant for MIDI note number 64 (E4, one octave+4 semitones above middle C=60)
- `VELOCITY_DEFAULT` is typically `0x64` (100) or `0x7F` (127) — check the header if exact value matters to you; functionally any 1–127 works

**That's the entire example.** No SysEx, no NRPN, no CC beyond bank-select — this is the simplest possible "make a sound" sketch.

## Raw MIDI byte cheat-sheet for this specific sketch

| Library call | Raw bytes sent | Meaning |
|---|---|---|
| `synth.begin(serial, 31250)` | *(none — just opens UART)* | UART0, 31250 baud, 8N1 |
| `synth.setInstrument(0, 0, GrandPiano_1)` | `B0 00 00`, `C0 00` | CC0 bank=0 on ch0, then Program Change 0 (GrandPiano_1) on ch0 |
| `synth.setNoteOn(0, 64, 100)` | `90 40 64` | Note On, ch0, note 64 (E4), velocity 100 |
| `synth.setNoteOff(0, 64)` | `80 40 00` | Note Off, ch0, note 64, velocity 0 |

(Hex above — bytes shown as sent on the wire, MSB nibble = status, low nibble = channel for the status bytes.)

## What an ESP-IDF port needs to do instead

Since ESP-IDF has no Arduino `Serial` object, you replace the whole `SAM2695Synth<T>` abstraction with:

1. **UART driver init** (`driver/uart.h`):
   ```c
   uart_config_t uart_config = {
       .baud_rate = 31250,
       .data_bits = UART_DATA_8_BITS,
       .parity    = UART_PARITY_DISABLE,
       .stop_bits = UART_STOP_BITS_1,
       .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
   };
   uart_param_config(UART_NUM_1, &uart_config);   // pick a free UART, not UART0 if that's your console
   uart_set_pin(UART_NUM_1, TX_GPIO, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
   uart_driver_install(UART_NUM_1, 256, 0, 0, NULL, 0);
   ```
   `TX_GPIO` must be whatever pin your board wires to the SAM2695's `MIDI_IN` pin. RX/RTS/CTS can be `UART_PIN_NO_CHANGE` since you're not reading the SAM2695's IRQ/status line in this simple flow.

2. **Small helper functions** that mirror the library API 1:1, each just calling `uart_write_bytes()`:
   ```c
   void sam2695_note_on(uart_port_t port, uint8_t ch, uint8_t note, uint8_t vel) {
       uint8_t msg[3] = { 0x90 | (ch & 0x0F), note & 0x7F, vel & 0x7F };
       uart_write_bytes(port, (const char*)msg, 3);
   }

   void sam2695_note_off(uart_port_t port, uint8_t ch, uint8_t note) {
       uint8_t msg[3] = { 0x80 | (ch & 0x0F), note & 0x7F, 0x00 };
       uart_write_bytes(port, (const char*)msg, 3);
   }

   void sam2695_set_instrument(uart_port_t port, uint8_t bank, uint8_t ch, uint8_t program) {
       uint8_t bank_msg[3] = { 0xB0 | (ch & 0x0F), 0x00, bank & 0x7F };  // CC0 = bank select MSB
       uint8_t pc_msg[2]   = { 0xC0 | (ch & 0x0F), program & 0x7F };
       uart_write_bytes(port, (const char*)bank_msg, 3);
       uart_write_bytes(port, (const char*)pc_msg, 2);
   }
   ```

3. **Startup delay** — `vTaskDelay(pdMS_TO_TICKS(1000))` after `uart_driver_install()`, matching the datasheet's power-up timing.

4. **Optional but recommended:** if you also want the parallel-port status/IRQ handling, or want to hit the extra features (reverb/chorus/EQ/NRPN), those all go through the *same* UART using the messages catalogued in [[09-SAM2695-NRPN-Messages]] and [[10-SAM2695-Parallel-Controls]] — build them the same way (raw `uint8_t` arrays over `uart_write_bytes`). No parallel bus wiring is implied by this sketch; it only ever uses the serial MIDI path (see [[08-SAM2695-Serial-Parallel-Modes]] for why that's the SAM2695's default power-up state, so no mode-switch handshake is needed).

## Instructions for a coding agent

If you're prompting an agent to generate the ESP-IDF equivalent, give it:
- This file
- [[11-SAM2695-MIDI-Implementation]] (for the full standard MIDI + SysEx message table if they need more than note on/off/program change)
- The target GPIO number for UART TX → SAM2695 `MIDI_IN` (confirm from your board's schematic — **don't let the agent assume a XIAO default pin** unless you're literally using the stock XIAO MIDI Synthesizer board)
- Explicit constraint: **baud rate must be exactly 31250**, 8N1, and consecutive writes should respect the datasheet's minimum inter-message timing if driving the parallel bus (not relevant for pure serial MIDI, which has no documented minimum spacing beyond normal UART framing)

## Related
- [[11-SAM2695-MIDI-Implementation]]
- [[08-SAM2695-Serial-Parallel-Modes]]
- [[09-SAM2695-NRPN-Messages]]
- [[05-SAM2695-Reset-Power-Down]]
- [[02-SAM2695-Pin-Description]]
