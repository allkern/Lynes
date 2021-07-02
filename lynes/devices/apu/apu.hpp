#pragma once

#include "../../global.hpp"

#include "memory.hpp"
#include "square.hpp"
#include "io.hpp"

#include <array>

namespace nes {
    namespace apu {
        void init() {
            sq0.init(&registers[0]);
            sq1.init(&registers[4]);
        }

        int16_t get_sample() {
            return (sq0.get_sample() + sq1.get_sample()) / 2;
        }
    }
}