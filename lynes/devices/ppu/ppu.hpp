#pragma once

#include "../../global.hpp"

#include <array>
#include <cmath>

#include "../cart.hpp"
#include "immintrin.h"

//#include "callbacks.hpp"
#include "memory.hpp"
#include "bus.hpp"
#include "io.hpp"

namespace nes {
    namespace ppu {
        typedef void (*frame_ready_callback_t)(uint32_t*);

        frame_ready_callback_t frame_ready_cb = nullptr;


        void init(frame_ready_callback_t fr) {
            frame_ready_cb = fr;
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

        void render() {
            for (int row = 0; row < 0x20; row++) {
                for (int c = 0; c < 0x20; c++) {
                    u8 attr = bus::read(0x23c0 | ((row >> 2) << 3) | (c >> 2));

                    int attr_index = (((row >> 1) & 0x1) << 1) | ((c >> 1) & 0x1),
                        pal = (attr >> (attr_index << 1)) & 0x3;

                    //_log(debug, "tile @ (%u, %u), attr_index=%u, pal=%u", c, row, attr_index << 1, pal);

                    u16 addr = (row << 5) | c;

                    u8 b = ram[addr];

                    for (int fy = 0; fy < 0x8; fy++) {
                        u8 l = cart::read((TEST_REG(PPUCTRL, BGPTADDR) ? 0x1000 : 0) + ((b << 4) | fy), true),
                           h = cart::read((TEST_REG(PPUCTRL, BGPTADDR) ? 0x1000 : 0) + ((b << 4) | 0x8 | fy), true);
                        
                        for (int fx = 0; fx < 0x8; fx++) {
                            size_t index = _pext_u32(((u16)h << 8) | l, 0x8080 >> fx);
                            u16 addr = 0x3f00 | (pal << 2) | index;

                            //_log(debug, "addr=%04x", addr);
                            u32 color = 0;
                            
                            if (index == 0) {
                                color = palette[bus::read(0x3f00)];
                            } else {
                                color = palette[bus::read(addr)];
                            }

                            frame.draw((c << 3) + fx, (row << 3) + fy, color);
                        }
                    }
                }
            }
        }
    }
}