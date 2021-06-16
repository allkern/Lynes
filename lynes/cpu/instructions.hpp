#pragma once

#include "../global.hpp"

#include "addressing_modes.hpp"
#include "opcode_table.hpp"
#include "registers.hpp"
#include "stack.hpp"
#include "bus.hpp"

#include <iostream>

namespace nes {
    namespace cpu {
        typedef void (*instruction_t)();

        using namespace registers;

        void i_adc() {
            u8 i = bus::read(operand), pa = a;

            u16 r = a + i + (p & CF);

            set_flags(CF, r & 0xff00);

            r &= 0xff;

            set_flags(NF, r & 0x80);
            set_flags(ZF, r == 0);
            set_flags(VF, (a ^ r) & (i ^ r) & 0x80);

            a = r & 0xff;
        }

        void i_sbc() {
            u8 i = bus::read(operand) ^ 0xff;
            u16 r = a + i + (p & CF);

            set_flags(CF, r & 0xff00);

            r &= 0xff;

            set_flags(NF, r & 0x80);
            set_flags(ZF, r == 0);
            set_flags(VF, (a ^ r) & (i ^ r) & 0x80);

            a = r & 0xff;
        }

        void i_ill() {}
        void i_and() { a &= bus::read(operand); set_flags(ZF, !a); set_flags(NF, a & 0x80); }
        void i_bcc() { if (!(p & CF)) { pc = operand; last_cycles++; cycles_elapsed += 1; } }
        void i_bcs() { if (p & CF) { pc = operand; last_cycles++; cycles_elapsed += 1; } }
        void i_beq() { if (p & ZF) { pc = operand; last_cycles++; cycles_elapsed += 1; } }
        void i_bit() { u8 b = bus::read(operand), r = b & a; set_flags(ZF, !r); set_flags(VF, b & 0x40); set_flags(NF, b & 0x80); }
        void i_bmi() { if (p & NF) { pc = operand; last_cycles++; cycles_elapsed += 1; } }
        void i_bne() { if (!(p & ZF)) { pc = operand; last_cycles++; cycles_elapsed += 1; } }
        void i_bpl() { if (!(p & NF)) { pc = operand; last_cycles++; cycles_elapsed += 1; } }
        void i_brk() { set_flags(IF, true); push(pc); push((u8)(p | 0b00110000)); set_flags(BF, false); pc = (bus::read(0xffff) << 8) | bus::read(0xfffe); }
        void i_bvc() { if (!(p & VF)) { pc = operand; last_cycles++; cycles_elapsed += 1; } }
        void i_bvs() { if (p & VF) { pc = operand; last_cycles++; cycles_elapsed += 1; } }
        void i_clc() { set_flags(CF, false); }
        void i_cld() { set_flags(DF, false); }
        void i_cli() { set_flags(IF, false); }
        void i_clv() { set_flags(VF, false); }
        void i_cmp() { u8 b = bus::read(operand), r = a - b; set_flags(CF, a >= b); set_flags(ZF, a == b); set_flags(NF, r & 0x80); }
        void i_cpx() { u8 b = bus::read(operand), r = x - b; set_flags(CF, x >= b); set_flags(ZF, x == b); set_flags(NF, r & 0x80); }
        void i_cpy() { u8 b = bus::read(operand), r = y - b; set_flags(CF, y >= b); set_flags(ZF, y == b); set_flags(NF, r & 0x80); }
        void i_dec() { u8 b = bus::read(operand); b--; bus::write(operand, b); set_flags(ZF, !b); set_flags(NF, b & 0x80); } // Side-effects?
        void i_dex() { x--; set_flags(ZF, !x); set_flags(NF, x & 0x80); }
        void i_dey() { y--; set_flags(ZF, !y); set_flags(NF, y & 0x80); }
        void i_eor() { a ^= bus::read(operand); set_flags(ZF, !a); set_flags(NF, a & 0x80); }
        void i_inc() { u8 b = bus::read(operand); b++; bus::write(operand, b); set_flags(ZF, !b); set_flags(NF, b & 0x80); } // Side-effects?
        void i_inx() { x++; set_flags(ZF, !x); set_flags(NF, x & 0x80); }
        void i_iny() { y++; set_flags(ZF, !y); set_flags(NF, y & 0x80); }
        void i_jmp() { pc = operand; }
        void i_jsr() { push(pc); pc = operand; }
        void i_lda() { u8 b = bus::read(operand); set_flags(ZF, !b); set_flags(NF, b & 0x80); a = b; }
        void i_ldx() { u8 b = bus::read(operand); set_flags(ZF, !b); set_flags(NF, b & 0x80); x = b; }
        void i_ldy() { u8 b = bus::read(operand); set_flags(ZF, !b); set_flags(NF, b & 0x80); y = b; }
        void i_nop() {}
        void i_ora() { a |= bus::read(operand); set_flags(ZF, !a); set_flags(NF, a & 0x80); }
        void i_pha() { if ((a == 0x3a) || (a == 0x39)) _log(debug, "push a=%02x", a); push(a); }
        void i_php() { if (((p | 0b00110000) == 0x3a) || ((p | 0b00110000) == 0x39)) _log(debug, "push p=%02x", p); push((u8)(p | 0b00110000)); }
        void i_pla() { a = pop1(); set_flags(ZF, !a); set_flags(NF, a & 0x80); }
        void i_plp() { u8 b = p; p = pop1() & 0b11001111; p |= b & 0b00110000; }
        void i_rti() { u8 b = p; p = pop1() & 0b11001111; p |= b & 0b00110000; set_flags(IF, false); pc = pop(); }
        void i_rts() { pc = pop(); }
        void i_sec() { set_flags(CF, true); }
        void i_sed() { set_flags(DF, true); }
        void i_sei() { set_flags(IF, true); }
        void i_sta() { bus::write(operand, a); }
        void i_stx() { bus::write(operand, x); }
        void i_sty() { bus::write(operand, y); }
        void i_tax() { set_flags(ZF, !a); set_flags(NF, a & 0x80); x = a; }
        void i_tay() { set_flags(ZF, !a); set_flags(NF, a & 0x80); y = a; }
        void i_txa() { set_flags(ZF, !x); set_flags(NF, x & 0x80); a = x; }
        void i_tya() { set_flags(ZF, !y); set_flags(NF, y & 0x80); a = y; }
        void i_txs() { sp = x; }
        void i_tsx() { set_flags(ZF, !sp); set_flags(NF, sp & 0x80); x = sp; }
        void i_asl() {
            if (opcode == 0x0a) {
                set_flags(CF, a & 0x80);

                a <<= 1;

                set_flags(ZF, !a);
                set_flags(NF, a & 0x80);
            } else {
                u8 b = bus::read(operand);

                set_flags(CF, b & 0x1);

                b >>= 1;

                set_flags(ZF, !b);
                set_flags(NF, b & 0x80);

                bus::write(operand, b);
            }
        }

