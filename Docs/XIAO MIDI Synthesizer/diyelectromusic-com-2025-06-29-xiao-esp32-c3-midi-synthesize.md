[Skip to content](https://diyelectromusic.com/2025/06/29/xiao-esp32-c3-midi-synthesizer-part-6/#content)

![](https://diyelectromusic.com/wp-content/uploads/2020/06/cropped-montage-2.png)

[Kevin](https://diyelectromusic.com/author/emalliab/)[Beginner](https://diyelectromusic.com/category/project-difficulty/beginner/), [MIDI Control](https://diyelectromusic.com/category/project-categories/midi-control/), [Synth Modules](https://diyelectromusic.com/category/project-categories/synth-modules/)June 29, 2025June 29, 20255 Minutes

Expanding on my previous posts, I thought it might be interesting to see how I might be able to add some additional IO to the MIDI Synth. This is an exploration of some options there.

- [Part 1](https://diyelectromusic.com/2025/06/26/xiao-esp32-c3-midi-synthesizer/) – Getting started and getting code running.
- [Part 2](https://diyelectromusic.com/2025/06/27/xiao-esp32-c3-midi-synthesizer-part-2/) – Swapping the ESP32-C3 for a SAMD21 to get USB MIDI.
- [Part 3](https://diyelectromusic.com/2025/06/27/xiao-esp32-c3-midi-synthesizer-part-3/) – Taking a deeper look at the SAM2695 itself.
- [Part 4](https://diyelectromusic.com/2025/06/27/xiao-esp32-c3-midi-synthesizer-part-4/) – A USB MIDI Synth Module using the SAMD21 again as a USB MIDI Host.
- [Part 5](https://diyelectromusic.com/2025/06/28/xiao-esp32-c3-midi-synthesizer-part-5/) – A Serial MIDI Synth Module using the original ESP32-C3.
- [Part 6](https://diyelectromusic.com/2025/06/29/xiao-esp32-c3-midi-synthesizer-part-6/) – Pairs the Synth with a XIAO Expansion board to add display and potentiometers.

![](https://diyelectromusic.com/wp-content/uploads/2025/06/img_8597.jpeg?w=1024)

_**Warning!** I strongly recommend using old or second hand equipment for your experiments.  I am not responsible for any damage to expensive instruments!_

These are the key tutorials for the main concepts used in this project:

- [Getting Started with the XIAO MIDI Synthesizer](https://wiki.seeedstudio.com/xiao_midi_synthesizer/)
- [XIAO SAMD21, Arduino and MIDI](https://diyelectromusic.com/2023/03/12/xiao-samd21-arduino-and-midi/)
- [XIAO SAMD21, Arduino and MIDI – Part 6](https://diyelectromusic.com/2023/04/11/xiao-samd21-arduino-and-midi-part-6/)

If you are new to microcontrollers, see the [Getting Started](https://diyelectromusic.wordpress.com/getting-started/) pages.

### The Synth Grove Connector

One option to immediately explore for me was the Grove connector on the Synth – highlighted by the blue rectangle in the photo below. I’m thinking at this stage of the XIAO Expander Module ( [more here](https://diyelectromusic.com/2023/04/11/xiao-samd21-arduino-and-midi-part-6/)) and how that might give some options for easily hooking up to the Synth.

![](https://diyelectromusic.com/wp-content/uploads/2025/06/img_8594.jpeg?w=1024)

There one obvious issue with this, and one not so obvious issue.

First, of course, there is no access to this connector through the case. My initial thought was to simply remove the PCB from the case and use it as a stand-alone board. On initial inspection it seemed that there were two screws holding it down. Not so, a more thorough inspection (after remove the two screws and still not being able to remove it), revealed a third screw underneath the “light pipe” for the LEDs.

Unfortunately that light pipe is pretty well wedged into the case making removal particularly tricky. But without removing the light pipe, it isn’t possible to get to the screw at all.

I did wonder about making a hole in the 3D printed case. A better option might be to get hold of the published 3D print files and add a hole and make my own (they are available via the product page).

But both options would probably end up changing the original case somehow – even if printing my own, I still need to get the original PCB out somehow and that brings me back to the light pipe issue.

The second issue isn’t quite so obvious. In that photo we can see that the pins for the Grove connector are labelled as follows (top to bottom):

- NC
- TX
- 5V
- GND

The UART on the XIAO expander board, which I’d like to use, is labelled:

- RX7
- TX6
- 3V3
- GND

Checking in with the Synth schematic, the connector is wired as follows:

![](https://diyelectromusic.com/wp-content/uploads/2025/06/image-13.png?w=336)

SYS\_MIDI connects to the MIDI\_IN pin of the SAM2695, so actually connecting “TX to TX” in this instance should be ok.

5V might be an issue though, as it really does look like (to me) that it really means 5V – it is the input to the TPL740F33 that generates the 3V3 power signal, as well as feeding the amplifier directly. The datasheet of the TPL740F33 does seem to imply that if receiving 3V3 it can still generate 3V3 so it might be ok? The amplifier obviously won’t be as powerful though running off 3V3.

Anyway, for now, instead I’ve just opted to use the GPIO again, wired into the expansion sockets with the XIAO removed.

At the XIAO expander end, I’ve used the additional pins rather than the Grove connector, as they support a 5V output.

![](https://diyelectromusic.com/wp-content/uploads/2025/06/xiao-expansion-midi-synth_bb.png?w=1024)

The downsides to this approach:

- I’m not using the Grove connectors, which would have been really neat.
- I have no access to the four buttons on the XIAO MIDI Synth.

But I do now have access to two I2C Grove connectors, a GPIO Grove, and the RX part of the UART Grove too as well as the on-board display.

If a XIAO SAMD21 is used, then the previous code for USB to the Synth can be used directly – see [XIAO ESP32-C3 MIDI Synthesizer – Part 2](https://diyelectromusic.com/2025/06/27/xiao-esp32-c3-midi-synthesizer-part-2/).

If the XIAO ESP32-C3 is used, then an additional serial MIDI connection is required. This can be connected to the Grove UART connector (using the RX pin, and leaving TX unconnected) or the RX pin of the additional 8-way pin header on the expansion board. Then the code from this will work directly: [XIAO ESP32-C3 MIDI Synthesizer – Part 5](https://diyelectromusic.com/2025/06/28/xiao-esp32-c3-midi-synthesizer-part-5/).

### Adding a Display and Program Control

I already have some code that has done this for a XIAO on an expansion board here [XIAO SAMD21, Arduino and MIDI – Part 6](https://diyelectromusic.com/2023/04/11/xiao-samd21-arduino-and-midi-part-6/).

But for this to work usefully with the Synth module, I need to adjust the routing so that MIDI goes from USB to serial, but the program change messages are also sent via serial to the synth module. That has already been address in previous parts, to I just need to merge the code with that from [XIAO ESP32-C3 MIDI Synthesizer – Part 4](https://diyelectromusic.com/2025/06/27/xiao-esp32-c3-midi-synthesizer-part-4/).

This is the result.

![](https://diyelectromusic.com/wp-content/uploads/2025/06/img_8595.jpeg?w=1024)

There is a bit of jitter on the analog pot, but that is only because I’m using the original fairly simplified algorithm to detect changes. If I was fussed about it, I’d reuse the averaging class from [Arduino MIDI Atari Paddles](https://diyelectromusic.com/2025/06/23/arduino-midi-atari-paddles/). And to be honest, a capacitor on the pot would probably go quite a long way too…

As a test, I also powered the device from the Grove UART port connecting it as follows:

- Expander GND – GND Synth
- Expander 3V3 – 5V IN Synth
- Expander TX – RX/D6 Synth
- Expander RX – N/C

And this all worked fine. So I think a Grove to Grove lead would work fine if I had access to the Synth’s Grove port.

This does mean that the exact same code can work with the M5 Synth module using a Grove to Grove lead. The downside of this, even though it is a lot simpler in connectivity terms, is that there is now external audio out like there is on the XIAO Synth.

![](https://diyelectromusic.com/wp-content/uploads/2025/06/img_8596.jpeg?w=1024)

For completeness the same code can be used with the XIAO ESP32-C3 and serial MIDI, see the photo at the start of this blog.

To turn off all USB handling in the code, the following must be commented out:

```
//#define HAS_USB
//#define SER_TO_USB
//#define MIDI_USB_PCCC
```

For other parts of the code, the Arduino abstraction for A0 maps over to the ESP32-C3 fine. The only thing to watch out for is the increased analog resolution from 10 to 12 bits, but a call to analogReadResolution(10) drops that back to the expected 10 bits.

Oh and the Serial port to use is different:

- XIAO SAMD21: Serial1
- XIAO ESP32-C3: Serial0

[Find it on GitHub here](https://github.com/diyelectromusic/sdemp/tree/main/src/SDEMP/XiaoSynthMIDIPCCC).

### Closing Thoughts

If I can be bothered, it would be nice to actually display the General MIDI voice name on the display. The SAM2695 also has its MT-32 mode, so having some means of selecting that might be interesting too.

And so far I’ve largely only messed about with driving it on a single MIDI channel, so there is a lot more that could be done there.

Kevin

### Share this:

- [Share on X (Opens in new window)X](https://diyelectromusic.com/2025/06/29/xiao-esp32-c3-midi-synthesizer-part-6/?share=twitter&nb=1)
- [Share on Facebook (Opens in new window)Facebook](https://diyelectromusic.com/2025/06/29/xiao-esp32-c3-midi-synthesizer-part-6/?share=facebook&nb=1)

LikeLoading...

### _Related_

[XIAO ESP32-C3 MIDI Synthesizer – Part 2](https://diyelectromusic.com/2025/06/27/xiao-esp32-c3-midi-synthesizer-part-2/ "XIAO ESP32-C3 MIDI Synthesizer &#8211; Part&nbsp;2")June 27, 2025In "Intermediate"

[XIAO ESP32-C3 MIDI Synthesizer – Part 5](https://diyelectromusic.com/2025/06/28/xiao-esp32-c3-midi-synthesizer-part-5/ "XIAO ESP32-C3 MIDI Synthesizer &#8211; Part&nbsp;5")June 28, 2025In "Beginner"

[XIAO USB Device to Serial MIDI Converter](https://diyelectromusic.com/2025/06/28/xiao-usb-device-to-serial-midi-converter/ "XIAO USB Device to Serial MIDI&nbsp;Converter")June 28, 2025In "MIDI Control"

- Tagged
- [control change](https://diyelectromusic.com/tag/control-change/)
- [esp32c3](https://diyelectromusic.com/tag/esp32c3/)
- [midi](https://diyelectromusic.com/tag/midi/)
- [program change](https://diyelectromusic.com/tag/program-change/)
- [SAM2695](https://diyelectromusic.com/tag/sam2695/)
- [samd21](https://diyelectromusic.com/tag/samd21/)
- [usb midi](https://diyelectromusic.com/tag/usb-midi/)
- [xiao](https://diyelectromusic.com/tag/xiao/)

![Unknown's avatar](https://0.gravatar.com/avatar/05a6c4f760c447e8abcc799efe819572ce2896b0b4ded36a78ef24563a866904?s=80&d=identicon&r=G)

## Published by Kevin

Just another soul meandering my way around the Internet ... [View all posts by Kevin](https://diyelectromusic.com/author/emalliab/)

**Published** June 29, 2025June 29, 2025

## 3 thoughts on “XIAO ESP32-C3 MIDI Synthesizer – Part 6”

1. [@diyelectromusic.com](https://diyelectromusic.com/@diyelectromusic.com) since I've lost track of where this was going, I can only commend the byzantine heights it's attaining 🙂 Makes my MI porting efforts look pedestrian. (ducks'n'runs)



[Like](https://diyelectromusic.com/2025/06/29/xiao-esp32-c3-midi-synthesizer-part-6/?like_comment=3954&_wpnonce=dccecd3944) Liked by [1 person](https://diyelectromusic.com/2025/06/29/xiao-esp32-c3-midi-synthesizer-part-6/#)







[Reply as](https://diyelectromusic.com/2025/06/29/xiao-esp32-c3-midi-synthesizer-part-6/)


Reply on the Fediverse







## Remote Reply









#### Original Comment URL




Paste the comment URL into the search field of your favorite open social app or platform.




Comment URL Copy







#### Your Profile




Or, if you know your own profile, we can start things that way! Why do I need to enter my profile?


This site is part of the ⁂ open social web, a network of interconnected social platforms (like Mastodon, Pixelfed, Friendica, and others). Unlike centralized social media, your account lives on a platform of your choice, and you can interact with people across different platforms.




By entering your profile, we can send you to your account where you can complete this action.






Your Fediverse profile ReplyLoading…








Save my profile for future comments.











1. I do like the idea that you imagine there was any kind of direction to begin with 🙂



      I’m just messing around, as usual!



      Kevin



      [Like](https://diyelectromusic.com/2025/06/29/xiao-esp32-c3-midi-synthesizer-part-6/?like_comment=3955&_wpnonce=5ce64e98d5) Like





      [Reply](https://diyelectromusic.com/2025/06/29/xiao-esp32-c3-midi-synthesizer-part-6/?replytocom=3955#respond)
2. [@diyelectromusic.com](https://diyelectromusic.com/@diyelectromusic.com) I had not heard of the SAM2695. Thanks for pointing it out. I think M5Stack missed a trick by not including a headphone jack in their module.



[Like](https://diyelectromusic.com/2025/06/29/xiao-esp32-c3-midi-synthesizer-part-6/?like_comment=3957&_wpnonce=c3f79697b7) Liked by [1 person](https://diyelectromusic.com/2025/06/29/xiao-esp32-c3-midi-synthesizer-part-6/#)







[Reply as](https://diyelectromusic.com/2025/06/29/xiao-esp32-c3-midi-synthesizer-part-6/)


Reply on the Fediverse







## Remote Reply









#### Original Comment URL




Paste the comment URL into the search field of your favorite open social app or platform.




Comment URL Copy







#### Your Profile




Or, if you know your own profile, we can start things that way! Why do I need to enter my profile?


This site is part of the ⁂ open social web, a network of interconnected social platforms (like Mastodon, Pixelfed, Friendica, and others). Unlike centralized social media, your account lives on a platform of your choice, and you can interact with people across different platforms.




By entering your profile, we can send you to your account where you can complete this action.






Your Fediverse profile ReplyLoading…








Save my profile for future comments.


### Leave a comment [Cancel reply](https://diyelectromusic.com/2025/06/29/xiao-esp32-c3-midi-synthesizer-part-6/\#respond)

Δ

- [Comment](https://diyelectromusic.com/2025/06/29/xiao-esp32-c3-midi-synthesizer-part-6/#comments)
- [Reblog](https://diyelectromusic.com/2025/06/29/xiao-esp32-c3-midi-synthesizer-part-6/)
- [Subscribe](https://diyelectromusic.com/2025/06/29/xiao-esp32-c3-midi-synthesizer-part-6/) [Subscribed](https://diyelectromusic.com/2025/06/29/xiao-esp32-c3-midi-synthesizer-part-6/)








  - [![](https://diyelectromusic.com/wp-content/uploads/2020/05/cropped-clef_512x512.png?w=50) Simple DIY Electronic Music Projects](https://diyelectromusic.com/)

Join 213 other subscribers

Sign me up

  - Already have a WordPress.com account? [Log in now.](https://wordpress.com/log-in?redirect_to=https%3A%2F%2Fr-login.wordpress.com%2Fremote-login.php%3Faction%3Dlink%26back%3Dhttps%253A%252F%252Fdiyelectromusic.com%252F2025%252F06%252F29%252Fxiao-esp32-c3-midi-synthesizer-part-6%252F)


- - [![](https://diyelectromusic.com/wp-content/uploads/2020/05/cropped-clef_512x512.png?w=50) Simple DIY Electronic Music Projects](https://diyelectromusic.com/)
  - [Subscribe](https://diyelectromusic.com/2025/06/29/xiao-esp32-c3-midi-synthesizer-part-6/) [Subscribed](https://diyelectromusic.com/2025/06/29/xiao-esp32-c3-midi-synthesizer-part-6/)
  - [Sign up](https://wordpress.com/start/)
  - [Log in](https://wordpress.com/log-in?redirect_to=https%3A%2F%2Fr-login.wordpress.com%2Fremote-login.php%3Faction%3Dlink%26back%3Dhttps%253A%252F%252Fdiyelectromusic.com%252F2025%252F06%252F29%252Fxiao-esp32-c3-midi-synthesizer-part-6%252F)
  - [Copy shortlink](https://wp.me/pc1YuP-4GX)
  - [Report this content](https://wordpress.com/abuse/?report_url=https://diyelectromusic.com/2025/06/29/xiao-esp32-c3-midi-synthesizer-part-6/)
  - [View post in Reader](https://wordpress.com/reader/blogs/177786911/posts/18039)
  - [Manage subscriptions](https://subscribe.wordpress.com/)
  - [Collapse this bar](https://diyelectromusic.com/2025/06/29/xiao-esp32-c3-midi-synthesizer-part-6/)

[Toggle photo metadata visibility](https://diyelectromusic.com/2025/06/29/xiao-esp32-c3-midi-synthesizer-part-6/#)[Toggle photo comments visibility](https://diyelectromusic.com/2025/06/29/xiao-esp32-c3-midi-synthesizer-part-6/#)

Loading Comments...

Write a Comment...

Email (Required)Name (Required)Website

%d