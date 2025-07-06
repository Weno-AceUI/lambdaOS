#include "MenuBar.h"
#include "LauncherMenu.h"
#include "AppMenu.h"
#include <fstream>
#include <sstream>
#include <filesystem>

namespace lambdaOS {
namespace ui {

MenuBar::MenuBar() : m_launcherIcon("icons/Launcher.svg"), m_appMenuIcon("icons/AppMenu.svg") {}
MenuBar::~MenuBar() {}

void MenuBar::loadIcons() {
    if (m_iconsLoaded) return;
    // Load launcher icon
    std::ifstream file("icons/Launcher.svg");
    std::stringstream buffer;
    buffer << file.rdbuf();
    m_launcherIcon = SVGImage(buffer.str());
    m_launcherIcon.parse();
    // Load app menu icon
    std::ifstream appMenuFile("icons/AppMenu.svg");
    std::stringstream appMenuBuffer;
    appMenuBuffer << appMenuFile.rdbuf();
    m_appMenuIcon = SVGImage(appMenuBuffer.str());
    m_appMenuIcon.parse();
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
    // Draw app menu icon (next to launcher)
    ctx.drawSVG(x + 44, y + 4, 28, 28, m_appMenuIcon);
    // Draw app icons (spaced horizontally after app menu)
    int iconX = x + 84;
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
    if (m_appMenuOpen && m_appMenu) {
        m_appMenu->draw(ctx, x + 44, y + height, 200, 300); // Example size/position
    }
}

bool MenuBar::onMouseEvent(int mouseX, int mouseY, int button, bool pressed) {
    // Check if launcher icon was clicked
    if (mouseX >= 8 && mouseX <= 36 && mouseY >= 4 && mouseY <= 32 && pressed) {
        if (!m_launcherMenu) m_launcherMenu = std::make_unique<LauncherMenu>();
        m_launcherMenuOpen = !m_launcherMenuOpen;
        return true;
    }
    // Check if app menu icon was clicked
    if (mouseX >= 44 && mouseX <= 72 && mouseY >= 4 && mouseY <= 32 && pressed) {
        if (!m_appMenu) m_appMenu = std::make_unique<AppMenu>();
        m_appMenuOpen = !m_appMenuOpen;
        return true;
    }
    // TODO: Handle app icon clicks
    if (m_launcherMenuOpen && m_launcherMenu) {
        return m_launcherMenu->onMouseEvent(mouseX, mouseY, button, pressed);
    }
    if (m_appMenuOpen && m_appMenu) {
        return m_appMenu->onMouseEvent(mouseX, mouseY, button, pressed);
    }
    return false;
}

} // namespace ui
} // namespace lambdaOS
