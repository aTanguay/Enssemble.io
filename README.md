# Enssemble — The Digital Scrap Band


**The Scrap Band**

The goal of Enssemble is to make experimenting with what I like to call a 'scrap band' a lot easier and cheaper. Placing musical members/nodes around your space gives you a completely different (and fun) experience compared to traditional two channel stereo. I personally strive to make them 100% wireless, so one can experiment at will. Running cables breaks the flow - and the metaphor.

To create a band, we make a collection of sound making devices, sequenced through one BLE MIDI source. I personally use AUMv3 on iOS for this. This is your brain (and can be a voice of course). Just about any kind of sequencing you'd want is possible this way...from generative, to traditional straight forward compositions.

**The Member Nodes**

Battery>Node/Amp>Speaker

To get going, you need nodes, or 'members' to your band. The cheapest way I have found to make a good node is a [Seeed Studio Xiao MIDI synthesizer](https://www.seeedstudio.com/XIAO-MIDI-Synthesizer-p-6462.html) , a USB power brick, and a thrifted speaker...that's it. This could be done for $30-$50, depending on what you have available...most people have at least a few USB bricks around. So, a whole 'band' could cost you as little as $100, depending on what you have available. And frankly batteries are optional, but I go for no wires here.

Now that's the cheapest 'good' option, but there's nothing stopping you from using the tiny speaker on the Xiao, or hooking up a stadium PA. That's your business.

**Enssemble Firmwares**

Right now, there are two paths for Enssemble firmware. The aforementioned Xiao, and then the ['AmyBoard'](https://www.amyboard.com/) . This is a very cool, very cheap, very capable board that gives you some things that the Xiao doesn't have. The downside being the lack of built in amplifier. The Xiao has a surprisingly capable little Class D amp inside. So when powered and connected to a bookshelf speaker, it's ready to go. You'll be surprised!

On the other hand, the Amyboard supports more modern synthesis, Sample playback, and (TypeA TRS) MIDI in and out. As well as CV, if you wanna get nuts. But, you'll need powered speakers of some type. Which isn't the end of the world, but makes things more complicated.

**Node control and sonic shaping**

For this, I've been using Mozaic on iOS. The firmware support program change and passing CCs to whatever tweakable parameters the synths have. This feature is a little rough right now, but it will get cleaned up. And keep in mind that the Xiao uses some older sound types, but that's half the fun...trying to make cool stuff out a more limited palette. There are both crazy cheesy stuff in there, and surprisingly nice. And hey, put the sound through a pedal and shape it if you don't like it.

**The Pretenious Statement**

The overall experiment here is in 'spatial-like music'...meaning music experienced in 3D dimensions, instead of just two stereo speakers. By being able to place these sound sources literally anywhere around a space, you can create just about any kind of 'experience' you want. You could simulate a 4 piece Jazz trio. You could have sounds in other rooms of a home, all contributing to music. You get the idea. A lot of the inspiration is [Susan Ciani](https://www.sevwave.com/) (who I've actually never seen) who sequences her music in 3D dimensions, leading to an performance that can only be experienced in person. Neat. Whew, those kind of statements make me queasy. Here's a shortened version...It'd be cool.

*(Note, these firmwares were made largely through vibe coding with human planning and oversight. If that sits with you wrong, for the love of god, keep it to yourself and don't use these free tools that somehow a computer is 100% responsible for, but also, somehow took me dozens of man-hours to make)* Eye-roll emoji


## What each node does

- Each unit advertises as a **BLE MIDI 1.0 peripheral** — connects to iOS as distinct nodes.
- Receives Note On/Off, Program Change, Pitch Bend, and CCs
- Per-unit MIDI channel filtering — each band member responds only to its assigned channel
- Real-time sound shaping via standard MIDI CCs (filter, reverb, chorus, volume, pan)
- Two platforms: **AMYboard** (ESP32-S3 + AMY software synth) and **XIAO MIDI Synthesizer** (ESP32-C3 + SAM2695 hardware GM wavetable)
- AMYboard firmware variants: **AMY synth** (256 patches) and **sample player** (WAV kits from SD)
- iOS control surface via **Mozaic** scripts



## The Band (The Firmwares)

| Name | Inspiration | MIDI Ch | Role | Default Patch | Firmware |
|------|-------------|-----|------|---------------|----------|
| **Eenoo** | Brian Eno | 1 | Pads/Ambient | Synth Pad | Amy and Xiao |
| **Prynz** | Prince | 2 | Lead | Synth Leads | Amy and Xiao |
| **Botsee** | Bootsie Collins | 3 | Bass | Synth Bass | Amy and Xiao |
| **Moroh** | Giorgio Moroder | 5 | Arps / Sequences | Synth Lead | Amy and Xiao |
| **8OhAte** | The 808 | 10 | Drums | GM Kits | Xiao Only |
| **Kneel**  | Neal Peart      | 10      | Drums                | Sample Kits   | Amy Only     |
| **Dapht**  | Daft Punk       | 4       | General Samples      | TBD           | Amy Only     |
| **Garee** | Gary Numan | ALL | MIDI Out to Hardware | None | Amy Only |

Named configs in `AmyBoard/configs/` — copy to `main/config.h` before flashing each board.



## Hardware

### AMYboard (ESP32-S3 + AMY)
- [AMYboard](https://www.amyboard.com/) — open-source, $29
- AMY software synth engine — 250 oscillators, 128 Juno-6 + 128 DX7 patches, GM drums
- ESP32-S3 dual-core: BLE MIDI on core 0, audio rendering on core 1
- 8MB octal PSRAM, SD card, I2S line out (3.5mm TRS stereo)
- I2C port for accessories, MIDI in/out (TRS), CV in/out, S/PDIF

### Seeed Studio XIAO MIDI Synthesizer - ESP32-C3 + SAM2695
- [Seeed Studio XIAO MIDI Synthesizer](https://www.seeedstudio.com/XIAO-MIDI-Synthesizer-p-6462.html)  — $22, tiny form factor, single-core RISC-V
- Dream SAM2695 — hardware GM wavetable synth, 128 GM patches + drum kits, 64-voice polyphony
- UART MIDI at 31250 baud, full CC/NRPN passthrough (filter, envelope, vibrato, reverb/chorus types)
- 3.5mm stereo line out + onboard Class-D amplifier



## Firmware Variants

### NSMBL_Synth — AMY Synth (`AmyBoard/NSMBL_Synth/`)
Full software synthesizer using the AMY engine. Default synths: Juno-6 (ch1), DX7 (ch2), GM drums (ch10). Supports program change to browse all 256 patches. Custom NSMBL CC handler for real-time sound shaping. Used by Eenoo, Prynz, and Botsee.

### NSMBL_SampleKits — Sample Player (`AmyBoard/NSMBL_SampleKits/`)
16-slice drum sample player. Reads WAV files with cue point markers from SD card. 4-voice polyphony, velocity-sensitive, chromatic mapping from C2. Used by Kneel.

### NSMBL_Synth — SAM2695 (`SeeedXiaoMIDI/NSMBL_Synth/`)
Hardware GM wavetable synth via SAM2695 chip. 128 GM patches, full CC passthrough, NRPN support for filter cutoff/resonance, envelope, and vibrato. BLE MIDI parser with running status support. Listens on **MIDI channel 1** (melody) and **channel 10** (GM drums); other channels are ignored (route in your host). See `Docs/SAM2695_MIDI_Reference.md` for full CC/NRPN map.

Per-unit identities live in `SeeedXiaoMIDI/NSMBL_Synth/configs/` — copy one to `main/config.h` before flashing so each module shows up with its own BLE name in AUM: **NSMBL_Eenoo** (pads, ch1), **NSMBL_Prynz** (lead, ch2), **NSMBL_Botsee** (bass, ch3), **NSMBL_Moroh** (arps, ch5), **NSMBL_8OhAte** (drums, ch10). Each unit listens only on its own channel.

Prebuilt per-member BINs are attached to the [v0.1.0 release](https://github.com/aTanguay/Enssemble.io/releases/tag/v0.1.0) — flash a unit without a toolchain:
```bash
esptool.py --chip esp32c3 -p /dev/cu.usbmodem101 -b 460800 write_flash 0x0 NSMBL_Eenoo_v0.1.bin
```
or at 
[ESP TOOL ONLINE](https://espressif.github.io/esptool-js/)
Connects, flash starting at 0x00 and go to town.

**Hardware buttons** (patch browsing, each press auditions the voice):

| Button | Tap | Hold |
|--------|-----|------|
| 0 | Family down (coarse, −8) | — |
| 1 | Family up (coarse, +8) | Volume up |
| 2 | Patch down (fine, −1) | Volume down |
| 3 | Patch up (fine, +1) | MIDI panic |

> ⚠️ **Build gotcha:** the ESP-IDF console **must** stay off UART0 (it's the SAM2695's MIDI port). If it doesn't, log output corrupts the chip and patch changes silently fail. `sdkconfig` must show `CONFIG_ESP_CONSOLE_UART_NUM=-1`. See PLANNING.md → Technical Notes.



## Installing

There are many precompiled firmwares available for these boards. The easiest way to flash them is with the online tool from esspressiff. Depending on how many nodes you're making, you can install some or all of the firmwares, depending on their usage. 
[ESP TOOL ONLINE](https://espressif.github.io/esptool-js/)


## Building

Requires [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/) v5.x. Not sure I'd do this

```bash
# Source ESP-IDF
source ~/esp/esp-idf/export.sh

# Copy band member config, then build and flash
cp AmyBoard/configs/config_Prynz.h AmyBoard/NSMBL_Synth/main/config.h
cd AmyBoard/NSMBL_Synth
idf.py build
idf.py -p /dev/cu.usbmodem* flash monitor
```



## iOS Setup

1. Power on units — each plays a startup bleep
2. Open **AUM**, tap MIDI plug icon, connect each unit via Bluetooth MIDI
3. Route MIDI channels to the corresponding units
4. Play with a keyboard app, sequencer (Helium), or Mozaic control surface

iOS handles 13+ simultaneous BLE connections — the full band is well within range.

***CREATE DEMO VIDEO!***



## Mozaic Control Surface

iOS scripts in `Mozaic/` for the [Mozaic](https://ruismaker.com) MIDI scripting app:

| Script | Purpose |
|--------|---------|
| **NSMBL_EEnoo_Controller.moz** | AMYboard: patch browsing, filter/reso/reverb/chorus knobs, vol/pan mode, sustain, MIDI panic |
| **NSMBL_XIAO_Controller.moz** | XIAO/SAM2695: Mix layout — XY filter pad, NRPNs, named reverb/chorus types, envelope, vibrato |
| NSMBL_EEnoo_PatchSelect.moz | Simple patch bank selector |
| NSMBL_EEnoo_SoundShaper.moz | CC knobs only |



## NSMBL CC Map

Standard MIDI CCs handled by the firmware on all channels. Works with any MIDI controller.

| CC | Parameter | Mapping |
|----|-----------|---------|
| 7 | Volume | Linear 0-127 |
| 10 | Pan | 0=Left, 64=Center, 127=Right |
| 70 | Filter Cutoff | Exponential ~13Hz-6400Hz |
| 71 | Resonance | Exponential Q 0.5-16 |
| 75/93 | Chorus Level | Linear 0=dry, 127=full |
| 91 | Reverb Level | Linear 0=dry, 127=full |

Handled by AMY internally: CC 0 (bank select), CC 64 (sustain), CC 123 (all notes off).

## License

MIT
