#include "cpu/cpu.hpp"

#define LOG_TARGET_LINUX
#include "log.hpp"

using namespace nes;

#include <iostream>
#include <iomanip>

int main() {
    _log::init("lynes", "lynes.log");

    cart::load("nestest.nes");

    cpu::init();
    cpu::handle_reset();
    cpu::registers::pc = 0xc000;

    int counter = 0;

    do {
        cpu::opcode = bus::read(cpu::registers::pc);

        //_log(debug, "op=%02x, a=%02x, x=%02x, y=%02x, p=%02x, sp=%02x, pc=%04x, cyc=%u",
        _log(debug, "%04X %02X A:%02X X:%02X Y:%02X P:%02X SP:%02X CYC:%u",
            cpu::registers::pc,
            cpu::opcode,
            cpu::registers::a,
            cpu::registers::x,
            cpu::registers::y,
            cpu::registers::p,
            cpu::registers::sp,
            cpu::cycles_elapsed
        );

        cpu::fetch();

        cpu::execute();
        cpu::handle_interrupts();

        //if (counter++ == 10) return 0;
    } while (cpu::cycles_elapsed <= 26554);

    _log(debug, "Test result: %02x", bus::read(0x02));

    return 0;
}