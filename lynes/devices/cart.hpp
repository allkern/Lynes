#pragma once

#include "../log.hpp"
#include "../global.hpp"

#include <fstream>
#include <vector>
#include <array>

#include "mappers/header.hpp"
#include "mappers/mapper.hpp"
#include "mappers/nrom.hpp"
#include "mappers/mmc1.hpp"

#define CART_BEGIN 0x4020
#define CART_END   0xffff

namespace nes {
    namespace cart {
        u8 mapper_number = 0;
        mapper_t* mapper = nullptr;

        void load(const char* file) {
            std::ifstream f;

            f.open(file, std::ios::binary);

            if (!(f.good() && f.is_open())) {
                _log(error, "Couldn't open ROM file \"%s\"", file);

                std::exit(1);
            }

            f.read((char*)&header, sizeof(header_t));

            mapper_number = ((header.flags[0] >> 4) & 0xf) | (((header.flags[1] >> 4) & 0xf) << 4);

            switch (mapper_number) {
                case 0x00: mapper = new nrom(&f, &header); break;
                case 0x01: mapper = new mmc1(&f, &header); break;
            }

            _log(debug, "mapper=%02x, magic=%.3s, prg_rom_size=%u (%u), chr_rom_size=%u (%u), flags={%02x, %02x, %02x, %02x, %02x}",
                mapper_number,
                &header.id[0],
                header.prg_rom_size * 0x4000,
                header.prg_rom_size,
                header.chr_rom_size * 0x2000,
                header.chr_rom_size,
                header.flags[0],
                header.flags[1],
                header.flags[2],
                header.flags[3],
                header.flags[4]
            );
        }

        u8 read(u16 addr, bool ppu = false) {
            return mapper->read(addr, ppu);
        }

        void write(u16 addr, u8 value, bool ppu = false) {
            return mapper->write(addr, value, ppu);
        }
    }
}