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
        // MMC3: MMC3A, MMC3B, MMC3C
        // Boards: TBROM, TEROM, TFROM, TGROM, TKROM, TK1ROM,
        //         TKSROM, TLROM, TL1ROM, TL2ROM, TLBROM, TLSROM,
        //         TNROM, TQROM, TR1ROM, TSROM, TVROM
        // iNES Mapper 4
        struct mmc3 : mapper_t {
            bool mirroring = false;
            header_t* hdr = nullptr;

            typedef std::array <u8, 0x2000> prg_rom_bank_t;
            typedef std::array <u8, 0x2000> prg_ram_t;
            typedef std::array <u8, 0x400> chr_rom_bank_t;

            typedef std::vector <prg_rom_bank_t> prg_rom_t;
            typedef std::vector <chr_rom_bank_t> chr_rom_t;

            prg_ram_t prg_ram;
            prg_rom_t prg_rom;
            chr_rom_t chr_rom;

            u8 bank_select = 0x0;

            int r[8] = { 0x0 };
            
            bool prg_ram_enabled = true,
                 prg_ram_write_protect = false;

            mmc3(std::ifstream* f, header_t* header) {
                hdr = header;
                mirroring = header->flags[0] & 0x1;

                prg_rom_bank_t prgb;
                chr_rom_bank_t chrb;

                // MMC3 PRG ROM banks are 8 KiB chunks each
                int prg_rom_size = header->prg_rom_size * 2;

                while (prg_rom_size--) {
                    f->read((char*)prgb.data(), 0x2000);

                    prg_rom.push_back(prgb);

                    prgb.fill(0);
                }

                // MMC3 CHR ROM banks are 1 KiB chunks each
                int chr_rom_size = header->chr_rom_size * 8;

                while (chr_rom_size--) {
                    f->read((char*)chrb.data(), 0x400);

                    chr_rom.push_back(chrb);

                    chrb.fill(0);
                }
            }

            u16 translate_ciram_address(u16 addr) override {
                u16 ciram_bits = addr & 0xc00;

                if (mirroring) ciram_bits >>= 1;

                return (addr & 0xf3ff) | (ciram_bits & 0x400);
            }

            u8 read(u16 addr, bool ppu) override {
                if (!ppu) {
                    if (IN_RANGE(0x6000, 0x7fff)) {
                        return prg_ram_enabled ? prg_ram.at(addr - 0x6000) : 0xff;
                    }
                    if (IN_RANGE(0x8000, 0x9fff)) {
                        return prg_rom.at((bank_select & 0x40) ? (prg_rom.size() - 2) : r[6]).at(addr - 0x8000);
                    }
                    if (IN_RANGE(0xa000, 0xbfff)) { return prg_rom.at(r[7]).at(addr - 0xa000); }
                    if (IN_RANGE(0xc000, 0xdfff)) {
                        return prg_rom.at((bank_select & 0x40) ? r[6] : (prg_rom.size() - 2)).at(addr - 0xc000);
                    }
                    if (IN_RANGE(0xe000, 0xffff)) {
                        return prg_rom.at(prg_rom.size() - 1).at(addr - 0xe000);
                    }
                } else {
                    if (IN_RANGE(0x0000, 0x1fff)) {
                        if (bank_select & 0x80) {
                            if (addr >= 0x1000) {
                                u8 bank = (r[(addr >> 11) & 0x1] & 0xfe) | ((addr >> 10) & 0x1);

                                return chr_rom.at(bank).at(addr & 0x3ff);
                            } else {
                                // Select bank register based on address
                                return chr_rom.at(r[((addr >> 10) & 0x3) + 2].at(addr & 0x3ff);
                            }
                        } else {
                            if (addr <= 0xfff) {
                                u8 bank = (r[(addr >> 11) & 0x1] & 0xfe) | ((addr >> 10) & 0x1);

                                return chr_rom.at(bank).at(addr & 0x3ff);
                            } else {
                                return chr_rom.at(r[((addr >> 10) & 0x3) + 2].at(addr & 0x3ff);
                            }
                        }
                    }
                }

                return 0x0;
            }

            void write(u16 addr, u8 value, bool ppu) override {
                if (!ppu) {
                    if (IN_RANGE(0x6000, 0x7fff)) {
                        if (prg_ram_enabled && !prg_ram_write_protect)
                            prg_ram.at(addr - 0x6000) = value;
                        return;
                    }
                    if (IN_RANGE(0x8000, 0x9fff)) {
                        if (addr & 0x1) {
                            r[bank_select & 0x7] = value;
                        } else {
                            bank_select = value;
                        }

                        return;
                    }
                    if (IN_RANGE(0xa000, 0xbfff)) {
                        if (addr & 0x1) {
                            prg_ram_enabled = value & 0x80;
                            prg_ram_write_protect = value & 0x40;
                        } else {
                            mirroring = value & 0x1;
                        }

                        return;
                    }

                    // To-do: Implement MMC3 IRQ logic here
                }
            }
        };
    }
}

#undef IN_RANGE
