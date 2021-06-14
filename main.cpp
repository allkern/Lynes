#include "cpu/cpu.hpp"

#define LOG_TARGET_LINUX
#include "log.hpp"

using namespace nes;

#include <iostream>
#include <iomanip>

int main() {
    _log::init("lynes");

    cpu::init();

    cpu::registers::pc = 0x4;

    do {
        cpu::cycle();

        _log(debug, "a=%02x, x=%02x, y=%02x, p=%02x, sp=%02x, pc=%04x: %02x %02x %02x %02x %02x %02x",
            cpu::registers::a,
            cpu::registers::x,
            cpu::registers::y,
            cpu::registers::p,
            cpu::registers::sp,
            cpu::registers::pc,
            bus::read(0),
            bus::read(1),
            bus::read(2),
            bus::read(3),
            bus::read(4),
            bus::read(5)
        );
    } while (cpu::opcode != 0x0);

    return 0;
}