[Skip to content](https://diyelectromusic.com/2025/06/28/xiao-esp32-c3-midi-synthesizer-part-5/#content)

![](https://diyelectromusic.com/wp-content/uploads/2020/06/cropped-montage-2.png)

[Kevin](https://diyelectromusic.com/author/emalliab/)[Beginner](https://diyelectromusic.com/category/project-difficulty/beginner/), [Synth Modules](https://diyelectromusic.com/category/project-categories/synth-modules/)June 28, 2025June 29, 20252 Minutes

So I was somewhat guilty that I’d replaced the XIAO microcontroller in the last couple of parts to get USB MIDI, so in this one I put the original ESP32-C3 back in and now have a version of [Part 4](https://diyelectromusic.com/2025/06/27/xiao-esp32-c3-midi-synthesizer-part-4/) for serial MIDI.

- [Part 1](https://diyelectromusic.com/2025/06/26/xiao-esp32-c3-midi-synthesizer/) – Getting started and getting code running.
- [Part 2](https://diyelectromusic.com/2025/06/27/xiao-esp32-c3-midi-synthesizer-part-2/) – Swapping the ESP32-C3 for a SAMD21 to get USB MIDI.
- [Part 3](https://diyelectromusic.com/2025/06/27/xiao-esp32-c3-midi-synthesizer-part-3/) – Taking a deeper look at the SAM2695 itself.
- [Part 4](https://diyelectromusic.com/2025/06/27/xiao-esp32-c3-midi-synthesizer-part-4/) – A USB MIDI Synth Module using the SAMD21 again as a USB MIDI Host.
- [Part 5](https://diyelectromusic.com/2025/06/28/xiao-esp32-c3-midi-synthesizer-part-5/) – A Serial MIDI Synth Module using the original ESP32-C3.
- [Part 6](https://diyelectromusic.com/2025/06/29/xiao-esp32-c3-midi-synthesizer-part-6/) – Pairs the Synth with a XIAO Expansion board to add display and potentiometers.

XIAO MIDI Synthesizer Serial MIDI - MakerTube

Error details

_**Warning!** I strongly recommend using old or second hand equipment for your experiments.  I am not responsible for any damage to expensive instruments!_

These are the key tutorials for the main concepts used in this project:

- [Getting Started with the XIAO MIDI Synthesizer](https://wiki.seeedstudio.com/xiao_midi_synthesizer/)
- [XIAO SAMD21, Arduino and MIDI](https://diyelectromusic.com/2023/03/12/xiao-samd21-arduino-and-midi/)

If you are new to microcontrollers, see the [Getting Started](https://diyelectromusic.wordpress.com/getting-started/) pages.

### The Circuit

This goes back to adding a 3V3 compatible serial MIDI board using the XIAO GPIO breakout pins as described in [Part 1](https://diyelectromusic.com/2025/06/26/xiao-esp32-c3-midi-synthesizer/).

This is powered from 3V3 and GND and connected to D6/RX (which may or may not be GPIO 6).

![](https://diyelectromusic.com/wp-content/uploads/2025/06/img_8585.jpeg?w=1024)

### The Code

This is all the same code from [Part 4](https://diyelectromusic.com/2025/06/27/xiao-esp32-c3-midi-synthesizer-part-4/) but with the USB handling removed and the bespoke serial port writing replaced with calls into the Arduino MIDI Library using Serial0 as the MIDI device:

```
MIDI_CREATE_INSTANCE(HardwareSerial, Serial0, MIDI);

void midiSendProgram (uint8_t prog) {
  MIDI.sendProgramChange(prog, MIDI_CHANNEL);
}

void midiSendControl (uint8_t cmd, uint8_t d) {
  MIDI.sendControlChange(cmd, d, MIDI_CHANNEL);
}

void setup() {
  MIDI.begin(MIDI_CHANNEL);
  ...
}

void loop() {
  MIDI.read();
  ...
}
```

Once again I’m relying on the fact that the serial MIDI transport has an automatic MIDI THRU enabled between RX and TX.

[Find it on GitHub here](https://github.com/diyelectromusic/sdemp/tree/main/src/SDEMP/XIAOSynthSerialMIDI).

### Closing Thoughts

Whilst the USB MIDI host solution is a lot more convenient in terms of being able to plug in MIDI controllers, the serial MIDI version is a lot easier when it comes to programming and powering the device.

The video shows a PC streaming a MIDI file to the XIAO Synth whilst I’m changing volume and voice.

Kevin

### Share this:

- [Share on X (Opens in new window)X](https://diyelectromusic.com/2025/06/28/xiao-esp32-c3-midi-synthesizer-part-5/?share=twitter&nb=1)
- [Share on Facebook (Opens in new window)Facebook](https://diyelectromusic.com/2025/06/28/xiao-esp32-c3-midi-synthesizer-part-5/?share=facebook&nb=1)

LikeLoading...

### _Related_

[Other SAM2695 Synths](https://diyelectromusic.com/2025/07/15/other-sam2695-synths/ "Other SAM2695 Synths")July 15, 2025In "Beginner"

[XIAO ESP32-C3 MIDI Synthesizer – Part 2](https://diyelectromusic.com/2025/06/27/xiao-esp32-c3-midi-synthesizer-part-2/ "XIAO ESP32-C3 MIDI Synthesizer &#8211; Part&nbsp;2")June 27, 2025In "Intermediate"

[XIAO ESP32-C3 MIDI Synthesizer](https://diyelectromusic.com/2025/06/26/xiao-esp32-c3-midi-synthesizer/ "XIAO ESP32-C3 MIDI&nbsp;Synthesizer")June 26, 2025In "Beginner"

- Tagged
- [control change](https://diyelectromusic.com/tag/control-change/)
- [midi](https://diyelectromusic.com/tag/midi/)
- [program change](https://diyelectromusic.com/tag/program-change/)
- [SAM2695](https://diyelectromusic.com/tag/sam2695/)
- [xiao](https://diyelectromusic.com/tag/xiao/)

![Unknown's avatar](https://0.gravatar.com/avatar/05a6c4f760c447e8abcc799efe819572ce2896b0b4ded36a78ef24563a866904?s=80&d=identicon&r=G)

## Published by Kevin

Just another soul meandering my way around the Internet ... [View all posts by Kevin](https://diyelectromusic.com/author/emalliab/)

**Published** June 28, 2025June 29, 2025

### Leave a comment [Cancel reply](https://diyelectromusic.com/2025/06/28/xiao-esp32-c3-midi-synthesizer-part-5/\#respond)

Δ

- [Comment](https://diyelectromusic.com/2025/06/28/xiao-esp32-c3-midi-synthesizer-part-5/#respond)
- [Reblog](https://diyelectromusic.com/2025/06/28/xiao-esp32-c3-midi-synthesizer-part-5/)
- [Subscribe](https://diyelectromusic.com/2025/06/28/xiao-esp32-c3-midi-synthesizer-part-5/) [Subscribed](https://diyelectromusic.com/2025/06/28/xiao-esp32-c3-midi-synthesizer-part-5/)








  - [![](https://diyelectromusic.com/wp-content/uploads/2020/05/cropped-clef_512x512.png?w=50) Simple DIY Electronic Music Projects](https://diyelectromusic.com/)

Join 213 other subscribers

Sign me up

  - Already have a WordPress.com account? [Log in now.](https://wordpress.com/log-in?redirect_to=https%3A%2F%2Fr-login.wordpress.com%2Fremote-login.php%3Faction%3Dlink%26back%3Dhttps%253A%252F%252Fdiyelectromusic.com%252F2025%252F06%252F28%252Fxiao-esp32-c3-midi-synthesizer-part-5%252F)


- - [![](https://diyelectromusic.com/wp-content/uploads/2020/05/cropped-clef_512x512.png?w=50) Simple DIY Electronic Music Projects](https://diyelectromusic.com/)
  - [Subscribe](https://diyelectromusic.com/2025/06/28/xiao-esp32-c3-midi-synthesizer-part-5/) [Subscribed](https://diyelectromusic.com/2025/06/28/xiao-esp32-c3-midi-synthesizer-part-5/)
  - [Sign up](https://wordpress.com/start/)
  - [Log in](https://wordpress.com/log-in?redirect_to=https%3A%2F%2Fr-login.wordpress.com%2Fremote-login.php%3Faction%3Dlink%26back%3Dhttps%253A%252F%252Fdiyelectromusic.com%252F2025%252F06%252F28%252Fxiao-esp32-c3-midi-synthesizer-part-5%252F)
  - [Copy shortlink](https://wp.me/pc1YuP-4Gn)
  - [Report this content](https://wordpress.com/abuse/?report_url=https://diyelectromusic.com/2025/06/28/xiao-esp32-c3-midi-synthesizer-part-5/)
  - [View post in Reader](https://wordpress.com/reader/blogs/177786911/posts/18003)
  - [Manage subscriptions](https://subscribe.wordpress.com/)
  - [Collapse this bar](https://diyelectromusic.com/2025/06/28/xiao-esp32-c3-midi-synthesizer-part-5/)

[Toggle photo metadata visibility](https://diyelectromusic.com/2025/06/28/xiao-esp32-c3-midi-synthesizer-part-5/#)[Toggle photo comments visibility](https://diyelectromusic.com/2025/06/28/xiao-esp32-c3-midi-synthesizer-part-5/#)

Loading Comments...

Write a Comment...

Email (Required)Name (Required)Website

%d