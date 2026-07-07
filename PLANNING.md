# PLANNING.md — Enssemble / "The Band"

## Project Vision

A set of wireless BLE MIDI synthesizer units deployed spatially around a room — each one a band member with its own voice, driven wirelessly from an iPhone. Sound comes from physically located objects rather than stereo speakers.

Each unit is a band member with a single-name identity: Eenoo, Prynz, Botsee, Moroh, 8OhAte, Kneel, Dapht, Garee.

The firmware is built with **ESP-IDF** and targets two hardware platforms:
- **XIAO MIDI Synthesizer** (ESP32-C3 + SAM2695 hardware GM chip)
- **AMYboard** (ESP32-S3 + AMY software synth engine)

---

## Current Status

ESP-IDF firmware working on both AMYboard and XIAO platforms.

**XIAO platform shipped as v0.1.0** — a 5-piece XIAO band (Eenoo, Prynz, Botsee, Moroh, 8OhAte), each a single-channel BLE MIDI voice with per-member prebuilt BINs attached to the [GitHub release](https://github.com/aTanguay/Enssemble.io/releases/tag/v0.1.0). The long SAM2695 "patch won't change" bug is solved (root cause: ESP-IDF console was on UART0, the SAM's MIDI port). Three XIAO units flashed so far (Prynz, Botsee, 8OhAte); Eenoo + Moroh pending hardware.

Three AMYboards available for spatial deployment. Mozaic iOS control surface replaces the planned TouchOSC phase. Next: flash the remaining units and run the first multi-unit spatial jam.

### Firmware Variants
| Firmware | Directory | Platform | Purpose | Status |
|----------|-----------|----------|---------|--------|
| NSMBL_Synth | `AmyBoard/NSMBL_Synth/` | AMYboard (ESP32-S3) | AMY synth engine (Juno, DX7, drums) | ✅ Verified |
| NSMBL_SampleKits | `AmyBoard/NSMBL_SampleKits/` | AMYboard (ESP32-S3) | 16-slice WAV sample player (SD card) | ✅ Verified |
| NSMBL_Synth | `SeeedXiaoMIDI/NSMBL_Synth/` | XIAO (ESP32-C3) | SAM2695 hardware GM wavetable synth | ✅ Verified |
| NSMBL_Bridge | `AmyBoard/NSMBL_Bridge/` | AMYboard (ESP32-S3) | BLE→DIN MIDI bridge (Garee); channel filter/remap | ✅ HW-verified (drives Akai MPC) |

Band member identity is set via `config.h` — copy a named config from `AmyBoard/configs/`
(AMY) or `SeeedXiaoMIDI/NSMBL_Synth/configs/` (XIAO) before building. XIAO members are
single-channel: Eenoo ch1, Prynz ch2, Botsee ch3, Moroh ch5, 8OhAte ch10.

### iOS Control Surface (Mozaic)
| Script | Purpose | Status |
|--------|---------|--------|
| `NSMBL_EEnoo_Controller.moz` | AMYboard: patch browser + sound shaper + panic | ✅ Working |
| `NSMBL_XIAO_Controller.moz` | XIAO: Mix layout with XY filter, NRPNs, named FX types | ✅ Working |
| `NSMBL_EEnoo_PatchSelect.moz` | Simple patch bank selector (superseded) | ✅ Working |
| `NSMBL_EEnoo_SoundShaper.moz` | CC knobs only (superseded) | ✅ Working |

### NSMBL CC Map (firmware, all channels)
| CC | Function | Range |
|----|----------|-------|
| 7 | Volume | 0-127 linear |
| 10 | Pan | 0=L, 64=center, 127=R |
| 70 | Filter Cutoff | Exponential ~13Hz-6400Hz |
| 71 | Resonance | Exponential Q 0.5-16 |
| 75/93 | Chorus Level | 0=dry, 127=full |
| 91 | Reverb Level | 0=dry, 127=full |

AMY handles internally: CC 0 (bank select), CC 64 (sustain), CC 123 (all notes off).

### The Band — Full Roster
*Names riff on famous musicians. Configs in `AmyBoard/configs/` (AMY) and `SeeedXiaoMIDI/NSMBL_Synth/configs/` (XIAO).*



| Name       | Inspiration     | MIDI Ch | Role                 | Default Patch | Firmware     | Xiao Status | Amy Status |
| ---------- | --------------- | ------- | -------------------- | ------------- | ------------ | ----------- | ---------- |
| **Eenoo**  | Brian Eno       | 1       | Pads/Ambient         | Synth Pad     | Amy and Xiao | ✅ Ready     |            |
| **Prynz**  | Prince          | 2       | Lead                 | Synth Leads   | Amy and Xiao | ✅ Ready     |            |
| **Botsee** | Bootsie Collins | 3       | Bass                 | Synth Bass    | Amy and Xiao | ✅ Ready     |            |
| **Moroh**  | Giorgio Moroder | 5       | Arps / Sequences     | Synth Lead    | Amy and Xiao | ✅ Ready     |            |
| **8OhAte** | The 808         | 10      | Drums                | GM Kits       | Xiao Only    | ✅ Ready     |            |
| **Kneel**  | Neal Peart      | 10      | Drums                | Sample Kits   | Amy Only     |             |            |
| **Dapht**  | Daft Punk       | 4       | General Samples      | TBD           | Amy Only     |             |            |
| **Garee**  | Gary Numan      | ALL     | MIDI Out to Hardware | None          | Amy Only     |             |            |

Named configs in `AmyBoard/configs/` (AMY) and `SeeedXiaoMIDI/NSMBL_Synth/configs/` (XIAO)
— copy one to `main/config.h` before flashing each board.



---

## Architecture

```
iPhone (AUM + Helium / TouchOSC)
        |
        | BLE MIDI 1.0 — one connection per unit
        | iOS confirmed: 13+ simultaneous connections possible
        | Latency: 3-15ms per unit
        |
        +-- Channel 1 --> Eenoo (Pads/Ambient)
        +-- Channel 2 --> Prynz (Lead)
        +-- Channel 3 --> Botsee (Bass)
        +-- Channel 4 --> Dapht (Samples) [Amy]
        +-- Channel 5 --> Moroh (Arps/Sequences)
        +-- Channel 10 -> 8OhAte (Drums, Xiao) / Kneel (Drums, Amy)
        +-- All chans -> Garee (MIDI out to hardware) [Amy]
                |
                v
AMYboard (ESP32-S3, dual-core):
  Core 0:
    +-- NimBLE BLE MIDI stack
    +-- MIDI message parsing & channel filter
    +-- Connection management
    +-- Button handler
  Core 1:
    +-- AMY synth engine (audio rendering)
    +-- I2S DAC output
  Shared (FreeRTOS queue between cores):
    +-- MIDI events from Core 0 -> Core 1
    +-- Per-unit config (device name, channel, patch)
    +-- Boot sound on startup

XIAO (ESP32-C3, single-core):
  +-- BLE MIDI stack (NimBLE) with running status support
  +-- SAM2695 via UART @ 31250 baud (128 GM + drum kits)
  +-- Full CC/NRPN passthrough (filter, envelope, vibrato, FX types)
  +-- 3.5mm stereo + onboard Class-D amp
```

---

## Phases

### Phase 1 — AMYboard BLE MIDI + Sample Player
*AMYboard first. Get BLE MIDI receiving on one core, AMY playing on the other.*

- [ ] ESP-IDF project structure with CMake (target: ESP32-S3)
- [ ] NimBLE BLE MIDI peripheral (advertise, connect, receive MIDI) — pinned to Core 0
- [ ] MIDI message parsing (Note On/Off, CC, Program Change, Pitch Bend)
- [ ] AMY synth engine integration — pinned to Core 1
- [ ] FreeRTOS queue for MIDI events between cores
- [ ] Simple sample player exposing AMY's PCM playback via MIDI notes
- [ ] I2S audio output configuration
- [ ] Per-unit configuration (device name, MIDI channel, default patch)
- [ ] Boot sound on startup
- [ ] Verified working: AUM connects, notes trigger samples, audio out confirmed

### Phase 2 — Multi-Unit & UX (AMYboard)
*Multiple AMYboard units with proper channel separation and standalone usability*

- [ ] Per-unit MIDI channel filtering
- [ ] Full CC passthrough (volume, pan, expression, sustain, mod, effects)
- [ ] Pitch Bend and Aftertouch passthrough
- [ ] Voice/patch selection via AMY
- [ ] Curated preset system per band role
- [ ] Audition note on voice change
- [ ] Favorite voice slots stored in NVS flash
- [ ] Reconnection handling — auto re-advertise after iOS disconnects
- [ ] Button input handler
- [ ] Multi-unit simultaneous operation verified

### Phase 2b — XIAO Platform ✅
*BLE MIDI on XIAO ESP32-C3 + SAM2695 hardware GM synth*

- [x] SAM2695 UART driver component (31250 baud)
- [x] Single-core architecture adapted for ESP32-C3 (RISC-V)
- [x] BLE MIDI parser with running status support
- [x] Full CC/NRPN passthrough to SAM2695
- [x] SAM2695 MIDI reference documented (`Docs/SAM2695_MIDI_Reference.md`)
- [x] Mozaic XIAO controller (Layout 0 Mix — XY filter, NRPNs, named FX types)

### Phase 3 — ~~TouchOSC~~ Mozaic Control Surface ✅
*Replaced TouchOSC plan with Mozaic iOS scripts — simpler, more flexible, already working.*

- [x] Patch browsing (prev/next per channel)
- [x] Sound shaping knobs (filter, resonance, reverb, chorus)
- [x] Volume/Pan mode toggle
- [x] Channel selector (Juno/DX7/Drums)
- [x] Sustain toggle
- [x] CC reset
- [x] MIDI Panic (all sound off, all channels)
- [ ] Multi-unit page — one Mozaic instance per band member

### Phase 4 — Deep Synth Control
*Platform-specific advanced features*

- [ ] XIAO/SAM2695: SysEx for 4-band stereo EQ, master volume/tuning
- [ ] AMYboard/AMY: Custom patches, FM synthesis, filter control
- [ ] I2C encoder/OLED UI for physical control (AMYboard)

### Phase 5 — "The Band" Physical System
*3D printed band members for room-scale deployment*

- Enclosure design — abstract sculptural forms, textile/fabric aesthetic
- LiPo battery integration for fully wireless operation
- Upgraded speakers housed in acoustic chambers
- Power on -> boot sound -> advertise -> AUM connects -> play

---

## Technical Notes

### BLE MIDI
- BLE MIDI 1.0 spec over NimBLE (ESP-IDF component, not Arduino library)
- Each unit is an independent BLE peripheral
- iOS handles 13+ simultaneous connections
- Latency: 3-15ms per unit

### SAM2695 (XIAO platform)
- Needs ~500-600ms after power-up before accepting MIDI
- Pitch Bend must be sent as raw MIDI bytes
- Bank Select (CC 0) must precede Program Change; only bank 0 (GM) / 127 (MT-32) valid
- Channel indexing: BLE MIDI uses 1-16, SAM2695 uses 0-15
- Drum channel (10) volume defaults low — set explicitly

### ⚠️ CRITICAL: keep the ESP-IDF console OFF UART0 (XIAO+SAM2695)
UART0 is the SAM2695's MIDI port. If the ESP-IDF console stays on UART0 (the
default!), every `ESP_LOGI` is transmitted to the chip as garbage MIDI and
corrupts its state — program changes stop working and every note plays one
stuck bell-like sound (intermittent, correlates with logging). This cost us a
full debugging session. Guard rails:
- `sdkconfig.defaults` must set `CONFIG_ESP_CONSOLE_UART_DEFAULT=n`,
  `CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG=y`, `CONFIG_ESP_CONSOLE_SECONDARY_NONE=y`
