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

## Phase 2 — Multi-Unit & UX (Next)

- [ ] Kit switching via MIDI Program Change (SmplCty)
- [ ] Modulino Buttons + Knob I2C drivers (when hardware arrives)
- [ ] BLE-to-DIN MIDI bridge mode (forward BLE MIDI out GPIO 14/15)
- [ ] Per-unit MIDI channel filtering (configurable)
- [ ] Full CC passthrough (volume, pan, expression, sustain, mod, effects)
- [ ] Pitch Bend and Aftertouch passthrough
- [ ] Curated preset system per band role
- [ ] Favorite voice slots stored in NVS flash
- [ ] Reconnection handling — auto re-advertise after iOS disconnects
- [ ] Multi-unit simultaneous operation verified
