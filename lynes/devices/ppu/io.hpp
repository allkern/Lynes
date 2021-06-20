#pragma once

#include "bus.hpp"

namespace nes {
    namespace ppu {
        void write(u16 addr, u8 value) {
            if (addr == 0x2006) {
                if (!ppuaddr_latch) {
                    ppuaddr = value << 8;
                    ppuaddr_latch = true;
                } else {
                    ppuaddr |= value;
                    ppuaddr_latch = false;
                }

                return;
            }

            if (addr == 0x2007) {
                bus::write(ppuaddr & 0x3fff, value);

                ppuaddr += TEST_REG(PPUCTRL, VADDRINC) ? 32 : 1;

                return;
            }

            r[addr & 0x7] = value;
        }

        u8 read(u16 addr) {
            if (addr == 0x2002) {
                u8 b = r[addr & 0x7] | 0x40; // Sprite 0 hit hack, fix
                r[0x2] &= 0x7f;
                return b;
            }

            if (addr == 0x2007) {
                u8 b = bus::read(ppuaddr & 0x3fff);
                
                ppuaddr += TEST_REG(PPUCTRL, VADDRINC) ? 32 : 1;

                return b;
            }

            return r[addr & 0x7];
        }
    }
}