- Editing `sdkconfig.defaults` does nothing if `sdkconfig` already exists —
  delete `sdkconfig` and rebuild to apply it
- Verify the built `sdkconfig` shows `CONFIG_ESP_CONSOLE_UART_NUM=-1` (NOT 0)
- The Arduino library avoids this by keeping debug on USB `Serial` + synth on `Serial0`

### Hardware — XIAO ESP32-C3
- Button pins (active LOW, internal pullup): GPIO 5, 4, 3, 2
- SAM2695 on UART0 at 31250 baud (TX=21, RX=20) — console MUST be on USB-JTAG, see above
- LED = GPIO 10, active LOW
- Patch nav buttons: 0/1 = family jump (coarse), 2/3 = single patch (fine)

### Hardware — AMYboard ESP32-S3
- AMY software synth engine (250 oscillators, dual-core rendering)
- I2S audio output (32-bit stereo via onboard DAC)
- 8MB octal PSRAM for samples and synth data
- SD card (SPI: MOSI=11, MISO=13, CLK=12, CS=10)
- I2S pins: MCLK=3, BCLK=8, WS=2, DOUT=6
- I2C port: SDA=17, SCL=18 (for Modulino accessories)
- MIDI out: GPIO 14 (Type A) / GPIO 15 (Type B)
- MIDI in: GPIO 21
- Line out: 3.5mm TRS stereo
- S/PDIF in/out, CV in/out (2x each)

