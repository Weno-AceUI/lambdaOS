#include "WindowFrame.h"
#include <iostream>

namespace lambdaOS {
namespace ui {

WindowFrameImpl::WindowFrameImpl() {}
WindowFrameImpl::~WindowFrameImpl() {}

void WindowFrameImpl::draw(int x, int y, int width, int height, const std::string& title) {
    std::cout << "Drawing window frame at (" << x << ", " << y << ") size [" << width << "x" << height << "] title: " << title << std::endl;
}

bool WindowFrameImpl::onMouseEvent(int mouseX, int mouseY, int button, bool pressed) {
    std::cout << "Mouse event at (" << mouseX << ", " << mouseY << ") button: " << button << " pressed: " << pressed << std::endl;
    return false;
}

void WindowFrameImpl::onResize(int newWidth, int newHeight) {
    lastWidth = newWidth;
    lastHeight = newHeight;
    std::cout << "Window resized to [" << newWidth << "x" << newHeight << "]" << std::endl;
}

} // namespace ui
} // namespace lambdaOS
