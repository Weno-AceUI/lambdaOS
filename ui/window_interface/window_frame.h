#pragma once

#include <cstdint>
#include <string>
#include <memory>

namespace lambdaOS {
namespace ui {

class Window; // Forward declaration

class WindowFrame {
public:
    virtual ~WindowFrame() = default;

    // Draw the window frame (borders, title bar, buttons, etc.)
    virtual void draw(int x, int y, int width, int height, const std::string& title) = 0;

    // Handle mouse events (returns true if event was handled)
    virtual bool onMouseEvent(int mouseX, int mouseY, int button, bool pressed) = 0;

    // Handle window resizing
    virtual void onResize(int newWidth, int newHeight) = 0;

    // Optional: set the window this frame belongs to
    virtual void setWindow(Window* window) { this->window = window; }

protected:
    Window* window = nullptr;
};

using WindowFramePtr = std::shared_ptr<WindowFrame>;

} // namespace ui
} // namespace lambdaOS