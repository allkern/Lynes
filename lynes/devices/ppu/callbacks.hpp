#pragma once

#include "memory.hpp"
#include "../../scheduler.hpp"

namespace nes {
    namespace ppu {
        int frame_counter = 0;

        void vblank_cb(scheduler::event_t&);

        void spr0_hit_flag_set(scheduler::event_t& event) {
            ppu::r[0x2] |= 0x40;

            scheduler::unschedule(event.name);
        }

        void vblank_flag_clear_cb(scheduler::event_t& this_event) {
            // Clear vblank + spr0 hit
            ppu::r[0x2] &= 0x3f;

            scheduler::unschedule(this_event.name);
            scheduler::schedule("vblank-nmi", scheduler::nmi, 27394, ppu::vblank_cb);
            scheduler::schedule("spr0-flag-set", scheduler::other, 4080, ppu::spr0_hit_flag_set);
        }

        void vblank_cb(scheduler::event_t& event) {
            frame_counter++;

            if (ppu::r[0x1] & 0x8)
                ppu::render();

            if (ppu::r[0x1] & 0x10)
                ppu::render_sprites();

            ppu::r[0x2] |= 0x80;

            if (ppu::r[PPUCTRL] & VBLKNMIE)
                cpu::handle_nmi();

            if (ppu::frame_ready_cb)
                ppu::frame_ready_cb(ppu::frame.get_buffer());

            scheduler::schedule("vblank-flag-clear", scheduler::other, 2385, vblank_flag_clear_cb);
            scheduler::unschedule(event.name);
        }
    }
}