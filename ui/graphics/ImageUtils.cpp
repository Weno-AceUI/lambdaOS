#include "ImageUtils.h"
#include "Image.h"
#include <algorithm>
#include <cmath>

Image applyGaussianBlur(const Image& src, float radius) {
    int r = int(radius);
    if (r <= 0) return src;

    int w = src.width, h = src.height;
    Image temp(w, h), out(w, h);

    // Horizontal pass
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int a = 0, rSum = 0, g = 0, b = 0, count = 0;
            for (int i = -r; i <= r; ++i) {
                int xi = std::clamp(x + i, 0, w - 1);
                uint32_t c = src.at(xi, y);
                a += (c >> 24) & 0xFF;
                rSum += (c >> 16) & 0xFF;
                g += (c >> 8) & 0xFF;
                b += c & 0xFF;
                ++count;
            }
            temp.at(x, y) = ((a / count) << 24) | ((rSum / count) << 16) |
                            ((g / count) << 8) | (b / count);
        }
    }

    // Vertical pass
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int a = 0, rSum = 0, g = 0, b = 0, count = 0;
            for (int i = -r; i <= r; ++i) {
                int yi = std::clamp(y + i, 0, h - 1);
                uint32_t c = temp.at(x, yi);
                a += (c >> 24) & 0xFF;
                rSum += (c >> 16) & 0xFF;
                g += (c >> 8) & 0xFF;
                b += c & 0xFF;
                ++count;
            }
            out.at(x, y) = ((a / count) << 24) | ((rSum / count) << 16) |
                           ((g / count) << 8) | (b / count);
        }
    }

    return out;
}