#pragma once
#include <vector>
#include <string>
#include <cstdint>

namespace lambdaOS {
namespace ui {

class Image {
public:
    Image();
    Image(int width, int height);
    bool loadBMP(const std::string& path); // Minimal BMP loader
    bool loadPNG(const std::string& path); // PNG loader
    int width() const;
    int height() const;
    const std::vector<uint32_t>& pixels() const;
private:
    int m_width = 0, m_height = 0;
    std::vector<uint32_t> m_pixels;
};

} // namespace ui
} // namespace lambdaOS
