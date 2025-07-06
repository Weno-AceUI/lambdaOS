#include "Image.h"
#include "lodepng.h"
#include <fstream>
#include <iostream>

namespace lambdaOS {
namespace ui {

Image::Image() {}
Image::Image(int width, int height) : m_width(width), m_height(height), m_pixels(width * height, 0xFF000000) {}

bool Image::loadBMP(const std::string& path) {
    // Minimal 24bpp BMP loader (no compression, no color table)
    std::ifstream file(path, std::ios::binary);
    if (!file) return false;
    uint8_t header[54];
    file.read(reinterpret_cast<char*>(header), 54);
    if (header[0] != 'B' || header[1] != 'M') return false;
    int w = *(int*)&header[18];
    int h = *(int*)&header[22];
    int bpp = *(short*)&header[28];
    if (bpp != 24) return false;
    m_width = w; m_height = h;
    m_pixels.resize(w * h);
    int row_padded = (w * 3 + 3) & (~3);
    std::vector<uint8_t> row(row_padded);
    for (int y = h - 1; y >= 0; --y) {
        file.read(reinterpret_cast<char*>(row.data()), row_padded);
        for (int x = 0; x < w; ++x) {
            uint8_t b = row[x * 3 + 0];
            uint8_t g = row[x * 3 + 1];
            uint8_t r = row[x * 3 + 2];
            m_pixels[y * w + x] = 0xFF000000 | (r << 16) | (g << 8) | b;
        }
    }
    return true;
}

bool Image::loadPNG(const std::string& path) {
    std::vector<unsigned char> png;
    std::vector<unsigned char> image;
    unsigned w, h;
    if (lodepng::load_file(png, path) != 0) return false;
    if (lodepng::decode(image, w, h, png) != 0) return false;
    m_width = w; m_height = h;
    m_pixels.resize(w * h);
    for (size_t i = 0; i < w * h; ++i) {
        uint8_t r = image[4 * i + 0];
        uint8_t g = image[4 * i + 1];
        uint8_t b = image[4 * i + 2];
        uint8_t a = image[4 * i + 3];
        m_pixels[i] = (a << 24) | (r << 16) | (g << 8) | b;
    }
    return true;
}

int Image::width() const { return m_width; }
int Image::height() const { return m_height; }
const std::vector<uint32_t>& Image::pixels() const { return m_pixels; }

} // namespace ui
} // namespace lambdaOS
