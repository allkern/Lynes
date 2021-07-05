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
        // NES-ANROM, NES_AMROM, NES-AN1ROM, NES-AOROM
        // iNES Mapper 7
        struct axrom : mapper_t {
            typedef std::array <u8, 0x8000> prg_rom_bank_t;
            typedef std::array <u8, 0x2000> chr_ram_t;

            typedef std::vector <prg_rom_bank_t> prg_rom_t;

            bool mirroring = false;

            header_t* hdr = nullptr;

            prg_rom_t prg_rom;
            chr_ram_t chr_ram;

            u8 prg_bank = 0;
            int nt_bank = 0;

            axrom(std::ifstream* f, header_t* header) {
                hdr = header;

                mirroring = header->flags[0] & 0x1;

                prg_rom_bank_t prgb;

                _log(debug, "axrom prg_rom_size=%u", header->prg_rom_size);

                while (header->prg_rom_size--) {
                    f->read((char*)prgb.data(), 0x8000);

                    prg_rom.push_back(prgb);

                    prgb.fill(0);
                }

                // CHRRAM always
            }

            u16 translate_ciram_address(u16 addr) override {
                u16 ciram_bits = addr & 0xc00;

                ciram_bits = nt_bank << 10;

                return (addr & 0xf3ff) | (ciram_bits & 0x400);
            }

            u8 read(u16 addr, bool ppu) override {
                if (!ppu) {
                    if (IN_RANGE(0x8000, 0xffff)) { return prg_rom.at(prg_bank).at(addr - 0x8000); }
                } else {
                    return chr_ram.at(addr);
                }

                return 0x0;
            }

            void write(u16 addr, u8 value, bool ppu) override {
                if (ppu) {
                    if (IN_RANGE(0x0000, 0x1fff)) { chr_ram.at(addr) = value; return; }
                }

                if (IN_RANGE(0x8000, 0xffff)) {
                    prg_bank = value & 0x7;
                    nt_bank = (value >> 4) & 0x1;
                    return;
                }
            }
        };
    }
}

#undef IN_RANGE