        void i_lsr() {
            set_flags(NF, false);

            if (opcode == 0x4a) {
                set_flags(CF, a & 0x1);

                a >>= 1;

                set_flags(ZF, !a);
            } else {
                u8 b = bus::read(operand);

                set_flags(CF, b & 0x1);

                b >>= 1;

                set_flags(ZF, !b);

                bus::write(operand, b);
            }
        }

        void i_rol() {
            bool c = p & CF;

            if (opcode == 0x2a) {
                set_flags(CF, a & 0x80);

                a <<= 1;

                if (c) a |= 0x1;

                set_flags(ZF, !a);
                set_flags(NF, a & 0x80);
            } else {
                u8 b = bus::read(operand);

                set_flags(CF, b & 0x80);

                b <<= 1;

                if (c) b |= 0x1;

                set_flags(ZF, !b);
                set_flags(NF, b & 0x80);

                bus::write(operand, b);
            }
        }

        void i_ror() {
            bool c = p & CF;

            if (opcode == 0x6a) {
                set_flags(CF, a & 0x1);

                a >>= 1;
                if (c) a |= 0x80;

                set_flags(ZF, !a);
                set_flags(NF, a & 0x80);
            } else {
                u8 b = bus::read(operand);

                set_flags(CF, b & 0x1);

                b >>= 1;
                if (c) b |= 0x80;

                set_flags(ZF, !b);
                set_flags(NF, b & 0x80);

                bus::write(operand, b);
            }
        }
    }
}