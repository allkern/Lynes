<p align="center">
  <img width="65%" height="65%" src="https://user-images.githubusercontent.com/15825466/122704016-27e8ea80-d229-11eb-99fc-35961f874bc9.png" alt="Lynes">
</p>

# Lynes
An experimental NES emulator written in C++

## Current state
Some NROM (mapper 0) games are playable.

This is a non-exhaustive list of playable games:
- Donkey Kong
- Donkey Kong Jr.
- Kung Fu
- Mario Bros.
- Millipede
- Popeye


### CPU
The CPU is fully implemented, `nestest.nes` passes all tests

### PPU
The PPU is crude. This is what's currently implemented:
- Nametable 1 rendering
- Color
- Sprites
- Interrupts __*__

What's to be implemented soon:
- Scrolling
- Nametable mirroring
 
**Interrupts are based on a scheduler, not on actual PPU operation*

### APU
The APU hasn't been implemented yet

### Cartridge support
List of implemented mappers:
- NROM (`NES-NROM-128-NN`, `NES-NROM-256-NN`)

### Input devices
Input is supported, devices implemented:
- NES Standard Controller
