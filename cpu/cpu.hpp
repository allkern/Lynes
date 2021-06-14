#pragma once

#include "opcode_table.hpp"
#include "registers.hpp"
#include "bus.hpp"

#include "../global.hpp"
#include "../log.hpp"

namespace nes {
    namespace cpu {
        using namespace registers;

        void init() {
            p = 0x00;
            sp = 0xfd;
        }

        void fetch() {
            opcode = bus::read(pc++);
        }

        void execute() {
            const opcode_t& decoded = opcode_map[opcode];

            decoded.addressing_mode();
            decoded.instruction();

            last_cycles = decoded.t;
            cycles_elapsed += last_cycles;
        }

        void cycle() {
            fetch();
            execute();
        }
    }
}