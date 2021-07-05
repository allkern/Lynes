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
        // NES-UNROM, NES_UOROM, HVC-UN1ROM
        // iNES Mapper 2
        struct uxrom : mapper_t {
            typedef std::array <u8, 0x4000> prg_rom_bank_t;
            typedef std::array <u8, 0x2000> chr_mem_t;

            typedef std::vector <prg_rom_bank_t> prg_rom_t;

            bool mirroring = false;

            header_t* hdr = nullptr;

            prg_rom_t prg_rom;
            chr_mem_t chr_mem;

            bool chr_ram_enabled = false;

            u8 prg_bank = 0;

            uxrom(std::ifstream* f, header_t* header) {
                hdr = header;

                mirroring = header->flags[0] & 0x1;

                prg_rom_bank_t prgb;

                while (header->prg_rom_size--) {
                    f->read((char*)prgb.data(), 0x4000);

                    prg_rom.push_back(prgb);

                    prgb.fill(0);
                }

                chr_ram_enabled = !header->chr_rom_size;

                if (chr_ram_enabled)
                    f->read((char*)chr_mem.data(), chr_mem.size());
            }

            u16 translate_ciram_address(u16 addr) override {
                u16 ciram_bits = addr & 0xc00;

                ciram_bits >>= !mirroring;
                ciram_bits &= 0x400;

                return (addr & 0xf3ff) | ciram_bits;
            }

            u8 read(u16 addr, bool ppu) override {
                if (!ppu) {
                    if (IN_RANGE(0x8000, 0xbfff)) { return prg_rom.at(prg_bank).at(addr - 0x8000); }
                    if (IN_RANGE(0xc000, 0xffff)) { return prg_rom.at(prg_rom.size() - 1).at(addr - 0xc000); }
                } else {
                    return chr_mem.at(addr);
                }

                return 0x0;
            }

            void write(u16 addr, u8 value, bool ppu) override {
                if (ppu && chr_ram_enabled) {
                    if (IN_RANGE(0x0000, 0x1fff)) { chr_mem.at(addr) = value; return; }
                }

                if (IN_RANGE(0x8000, 0xffff)) { prg_bank = value; return; }
            }
        };
    }
}

#undef IN_RANGE