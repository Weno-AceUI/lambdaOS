#pragma once

#include <string>
#include <memory>

namespace lambdaOS {
namespace ui {

class WindowTitleBar {
public:
    WindowTitleBar();
    virtual ~WindowTitleBar();

    // Draw the title bar
    void draw(int x, int y, int width, const std::string& title);

    // Handle mouse events (returns true if event was handled)
    bool onMouseEvent(int mouseX, int mouseY, int button, bool pressed);

    // Set the title
    void setTitle(const std::string& title);
    const std::string& getTitle() const;

private:
    std::string m_title;
};

using WindowTitleBarPtr = std::shared_ptr<WindowTitleBar>;

} // namespace ui
} // namespace lambdaOS
