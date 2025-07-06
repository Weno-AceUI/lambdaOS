#include "MenuBar.h"
#include "LauncherMenu.h"
#include <fstream>
#include <sstream>
#include <filesystem>

namespace lambdaOS {
namespace ui {

MenuBar::MenuBar() : m_launcherIcon("icons/Launcher.svg") {}
MenuBar::~MenuBar() {}

void MenuBar::loadIcons() {
    if (m_iconsLoaded) return;
    // Load launcher icon
    std::ifstream file("icons/Launcher.svg");
    std::stringstream buffer;
    buffer << file.rdbuf();
    m_launcherIcon = SVGImage(buffer.str());
    m_launcherIcon.parse();
    // Load app icons from app_icons/
    m_appIcons.clear();
    for (const auto& entry : std::filesystem::directory_iterator("app_icons")) {
        if (entry.path().extension() == ".svg") {
            std::ifstream iconFile(entry.path());
            std::stringstream iconBuffer;
            iconBuffer << iconFile.rdbuf();
            SVGImage icon(iconBuffer.str());
            icon.parse();
            m_appIcons.push_back(icon);
        }
    }
    m_iconsLoaded = true;
}

void MenuBar::draw(GraphicsContext& ctx, int x, int y, int width, int height) {
    loadIcons();
    // Draw menubar background
    ctx.drawRect(x, y, width, height, 0xFFEFEFEF);
    // Draw launcher icon
    ctx.drawSVG(x + 8, y + 4, 28, 28, m_launcherIcon);
    // Draw app icons (spaced horizontally after launcher)
    int iconX = x + 48;
    int iconY = y + 4;
    int iconW = 28, iconH = 28;
    for (const auto& icon : m_appIcons) {
        ctx.drawSVG(iconX, iconY, iconW, iconH, icon);
        iconX += iconW + 8;
    }
    // TODO: Draw tray icons (right)
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
    // TODO: Handle app icon clicks
    if (m_launcherMenuOpen && m_launcherMenu) {
        return m_launcherMenu->onMouseEvent(mouseX, mouseY, button, pressed);
    }
    return false;
}

} // namespace ui
} // namespace lambdaOS
