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
#include "mappers/mmc3.hpp"
#include "mappers/uxrom.hpp"
#include "mappers/axrom.hpp"

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
                case 0x02: mapper = new uxrom(&f, &header); break;
                case 0x04: mapper = new mmc3(&f, &header); break;
                case 0x07: mapper = new axrom(&f, &header); break;
            }

            // if (mapper_number == 0x4) {
            //     //_log(debug, "readtest0 addr=%04x, return=%02x", 0x0000, mapper->read(0x0000, true));
            //     //_log(debug, "readtest1 addr=%04x, return=%02x", 0x03ff, mapper->read(0x03ff, true));
            //     //_log(debug, "readtest2 addr=%04x, return=%02x", 0x0400, mapper->read(0x0400, true));
            //     //_log(debug, "readtest3 addr=%04x, return=%02x", 0x07ff, mapper->read(0x07ff, true));
            //     //_log(debug, "readtest4 addr=%04x, return=%02x", 0x0800, mapper->read(0x0800, true));
            //     //_log(debug, "readtest5 addr=%04x, return=%02x", 0x0bff, mapper->read(0x0bff, true));
            //     //_log(debug, "readtest6 addr=%04x, return=%02x", 0x0c00, mapper->read(0x0c00, true));
            //     //_log(debug, "readtest7 addr=%04x, return=%02x", 0x0fff, mapper->read(0x0fff, true));
            //     mapper->read(0x1000, true);
            //     mapper->read(0x13ff, true);
            //     mapper->read(0x1400, true);
            //     mapper->read(0x17ff, true);
            //     mapper->read(0x1800, true);
            //     mapper->read(0x1bff, true);
            //     mapper->read(0x1c00, true);
            //     mapper->read(0x1fff, true);
            // }

            // std::exit(1);

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

        u16 translate_ciram_address(u16 addr) {
            return mapper->translate_ciram_address(addr);
        }

        u8 read(u16 addr, bool ppu = false) {
            return mapper->read(addr, ppu);
        }

        void write(u16 addr, u8 value, bool ppu = false) {
            return mapper->write(addr, value, ppu);
        }
    }
}