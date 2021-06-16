#pragma once

#include "../global.hpp"

#include <array>

#define RAM_BEGIN 0x0000
#define RAM_END   0x1fff

namespace nes {
    namespace ram {
        typedef std::array <u8, 0x800> ram_t;

        ram_t buf;

        u8 read(u16 addr) {
            return buf[addr & 0x7ff];
        }

        void write(u16 addr, u8 value) {
            buf[addr & 0x7ff] = value;
        }
    }
}