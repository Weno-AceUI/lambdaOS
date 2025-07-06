#include "AppMenu.h"
#include <iostream>

namespace lambdaOS {
namespace ui {

AppMenu::AppMenu() {}
AppMenu::~AppMenu() {}

void AppMenu::draw(GraphicsContext& ctx, int x, int y, int width, int height) {
    ctx.drawRect(x, y, width, height, 0xFFF8F8F8);
    ctx.drawText(x + 10, y + 30, "Application Menu (add your app actions here)", 0xFF000000);
}

bool AppMenu::onMouseEvent(int mouseX, int mouseY, int button, bool pressed) {
    std::cout << "AppMenu mouse event at (" << mouseX << ", " << mouseY << ") button: " << button << " pressed: " << pressed << std::endl;
    return false;
}

} // namespace ui
} // namespace lambdaOS
