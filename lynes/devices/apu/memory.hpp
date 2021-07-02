#pragma once

#include "../../global.hpp"

#include <array>

namespace nes {
    namespace apu {
        typedef std::array <u8, 0x18> registers_t;

        registers_t registers;
    }
}