#pragma once

#include "../../global.hpp"

namespace nes {
    namespace cart {
        struct mapper_t {
            virtual u8 read(u16, bool) = 0;
            virtual void write(u16, u8, bool) = 0;
        };
    }
}