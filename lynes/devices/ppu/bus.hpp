#pragma once

#include "../cart.hpp"

#include "memory.hpp"

#define IN_RANGE(l, h) ((addr >= l) && (addr <= h))

namespace nes {
    namespace ppu {
        namespace bus {
            u8 read(u16 addr) {
                if (IN_RANGE(0x0000, 0x1fff)) return cart::read(addr, true);
                if (IN_RANGE(0x2000, 0x3eff)) return ram.at(cart::translate_ciram_address(addr) - 0x2000);
                if (IN_RANGE(0x3f00, 0x3f1f)) return palette_ram.at(addr - 0x3f00);

                return 0x0;
            }

            void write(u16 addr, u8 value) {
                if (IN_RANGE(0x0000, 0x1fff)) { cart::write(addr, value, true); return; }
                if (IN_RANGE(0x2000, 0x3eff)) { ram.at(cart::translate_ciram_address(addr) - 0x2000) = value; return; }
                if (IN_RANGE(0X3f00, 0x3f1f)) {
                    if (
                        (addr == 0x3f10) ||
                        (addr == 0x3f14) ||
                        (addr == 0x3f18) ||
                        (addr == 0x3f1c)
                    ) {
                        addr &= ~0x0010;
                    }
                    palette_ram.at(addr - 0x3f00) = value; return;
                }
            }
        }
    }
}

#undef IN_RANGE