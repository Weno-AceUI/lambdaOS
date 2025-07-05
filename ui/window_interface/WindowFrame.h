#pragma once

#include "window_frame.h"
#include <string>

namespace lambdaOS {
namespace ui {

class WindowFrameImpl : public WindowFrame {
public:
    WindowFrameImpl();
    virtual ~WindowFrameImpl() override;

    void draw(int x, int y, int width, int height, const std::string& title) override;
    bool onMouseEvent(int mouseX, int mouseY, int button, bool pressed) override;
    void onResize(int newWidth, int newHeight) override;

private:
    int lastWidth = 0;
    int lastHeight = 0;
};

} // namespace ui
} // namespace lambdaOS
