---
tags: [firmware, sam2695, midi, sounds, general-midi]
---

# SAM2695 — Sounds: General MIDI + MT-32 Variation

⬅ [[00-SAM2695-Index]]

## Main sounds — General MIDI (all channels except 10)
Program Change (PC) 1–128 maps to the standard GM instrument set.

| PC | Instrument | PC | Instrument | PC | Instrument | PC | Instrument |
|---|---|---|---|---|---|---|---|
| 1 | Grand Piano 1 | 33 | Acoustic Bass | 65 | Soprano Sax | 97 | FX 1 (rain) |
| 2 | Bright Piano 2 | 34 | Finger Bass | 66 | Alto Sax | 98 | FX 2 (soundtrack) |
| 3 | El. Grd Piano 3 | 35 | Picked Bass | 67 | Tenor Sax | 99 | FX 3 (crystal) |
| 4 | Honky-tonk Piano | 36 | Fretless Bass | 68 | Baritone Sax | 100 | FX 4 (atmosphere) |
| 5 | El. Piano 1 | 37 | Slap Bass 1 | 69 | Oboe | 101 | FX 5 (brightness) |
| 6 | El. Piano 2 | 38 | Slap Bass 2 | 70 | English Horn | 102 | FX 6 (goblins) |
| 7 | Harpsichord | 39 | Synth Bass 1 | 71 | Bassoon | 103 | FX 7 (echoes) |
| 8 | Clavi | 40 | Synth Bass 2 | 72 | Clarinet | 104 | FX 8 (sci-fi) |
| 9 | Celesta | 41 | Violin | 73 | Piccolo | 105 | Sitar |
| 10 | Glockenspiel | 42 | Viola | 74 | Flute | 106 | Banjo |
| 11 | Music Box | 43 | Cello | 75 | Recorder | 107 | Shamisen |
| 12 | Vibraphone | 44 | Contrabass | 76 | Pan Flute | 108 | Koto |
| 13 | Marimba | 45 | Tremolo Strings | 77 | Blown Bottle | 109 | Kalimba |
| 14 | Xylophone | 46 | Pizzicato Strings | 78 | Shakuhachi | 110 | Bag pipe |
| 15 | Tubular Bells | 47 | Orchestral Harp | 79 | Whistle | 111 | Fiddle |
| 16 | Santur | 48 | Timpani | 80 | Ocarina | 112 | Shanai |
| 17 | Drawbar Organ | 49 | String Ensemble 1 | 81 | Lead 1 (square) | 113 | Tinkle Bell |
| 18 | Percussive Organ | 50 | String Ensemble 2 | 82 | Lead 2 (sawtooth) | 114 | Agogo |
| 19 | Rock Organ | 51 | Synth Strings 1 | 83 | Lead 3 (calliope) | 115 | Steel Drums |
| 20 | Church Organ | 52 | Synth Strings 2 | 84 | Lead 4 (chiff) | 116 | Woodblock |
| 21 | Reed Organ | 53 | Choir Aahs | 85 | Lead 5 (charang) | 117 | Taiko Drum |
| 22 | Accordion (french) | 54 | Voice Oohs | 86 | Lead 6 (voice) | 118 | Melodic Tom |
| 23 | Harmonica | 55 | Synth Voice | 87 | Lead 7 (fifths) | 119 | Synth Drum |
| 24 | Tango Accordion | 56 | Orchestra Hit | 88 | Lead 8 (bass+lead) | 120 | Reverse Cymbal |
| 25 | Ac. Guitar (nylon) | 57 | Trumpet | 89 | Pad 1 (fantasia) | 121 | Gt. Fret Noise |
| 26 | Ac. Guitar (steel) | 58 | Trombone | 90 | Pad 2 (warm) | 122 | Breath Noise |
| 27 | El. Guitar (jazz) | 59 | Tuba | 91 | Pad 3 (polysynth) | 123 | Seashore |
| 28 | El. Guitar (clean) | 60 | Muted Trumpet | 92 | Pad 4 (choir) | 124 | Bird Tweet |
| 29 | El. Guitar (muted) | 61 | French Horn | 93 | Pad 5 (bowed) | 125 | Teleph. Ring |
| 30 | Overdriven Guitar | 62 | Brass Section | 94 | Pad 6 (metallic) | 126 | Helicopter |
| 31 | Distortion Guitar | 63 | Synth Brass 1 | 95 | Pad 7 (halo) | 127 | Applause |
| 32 | Guitar harmonics | 64 | Synth Brass 2 | 96 | Pad 8 (sweep) | 128 | Gunshot |

