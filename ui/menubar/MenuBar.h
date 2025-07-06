#pragma once
#include <vector>
#include <string>
#include <memory>
#include "../graphics/GraphicsContext.h"
#include "../graphics/SVGImage.h"

namespace lambdaOS {
namespace ui {

class LauncherMenu;

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
    std::vector<SVGImage> m_trayIcons;
    std::vector<SVGImage> m_appIcons;
    std::unique_ptr<LauncherMenu> m_launcherMenu;
    bool m_launcherMenuOpen = false;
};

} // namespace ui
} // namespace lambdaOS
