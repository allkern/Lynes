#pragma once

#include "../../global.hpp"

#include <fstream>
#include <vector>
#include <array>

#include "mapper.hpp"
#include "header.hpp"

#define IN_RANGE(l, h) ((addr >= l) && (addr <= h))

namespace nes {
    namespace cart {
        // NES-NROM-128, NES-NROM-256
        // iNES Mapper 0
        struct nrom : mapper_t {
            typedef std::array <u8, 0x8000> prg_rom_t;
            typedef std::array <u8, 0x2000> prg_ram_t;
            typedef std::array <u8, 0x2000> chr_rom_t;

            header_t* hdr = nullptr;

            prg_rom_t prg_rom;
            chr_rom_t chr_rom;
            prg_ram_t prg_ram;

            nrom(std::ifstream* f, header_t* header) {
                hdr = header;

                f->read((char*)prg_rom.data(), 0x4000 * header->prg_rom_size);
                f->read((char*)chr_rom.data(), chr_rom.size());
            }

            u8 read(u16 addr, bool ppu) override {
                // Handle CPU read
                if (!ppu) {
                    if (IN_RANGE(0x6000, 0x7fff)) { return prg_ram.at(addr - 0x6000); }
                    if (IN_RANGE(0x8000, 0xffff)) { return prg_rom.at((addr - 0x8000) & ((hdr->prg_rom_size - 1) ? 0x7fff : 0x3fff)); }
                } else { // Handle PPU read
                    return chr_rom.at(addr);
                }

                return 0x0;
            }

            void write(u16 addr, u8 value, bool ppu) override {
                if (IN_RANGE(0x6000, 0x7fff)) { prg_ram.at(addr - 0x6000) = value; }
            }
        };
    }
}

#undef IN_RANGE