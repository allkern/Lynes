#pragma once

#include <algorithm>
#include <iostream>
#include <cstdint>
#include <iomanip>
#include <thread>
#include <vector>
#include <cmath>
#include <mutex>

#define TAU   6.28318530717
#define PI    3.14159265359
#define TAU4  1.57079632679
#define PI180 0.01745329251
#define DEFAULT_THREADS 1

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

        static std::vector <uint32_t> buf;

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

    bool prescale = false;

    namespace prescaler {
        namespace original {
            void* buf;
            int width, height;
        }

        void* input;
        int ps_scale = 0;

        int width = 0, height = 0;

        static std::vector <uint32_t> buf;

        void init(int scale) {
            original::buf    = input::buf;
            original::width  = input::w;
            original::height = input::h;

            ps_scale = scale;
            width = input::w * scale;
            height = input::h * scale;

            output::w = width;
            output::h = height;
            output::buf.resize(output::w * output::h);

            buf.resize(width * height);

            input::buf = buf.data();
            input::w = width;
            input::h = height;

            line.resize(output::w);
            luma.resize(output::w);
            chroma.resize(output::w);

            prescale = true;
        }

        void update() {
            for (int y = 0; y < original::height; y++) {
                for (int x = 0; x < original::width; x++) {
                    int sx = x * ps_scale,
                        sy = y * ps_scale;

                    uint32_t col = static_cast<uint32_t*>(original::buf)[x + (y * original::width)];

                    for (int dy = 0; dy < ps_scale; dy++) {
                        for (int dx = 0; dx < ps_scale; dx++) {
                            size_t index = (sx+dx) + ((sy+dy) * width);

                            if (index < (width * height))
                                buf[index] = col;
                        }
                    }
                }
            }
        }
    }

    double sin_table[360], cos_table[360];

    void init(void* buf, size_t width, size_t height, bool ntsc_encoded = false) {
        input::buf  = buf;
        input::w    = width;
        input::h    = height;
        input::size = width * height * 4;

        output::w    = input::w;
        output::h    = input::h;
        output::size = input::size;

        for (double t = 0; t < 360; t++) {
            sin_table[(int)t] = std::sin(t * PI180);
            cos_table[(int)t] = std::cos(t * PI180);
        }

        //output::buf.resize(output::size);

        //line.resize(output::w);
        //luma.resize(output::w);
        //chroma.resize(output::w);

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
        c.s = ((0.596 * r) - (0.275 * g) - (0.321 * b)) / 255;
        c.p = ((0.212 * r) - (0.523 * g) + (0.311 * b)) / 255;
        c.p += 90;

        return c;
    }

    void codec_region(int ps, int l) {
        for (int py = ps; py < (ps + l); py++) {
            double phase = 0, saturation = 0;
            // Generate raw NTSC signal
            for (int px = 0; px < input::w; px++) {
                auto c = rgb_to_ntsc(input::get(px, py));
                double t = NTSC_ENCODER_FREQUENCY * px;

                // Chroma components are encoded as carrier wave phase and amplitude
                if (!(px % 6)) {
                    phase = c.p;
                    saturation = c.s;
                }

                // Luma is added to the carrier wave as a DC offset
                luma[px] = c.y;
                chroma[px] = std::sin((phase + t) * PI180) * saturation;
            }

            for (int px = 0; px < input::w; px++) {
                double y  = 0, i  = 0, q  = 0,
                               ci = 0, cq = 0;

                #pragma omp parallel for
                for (int d = -2; d < 2; d++) {
                    size_t dx = std::clamp(px + d, 0, (int)input::w);

                    double c = chroma[dx], l = luma[dx];

                    if (!l) continue;

                    double phase = (px + d) * TAU4,
                           luma_phase = phase + frames_rendered * 3 + py * 1.2;

                    ci += c * std::cos(phase);
                    cq += c * std::sin(phase);
                    y += l;
                    i += l * std::cos(luma_phase);
                    q += l * std::sin(luma_phase);
                }

                ci /= 4.0;
                cq /= 4.0;
                y /= 4.0;
                i /= 4.0;
                q /= 4.0;
                i += ci;
                q += cq;

                line[px] = rgb(
                    std::clamp((y + ( 0.956 * i) + ( 0.621 * q)) * 0xff, 0.0, 255.0),
                    std::clamp((y + (-0.272 * i) + (-0.647 * q)) * 0xff, 0.0, 255.0),
                    std::clamp((y + (-1.107 * i) + ( 1.704 * q)) * 0xff, 0.0, 255.0)
                );
            }

            for (int i = 0; i < output::w; i++)
                output::draw(i, py, line[i]);
        }
    }

    void codec() {
        if (prescale)
            prescaler::update();

        codec_region(0, output::h);

        frames_rendered++;
    }
}