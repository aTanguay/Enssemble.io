---
tags: [firmware, sam2695, midi, drums]
---

# SAM2695 — Drum Set Table (MIDI Channel 10)

⬅ [[00-SAM2695-Index]]

Select drumset via Program Change on channel 10. Five drum-kit programs: **Prog 1** Standard, **Prog 17** Power, **Prog 41** Brush, **Prog 49** Orchestra, **Prog 127/128** CM-64/32 (Partial).

`*` = no sound. Blank = same sound as Standard Set. `[EXC]` = sounds sharing an EXC number are mutually exclusive (choking each other, like real hi-hats).

| Note | Standard (1) | Power (17) | Brush (41) | Orchestra (49) | CM-64/32 (127/128) |
|---|---|---|---|---|---|
| 27 D#1 | | | | Closed Hi Hat [EXC1] | * |
| 28 E1 | | | | Pedal Hi-Hat [EXC1] | * |
| 29 F1 | | | | Open Hi Hat [EXC1] | * |
| 30 F#1 | | | | Ride Cymbal | * |
| 31 G1 | | | | | * |
| 32 G#1 | | | | | * |
| 33 A1 | | | | | * |
| 34 A#1 | | | | | * |
| 35 B1 | Kick drum2 | | Jazz BD 2 | | Kick drum |
| 36 C2 | Kick drum1 | | Jazz BD 1 | | Kick drum |
| 37 C#2 | Side Stick | | | | Rim Shot |
| 38 D2 | Snare Drum 1 | Gated Snare | Brush Tap | Snare Drum 2 | Snare Drum |
| 39 D#2 | Hand Clap | | Brush Slap | Castanets | Hand Clap |
| 40 E2 | Snare Drum 2 | | Brush Swirl | Snare Drum 2 | Elec Snare Drum |
| 41 F2 | Low Floor Tom | | | Timpani F | Acoustic Low Tom |
| 42 F#2 | Closed Hi Hat [EXC1] | | | Timpani F# | Closed Hi-Hat [EXC1] |
| 43 G2 | High Floor Tom | | | Timpani G | Acoustic Low Tom |
| 44 G#2 | Pedal Hi-Hat [EXC1] | | | Timpani G# | Open Hi-Hat 2 |
| 45 A2 | Low Tom | | | Timpani A | Acoustic Middle Tom |
| 46 A#2 | Open Hi-Hat [EXC1] | | | Timpani A# | Open Hi-Hat 1 [EXC1] |
| 47 B2 | Low-Mid Tom | | | Timpani B | Acoustic Middle Tom |
| 48 C3 | Hi Mid Tom | | | Timpani c | Acoustic High Tom |
| 49 C#3 | Crash Cymbal 1 | | | Timpani c# | Crash Cymbal |
| 50 D3 | High Tom | | | Timpani d | Acoustic High Tom |
| 51 D#3 | Ride Cymbal 1 | | | Timpani d# | Ride Cymbal |
| 52 E3 | Chinese Cymbal | | | Timpani e | * |
| 53 F3 | Ride Bell | | | Timpani f | * |
| 54 F#3 | Tambourine | | | | Tambourine |
| 55 G3 | Splash Cymbal | | | | * |
| 56 G#3 | Cowbell | | | | Cowbell |
| 57 A3 | Crash Cymbal 2 | | | | * |
| 58 A#3 | Vibraslap | | | | * |
| 59 B3 | Ride Cymbal 2 | | | | * |
| 60 C4 | Hi Bongo | | | | |
| 61 C#4 | Low Bongo | | | | |
| 62 D4 | Mute Hi Conga | | | | |
| 63 D#4 | Open Hi Conga | | | | |
| 64 E4 | Low Conga | | | | |
| 65 F4 | High Timbale | | | | |
| 66 F#4 | Low Timbale | | | | |
| 67 G4 | High Agogo | | | | |
| 68 G#4 | Low Agogo | | | | |
| 69 A4 | Cabasa | | | | |
| 70 A#4 | Maracas | | | | |
| 71 B4 | Short Whistle[EXC2] | | | | |
| 72 C5 | Long Whistle[EXC2] | | | | |
| 73 C#5 | Short Guiro [EXC3] | | | | Vibra Slap |
| 74 D5 | Long Guiro [EXC3] | | | | * |
| 75 D#5 | Claves | | | | Claves |
| 76 E5 | Hi Wood Block | | | | * |
| 77 F5 | Low Wood Block | | | | * |
| 78 F#5 | Mute Cuica [EXC4] | | | | * |
| 79 G5 | Open Cuica [EXC4] | | | | * |
| 80 G#5 | Mute Triangle [EXC5] | | | | * |
| 81 A5 | Open Triangle [EXC5] | | | | * |
| 82 A#5 | | | | | Applauses |
| 83 B5 | | | | | * |
| 84 C6 | | | | | * |
| 85 C#6 | | | | | * |
| 86 D6 | | | | | * |
| 87 D#6 | | | | | * |
| 88 E6 | | | | Applauses | * |
| 89 F6 | | | | | * |
| 90 F#6 | | | | | * |
| 91 G6 | | | | | * |
| 92 G#6 | | | | | * |
| 93 A6 | | | | | * |
| 94 A#6 | | | | | Helicopter |
| 95 B6 | | | | | * |
| 96 C7 | | | | | Gun Shot |
| 97 C#7 | | | | | * |
| 98 D7 | | | | | * |
| 99 D#7 | | | | | * |
| 100 E7 | | | | | * |
| 101 F7 | | | | | * |
| 102 F#7 | | | | | Birds |
| 103 G7 | | | | | * |
| 104 G#7 | | | | | * |
| 105 A7 | | | | | * |
| 106 A#7 | | | | | SeaShore |

## Related
- [[12-SAM2695-Sounds-GM-MT32]]
- [[11-SAM2695-MIDI-Implementation]]
