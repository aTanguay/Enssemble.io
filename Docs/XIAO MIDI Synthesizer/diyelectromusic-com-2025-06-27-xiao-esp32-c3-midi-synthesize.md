[Skip to content](https://diyelectromusic.com/2025/06/27/xiao-esp32-c3-midi-synthesizer-part-4/#content)

![](https://diyelectromusic.com/wp-content/uploads/2020/06/cropped-montage-2.png)

[Kevin](https://diyelectromusic.com/author/emalliab/)[Intermediate](https://diyelectromusic.com/category/project-difficulty/intermediate/), [Synth Modules](https://diyelectromusic.com/category/project-categories/synth-modules/)June 27, 2025June 29, 20254 Minutes

In [Part 2](https://diyelectromusic.com/2025/06/27/xiao-esp32-c3-midi-synthesizer-part-2/) I looked at how to add USB MIDI to the XIAO MIDI Synthesizer and in [Part 3](https://diyelectromusic.com/2025/06/27/xiao-esp32-c3-midi-synthesizer-part-3/) I looked at some of the properties of the SAM2695 synth chip itself. This post combines the two into a relatively simple, but playable, synth.

- [Part 1](https://diyelectromusic.com/2025/06/26/xiao-esp32-c3-midi-synthesizer/) – Getting started and getting code running.
- [Part 2](https://diyelectromusic.com/2025/06/27/xiao-esp32-c3-midi-synthesizer-part-2/) – Swapping the ESP32-C3 for a SAMD21 to get USB MIDI.
- [Part 3](https://diyelectromusic.com/2025/06/27/xiao-esp32-c3-midi-synthesizer-part-3/) – Taking a deeper look at the SAM2695 itself.
- [Part 4](https://diyelectromusic.com/2025/06/27/xiao-esp32-c3-midi-synthesizer-part-4/) – A USB MIDI Synth Module using the SAMD21 again as a USB MIDI Host.
- [Part 5](https://diyelectromusic.com/2025/06/28/xiao-esp32-c3-midi-synthesizer-part-5/) – A Serial MIDI Synth Module using the original ESP32-C3.
- [Part 6](https://diyelectromusic.com/2025/06/29/xiao-esp32-c3-midi-synthesizer-part-6/) – Pairs the Synth with a XIAO Expansion board to add display and potentiometers.

XIAO MIDI Synthesizer USB MIDI Host - MakerTube

Error details

_**Warning!** I strongly recommend using old or second hand equipment for your experiments.  I am not responsible for any damage to expensive instruments!_

These are the key tutorials for the main concepts used in this project:

- [Getting Started with the XIAO MIDI Synthesizer](https://wiki.seeedstudio.com/xiao_midi_synthesizer/)
- [XIAO SAMD21, Arduino and MIDI](https://diyelectromusic.com/2023/03/12/xiao-samd21-arduino-and-midi/)

If you are new to microcontrollers, see the [Getting Started](https://diyelectromusic.wordpress.com/getting-started/) pages.

### The Circuit

I’m wanting to plug a USB MIDI keyboard into my XIAO Synth, so I’m going to swap the XIAO ESP32-C3 out once again for a XIAO SAMD21 as described in [Part 2](https://diyelectromusic.com/2025/06/27/xiao-esp32-c3-midi-synthesizer-part-2/). A future post will go back and create a serial MIDI version using the unmodified XIAO Synth.

Recall that a source of 5V power is required, and below I’m just using the 5V USB passthrough from the original XIAO ESP32-C3.

![](https://diyelectromusic.com/wp-content/uploads/2025/06/img_8589.jpg?w=1024)

There is one issue to watch out for in this kind of configuration. Depending on how the USB power is provided, there might not be a common ground point between the XIAO’s power and any audio amplification used.

Apparently GND is always passed through many USB charger blocks.

To prevent interference and noise, it may be necessary to ground the USB connection providing power.

### The Code

I’m using a combination of the SAMD21 USB Host Library “USB MIDI Converter” example and some GPIO and MIDI handling.

The general thread of the code will be as follows:

```
loop():
  Do any USB host processing (e.g. plug-and-play)
  IF USB MIDI messages received THEN
    Send to serial MIDI
  IF any buttons pressed THEN
    Handle button IO
    Generate any additional MIDI messages as required
    Send to serial MIDI
```

In particular it should be noted that the MIDI “listening” is one-way only USB to serial MIDI; and that any internal control events that generate MIDI are also only going one way – to serial MIDI.

The buttons will be used to do the following:

- Button 0 and 1: Program Select Up/Down
- Button 2 and 3: Channel Volume Up/Down

A much more sophisticated interface could be developed – e.g. using one of the buttons to change modes for the other buttons, to allow the selection of different things, but for now, this is plenty.

There are several layers to the code to allow this however, so I’ll cover them briefly here.

- Main Loop button IO: Checks for the main H->L, L->H, L->L, H->H events and calls functions for all but the last (buttons are pulled high so H->H means “nothing happening).
- Event functions for Pressed, Released, Hold which call program or volume handling functions as required.
- Program/volume handling functions that update the values and then trigger the appropriate MIDI messages to be sent.
- Functions to send a MIDI Program Change or Control Change message as required.

In the end I’ve only triggered events on button Pressed, so the Release and Hold functions don’t cause any further action to take place, but the model is there for use in the future if required.

Note: as I’m not using the Arduino MIDI Library, I just build PC or CC messages directly and call out to Serial1.Write as shown below.

```
void midiSendControl (uint8_t cmd, uint8_t d) {
  uint8_t buf[3];
  buf[0] = MIDI_CC | (MIDI_CHANNEL-1);
  buf[1] = cmd;
  buf[2] = d;
  Serial1.write(buf, 3);
}
```

Simple, but it works. Note PC are only two bytes in size not three.

If performance seems to be an issue, then I have a few things to adjust in the scheduling:

- I can split the digitalRead() of each button over several scans to allow MIDI processing to happen in between.
- I can switch to the XIAO equivalent of direct PORT IO to try to read all IO pins at the same time. I don’t know what this looks like for the SAMD21 however and it would make it hardware specific, so I’d really rather not do that.
- I can remove the waiting of 1mS. This was in the original USB converter, so I kept it in here too. I ought to measure the free running loop() time to see if it is needed.

[Find it on GitHub here](https://github.com/diyelectromusic/sdemp/tree/main/src/SDEMP/XIAOSynthUSBMIDIHost).

### Closing Thoughts

In the video I cycle through a few of the programs whilst controlling the synth from a keyboard.

At one point I remove the external audio connection (yes, I should have turned it down first!) to contrast the sound with the internal speaker. Yes, it is a bit “tinny” but it isn’t too bad.

This really is just the very “tip of the iceberg” given the whole range of parameters available that were mentioned in [Part 3](https://diyelectromusic.com/2025/06/27/xiao-esp32-c3-midi-synthesizer-part-3/).

Kevin

### Share this:

- [Share on X (Opens in new window)X](https://diyelectromusic.com/2025/06/27/xiao-esp32-c3-midi-synthesizer-part-4/?share=twitter&nb=1)
- [Share on Facebook (Opens in new window)Facebook](https://diyelectromusic.com/2025/06/27/xiao-esp32-c3-midi-synthesizer-part-4/?share=facebook&nb=1)

LikeLoading...

### _Related_

[XIAO ESP32-C3 MIDI Synthesizer – Part 2](https://diyelectromusic.com/2025/06/27/xiao-esp32-c3-midi-synthesizer-part-2/ "XIAO ESP32-C3 MIDI Synthesizer &#8211; Part&nbsp;2")June 27, 2025In "Intermediate"

[XIAO ESP32-C3 MIDI Synthesizer – Part 5](https://diyelectromusic.com/2025/06/28/xiao-esp32-c3-midi-synthesizer-part-5/ "XIAO ESP32-C3 MIDI Synthesizer &#8211; Part&nbsp;5")June 28, 2025In "Beginner"

[XIAO USB Device to Serial MIDI Converter](https://diyelectromusic.com/2025/06/28/xiao-usb-device-to-serial-midi-converter/ "XIAO USB Device to Serial MIDI&nbsp;Converter")June 28, 2025In "MIDI Control"

- Tagged
- [control change](https://diyelectromusic.com/tag/control-change/)
- [midi](https://diyelectromusic.com/tag/midi/)
- [program change](https://diyelectromusic.com/tag/program-change/)
- [SAM2695](https://diyelectromusic.com/tag/sam2695/)
- [samd21](https://diyelectromusic.com/tag/samd21/)
- [usb host](https://diyelectromusic.com/tag/usb-host/)
- [xiao](https://diyelectromusic.com/tag/xiao/)

![Unknown's avatar](https://0.gravatar.com/avatar/05a6c4f760c447e8abcc799efe819572ce2896b0b4ded36a78ef24563a866904?s=80&d=identicon&r=G)

## Published by Kevin

Just another soul meandering my way around the Internet ... [View all posts by Kevin](https://diyelectromusic.com/author/emalliab/)

**Published** June 27, 2025June 29, 2025

### Leave a comment [Cancel reply](https://diyelectromusic.com/2025/06/27/xiao-esp32-c3-midi-synthesizer-part-4/\#respond)

Δ

- [Comment](https://diyelectromusic.com/2025/06/27/xiao-esp32-c3-midi-synthesizer-part-4/#respond)
- [Reblog](https://diyelectromusic.com/2025/06/27/xiao-esp32-c3-midi-synthesizer-part-4/)
- [Subscribe](https://diyelectromusic.com/2025/06/27/xiao-esp32-c3-midi-synthesizer-part-4/) [Subscribed](https://diyelectromusic.com/2025/06/27/xiao-esp32-c3-midi-synthesizer-part-4/)








  - [![](https://diyelectromusic.com/wp-content/uploads/2020/05/cropped-clef_512x512.png?w=50) Simple DIY Electronic Music Projects](https://diyelectromusic.com/)

Join 213 other subscribers

Sign me up

  - Already have a WordPress.com account? [Log in now.](https://wordpress.com/log-in?redirect_to=https%3A%2F%2Fr-login.wordpress.com%2Fremote-login.php%3Faction%3Dlink%26back%3Dhttps%253A%252F%252Fdiyelectromusic.com%252F2025%252F06%252F27%252Fxiao-esp32-c3-midi-synthesizer-part-4%252F)


- - [![](https://diyelectromusic.com/wp-content/uploads/2020/05/cropped-clef_512x512.png?w=50) Simple DIY Electronic Music Projects](https://diyelectromusic.com/)
  - [Subscribe](https://diyelectromusic.com/2025/06/27/xiao-esp32-c3-midi-synthesizer-part-4/) [Subscribed](https://diyelectromusic.com/2025/06/27/xiao-esp32-c3-midi-synthesizer-part-4/)
  - [Sign up](https://wordpress.com/start/)
  - [Log in](https://wordpress.com/log-in?redirect_to=https%3A%2F%2Fr-login.wordpress.com%2Fremote-login.php%3Faction%3Dlink%26back%3Dhttps%253A%252F%252Fdiyelectromusic.com%252F2025%252F06%252F27%252Fxiao-esp32-c3-midi-synthesizer-part-4%252F)
  - [Copy shortlink](https://wp.me/pc1YuP-4FZ)
  - [Report this content](https://wordpress.com/abuse/?report_url=https://diyelectromusic.com/2025/06/27/xiao-esp32-c3-midi-synthesizer-part-4/)
  - [View post in Reader](https://wordpress.com/reader/blogs/177786911/posts/17979)
  - [Manage subscriptions](https://subscribe.wordpress.com/)
  - [Collapse this bar](https://diyelectromusic.com/2025/06/27/xiao-esp32-c3-midi-synthesizer-part-4/)

[Toggle photo metadata visibility](https://diyelectromusic.com/2025/06/27/xiao-esp32-c3-midi-synthesizer-part-4/#)[Toggle photo comments visibility](https://diyelectromusic.com/2025/06/27/xiao-esp32-c3-midi-synthesizer-part-4/#)

Loading Comments...

Write a Comment...

Email (Required)Name (Required)Website

%d