### iOS BLE Limits
- iOS handles 13+ simultaneous BLE connections
- iPad Pro tested to 15 concurrent
- 4-5 band members is comfortably within limits
- Each unit is independent — no bandwidth competition

---

## Session Log

### 2026-07-07 — Garee bridge live: BLE → DIN MIDI drives real hardware
- **Scaffolded `AmyBoard/NSMBL_Bridge/` (Garee)** — a pure BLE→DIN MIDI forwarder
  (no AMY engine). Reuses the XIAO BLE parser; new `midi_out.c` re-serializes to
  UART1. Configurable `BRIDGE_IN`/`BRIDGE_OUT`; default = forward ALL, remap to ch1.
  Console on USB-Serial-JTAG (off the MIDI UART).
- **Solved a hard, non-obvious hardware bug: no MIDI reached the Akai MPC.** The
  firmware was transmitting fine on GPIO14 (confirmed via per-note logs), and the
  MPC/cable were proven good — yet silence on both Type-A and Type-B adapters.
- **Root cause (from the v1.4 schematic in `Docs/Amyboard/tulipcc-main`):** the OUT
  jack is TRS `tip=GPIO14 (data), ring=GPIO15, sleeve=GND` with **no 3.3V rail**.
  A DIN MIDI opto needs a current source, which on this board must be **firmware
  holding the non-data leg (GPIO15) HIGH**. We only drove GPIO14 → opto never
  conducted. Fix matches stock `amyboard_set_midi_out()`:
  `gpio_set_direction(15,OUTPUT); gpio_set_level(15,1);`. Type A = TX14/SRC15,
  Type B = TX15/SRC14. **Verified live driving the MPC.** Full write-up in the
  bridge README + memory `reference-amyboard-midi-trs`.

