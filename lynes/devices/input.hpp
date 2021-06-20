#pragma once

#include "../global.hpp"
#include "../log.hpp"

#define DPAD_RIGHT    0b10000000
#define DPAD_LEFT     0b01000000
#define DPAD_DOWN     0b00100000
#define DPAD_UP       0b00010000
#define BUTTON_START  0b00001000
#define BUTTON_SELECT 0b00000100   
#define BUTTON_B      0b00000010
#define BUTTON_A      0b00000001

#define INPUT_BEGIN 0x4016
#define INPUT_END   0x4017

namespace nes {
    namespace input {
        bool strobe = false;
        u8 buttons;
        int index;

        void keydown(u8 k) {
            buttons |= k;
        }

        void keyup(u8 k) {
            buttons &= ~k;
        }

        void write(u16 addr, u8 value) {
            if (addr == 0x4016) {
                strobe = value & 0x1;

                if (strobe)
                    index = 0;
            }
        }

        u8 read(u16 addr) {
            if (index > 7) return 1;

            return (buttons >> index++) & 0x1;
        }
    }
}