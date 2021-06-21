#pragma once

#include "memory.hpp"
#include "../../scheduler.hpp"

namespace nes {
    namespace ppu {
        void vblank_cb(scheduler::event_t&);
        void vblank_flag_clear_cb(scheduler::event_t& this_event) {
            ppu::r[0x2] &= 0x7f;

            scheduler::unschedule(this_event.name);
            scheduler::schedule("vblank-nmi", scheduler::nmi, 27393, ppu::vblank_cb);
        }

        void vblank_cb(scheduler::event_t& event) {
            if (ppu::r[0x1] & 0x8)
                ppu::render();

            if (ppu::r[0x1] & 0x10)
                ppu::render_sprites();

            //_log(debug, "vblank, trigger=%i, type=%u, reload=%u, cpucyc=%u", event.trigger, event.type, event.reload, cpu::cycles_elapsed);
            ppu::r[0x2] |= 0x80;

            if (ppu::r[PPUCTRL] & VBLKNMIE)
                cpu::handle_nmi();

            if (ppu::frame_ready_cb)
                ppu::frame_ready_cb(ppu::frame.get_buffer());

            scheduler::schedule("vblank-flag-clear", scheduler::other, 2387, vblank_flag_clear_cb);
            scheduler::unschedule(event.name);
        }
    }
}