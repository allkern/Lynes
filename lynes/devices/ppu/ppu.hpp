#pragma once

#include "../../global.hpp"

#include <array>
#include <cmath>

#include "../cart.hpp"
#include "immintrin.h"

#include "memory.hpp"
#include "bus.hpp"
#include "io.hpp"

namespace nes {
    namespace ppu {
        typedef void (*frame_ready_callback_t)(uint32_t*);

        frame_ready_callback_t frame_ready_cb = nullptr;

        enum Action {
            Nop = 0,
            SkipOnOdd,
            LoadNT,
            LoadAT,
            FakeNT,
            LoadLowBG,
            LoadHighBGAndIncH,
            LoadHighBGAndIncV,
            AssignH,
            AssignV,
            SetVBlank,
            ClearFlags
        };

        using Table = std::array<std::array<Action, 341>, 262>;
        // TODO: make the whole thing constexpr
        Table transitions = { Nop };

        void fill_line(size_t line) {
            for (size_t cycle = 2; cycle <= 256;) {
                transitions[line][cycle] = Action::LoadNT;            cycle += 2;
                transitions[line][cycle] = Action::LoadAT;            cycle += 2;
                transitions[line][cycle] = Action::LoadLowBG;         cycle += 2;
                transitions[line][cycle] = Action::LoadHighBGAndIncH; cycle += 2;
            }

            transitions[line][256] = Action::LoadHighBGAndIncV;
            transitions[line][257] = Action::AssignH;

            transitions[line][330] = transitions[line][322] = Action::LoadNT;
            transitions[line][332] = transitions[line][324] = Action::LoadAT;
            transitions[line][334] = transitions[line][326] = Action::LoadLowBG;
            transitions[line][336] = transitions[line][328] = Action::LoadHighBGAndIncH;

            // TODO: is this even necessary?
            transitions[line][338] = transitions[line][340] = Action::FakeNT;
        }

        void init(frame_ready_callback_t fr) {
            frame_ready_cb = fr;

            transitions[0][0] = Action::SkipOnOdd;

            for (size_t line = 0; line <= 239; line++)
                fill_line(line);

            transitions[241][1] = Action::SetVBlank;
            transitions[261][1] = Action::ClearFlags;

            fill_line(261);

            for (size_t cycle = 280; cycle <= 304; cycle++)
                transitions[261][cycle] = Action::AssignV;
        }

        void reset() {
        }

        uint32_t palette[] {
            /////          0                        1                        2                        3                        4                        5                        6                        7                        8                        9                        a                        b                        c                        d                        e                        f
            /*0*/ lgw::rgb( 84,  84,  84), lgw::rgb(  0,  30, 116), lgw::rgb(  8,  16, 144), lgw::rgb( 48,   0, 136), lgw::rgb( 68,   0, 100), lgw::rgb( 92,   0,  48), lgw::rgb( 84,   4,   0), lgw::rgb( 60,  24,   0), lgw::rgb( 32,  42,   0), lgw::rgb(  8,  58,   0), lgw::rgb(  0,  64,   0), lgw::rgb(  0,  60,   0), lgw::rgb(  0,  50,  60), lgw::rgb(  0,   0,   0), lgw::rgb(  0,   0,   0), lgw::rgb(  0,   0,   0),
            /*1*/ lgw::rgb(152, 150, 152), lgw::rgb(  8,  76, 196), lgw::rgb( 48,  50, 236), lgw::rgb( 92,  30, 228), lgw::rgb(136,  20, 176), lgw::rgb(160,  20, 100), lgw::rgb(152,  34,  32), lgw::rgb(120,  60,   0), lgw::rgb( 84,  90,   0), lgw::rgb( 40, 114,   0), lgw::rgb(  8, 124,   0), lgw::rgb(  0, 118,  40), lgw::rgb(  0, 102, 120), lgw::rgb(  0,   0,   0), lgw::rgb(  0,   0,   0), lgw::rgb(  0,   0,   0),
            /*2*/ lgw::rgb(236, 238, 236), lgw::rgb( 76, 154, 236), lgw::rgb(120, 124, 236), lgw::rgb(176,  98, 236), lgw::rgb(228,  84, 236), lgw::rgb(236,  88, 180), lgw::rgb(236, 106, 100), lgw::rgb(212, 136,  32), lgw::rgb(160, 170,   0), lgw::rgb(116, 196,   0), lgw::rgb( 76, 208,  32), lgw::rgb( 56, 204, 108), lgw::rgb( 56, 180, 204), lgw::rgb( 60,  60,  60), lgw::rgb(  0,   0,   0), lgw::rgb(  0,   0,   0),
            /*3*/ lgw::rgb(236, 238, 236), lgw::rgb(168, 204, 236), lgw::rgb(188, 188, 236), lgw::rgb(212, 178, 236), lgw::rgb(236, 174, 236), lgw::rgb(236, 174, 212), lgw::rgb(236, 180, 176), lgw::rgb(228, 196, 144), lgw::rgb(204, 210, 120), lgw::rgb(180, 222, 120), lgw::rgb(168, 226, 144), lgw::rgb(152, 226, 180), lgw::rgb(160, 214, 228), lgw::rgb(160, 162, 160), lgw::rgb(  0,   0,   0), lgw::rgb(  0,   0,   0)
        };

