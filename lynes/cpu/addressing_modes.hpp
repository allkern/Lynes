#pragma once

#include "../global.hpp"
#include "bus.hpp"
#include "registers.hpp"

#include "../log.hpp"

#define R(a) (bus::read(a))
#define IMM (bus::read(registers::pc++))
#define X registers::x
#define Y registers::y
#define PC registers::pc

#include <iostream>

namespace nes {
    namespace cpu {
        typedef void (*addressing_mode_t)();

        void abs() {
            u8 l = IMM, h = IMM;

            operand = (h << 8) | l;
        }

        void zpg() { operand = IMM; }
        void zpx() { operand = (IMM + X) & 0xff; }
        void zpy() { operand = (IMM + Y) & 0xff; }

        void abx() {
            u8 l = IMM, h = IMM;

            operand = ((h << 8) | l) + X;
        }

        void aby() {
            u8 l = IMM, h = IMM;

            operand = ((h << 8) | l) + Y;
        }

        void idx() {
            u8 off = (IMM + X) & 0xff;
            
            operand = (R((off + 1) & 0xff) << 8) | R(off);
        }

        void idy() {
            u8 off = IMM,
               l = R(off),
               h = R((off + 1) & 0xff);

            operand = ((h << 8) | l) + Y;
        }

        void imp() {}
        void acc() {}
        void imm() { operand = PC++; }
        void rel() { operand = PC + (int8_t)IMM; }

        void ind() {
            u8 l = IMM, h = IMM;

            u16 addr = (h << 8) | l;

            operand = (R(addr + 1) << 8) | R(addr);
        }
    }
}

#undef R
#undef X
#undef Y
#undef IMM
#undef PC