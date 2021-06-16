#pragma once

#include "global.hpp"

#include "devices/ram.hpp"
#include "devices/ppu.hpp"
#include "devices/cart.hpp"

#define IN_RANGE(l, h) ((addr >= l) && (addr <= h))

namespace nes {
    namespace bus {
        u8 read(u16 addr) {
            if (IN_RANGE(RAM_BEGIN , RAM_END )) { return ram::read(addr); }
            if (IN_RANGE(PPU_BEGIN , PPU_END )) { return ppu::read(addr); }
            if (IN_RANGE(CART_BEGIN, CART_END)) { return cart::read(addr); }

            return 0x0;
        }

        void write(u16 addr, u8 value) {
            if (IN_RANGE(RAM_BEGIN, RAM_END)) { ram::write(addr, value); }
            if (IN_RANGE(PPU_BEGIN, PPU_END)) { ppu::write(addr, value); }
        }
    }
}

#undef IN_RANGE