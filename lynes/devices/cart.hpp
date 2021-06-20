#pragma once

#include "../log.hpp"
#include "../global.hpp"

#include <fstream>
#include <vector>
#include <array>

#include "mappers/header.hpp"
#include "mappers/mapper.hpp"
#include "mappers/nrom.hpp"

#define CART_BEGIN 0x4020
#define CART_END   0xffff

namespace nes {
    namespace cart {
        mapper_t* mapper = nullptr;

        void load(const char* file) {
            std::ifstream f;

            f.open(file, std::ios::binary);

            if (!(f.good() && f.is_open())) {
                _log(error, "Couldn't open ROM file \"%s\"", file);

                std::exit(1);
            }

            f.read((char*)&header, sizeof(header_t));

            switch (((header.flags[0] >> 4) & 0xf) | (((header.flags[1] >> 4) & 0xf) << 4)) {
                case 0x00: mapper = new nrom(&f, &header); break;
            }

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
        }

        u8 read(u16 addr, bool ppu = false) {
            return mapper->read(addr, ppu);
        }

        void write(u16 addr, u8 value, bool ppu = false) {
            return mapper->write(addr, value, ppu);
        }
    }
}