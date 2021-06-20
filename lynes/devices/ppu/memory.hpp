#pragma once

#include "../../global.hpp"

#include <array>

#define PPU_WIDTH  256
#define PPU_HEIGHT 240

#define PPU_BEGIN 0x2000
#define PPU_END   0x2007

#define LGW_FORMAT_RGBA8888
#include "lgw/framebuffer.hpp"

#define TEST_REG(R, B) (r[R] & B) 

#define PPUCTRL 0x0
#define VADDRINC 0b00000100
#define BGPTADDR 0b00010000
#define VBLKNMIE 0b10000000

namespace nes {
    namespace ppu {
        // PPU only
        std::array <u8, 0x2000> ram;

        lgw::framebuffer <PPU_WIDTH, PPU_HEIGHT> frame;

        // CPU exposed
        u8 r[0x8] = { 0 };

        bool ppuaddr_latch = false;
        u16 ppuaddr = 0x0;
    }
}


#undef PPU_WIDTH
#undef PPU_HEIGHT