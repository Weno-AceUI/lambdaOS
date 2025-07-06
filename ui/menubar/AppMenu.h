#pragma once
#include <string>
#include "../graphics/GraphicsContext.h"

namespace lambdaOS {
namespace ui {

class AppMenu {
public:
    AppMenu();
    ~AppMenu();

    void draw(GraphicsContext& ctx, int x, int y, int width, int height);
    bool onMouseEvent(int mouseX, int mouseY, int button, bool pressed);
};

} // namespace ui
} // namespace lambdaOS
