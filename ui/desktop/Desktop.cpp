#include "Desktop.h"
#include "window_interface/WindowFrame.h"

namespace lambdaOS {
namespace ui {

Desktop::Desktop(int width, int height)
    : m_width(width), m_height(height) {}

Desktop::~Desktop() {}

void Desktop::draw(GraphicsContext& ctx) {
    if (m_bgImage) {
        ctx.drawImage(0, 0, *m_bgImage);
    } else {
        ctx.clear(m_bgColor);
    }
    // Draw all windows
    for (const auto& win : m_windows) {
        if (win) {
            win->draw(50, 50, 600, 400, "Sample Window"); // Example position/size/title
        }
    }
}

void Desktop::addWindow(std::shared_ptr<WindowFrame> window) {
    m_windows.push_back(window);
}

void Desktop::removeWindow(std::shared_ptr<WindowFrame> window) {
    m_windows.erase(std::remove(m_windows.begin(), m_windows.end(), window), m_windows.end());
}

void Desktop::setBackgroundColor(uint32_t color) {
    m_bgColor = color;
}

void Desktop::setBackgroundImage(const Image& img) {
    m_bgImage = std::make_unique<Image>(img);
}

} // namespace ui
} // namespace lambdaOS
