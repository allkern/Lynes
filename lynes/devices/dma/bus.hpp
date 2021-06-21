#pragma once

#include "global.hpp"

#include "devices/ppu/ppu.hpp"
#include "devices/input.hpp"
#include "devices/cart.hpp"
#include "devices/ram.hpp"

#define IN_RANGE(l, h) ((addr >= l) && (addr <= h))

namespace nes {
    namespace dma {
        // DMA Bus
        // Exactly the same as the normal CPU bus, but without
        // circular dependencies, and writing.
        namespace bus {
            u8 read(u16 addr) {
                //_log(debug, "read addr=%04x", addr);
                if (IN_RANGE(RAM_BEGIN  , RAM_END  )) { return ram::read(addr); }
                if (IN_RANGE(PPU_BEGIN  , PPU_END  )) { return ppu::read(addr); }
                if (IN_RANGE(CART_BEGIN , CART_END )) { return cart::read(addr); }
                if (IN_RANGE(INPUT_BEGIN, INPUT_END)) { return input::read(addr); }

                return 0x0;
            }
        }
    }
}

#undef IN_RANGE