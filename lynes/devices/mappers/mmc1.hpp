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
        struct mmc1 : mapper_t {
            typedef std::array <u8, 0x8000> prg_rom_bank_t;
            typedef std::array <u8, 0x1000> chr_mem_bank_t;

            typedef std::vector <prg_rom_bank_t> prg_rom_t;
            typedef std::vector <chr_mem_bank_t> chr_mem_t;
            typedef std::array <u8, 0x2000> prg_ram_t;

            header_t* hdr = nullptr;

            prg_rom_t prg_rom;
            chr_mem_t chr_rom;
            chr_mem_t chr_ram;
            prg_ram_t prg_ram;
            chr_mem_t* chr_mem;

            struct control_t {
                enum mirroring_t : int {
                    one_screen_lb,
                    one_screen_ub,
                    vertical,
                    horizontal
                } mirroring;

                enum prg_bank_mode_t : int {
                    all_banks0,
                    all_banks1,
                    lower_fixed,
                    upper_fixed,
                } prg_bank_mode = upper_fixed;

                enum chr_bank_mode_t : bool {
                    all_banks,
                    upper_lower
                } chr_bank_mode = upper_lower;

                int lower_chr_bank = 0,
                    upper_chr_bank = 0,
                    prg_bank = 0;
                
                bool prg_ram_enable = true;
            } control;

            bool chr_ram_enabled = false;
            u8 sr = 0x10, sr_value = 0x0;

            mmc1(std::ifstream* f, header_t* header) {
                hdr = header;

                prg_rom_bank_t prgb;
                chr_mem_bank_t chrb;


                while (header->prg_rom_size--) {
                    f->read((char*)prgb.data(), 0x4000);

                    prg_rom.push_back(prgb);

                    prgb.fill(0);
                }

                chr_ram_enabled = !header->chr_rom_size;

                if (chr_ram_enabled) {
                    chr_ram.resize(2);
                    chr_mem = &chr_ram;

                    return;
                }

                header->chr_rom_size <<= 1;

                while (header->chr_rom_size--) {
                    f->read((char*)chrb.data(), 0x1000);

                    chr_rom.push_back(chrb);
                }

                chr_mem = &chr_rom;
            }

            u8 read(u16 addr, bool ppu) override {
                if (!ppu) {
                    if (IN_RANGE(0x6000, 0x7fff)) return prg_ram[addr - 0x6000];
                    if (IN_RANGE(0x8000, 0xbfff)) {
                        switch (control.prg_bank_mode) {
                            case control_t::lower_fixed: return prg_rom[0].at(addr - 0x8000);
                            case control_t::upper_fixed: return prg_rom[control.prg_bank].at(addr - 0x8000);
                            default: return prg_rom[control.prg_bank].at(addr - 0x8000);
                        }
                    }
                    if (IN_RANGE(0xc000, 0xffff)) {
                        switch (control.prg_bank_mode) {
                            case control_t::upper_fixed: return prg_rom[prg_rom.size() - 1].at(addr - 0xc000);
                            case control_t::lower_fixed: return prg_rom[control.prg_bank].at(addr - 0xc000);
                            default: return prg_rom[control.prg_bank + 1].at(addr - 0xc000);
                        }
                    }
                } else {
                    if (IN_RANGE(0x0000, 0x0fff)) {
                        switch (control.chr_bank_mode) {
                            case control_t::all_banks: return chr_mem->at(0).at(addr);
                            case control_t::upper_lower: return chr_mem->at(control.lower_chr_bank).at(addr);
                        }
                    }
                    if (IN_RANGE(0x1000, 0x1fff)) {
                        switch (control.chr_bank_mode) {
                            case control_t::all_banks: return chr_mem->at(chr_mem->size() - 1).at(addr - 0x1000);
                            case control_t::upper_lower: return chr_mem->at(control.upper_chr_bank).at(addr - 0x1000);
                        }
                    }
                }

                return 0x0;
            }

            bool sr_full = false;

            void write(u16 addr, u8 value, bool ppu) override {
                if (!ppu) {
                    if (IN_RANGE(0x6000, 0x7fff)) { prg_ram[addr - 0x6000] = value; return; }
                    if (IN_RANGE(0x8000, 0xffff)) {
                        if (value & 0x80) { sr = 0x10; return; }

                        if (sr & 0x1) {
                            sr_full = true;
                            //goto full;
                        } else {
                            sr >>= 1;
                            sr |= ((value & 0x1) << 4);
                        }
                    }

                    full:

                    if (sr_full) {
                        sr_value = sr >> 1 | ((value & 0x1) << 4);

                        sr = 0x10;
                        sr_full = false;

                        if (IN_RANGE(0x8000, 0x9fff)) {
                            u8 old = control.chr_bank_mode;
                            control.mirroring = (control_t::mirroring_t)(sr_value & 0x3);
                            control.prg_bank_mode = (control_t::prg_bank_mode_t)((sr_value >> 2) & 0x3);
                            control.chr_bank_mode = (control_t::chr_bank_mode_t)((sr_value >> 4) & 0x1);

                            return;
                        }
                        if (IN_RANGE(0xa000, 0xbfff)) { control.lower_chr_bank = sr_value; return; }
                        if (IN_RANGE(0xc000, 0xdfff)) { control.upper_chr_bank = sr_value; return; }
                        if (IN_RANGE(0xe000, 0xffff)) { control.prg_bank = sr_value; }
                    }
                } else {
                    if (IN_RANGE(0x0000, 0x0fff)) { chr_ram.at(0).at(addr) = value; return; }
                    if (IN_RANGE(0x1000, 0x1fff)) { chr_ram.at(1).at(addr-0x1000) = value; return; }
                }
            }
        };
    }
}

#undef IN_RANGE