#pragma once

#include "../log.hpp"
#include "../global.hpp"

#include <fstream>
#include <vector>
#include <array>

#define CART_BEGIN 0x4020
#define CART_END   0xffff

namespace nes {
    namespace cart {
        struct header_t {
            char id[4];
            u8 prg_rom_size,
               chr_rom_size,
               flags[5],
               pad[5];
        } header;

        u8 mapper = 0;

        std::array <u8, 0x4000> rom;

        void load(const char* file) {
            std::ifstream f;

            f.open(file, std::ios::binary);

            if (!(f.good() && f.is_open())) {
                _log(error, "Couldn't open ROM file \"%s\"", file);

                std::exit(1);
            }

            f.read((char*)&header, sizeof(header_t));

            mapper = (header.flags[0] >> 4) & 0xf | ((header.flags[1] >> 4) & 0xf) << 4;

            // _log(debug, "mapper=%02x, magic=%s, prg_rom_size=%u, chr_rom_size=%u, flags={%02x, %02x, %02x, %02x, %02x}",
            //     mapper,
            //     &header.id[0],
            //     header.prg_rom_size,
            //     header.chr_rom_size,
            //     header.flags[0],
            //     header.flags[1],
            //     header.flags[2],
            //     header.flags[3],
            //     header.flags[4]
            // );

            f.read((char*)rom.data(), 0x4000 * header.prg_rom_size);
        }

        u8 read(u16 addr) {
            if ((addr >= 0x8000) && (addr <= 0xffff)) {
                return rom[(addr - 0x8000) & 0x3fff];
            }

            return 0x0;
        }
    }
}