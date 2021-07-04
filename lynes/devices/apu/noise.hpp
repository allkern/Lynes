#pragma once

#include "../../global.hpp"

#include "common.hpp"

#include <ctime>
#include <cmath>

namespace nes {
    namespace apu {
        double noise_freq_lut[] = {
            4   , 8   , 16  , 32  , 64  , 96  , 128 , 160 ,
            202 , 254 , 380 , 508 , 762 , 1016, 2034, 4068
        };

        int r = 0;

        int16_t generate_noise_sample(double t, double f, double a) {
            if ((!f) || (f >= NES_NATIVE_SAMPLERATE) || (!a)) return 0x0;

            double c = NES_NATIVE_SAMPLERATE / f;
            
            if (!(((u32)std::round(t) % (u32)std::round(c)))) r = (rand() % 3) - 1;

            return r * 0x3fff;
        }

        struct noise_t {
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
                        return generate_noise_sample(clk++, freq, 1.0);
                    } else {
                        playing = false;
                    }
                } else {
                    return 0x0;
                }

                return 0x0;
            }

            void update() {
                freq = noise_freq_lut[sr[2] & 0xf] * 100;
                double length = length_counter_lut[(sr[3] >> 3) & 0x1f] * 2;

                remaining_samples = (double)(length / 254) * NES_NATIVE_SAMPLERATE;

                playing = true;

                //_log(debug, "freq=%f, remaining_samples=%u, length_count=%u(%02x)", freq, remaining_samples, (sr[3] >> 3) & 0x1f, (sr[3] >> 3) & 0x1f);
            }
        } noi;
    }
}