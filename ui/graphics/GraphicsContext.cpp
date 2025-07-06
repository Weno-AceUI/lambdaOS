#include "GraphicsContext.h"
#include "SVGImage.h"
#include <cstring>
#include <iostream>
#include <cmath>

namespace lambdaOS {
namespace ui {

GraphicsContext::GraphicsContext(int width, int height)
    : m_width(width), m_height(height), m_buffer(width * height, 0xFFFFFFFF) {}

GraphicsContext::~GraphicsContext() {}

void GraphicsContext::clear(uint32_t color) {
    std::fill(m_buffer.begin(), m_buffer.end(), color);
}

void GraphicsContext::drawRect(int x, int y, int w, int h, uint32_t color) {
    for (int j = y; j < y + h; ++j) {
        for (int i = x; i < x + w; ++i) {
            if (i >= 0 && i < m_width && j >= 0 && j < m_height)
                m_buffer[j * m_width + i] = color;
        }
    }
}

void GraphicsContext::drawCircle(int cx, int cy, int r, uint32_t color) {
    for (int y0 = -r; y0 <= r; ++y0) {
        for (int x0 = -r; x0 <= r; ++x0) {
            if (x0 * x0 + y0 * y0 <= r * r) {
                int px = cx + x0;
                int py = cy + y0;
                if (px >= 0 && px < m_width && py >= 0 && py < m_height)
                    m_buffer[py * m_width + px] = color;
            }
        }
    }
}

void GraphicsContext::drawLine(int x1, int y1, int x2, int y2, uint32_t color) {
    int dx = std::abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -std::abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx + dy, e2;
    while (true) {
        if (x1 >= 0 && x1 < m_width && y1 >= 0 && y1 < m_height)
            m_buffer[y1 * m_width + x1] = color;
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
}

void GraphicsContext::drawText(int x, int y, const std::string& text, uint32_t color) {
    // Placeholder: just print to console
    std::cout << "DrawText: '" << text << "' at (" << x << ", " << y << ") color: " << color << std::endl;
}

void GraphicsContext::drawSVG(int x, int y, int w, int h, const SVGImage& svg) {
    // Draw rects
    for (const auto& r : svg.rects) {
        drawRect(x + (int)r.x, y + (int)r.y, (int)r.width, (int)r.height, r.color);
    }
    // Draw circles
    for (const auto& c : svg.circles) {
        drawCircle(x + (int)c.cx, y + (int)c.cy, (int)c.r, c.color);
    }
    // Draw lines
    for (const auto& l : svg.lines) {
        drawLine(x + (int)l.x1, y + (int)l.y1, x + (int)l.x2, y + (int)l.y2, l.color);
    }
}

const std::vector<uint32_t>& GraphicsContext::getBuffer() const {
    return m_buffer;
}

int GraphicsContext::getWidth() const { return m_width; }
int GraphicsContext::getHeight() const { return m_height; }

} // namespace ui
} // namespace lambdaOS