### 2026-07-06 — XIAO band shipped: v0.1.0, 5 single-channel members
- **Cut the v0.1.0 GitHub release** with a merged, flash-at-0x0 BIN (no toolchain needed).
- **Synced the whole roster naming** across docs + configs: Eenoo, Prynz, Botsee
  (Bootsie Collins), Moroh (Giorgio Moroder), 8OhAte, Kneel, Dapht, Garee. Renamed
  AMY + XIAO configs to match.
- **Made each XIAO unit single-channel** so it responds only to its own port (synths on
  ch1/2/3/5 via DRUM_CHANNEL disabled; 8OhAte on ch10 = GM drums). Boot flourish is now
  conditional: drummers play the beat, synths play a hello note in their own voice.
- **Built per-member BINs** (Eenoo/Prynz/Botsee/Moroh/8OhAte) and attached them to the
  release. Flashed 3 units live: Prynz, Botsee, 8OhAte (hearing real drums!).
- Review pass on the XIAO firmware: fixed panic to release sustain (CC64=0), tidied stale
  comments; deferred items logged (audition blocking the MIDI task needs a UART mutex first).

### 2026-07-02 — XIAO patch-nav MVP + the UART0/console bug
- **Root-caused the "patch never changes / stuck bell" bug**: the ESP-IDF debug
  console was on UART0 — the SAM2695's MIDI port. Log text was being transmitted
  to the chip as garbage MIDI and corrupting its state. Fix: force console to
  USB-Serial-JTAG, delete stale `sdkconfig`, rebuild (`CONFIG_ESP_CONSOLE_UART_NUM=-1`).
  See the CRITICAL warning in Technical Notes. Verified on two units.
