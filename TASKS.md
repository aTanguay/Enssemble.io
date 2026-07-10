# TASKS.md — Current Priorities

## Phase 1 — AMYboard BLE MIDI Firmware ✅

### NSMBL_SmplCty (Sample Player) ✅
- [x] ESP-IDF project structure with CMake (target: ESP32-S3)
- [x] NimBLE BLE MIDI peripheral — pinned to Core 0
- [x] MIDI message parsing (Note On/Off, CC, Program Change, Pitch Bend)
- [x] FreeRTOS queue for MIDI events between cores
- [x] SD card driver (SPI, FAT32, long filenames)
- [x] WAV cue point parser for 16-slice kit files
- [x] 4-voice polyphonic sample player with velocity scaling — pinned to Core 1
- [x] I2S audio output (16-bit stereo, 44.1kHz)
- [x] Per-unit configuration (NSMBL_SmplCty, channel 1)
- [x] End-to-end verified: iPhone → BLE MIDI → 808 drum playback

### NSMBL_EEnoo (AMY Synth) ✅
- [x] AMY synth engine integrated as ESP-IDF component (250 oscillators)
- [x] I2S audio output via AMY (32-bit stereo, 44.1kHz)
- [x] All-channel MIDI pass-through (Juno ch1, DX7 ch2, drums ch10)
- [x] Startup bleep confirmation
- [x] BLE MIDI shared from SmplCty codebase
- [x] End-to-end verified: iPhone → BLE MIDI → AMY synth playback

## Phase 2 — Multi-Unit & UX (In Progress)

### Done
- [x] Full CC passthrough — NSMBL CC handler: volume, pan, filter, resonance, reverb, chorus (all channels)
- [x] Pitch Bend passthrough (working since EEnoo launch)
- [x] Per-unit MIDI channel filtering (configurable via config.h, 0=all)
- [x] Mozaic control surface — patch browsing, sound shaping, sustain, panic
- [x] Band member configs created: Eenoo (pads), Prynz (lead), Botsee (bass), Kneel (drums)
- [x] Named configs in `AmyBoard/configs/` — copy to config.h before flashing
- [x] **AMY sample player (`NSMBL_SampleKits`) parser brought up to snuff** — ported the
  running-status BLE parser from the Xiao/Garee rewrite (fixes dropped/corrupted hits on
  ch10 drum streams that use running status). Built clean, flashed + boot-verified on the
  AMYboard (SD mount, 16-slice kit load, BLE connect). (2026-07-09)
  - [ ] Flash with `config_Kneel.h` identity if this unit should join the roster as Kneel
    (currently flashed as generic `NSMBL_SampleKits`, ch10)

### AMYboard sample player — I2C encoder + OLED kit selection (In Progress, 2026-07-09)
*Plan: `~/.claude/plans/warm-mapping-cake.md`. Modulino Knob [ABX00107] @ 0x76/0x74 +
SH1107 128x128 OLED @ 0x3C on the Qwiic bus (SDA=17/SCL=18).*
- [x] Shared `i2c_master` bus (`i2c_bus.c`)
- [x] Modulino Knob driver (`modulino_knob.c`) — wrap-safe delta + button edge
- [x] SH1107 OLED via vendored u8g2 (`oled.c` + `components/u8g2/`, fonts trimmed 38 MB→26 KB)
- [x] Runtime-safe kit swap (`sample_player.c`) — local-load-then-swap under `s_kit_mutex`,
  render never stalls, **latent reload leak fixed**
- [x] Kit scan of `/sdcard/Kits/*.wav` (`ui.c`) — sorted, ext-stripped; **43 kits** found on
  the card (raised `MAX_KITS` 32→96)
- [x] UI loop — turn scrolls, push loads with a "Loading…" screen
- [x] Builds clean; boot-verified on HW (bus up, scan, graceful no-peripheral fallback, 808s intact)
- [x] **Live bring-up ✅** — OLED kit list renders; knob scrolls (found at **0x3B** after the
  address correction); push → "Loading…" → new kit plays; race-safe swap holds under playback.
  SH1107 needed the **`_seeed_` variant** (`OLED_SETUP`): the generic `_128x128_` applies a
  bogus `x_offset=96` that wrapped the display; seeed uses the same init with offset 0.
  **Feature complete.**
- [ ] (future) Modulino **Buttons** (mode/audition/panic); kit switch via MIDI Program Change;
  persist last kit in NVS

## Phase 2b — XIAO Platform (In Progress)

### Done
- [x] XIAO ESP-IDF firmware (`SeeedXiaoMIDI/NSMBL_Synth/`) — BLE MIDI + SAM2695 UART
- [x] SAM2695 MIDI reference extracted from chip spec (`Docs/SAM2695_MIDI_Reference.md`)
- [x] Full CC/NRPN passthrough — firmware forwards all CCs to SAM2695, NRPNs work natively
- [x] Mozaic XIAO controller (`Mozaic/NSMBL_XIAO_Controller.moz`) — Layout 0 (Mix)
  - XY pad: filter cutoff + resonance via NRPN
  - 10 knobs: volume, expression, reverb/chorus send+type, attack, release, vibrato rate/depth
  - 8 pads: patch/bank navigation, sustain, reset, PANIC
