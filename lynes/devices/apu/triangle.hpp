#pragma once

#include "../../global.hpp"

#include "common.hpp"

#include <cmath>

namespace nes {
    namespace apu {
        double triangle_values[] = {
            1.00, 0.93, 0.86, 0.80, 0.73, 0.66, 0.60, 0.53, 0.46, 0.40, 0.33, 0.26, 0.20, 0.13, 0.06, 0.00,
            0.00, 0.06, 0.13, 0.20, 0.26, 0.33, 0.40, 0.46, 0.53, 0.60, 0.66, 0.73, 0.80, 0.86, 0.93, 1.00
        };

        int sample = 0;
        double value = 0.0;

        int16_t generate_triangle_sample(double t, double f, double a) {
            if ((!f) || (!a)) return 0x0;

            double c = ((double)NES_NATIVE_SAMPLERATE / f),
                   sl = c / 32;

            if (!(uint32_t)sl) sl = 1;

            if (!((uint32_t)std::round(t) % (uint32_t)sl)) {
                value = triangle_values[sample++];
                sample &= 0x1f;
            }

            return value * 0x7fff;
        }

        struct triangle_t {
            double clk = 0.0;
            u8* sr = nullptr, seq = 0;

            bool playing = false;
            size_t remaining_samples = 0;
            double freq = 0.0;

            void init(u8* sr_ptr) {
                sr = sr_ptr;
            }

            int16_t get_sample() {
                if (playing) {
                    if (remaining_samples--) {
                        return generate_triangle_sample(clk++, freq, 1.0);
                    } else {
                        playing = false;
                    }
                } else {
                    return 0x0;
                }

                return 0x0;
            }

            void update() {
                freq = (sr[2] | ((sr[3] & 0x7) << 8));
                freq = NES_NATIVE_SAMPLERATE / (32 * (freq + 1));
                double length = length_counter_lut[(sr[3] >> 3) & 0x1f];

                remaining_samples = (double)(length / 254) * NES_NATIVE_SAMPLERATE;

                playing = true;

                //_log(debug, "freq=%f, remaining_samples=%u, length_count=%u(%02x)", freq, remaining_samples, (sr[3] >> 3) & 0x1f, (sr[3] >> 3) & 0x1f);
            }
        } tri;
    }
}