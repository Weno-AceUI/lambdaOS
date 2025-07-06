#pragma once
#include <vector>
#include <string>
#include <memory>
#include "../graphics/GraphicsContext.h"
#include "../graphics/SVGImage.h"

namespace lambdaOS {
namespace ui {

class LauncherMenu;
class AppMenu;

class MenuBar {
public:
    MenuBar();
    ~MenuBar();

    void draw(GraphicsContext& ctx, int x, int y, int width, int height);
    bool onMouseEvent(int mouseX, int mouseY, int button, bool pressed);

private:
    void loadIcons();
    bool m_iconsLoaded = false;
    SVGImage m_launcherIcon;
    SVGImage m_appMenuIcon;
    std::vector<SVGImage> m_trayIcons;
    std::vector<SVGImage> m_appIcons;
    std::unique_ptr<LauncherMenu> m_launcherMenu;
    std::unique_ptr<AppMenu> m_appMenu;
    bool m_launcherMenuOpen = false;
    bool m_appMenuOpen = false;
};

} // namespace ui
} // namespace lambdaOS
