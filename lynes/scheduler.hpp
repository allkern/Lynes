#pragma once

#include "cpu/interrupts.hpp"

#include <algorithm>
#include <vector>

namespace nes {
    namespace scheduler {
        struct event_t;

        typedef void (*event_callback_t)(event_t&);

        enum type_t {
            nmi,
            irq,
            other
        };

        struct event_t {
            int trigger, reload;

            type_t type;

            event_callback_t cb = nullptr;

            const char* name;
        };

        std::vector <event_t> events;

        void schedule(const char* name, type_t type, int trigger, event_callback_t cb = nullptr, int reload = 0) {
            events.push_back(event_t{trigger, ((!reload) ? trigger : reload), type, cb, name});
        }

        void unschedule(const char* name) {
            events.erase(
                std::find_if(
                    std::begin(events),
                    std::end(events),
                    [name](event_t& event){ return event.name == name; }
                )
            );
        }

        long prev_cycles = 0, d = 0;

        void update() {
            d = cpu::cycles_elapsed - prev_cycles;
            prev_cycles = cpu::cycles_elapsed;

            for (event_t& event : events) {
                event.trigger -= d;

                if (event.trigger <= 0) {
                    event.trigger += event.reload;

                    if (event.cb)
                        event.cb(event);
                }
            }
        }
    }
}