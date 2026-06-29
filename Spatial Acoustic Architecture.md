# Project Manifesto: Spatial Acoustic Architecture

This project is dedicated to moving beyond the limitations of traditional stereo reproduction by exploring Spatial Acoustic Architecture. The philosophy is simple: music should not be a flat, compressed experience. It should be a physical event where sound occupies the same three-dimensional space as the listener.

## Core Philosophy

- **Sound as Physicality:** We reject the "stereo bore." By distributing sound sources physically throughout a room, we allow the room's natural acoustics to act as the final mixing console.
- **The Instrument is the Room:** The listener is not a passive audience member; they are an active participant, and their movement through the space determines the mix.
- **Zero-Friction Immersion:** Accessibility is paramount. The goal is to provide a setup that is easy for non-technical users to assemble, configure, and operate while maintaining the depth required for advanced musical performance.
- **Open Synthesis:** We favor permanent, open-source platforms like the AMYboard over transient maker-market hardware, ensuring the project remains sustainable and hackable for years to come.

## Project Roadmap

| **Phase**               | **Objective**                                                | **Primary Tech**                     |
| ----------------------- | ------------------------------------------------------------ | ------------------------------------ |
| 1. Foundation           | Establish BLE MIDI firmware on AMYboard (ESP32-S3) via ESP-IDF. | NimBLE, ESP-IDF, AMY Engine          |
| 2. Voice Categorization | Develop specialized firmware for "Band Members" (e.g., PCM Sampler, Analog Poly-Synth, FM Bassist). | AMY Wire Messages, Custom C Binaries |
| 3. The Conductor        | Implement a centralized "Band Leader" interface in AUM using Mozaic for patch management. | AUM3, Mozaic, MIDI CC/PC             |
| 4. Expansion            | Integration of tactile UI (I2C Encoders/OLED) for physical control in room-scale setups. | Grove I2C, Physical Encoders         |

## Distribution Strategy: The "DIY Ecosystem"

To avoid the pitfalls of hardware manufacturing, this project operates on a frictionless DIY model:

1. **GitHub Hub:** The central repository hosting source code, Mozaic scripts, and wiring guides.
2. **Web Flashers:** Use of ESP Web Tools to allow users to install firmware via browser without installing development environments.
3. **Community Sketches:** Sharing specialized configurations that allow users to buy stock boards and instantly transform them into designated band members.

## The Long-Term Goal

The success of this project is not measured by market share or competing with professional synthesizers. It is measured by the ability to democratize spatial audio, turning every living room into a site-specific concert hall.