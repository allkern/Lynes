#pragma once

#include "../../global.hpp"

#include "common.hpp"

#include <cmath>

namespace nes {
    namespace apu {
        u8 square_duty_cycles[] = {
            0b00000001,
            0b00000011,
            0b00001111,
            0b11111100
        };

        double duties[] = { 8.0, 4.0, 2.0, 1.3 };

        int16_t generate_square_sample(double t, double f, double a, double dc) {
            if ((!dc) || (!f) || (!a)) return 0x0;

            double c = NES_NATIVE_SAMPLERATE / f,
                   h = c / dc;
                   
            if (!((u32)std::round(c))) return 0x0;

            double s = sign(((u32)std::round(t) % (u32)std::round(c)) - h);

            return s * (a * 0x7fff);
        }

        struct square_t {
            double clk = 0.0;
            u8* sr = nullptr, seq = 0;

            bool playing = false;
            size_t remaining_samples = 0;
            double freq = 0.0;
            double duty = 2;

            void init(u8* sr_ptr) {
                sr = sr_ptr;
            }

            int16_t get_sample() {
                if (playing) {
                    if (remaining_samples--) {
                        return generate_square_sample(clk++, freq, 1.0, duty);
                    } else {
                        playing = false;
                    }
                } else {
                    return 0x0;
                }

                return 0x0;
            }

            void update() {
                duty = duties[(sr[0] >> 6) & 0x3];
                freq = (sr[2] | ((sr[3] & 0x7) << 8));
                freq = NES_NATIVE_SAMPLERATE / (16 * (freq + 1));
                double length = length_counter_lut[(sr[3] >> 3) & 0x1f] / 5;

                remaining_samples = (double)(length / 254) * NES_NATIVE_SAMPLERATE;

                playing = true;

                //_log(debug, "freq=%f, duty=%u, rduty=%f, remaining_samples=%u", freq, (sr[0] >> 6) & 0x3, duty, remaining_samples);
            }
        } sq0, sq1;
    }
}