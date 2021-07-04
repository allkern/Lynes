#pragma once

#include "../../global.hpp"

#include "common.hpp"
#include "memory.hpp"
#include "square.hpp"
#include "triangle.hpp"
#include "noise.hpp"
#include "io.hpp"

#include <array>

namespace nes {
    namespace apu {
        void init() {
            sq0.init(&registers[0x0]);
            sq1.init(&registers[0x4]);
            tri.init(&registers[0x8]);
            noi.init(&registers[0xc]);
        }

        int16_t get_sample() {
            return (sq0.get_sample() + sq1.get_sample() + tri.get_sample() + noi.get_sample()) / 4;
        }
    }
}