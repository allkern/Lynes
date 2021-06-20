# Lynes
An experimental NES emulator written in C++

The CPU is fully implemented, `nestest.nes` passes all tests

The PPU is extremely crude, only basic nametable rendering is supported, grayscale only, color and sprites not supported.
Interrupts are based on a scheduler, not on actual PPU operation

Input is supported, devices implemented:
- NES Standard Controller

APU not implemented yet
