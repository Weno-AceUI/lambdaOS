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

private:
    int m_width, m_height;
    std::vector<uint32_t> m_buffer;
};

} // namespace ui
} // namespace lambdaOS
