#pragma once

#ifdef _WIN32
#define LYNES_SDL_WINDOW_FLAGS SDL_WINDOW_VULKAN
#endif

#ifdef __linux__
#define LYNES_SDL_WINDOW_FLAGS SDL_WINDOW_OPENGL
#endif

#include "SDL.h"

#include "ntsc/ntsc.hpp"

#include <chrono>
#include <unordered_map>

#define PPU_WIDTH  256
#define PPU_HEIGHT 240

#define STR1(m) #m
#define STR(m) STR1(m)

namespace frontend {
    namespace window {
        namespace sdl {
            SDL_Window* window = nullptr;
            SDL_Renderer* renderer = nullptr;
            SDL_Texture* texture = nullptr;
        }

        typedef void (*key_event_callback_t) (SDL_Keycode);
        typedef void (*rom_drop_callback_t) (const char*);

        key_event_callback_t keydown_cb, keyup_cb;
        rom_drop_callback_t rom_drop_cb;

        // FPS tracking stuff
        auto start = std::chrono::high_resolution_clock::now(), end = start;
        size_t frames_rendered = 0, fps = 0;

        inline void register_keydown_cb(const key_event_callback_t& kd) {
            keydown_cb = kd;
        }

        inline void register_keyup_cb(const key_event_callback_t& ku) {
            keyup_cb = ku;
        }

        // To-do: Make ROM dropping better
        inline void register_rom_dropped_cb(const rom_drop_callback_t& rd) {
            rom_drop_cb = rd;
        }

        bool open = true;

        bool is_open() {
            return open;
        }

        size_t get_fps() {
            return fps;
        }

        bool fps_ready() {
            end = std::chrono::high_resolution_clock::now();

            std::chrono::duration <double> d = end - start;

            return std::chrono::duration_cast<std::chrono::seconds>(d).count() == 1;
        }

        void close() {
            open = false;
            
            SDL_DestroyTexture(sdl::texture);
            SDL_DestroyRenderer(sdl::renderer);
            SDL_DestroyWindow(sdl::window);

            SDL_Quit();
        }

        bool ntsc_filter_enabled = false;
        uint32_t* ntsc_buf = nullptr;

        int width, height;

        void init(size_t scale, bool ntsc_filter = false, int prescale = 0) {
            uint32_t SDL_INIT_FLAGS = SDL_INIT_VIDEO | SDL_INIT_EVENTS;

            SDL_Init(SDL_INIT_FLAGS);

            width = PPU_WIDTH;
            height = PPU_HEIGHT;

            if (ntsc_filter) {
                SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

                ntsc::init(ppu::frame.get_buffer(), width, height);

                if (prescale > 1) {
                    ntsc::prescaler::init(prescale);

                    width *= prescale;
                    height *= prescale;
                }

                ntsc_buf = ntsc::output::get_buffer();
            }

            sdl::window = SDL_CreateWindow(
                "Lynes " STR(LYNES_COMMIT_HASH),
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                PPU_WIDTH * scale, PPU_HEIGHT * scale,
                LYNES_SDL_WINDOW_FLAGS
            );

            sdl::renderer = SDL_CreateRenderer(
                sdl::window,
                -1,
                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
            );

            sdl::texture = SDL_CreateTexture(
                sdl::renderer,
                SDL_PIXELFORMAT_RGBA8888,
                SDL_TEXTUREACCESS_STREAMING,
                width, height
            );

            ntsc_filter_enabled = ntsc_filter;
        }

        void update(uint32_t* raw_buf) {
            end = std::chrono::high_resolution_clock::now();

            std::chrono::duration <double> d = end - start;

            if (std::chrono::duration_cast<std::chrono::seconds>(d).count() == 1) {
                char buf[100];
                fps = frames_rendered;
                sprintf(&buf[0], "Lynes %s | %f fps", STR(LYNES_COMMIT_HASH), (float)fps);
                SDL_SetWindowTitle(sdl::window, &buf[0]);
                frames_rendered = 0;
                start = std::chrono::high_resolution_clock::now();
                end = start;
            }

            if (ntsc_filter_enabled)
                ntsc::codec();

            SDL_UpdateTexture(
                sdl::texture,
                NULL,
                ntsc_filter_enabled ? ntsc_buf : raw_buf,
                width * sizeof(uint32_t)
            );

            SDL_RenderCopy(sdl::renderer, sdl::texture, NULL, NULL);

            SDL_RenderPresent(sdl::renderer);

            frames_rendered++;

            SDL_Event event;

            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_DROPFILE: { rom_drop_cb(event.drop.file); } break;
                    case SDL_QUIT: { close(); } break;
                    case SDL_KEYDOWN: { keydown_cb(event.key.keysym.sym); } break;
                    case SDL_KEYUP: { keyup_cb(event.key.keysym.sym); } break;
                }
            }
        }
    }
}