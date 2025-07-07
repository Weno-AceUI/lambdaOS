#pragma once
#include "graphics/GraphicsContext.h"
#include "graphics/Image.h"
#include <vector>
#include <memory>

namespace lambdaOS {
namespace ui {

class WindowFrame;

class Desktop {
public:
    Desktop(int width, int height);
    ~Desktop();

    void draw(GraphicsContext& ctx);
    void addWindow(std::shared_ptr<WindowFrame> window);
    void removeWindow(std::shared_ptr<WindowFrame> window);
    void setBackgroundColor(uint32_t color);
    void setBackgroundImage(const Image& img);

private:
    int m_width, m_height;
    uint32_t m_bgColor = 0xFF1E222A;
    std::vector<std::shared_ptr<WindowFrame>> m_windows;
    std::unique_ptr<Image> m_bgImage;
};

} // namespace ui
} // namespace lambdaOS
