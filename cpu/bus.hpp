#pragma once

#include "../global.hpp"

#include <array>

/*
; $00FE + $0003 (254 + 3 in decimal)

; Store LSB of first value in memory at address $0000.
LDA #$FE
STA $00

; Store MSB of first value in memory at address $0001.
LDA #$00
STA $01

; Store LSB of second value in memory at address $0002.
LDA #$03
STA $02

; Store MSB of second value in memory at address $0003.
LDA #$00
STA $03

CLC       ; Clear the Carry flag.

; Add the LSBs
LDA $00   ; Load LSB of first value into the Accumulator.
ADC $02   ; Add LSB of second value to the Accumulator.
STA $04   ; Store LSB of result in memory at address $0004.

; Add the MSBs, including the carry bit from the first addition.
LDA $01   ; Load MSB of first value into the Accumulator.
ADC $03   ; Add MSB of second value to the Accumulator.
STA $05   ; Store LSB of result in memory at address $0005.
*/

/* Untitled1 (6/13/2021 11:59:17 PM)
   StartOffset(h): 00000000, EndOffset(h): 00000026, Length(h): 00000027 */

unsigned char rawData[39] = {
	0xA9, 0x80, 0x8D, 0x00, 0x00, 0xA9, 0xFF, 0x8D, 0x01, 0x00, 0xA9, 0xFB,
	0x8D, 0x02, 0x00, 0xA9, 0xFF, 0x8D, 0x03, 0x00, 0x18, 0xAD, 0x00, 0x00,
	0x6D, 0x02, 0x00, 0x8D, 0x04, 0x00, 0xAD, 0x01, 0x00, 0x6D, 0x03, 0x00,
	0x8D, 0x05, 0x00
};

namespace nes {
    namespace bus {
        std::array <u8, 0x200> memory = {
            0x00, 0x00, 0x00, 0x00, 0x2a, 0x26, 0x00, 0x36, 0x00, 0x2e, 0x00, 0x00, 0x3e, 0x00, 0x00
        };

        u8 read(u16 addr) {
            if (addr <= 0x1ff) {
                return memory[addr];
            } else {
                return 0x0;
            }
        }

        void write(u16 addr, u8 value) {
            if (addr <= 0x1ff) memory[addr] = value;
        }
    }
}