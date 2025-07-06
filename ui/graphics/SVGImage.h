#pragma once
#include <string>
#include <vector>

namespace lambdaOS {
namespace ui {

struct SVGRect {
    float x, y, width, height;
    uint32_t color;
};
struct SVGCircle {
    float cx, cy, r;
    uint32_t color;
};
struct SVGLine {
    float x1, y1, x2, y2;
    uint32_t color;
};

class SVGImage {
public:
    SVGImage(const std::string& svgData);
    ~SVGImage();

    void parse();
    const std::string& data() const;

    std::vector<SVGRect> rects;
    std::vector<SVGCircle> circles;
    std::vector<SVGLine> lines;

private:
    std::string m_svgData;
};

} // namespace ui
} // namespace lambdaOS
