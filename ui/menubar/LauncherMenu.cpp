#include "LauncherMenu.h"
#include <iostream>

namespace lambdaOS {
namespace ui {

LauncherMenu::LauncherMenu() {}
LauncherMenu::~LauncherMenu() {}

void LauncherMenu::draw(GraphicsContext& ctx, int x, int y, int width, int height) {
    ctx.drawRect(x, y, width, height, 0xFFFFFFFF);
    ctx.drawText(x + 10, y + 30, "Launcher Menu (add your apps here)", 0xFF000000);
}

bool LauncherMenu::onMouseEvent(int mouseX, int mouseY, int button, bool pressed) {
    // Placeholder: print event
    std::cout << "LauncherMenu mouse event at (" << mouseX << ", " << mouseY << ") button: " << button << " pressed: " << pressed << std::endl;
    return false;
}

} // namespace ui
} // namespace lambdaOS
