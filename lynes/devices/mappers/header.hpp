#pragma once

#include "../../log.hpp"
#include "../../global.hpp"

namespace nes {
    namespace cart {
        struct header_t {
            char id[4];
            u8 prg_rom_size,
               chr_rom_size,
               flags[5],
               pad[5];
        } header;
    }
}