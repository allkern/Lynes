#pragma once

#include "bus.hpp"

namespace nes {
    namespace ppu {
        void write(u16 addr, u8 value) {
            if (addr == 0x2005) {
                if (!ppuscroll_latch) {
                    fine_x = value;
                    ppuscroll_latch = true;
                } else {
                    fine_y = value;
                    ppuscroll_latch = false;
                }

                return;
            }

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

            if (addr == 0x2003) {
                oamaddr = value;
                return;
            }

            if (addr == 0x2004) {
                oam.at(oamaddr++) = value;

                return;
            }

            if (addr == 0x2007) {
                bus::write(ppuaddr & 0x3fff, value);

                ppuaddr += TEST_REG(PPUCTRL, VADDRINC) ? 32 : 1;

                return;
            }

            if (addr == 0x2000) {
                // if (value & BGPTADDR)
                //     _log(debug, "changing pattern table base to %04x", 0x1000 * ((value >> 4) & 0x1));
            }

            r[addr & 0x7] = value;
        }

        u8 ppudata_read_buf = 0;

        u8 read(u16 addr) {
            if (addr == 0x2002) {
                u8 b = r[addr & 0x7]; // Sprite 0 hit hack, fix
                r[0x2] &= 0x7f;
                return b;
            }

            if (addr == 0x2004) {
                return oam.at(oamaddr++);
            }

            if (addr == 0x2007) {
                u8 b = ppudata_read_buf;

                if (ppuaddr <= 0x3eff)
                    ppudata_read_buf = bus::read(ppuaddr & 0x3fff);
                
                ppuaddr += TEST_REG(PPUCTRL, VADDRINC) ? 32 : 1;

                return b;
            }

            return r[addr & 0x7];
        }
    }
}