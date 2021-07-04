#pragma once

#include <cstdint>

namespace nes {
    typedef uint8_t  u8;
    typedef uint16_t u16;
    typedef uint32_t u32;
    typedef  int8_t  i8;

    template <typename T> inline int sign(T val) {
        return (T(0) < val) - (val < T(0));
    }
}