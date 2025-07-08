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
// Renders a blurred background behind the specified rectangle
void drawBackgroundBlur(int x, int y, int width, int height, float blurRadius = 8.0f);
Image GraphicsContext::readPixels(int x, int y, int w, int h) {
    Image img(w, h);
    for (int j = 0; j < h; ++j)
        for (int i = 0; i < w; ++i)
            img.at(i, j) = getFramebufferPixel(x + i, y + j); // ← you’ll replace this
    return img;
}
void GraphicsContext::drawBackgroundBlur(int x, int y, int width, int height, float blurRadius) {
    // Read screen region
    Image captured = readPixels(x, y, width, height);

    // Apply blur
    Image blurred = applyGaussianBlur(captured, blurRadius);

    // Draw blurred image back
    drawImage(blurred, x, y, width, height); // <- implement if not done

    // Optional: Frosted overlay
    drawRect(x, y, width, height, 0x40FFFFFF);
}
Image applyGaussianBlur(const Image& src, float radius) {
    int r = int(radius);
    if (r <= 0) return src;

    int w = src.width, h = src.height;
    Image temp(w, h), out(w, h);

    // Horizontal pass
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int rSum = 0, gSum = 0, bSum = 0, aSum = 0, count = 0;
            for (int i = -r; i <= r; ++i) {
                int xi = std::clamp(x + i, 0, w - 1);
                uint32_t c = src.at(xi, y);
                aSum += (c >> 24) & 0xFF;
                rSum += (c >> 16) & 0xFF;
                gSum += (c >> 8) & 0xFF;
                bSum += c & 0xFF;
                ++count;
            }
            temp.at(x, y) = ((aSum / count) << 24) |
                            ((rSum / count) << 16) |
                            ((gSum / count) << 8) |
                            (bSum / count);
        }
    }

    // Vertical pass
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int rSum = 0, gSum = 0, bSum = 0, aSum = 0, count = 0;
            for (int i = -r; i <= r; ++i) {
                int yi = std::clamp(y + i, 0, h - 1);
                uint32_t c = temp.at(x, yi);
                aSum += (c >> 24) & 0xFF;
                rSum += (c >> 16) & 0xFF;
                gSum += (c >> 8) & 0xFF;
                bSum += c & 0xFF;
                ++count;
            }
            out.at(x, y) = ((aSum / count) << 24) |
                           ((rSum / count) << 16) |
                           ((gSum / count) << 8) |
                           (bSum / count);
        }
    }

    return out;
}

private:
    int m_width, m_height;
    std::vector<uint32_t> m_buffer;
};

struct Image {
    int width, height;
    std::vector<uint32_t> pixels; // Stored as 0xAARRGGBB

    Image(int w, int h) : width(w), height(h), pixels(w * h, 0) {}

    uint32_t& at(int x, int y) {
        return pixels[y * width + x];
    }

    const uint32_t& at(int x, int y) const {
        return pixels[y * width + x];
    }
};

uint32_t makeARGB(uint8_t a, uint8_t r, uint8_t g, uint8_t b);

} // namespace ui
} // namespace lambdaOS
