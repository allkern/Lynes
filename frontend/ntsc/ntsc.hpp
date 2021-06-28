#pragma once

#include <algorithm>
#include <cstdint>
#include <vector>
#include <cmath>
#include <iomanip>
#include <iostream>

#define TAU   6.28318530717
#define PI    3.14159265359
#define TAU4  1.57079632679
#define PI180 0.01745329251

namespace ntsc {
    // Generic NTSC filter for arbitrary images
    // WARNING: Extremely slow
    // Now really, this is really bad, it doesn't even support color yet
    // It will look better in the future.
    //
    // Support getting NTSC encoded images as well as 
    // normal images (in which case encode, then decode).
    //
    // Gameplan:
    // - Encoding:
    // Somehow translate RGB colors to our NTSC color space
    // probably involving some kind of RGB -> YUV, YIQ conversion?
    // 
    // - Decoding:
    // Low-pass encoded sample(s), subtract with original signal
    // Decode both luma and chroma samples, convert to RGB
    // Add RGB samples together.
    //
    // Essentially:
    // rgb -> y ---> carrier ----+-|+|-> NTSC signal -|low-pass|-+-> y -|NTSC decoding|-> y'rgb -+-|+| -> rgb' (with artifacts)
    //  |---> s -+-> subcarrier -+                               +-> c -|NTSC decoding|-> c'rgb -+
    //  +---> p -+

    bool encoded = false;

    // Access a 2D image buffer with clamped coordinates
    namespace input {
        size_t w, h;
        size_t size;

        void* buf;

        uint32_t get(int x, int y) {
            x = std::clamp(x, 0, (int)w);
            y = std::clamp(y, 0, (int)h);

            return static_cast<uint32_t*>(buf)[x + (y * w)];
        }
    }

    uint32_t rgb(int y, int a = 0xff) {
        y &= 0xff;
        a &= 0xff;

        return y << 24 | y << 16 | y << 8 | a;
    }

    uint32_t rgb(int r, int g, int b, int a = 0xff) {
        r &= 0xff;
        g &= 0xff;
        b &= 0xff;
        a &= 0xff;

        return r << 24 | g << 16 | b << 8 | a;
    }

    namespace output {
        size_t w, h;
        size_t size;

        std::vector <uint32_t> buf;

        uint32_t get(int x, int y) {
            x = std::clamp(x, 0, (int)w);
            y = std::clamp(y, 0, (int)h);

            return buf.at(x + (y * w));
        }

        void draw(int x, int y, uint32_t c) {
            x = std::clamp(x, 0, (int)w);
            y = std::clamp(y, 0, (int)h);

            buf.at(x + (y * w)) = c;
        }

        uint32_t* get_buffer() {
            return buf.data();
        }
    }

    std::vector <uint32_t> line;
    std::vector <double> luma, chroma;

    void init(void* buf, size_t width, size_t height, bool ntsc_encoded = false) {
        input::buf  = buf;
        input::w    = width;
        input::h    = height;
        input::size = width * height * 4;

        output::w    = input::w;
        output::h    = input::h;
        output::size = input::size;

        output::buf.resize(output::size);

        line.resize(output::w);
        luma.resize(output::w);
        chroma.resize(output::w);

        encoded = ntsc_encoded;
    }

    #define NTSC_ENCODER_FREQUENCY 90

    struct ntsc_color_t {
        double y; // IRE
        double s; // Saturation
        double p; // Phase
    };

    ntsc_color_t rgb_to_ntsc(double r, double g, double b) {
        ntsc_color_t c;

        c.y = (0.299 * r) + (0.587 * g) + (0.114 * b);
        c.s = 0.0;
        c.p = 0.0;

        return c;
    }

    size_t frames_rendered = 0;

    ntsc_color_t rgb_to_ntsc(uint32_t rgb) {
        ntsc_color_t c;

        int r = (rgb >> 24) & 0xff,
            g = (rgb >> 16) & 0xff,
            b = (rgb >> 8) & 0xff;

        c.y = (0.299 * r) +
              (0.587 * g) +
              (0.114 * b);
        c.y /= 255.0;
        c.s = 0.0;
        c.p = 0.0;

        return c;
    }

    void codec() {
        //#pragma omp parallel for
        for (int py = 0; py < input::h; py++) {
            // Generate raw NTSC signal
            for (int px = 0; px < input::w; px++) {
                auto c = rgb_to_ntsc(input::get(px, py));
                double t = NTSC_ENCODER_FREQUENCY * px;

                // Chroma components are encoded as carrier wave phase and amplitude
                double subcarrier = std::sin((c.p + t) * PI180) * c.s;

                // Luma is added to the carrier wave as a DC offset
                luma[px] = c.y;
                chroma[px] = subcarrier;
            }

            #pragma omp parallel for
            for (int px = 0; px < input::w - 1; px++) {
                double y  = 0, i  = 0, q  = 0,
                               ci = 0, cq = 0;

                for (int d = -2; d < 2; d++) {
                    size_t dx = std::clamp(px + d, 0, (int)input::w);

                    double phase = (px + d) * TAU4,
                           luma_phase = phase + frames_rendered * 3 + py * 1.1;

                    double c = chroma[dx], l = luma[dx];

                    ci += c * std::cos(phase);
                    cq += c * std::sin(phase);
                    y += l;
                    i += l * std::cos(luma_phase);
                    q += l * std::sin(luma_phase);
                }

                ci /= 3.0;
                cq /= 3.0;
                y /= 4.0;
                i /= 3.0;
                q /= 3.0;
                i += ci;
                q += cq;

                line[px] = rgb(
                    std::clamp((y + ( 0.956 * i) + ( 0.621 * q)) * 0xff, 0.0, 255.0),
                    std::clamp((y + (-0.272 * i) + (-0.647 * q)) * 0xff, 0.0, 255.0),
                    std::clamp((y + (-1.107 * i) + ( 1.704 * q)) * 0xff, 0.0, 255.0)
                );
            }

            for (int i = 0; i < input::w; i++)
                output::draw(i, py, line[i]);
        }

        frames_rendered++;
    }
}