Instruments needing **2 voices** (2-layer): see [[16-SAM2695-Appendix]]#instruments-requiring-2-voices — matters for polyphony budgeting alongside [[14-SAM2695-Polyphony-Effects-Select]].

## MT-32 sound variation #127
(all channels except 10). To select: send `CC 0 = 127`, then Program Change.

| PC | Name | PC | Name | PC | Name | PC | Name |
|---|---|---|---|---|---|---|---|
| 1 | Piano 1 | 2 | Piano 2 | 3 | Piano 3 | 4 | Detuned EP 1 |
| 5 | E.Piano1 | 6 | E.Piano2 | 7 | Detuned EP2 | 8 | Honky-Tonk |
| 9 | Organ 1 | 10 | Organ 2 | 11 | Organ 3 | 12 | Detuned Or. 1 |
| 13 | Church Org. 2 | 14 | Church Org. | 15 | Church Org. | 16 | Accordion Fr. |
| 17 | Harpsichord | 18 | Coupled Hps. | 19 | Coupled Hps. | 20 | Clav. |
| 21 | Clav. | 22 | Clav. | 23 | Celesta | 24 | Celesta |
| 25 | Synth Brass1 | 26 | Synth Brass2 | 27 | Synth Brass3 | 28 | Synth Brass4 |
| 29 | Synth Bass1 | 30 | Synth Bass2 | 31 | Synth Bass3 | 32 | Synth Bass4 |
| 33 | Fantasia | 34 | Syn Calliope | 35 | Choir Aahs | 36 | Bowed Glass |
| 37 | Soundtrack | 38 | Atmosphere | 39 | Crystal | 40 | Bag Pipe |
| 41 | Tinkle Bell | 42 | Ice Rain | 43 | Oboe | 44 | Pan Flute |
| 45 | Saw Wave | 46 | Charang | 47 | Tubular Bells | 48 | Square Wave |
| 49 | Strings | 50 | Tremolo Str. | 51 | Slow Strings | 52 | Pizzicato Str. |
| 53 | Violin | 54 | Viola | 55 | Cello | 56 | Cello |
| 57 | Contrabass | 58 | Harp | 59 | Harp | 60 | Nylon-str. Gt |
| 61 | Steel-Str. Gt | 62 | Chorus Gt. | 63 | Funk Gt. | 64 | Sitar |
| 65 | Acoustic Bs. | 66 | Fingered Bs. | 67 | Picked Bs. | 68 | Fretless Bs. |
| 69 | Slap Bs. 1 | 70 | Slap Bs. 2 | 71 | Fretless Bs. | 72 | Fretless Bs. |
| 73 | Flute | 74 | Flute | 75 | Piccolo | 76 | Piccolo |
| 77 | Recorder | 78 | Pan Flute | 79 | Soprano Sax | 80 | Alto Sax |
| 81 | Tenor Sax | 82 | Baritone Sax | 83 | Clarinet | 84 | Clarinet |
| 85 | Oboe | 86 | English Horn | 87 | Bassoon | 88 | Harmonica |
| 89 | Trumped | 90 | Muted Trumpet | 91 | Trombone | 92 | Trombone |
| 93 | French Horn | 94 | French Horn | 95 | Tuba | 96 | Brass |
| 97 | Brass 2 | 98 | Vibraphone | 99 | Vibraphone | 100 | Kalimba |
| 101 | Tinkle Bell | 102 | Glockenspiel | 103 | Tubular-Bell | 104 | Xylophone |
| 105 | Marimba | 106 | Koto | 107 | Taisho Koto | 108 | Shakuhachi |
| 109 | Whistle | 110 | Whistle | 111 | Bottle Blow | 112 | Pan Flute |
| 113 | Timpani | 114 | Melo Tom | 115 | Melo Tom | 116 | Synth Drum |
| 117 | Synth Drum | 118 | Taiko | 119 | Taiko | 120 | Reverse Cym. |
| 121 | Castanets | 122 | Tinkle Bell | 123 | Orchestra Hit | 124 | Telephone |
| 125 | Bird | 126 | Helicopter | 127 | Bowed Glass | 128 | Ice Rain |

## Related
- [[13-SAM2695-Drum-Sets]]
- [[11-SAM2695-MIDI-Implementation]]
- [[16-SAM2695-Appendix]]
