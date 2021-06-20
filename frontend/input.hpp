#pragma once

#include "SDL_keycode.h"

#include "devices/input.hpp"

namespace frontend {
    namespace input {
        void keydown(SDL_Keycode k) {
            switch (k) {
                case SDLK_RETURN: { nes::input::keydown(BUTTON_START); return; }
                case SDLK_q     : { nes::input::keydown(BUTTON_SELECT); return; }
                case SDLK_a     : { nes::input::keydown(BUTTON_A); return; }
                case SDLK_s     : { nes::input::keydown(BUTTON_B); return; }
                case SDLK_UP    : { nes::input::keydown(DPAD_UP); return; }
                case SDLK_DOWN  : { nes::input::keydown(DPAD_DOWN); return; }
                case SDLK_LEFT  : { nes::input::keydown(DPAD_LEFT); return; }
                case SDLK_RIGHT : { nes::input::keydown(DPAD_RIGHT); return; }
            } 
        }

        void keyup(SDL_Keycode k) {
            switch (k) {
                case SDLK_RETURN: { nes::input::keyup(BUTTON_START); return; }
                case SDLK_q     : { nes::input::keyup(BUTTON_SELECT); return; }
                case SDLK_a     : { nes::input::keyup(BUTTON_A); return; }
                case SDLK_s     : { nes::input::keyup(BUTTON_B); return; }
                case SDLK_UP    : { nes::input::keyup(DPAD_UP); return; }
                case SDLK_DOWN  : { nes::input::keyup(DPAD_DOWN); return; }
                case SDLK_LEFT  : { nes::input::keyup(DPAD_LEFT); return; }
                case SDLK_RIGHT : { nes::input::keyup(DPAD_RIGHT); return; }
            } 
        }
    }
}