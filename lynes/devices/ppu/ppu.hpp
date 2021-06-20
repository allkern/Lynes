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

        size_t clk = 0, clk2 = 0;

        uint32_t palette[] = {
            lgw::rgb(0x00),
            lgw::rgb(0x55),
            lgw::rgb(0xaa),
            lgw::rgb(0xff)
        };

        void render() {
            for (int row = 0; row < 0x20; row++) {
                for (int c = 0; c < 0x20; c++) {
                    u16 addr = (row << 5) | c;

                    u8 b = ram[addr];

                    for (int fy = 0; fy < 0x8; fy++) {
                        u8 l = cart::read((TEST_REG(PPUCTRL, BGPTADDR) ? 0x1000 : 0) + ((b << 4) | fy), true),
                           h = cart::read((TEST_REG(PPUCTRL, BGPTADDR) ? 0x1000 : 0) + ((b << 4) | 0x8 | fy), true);
                        
                        for (int fx = 0; fx < 0x8; fx++) {
                            size_t color = _pext_u32(((u16)h << 8) | l, 0x8080 >> fx);

                            frame.draw((c << 3) + fx, (row << 3) + fy, palette[color]);
                        }
                    }
                }
            }
        }
    }
}