        void render_sprites() {
            int i = 0;
            int height = TEST_REG(PPUCTRL, SPRISIZE) ? 16 : 8;

            while (i < 0x100) {
                u8 y = oam.at(i++),
                   t = oam.at(i++),
                   a = oam.at(i++),
                   x = oam.at(i++);

                if (y >= 0xef) continue;

                int pal = a & 0x3;
                   
                for (int fy = 0; fy < height; fy++) {
                    int fyr = (a & 0x80) ? ((height - 1) - fy) : fy;

                    u8 h, l;

                    if (TEST_REG(PPUCTRL, SPRISIZE)) {
                        u16 off = (t & 0x1) ? 0x1000 : 0x0;

                        u16 addr = off | ((t + ((fy & 0x8) ? 1 : 0)) << 4) | (fy & 0x7);

                        l = cart::read(addr, true);
                        h = cart::read(addr | 0x8, true);
                    } else {
                        l = cart::read((TEST_REG(PPUCTRL, SPPTADDR) ? 0x1000 : 0) + ((t << 4) | fyr), true),
                        h = cart::read((TEST_REG(PPUCTRL, SPPTADDR) ? 0x1000 : 0) + ((t << 4) | 0x8 | fyr), true);
                    }

                    for (int fx = 0; fx < 0x8; fx++) {
                        u16 mask = (a & 0x40) ? (0x0101 << fx) : (0x8080 >> fx);
                        size_t index = _pext_u32(((u16)h << 8) | l, mask);
                        u16 addr = 0x3f10 | (pal << 2) | index;

                        if (index) {
                            u32 color = palette[bus::read(addr)];
                            
                            if ((x + fx) < PPU_WIDTH)
                                frame.draw(x + fx, (y + 1) + fy, color);
                        }
                    }
                }
            }
        }

        void render() {
            u16 addr = 0x0;

            for (int y = 0; y < PPU_HEIGHT; y++) {
                int row = y >> 3;

                for (int x = 0; x <= PPU_WIDTH; x++) {
                    u16 base_nta = (r[0x0] & 0x3) * 0x400,
                        base_nta_xoff = (r[0x0] & 0x3) * 0xff;

                    int sx = (base_nta_xoff + (x + fine_x)) & 0x1ff;
                    int col = sx >> 3;

                    addr = base_nta + (((col >> 5) * 0x400) - base_nta) + (row << 5 | (col & 0x1f));

                    u8 tile = ram[addr];

                    u16 tm_base = TEST_REG(PPUCTRL, BGPTADDR) ? 0x1000 : 0;

                    u16 attr_addr_base = base_nta + (((col >> 5) * 0x400) - base_nta);

                    u8 l = cart::read(tm_base + ((tile << 4) | y & 0x7), true),
                       h = cart::read(tm_base + ((tile << 4) | 0x8 | y & 0x7), true),
                       a = bus::read((0x23c0 + attr_addr_base) + ((col & 0x1f) >> 2) + ((row >> 2) << 3));

                    int index = _pext_u32(((u16)h << 8) | l, 0x8080 >> (sx & 0x7)),
                        attr_index = (((row >> 1) & 0x1) << 1) | ((col >> 1) & 0x1),
                        pal = (a >> (attr_index << 1)) & 0x3;

                    u16 attr_addr = 0x3f00 | (pal << 2) | index;

                    u32 color = 0;

                    if (index == 0) {
                        color = palette[bus::read(0x3f00)];
                    } else {
                        color = palette[bus::read(attr_addr)];
                    }

                    frame.draw(x, y, color);
                }
            }
        }
    }
}

#undef PPU_WIDTH
#undef PPU_HEIGHT