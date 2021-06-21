#pragma once

#include "bus.hpp"

#define DMA_BEGIN 0x4014
#define DMA_END   0x4014

namespace nes {
    namespace dma {
        void write(u16 addr, u8 value) {
            for (int i = 0; i < 0x100; i++)
                ppu::oam.at(ppu::oamaddr++) = bus::read((value << 8) | i);
        }
    }
}