- [x] BLE MIDI parser rewrite — proper running status support, debug logging
- [x] SAM2695 bank fix — clamp to GM (0) / MT-32 (127), buttons+Mozaic as GM/MT-32 selectors
- [x] Hardware buttons working — gpio_reset_pin + 50ms debounce (root cause was seating, not firmware)
- [x] Volume-tracking fix — guard was broken in all-channel mode (MIDI_CHANNEL 0)
- [x] **Program changes fixed** — real cause was ESP-IDF console on UART0 corrupting the
  SAM's MIDI; moved console to USB-JTAG (`CONFIG_ESP_CONSOLE_UART_NUM=-1`). GM System On at boot.
- [x] Channel model — listen on ch1 (melody) + ch10 (drums), ignore the rest
- [x] **Patch-nav MVP** — buttons 0/1 family jump (coarse), 2/3 single patch (fine), each auditions
- [x] Review-pass cleanup — panic releases sustain (CC64=0), stale comments fixed
- [x] **v0.1.0 GitHub release** — merged flash-at-0x0 BIN, no toolchain needed
- [x] Roster naming sync across docs + AMY/XIAO configs
- [x] **5 single-channel XIAO band variants** — each listens only on its own port:
  Eenoo (ch1), Prynz (ch2), Botsee (ch3), Moroh (ch5), 8OhAte (ch10); per-member
  prebuilt BINs built and attached to the v0.1.0 release
- [x] Flashed live: Prynz, Botsee, 8OhAte (drums confirmed!)

### Deferred — XIAO firmware hygiene (low priority, not blocking)
- [ ] **Audition blocks the MIDI task (~780ms)** — `audition_voice()` + PC delays run inside
  `midi_task`, so incoming BLE events queue up while the preview plays; a held note's
  note-off can stall, and a PC flood can overflow the 64-deep queue (silent drops).
  Fix deliberately: add a UART write mutex FIRST (two tasks must never interleave bytes
  to the SAM2695 — see the UART0 saga), then move the audition to its own task, then
  hardware-test with a live MIDI stream. Do NOT do this casually.
- [ ] Decide volume-button intent: long-press volume only adjusts VOICE_CHANNEL (ch1);
  drums (ch10) stay at boot volume. If buttons should be "device volume", also send ch10.
- [ ] Move `ble_midi_is_connected()` declaration into `ble_midi.h` (currently extern'd
  inline in main.c's led_task)
- [ ] Streamline PC band-aids (30ms bank/PC gaps, 250ms audition delay) now that the real
  UART0 bug is fixed — kept for now, "don't press our luck" (works reliably as-is)
- [ ] Long-press timing 600ms → 1000ms to match Seeed reference (feel preference)
- [ ] BLE RX buffer is fixed `uint8_t buf[128]` in `ble_midi.c` — size to negotiated MTU to avoid truncating very large bursts
- [ ] Parser SysEx handling consumes the 0xF7 terminator as a timestamp (harmless, not strictly correct)

### Next — MVP for Spatial Composition
- [ ] Flash remaining XIAO units: Eenoo (ch1, from the basement) + Moroh (ch5)
- [ ] Verify 5-unit simultaneous BLE connection from iPhone/AUM (route ch1/2/3/5 + ch10)
- [ ] Connect each unit to a speaker in a different location
- [ ] First spatial composition test (2 XIAO + 1 AMY already sounded amazing)
- [ ] Flash 3 AMYboards: Eenoo (ch1 pads), Prynz (ch2 lead), Botsee (ch3 bass)
- [ ] Verify XIAO Mozaic controller end-to-end (CCs, NRPNs, program changes)

### Later
- [ ] Kit switching via MIDI Program Change (SmplCty)
- [ ] Modulino Buttons + Knob I2C drivers (when hardware arrives)
- [x] **Garee (Gary Numan)** — AMYboard BLE→DIN MIDI bridge scaffolded
  (`AmyBoard/NSMBL_Bridge/`), builds clean for ESP32-S3. MIDI out UART1 GPIO14
  (Type A), 31250. Configurable `BRIDGE_IN` (all/one channel) + `BRIDGE_OUT`
  (passthrough/remap); default config_Garee.h forwards ALL → remaps to ch1 so
  fixed-channel hardware just plays.
  - [x] Flashed + **hardware-verified driving an Akai MPC over Type-A TRS**
    (2026-07-07). Key fix: **hold the non-data TRS leg (GPIO15) HIGH as the MIDI
    current source** — the OUT jack has no 3.3V rail, so a DIN opto gets total
    silence otherwise. Full write-up in `AmyBoard/NSMBL_Bridge/README.md`.
  - [ ] (future) MIDI thru/merge from DIN in (GPIO21); SysEx/clock passthrough
  - [x] (cleanup) reflashed the clean build — per-note diagnostic log removed (2026-07-07)
- [ ] **CV output AMYboard variant** — pitch/gate (and LFO/env) CV out to patch a band
  member into modular/Eurorack; AMYboard has 2x CV out
- [ ] Moroh AMY config (roster lists Moroh as Amy+Xiao; only the XIAO config exists so far)
- [ ] Favorite voice slots stored in NVS flash
- [ ] Reconnection handling — auto re-advertise after iOS disconnects
- [ ] Per-unit Mozaic scripts (one controller per band member)
- [ ] Dapht (Daft Punk) — sampler/granular unit, design TBD after first spatial jam
