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
- [x] Band member configs created: EEnoo (pads), Pryns (lead), Aufde (bass), Kneel (drums)
- [x] Named configs in `AmyBoard/configs/` — copy to config.h before flashing

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

### Deferred — XIAO firmware hygiene (low priority, not blocking)
- [ ] Long-press timing 600ms → 1000ms to match Seeed reference (feel preference)
- [ ] BLE RX buffer is fixed `uint8_t buf[128]` in `ble_midi.c` — size to negotiated MTU to avoid truncating very large bursts
- [ ] Parser SysEx handling consumes the 0xF7 terminator as a timestamp (harmless, not strictly correct)

### Next — MVP for Spatial Composition
- [ ] Flash 3 AMYboards: EEnoo (ch1 pads), Pryns (ch2 lead), Aufde (ch3 bass)
- [ ] Verify 3-unit simultaneous BLE connection from iPhone/AUM
- [ ] Connect each unit to a speaker in a different location
- [ ] First spatial composition test
- [ ] Verify XIAO Mozaic controller end-to-end (CCs, NRPNs, program changes)

### Later
- [ ] Kit switching via MIDI Program Change (SmplCty)
- [ ] Modulino Buttons + Knob I2C drivers (when hardware arrives)
- [ ] BLE-to-DIN MIDI bridge mode (forward BLE MIDI out GPIO 14/15)
- [ ] Favorite voice slots stored in NVS flash
- [ ] Reconnection handling — auto re-advertise after iOS disconnects
- [ ] Per-unit Mozaic scripts (one controller per band member)
- [ ] Dapht (Daft Punk) — sampler/granular unit, design TBD after first spatial jam
