#include "cpu/cpu.hpp"

//#define LYNES_TEST_MODE

#define LOG_TARGET_LINUX
#include "log.hpp"

using namespace nes;

#include <iostream>
#include <iomanip>

#include "window.hpp"
#include "input.hpp"
#include "scheduler.hpp"
#include "devices/ppu/callbacks.hpp"

using namespace frontend;

int main(int argc, char* argv[]) {
    _log::init("lynes");

    scheduler::schedule("vblank-nmi", scheduler::nmi, 27390, ppu::vblank_cb);

    cart::load(argv[1]);

    window::register_keydown_cb(frontend::input::keydown);
    window::register_keyup_cb(frontend::input::keyup);

    cpu::init();
    cpu::handle_reset();

    window::init(3);

    ppu::init(window::update);

#ifdef LYNES_TEST_MODE
    do {
        cpu::opcode = bus::read(cpu::registers::pc);

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

        cpu::cycle();
        scheduler::update();
    } while (cpu::cycles_elapsed <= 57250);
#endif

int counter = 1000000;

#ifndef LYNES_TEST_MODE
    while (window::is_open()) {
        cpu::cycle();
        scheduler::update();
    }
#endif

    window::close();

    return 0;
}