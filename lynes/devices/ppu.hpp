#pragma once

#include "../global.hpp"

#include <array>

#define PPU_BEGIN 0x2000
#define PPU_END   0x3fff

namespace nes {
    namespace ppu {
        u8 r[0x8] = { 0 };

        u8 read(u16 addr) {
            return r[addr & 0x7];
        }

        void write(u16 addr, u8 value) {
            r[addr & 0x7] = value;
        }
    }
}