#include "MenuBar.h"
#include "LauncherMenu.h"
#include <fstream>
#include <sstream>

namespace lambdaOS {
namespace ui {

MenuBar::MenuBar() : m_launcherIcon("icons/Launcher.svg") {}
MenuBar::~MenuBar() {}

void MenuBar::loadIcons() {
    if (m_iconsLoaded) return;
    // Load launcher icon (replace with your actual path)
    std::ifstream file("icons/Launcher.svg");
    std::stringstream buffer;
    buffer << file.rdbuf();
    m_launcherIcon = SVGImage(buffer.str());
    m_launcherIcon.parse();
    // TODO: Load tray and app icons
    m_iconsLoaded = true;
}

void MenuBar::draw(GraphicsContext& ctx, int x, int y, int width, int height) {
    loadIcons();
    // Draw menubar background
    ctx.drawRect(x, y, width, height, 0xFFEFEFEF);
    // Draw launcher icon
    ctx.drawSVG(x + 8, y + 4, 28, 28, m_launcherIcon);
    // TODO: Draw tray icons (right), app icons (center/left)
    if (m_launcherMenuOpen && m_launcherMenu) {
        m_launcherMenu->draw(ctx, x, y + height, width, 300); // Example size
    }
}

bool MenuBar::onMouseEvent(int mouseX, int mouseY, int button, bool pressed) {
    // Check if launcher icon was clicked
    if (mouseX >= 8 && mouseX <= 36 && mouseY >= 4 && mouseY <= 32 && pressed) {
        if (!m_launcherMenu) m_launcherMenu = std::make_unique<LauncherMenu>();
        m_launcherMenuOpen = !m_launcherMenuOpen;
        return true;
    }
    // TODO: Handle tray/app icon clicks
    if (m_launcherMenuOpen && m_launcherMenu) {
        return m_launcherMenu->onMouseEvent(mouseX, mouseY, button, pressed);
    }
    return false;
}

} // namespace ui
} // namespace lambdaOS