- Chased many red herrings first (bank clamp, timing gaps, GM-On, wavetable
  delay) — all band-aids; the console was the real cause. GM-On kept as good practice.
- Confirmed the SAM2695 is multitimbral; unit listens on ch1 (melody) + ch10 (drums),
  ignores other channels (host routes). Buttons own patch selection.
- Patch nav: buttons 0/1 jump by GM family (coarse, 16 voices), 2/3 step by one
  patch (fine). Each press auditions a single C4 preview note.
- Also fixed earlier this session: BLE running-status parser, GM/MT-32 bank
  handling, button hardening (gpio_reset_pin + debounce), volume tracking.

### 2026-07-01 — XIAO Platform: SAM2695 Reference, Mozaic Controller, BLE Parser Fix
- Extracted full SAM2695 MIDI reference from chip spec PDF → `Docs/SAM2695_MIDI_Reference.md`
- Documented all CCs, reverb/chorus types, NRPNs (filter, envelope, vibrato), RPNs, SysEx deep params, 4-band EQ
- Created Mozaic XIAO controller (`NSMBL_XIAO_Controller.moz`) — Layout 0 (Mix)
  - XY pad: filter cutoff + resonance via NRPN
  - 10 knobs: volume, expression, reverb/chorus send+type (with named labels), attack, release, vibrato rate/depth
  - 8 pads: patch/bank nav, sustain, CC reset, PANIC
- Rewrote BLE MIDI parser (`ble_midi.c`) — proper running status support
  - Old parser had duplicate timestamp-skip blocks that broke when iOS omitted status bytes
  - Timestamps were misinterpreted as status bytes, corrupting CCs into phantom Note Offs
  - New parser tracks running_status, uses data_bytes_for_status() lookup, handles SysRT/SysEx
  - Added raw hex dump at DEBUG level for troubleshooting
- Confirmed XIAO firmware forwards all CCs to SAM2695 — no firmware changes needed for new CC types

### 2026-06-30 — NSMBL_EEnoo + Mozaic Control Surface
- Integrated AMY synth engine as ESP-IDF component (250 oscillators)
- AMY handles I2S, dual-core rendering, and voice management internally
- Default synths: Juno-6 (ch1), DX7 (ch2), GM drums (ch10)
- All-channel MIDI pass-through (no channel filter)
- Reorganized project: `AmyBoard/NSMBL_SmplCty/` and `AmyBoard/NSMBL_EEnoo/`
- Custom NSMBL CC handler replacing AMY's Juno-only example — filter, resonance, reverb, chorus, volume, pan on all channels
- Mozaic iOS control surface: learned API from scratch, built 3 scripts
- Unified Controller script: patch browsing, sound shaping, sustain, panic
- Phase 3 (TouchOSC) rendered unnecessary by Mozaic approach
- Three AMYboards available for first spatial composition test

### 2026-06-29 — NSMBL_SmplCty (Sample Player) Working
- First ESP-IDF firmware from scratch — BLE MIDI + sample playback
- WAV cue point parser for 16-slice drum kits
- 4-voice polyphony with velocity scaling, I2S output
- SD card with FAT32 long filename support
- End-to-end verified: iPhone → AUM → BLE → 808 drums

### 2026-06-29 — ESP-IDF Migration Started
- Decision: move from Arduino to ESP-IDF for both XIAO and AMYboard platforms
- Updated all project documentation for new framework
- Arduino-era prototype code preserved in git history

### 2026-06-26 — Multi-Unit Success (Arduino era)
- Dual-unit operation achieved: EEnoo + Ryngo both connected to AUM
- Drums sequenced via Helium on channel 10
- This validated the spatial multi-unit concept
