#pragma once
#include <string>
#include "../graphics/GraphicsContext.h"

namespace lambdaOS {
namespace ui {

class LauncherMenu {
public:
    LauncherMenu();
    ~LauncherMenu();

    void draw(GraphicsContext& ctx, int x, int y, int width, int height);
    bool onMouseEvent(int mouseX, int mouseY, int button, bool pressed);
};

} // namespace ui
} // namespace lambdaOS
