#include "WindowTitleBar.h"
#include <iostream>

namespace lambdaOS {
namespace ui {

WindowTitleBar::WindowTitleBar() {}
WindowTitleBar::~WindowTitleBar() {}

void WindowTitleBar::draw(int x, int y, int width, const std::string& title) {
    std::cout << "Drawing title bar at (" << x << ", " << y << ") width [" << width << "] title: " << title << std::endl;
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
