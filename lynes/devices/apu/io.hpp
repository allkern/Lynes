#pragma once

#include "memory.hpp"

#include "../../global.hpp"
#include "../../log.hpp"

#define APU_BEGIN 0x4000
#define APU_END 0x4017

namespace nes {
    namespace apu {
        u16 timer = 0,
            length = 0;

        u8 read(u16 addr) {
            return 0x0;
        }

        void write(u16 addr, u8 value) {
            registers[addr-0x4000] = value;

            if (addr == 0x4003) sq0.update();
            if (addr == 0x4007) sq1.update();
        }
    }
}