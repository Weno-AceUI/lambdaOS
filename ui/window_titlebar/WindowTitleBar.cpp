#include "WindowTitleBar.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace lambdaOS {
namespace ui {

WindowTitleBar::WindowTitleBar()
    : m_iconClose(""), m_iconMaximize(""), m_iconMinimize("") {}
WindowTitleBar::~WindowTitleBar() {}

void WindowTitleBar::loadIcons() {
    if (m_iconsLoaded) return;
    auto loadSVG = [](const std::string& path) -> SVGImage {
        std::ifstream file(path);
        std::stringstream buffer;
        buffer << file.rdbuf();
        SVGImage img(buffer.str());
        img.parse();
        return img;
    };
    m_iconClose = loadSVG("ui/window_titlebar/icons/close.svg");
    m_iconMaximize = loadSVG("ui/window_titlebar/icons/Maximize.svg");
    m_iconMinimize = loadSVG("ui/window_titlebar/icons/Minimize 2.svg");
    m_iconsLoaded = true;
}

void WindowTitleBar::draw(GraphicsContext& ctx, int x, int y, int width, const std::string& title) {
    loadIcons();
    // Draw title text
    ctx.drawText(x + 40, y + 10, title, 0xFF000000);
    // Draw icons (right-aligned)
    int iconY = y + 5;
    int iconW = 20, iconH = 20;
    ctx.drawSVG(x + width - 30, iconY, iconW, iconH, m_iconClose);
    ctx.drawSVG(x + width - 60, iconY, iconW, iconH, m_iconMaximize);
    ctx.drawSVG(x + width - 90, iconY, iconW, iconH, m_iconMinimize);
}

bool WindowTitleBar::onMouseEvent(int mouseX, int mouseY, int button, bool pressed) {
    std::cout << "Title bar mouse event at (" << mouseX << ", " << mouseY << ") button: " << button << " pressed: " << pressed << std::endl;
    return false;
}

void WindowTitleBar::setTitle(const std::string& title) {
    m_title = title;
}

const std::string& WindowTitleBar::getTitle() const {
    return m_title;
}

} // namespace ui
} // namespace lambdaOS
