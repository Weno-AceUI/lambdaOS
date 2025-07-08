#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace lambdaOS {
namespace ui {

class SVGImage;
class Image;

class GraphicsContext {
public:
    GraphicsContext(int width, int height);
    ~GraphicsContext();

    void clear(uint32_t color);
    void drawRect(int x, int y, int w, int h, uint32_t color);
    void drawCircle(int cx, int cy, int r, uint32_t color);
    void drawLine(int x1, int y1, int x2, int y2, uint32_t color);
    void drawText(int x, int y, const std::string& text, uint32_t color);
    void drawSVG(int x, int y, int w, int h, const SVGImage& svg);
    void drawImage(int x, int y, const Image& img);
    const std::vector<uint32_t>& getBuffer() const;
    int getWidth() const;
    int getHeight() const;
    void drawRectWithInnerShadow(int x, int y, int width, int height,
                                 uint32_t fillColor,
                                 uint32_t shadowColor = 0x20000000,
                                 int shadowSize = 4);

private:
    int m_width, m_height;
    std::vector<uint32_t> m_buffer;
};

uint32_t makeARGB(uint8_t a, uint8_t r, uint8_t g, uint8_t b);

} // namespace ui
} // namespace lambdaOS
