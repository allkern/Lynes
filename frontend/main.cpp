#include "cpu/cpu.hpp"

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

    scheduler::schedule("vblank-nmi", scheduler::nmi, 27393, ppu::vblank_cb);

    cart::load(argv[1]);

    window::register_keydown_cb(frontend::input::keydown);
    window::register_keyup_cb(frontend::input::keyup);

    cpu::init();
    cpu::handle_reset();

    window::init(3);

    ppu::init(window::update);

    while (window::is_open()) {
        cpu::cycle();
        scheduler::update();
    }

    window::close();

    return 0;
}