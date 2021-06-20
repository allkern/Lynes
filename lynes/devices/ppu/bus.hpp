#pragma once

#include "../cart.hpp"

#include "memory.hpp"

#define IN_RANGE(l, h) ((addr >= l) && (addr <= h))

namespace nes {
    namespace ppu {
        namespace bus {
            u8 read(u16 addr) {
                if (IN_RANGE(0x0000, 0x1fff)) return cart::read(addr, true);
                if (IN_RANGE(0x2000, 0x3fff)) return ram.at(addr - 0x2000);

                return 0x0;
            }

            void write(u16 addr, u8 value) {
                if (IN_RANGE(0x2000, 0x3fff)) { ram.at(addr - 0x2000) = value; return; }
            }
        }
    }
}

#undef IN_RANGE