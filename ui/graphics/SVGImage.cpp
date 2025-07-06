#include "SVGImage.h"
#include <regex>
#include <sstream>
#include <iostream>

namespace lambdaOS {
namespace ui {

static uint32_t parseColor(const std::string& colorStr) {
    // Only supports #RRGGBB
    if (colorStr.size() == 7 && colorStr[0] == '#') {
        unsigned int rgb = std::stoul(colorStr.substr(1), nullptr, 16);
        return 0xFF000000 | rgb;
    }
    return 0xFF000000; // default black
}

SVGImage::SVGImage(const std::string& svgData) : m_svgData(svgData) {}
SVGImage::~SVGImage() {}

void SVGImage::parse() {
    rects.clear(); circles.clear(); lines.clear();
    std::regex rectRe(R"(<rect[^>]*x=\"([\d.]+)\"[^>]*y=\"([\d.]+)\"[^>]*width=\"([\d.]+)\"[^>]*height=\"([\d.]+)\"[^>]*fill=\"([^\"]+)\"[^"]*/?>)");
    std::regex circleRe(R"(<circle[^>]*cx=\"([\d.]+)\"[^>]*cy=\"([\d.]+)\"[^>]*r=\"([\d.]+)\"[^>]*fill=\"([^\"]+)\"[^"]*/?>)");
    std::regex lineRe(R"(<line[^>]*x1=\"([\d.]+)\"[^>]*y1=\"([\d.]+)\"[^>]*x2=\"([\d.]+)\"[^>]*y2=\"([\d.]+)\"[^>]*stroke=\"([^\"]+)\"[^"]*/?>)");
    std::smatch m;
    std::string s = m_svgData;
    auto begin = s.cbegin(), end = s.cend();
    while (std::regex_search(begin, end, m, rectRe)) {
        SVGRect r;
        r.x = std::stof(m[1]); r.y = std::stof(m[2]);
        r.width = std::stof(m[3]); r.height = std::stof(m[4]);
        r.color = parseColor(m[5]);
        rects.push_back(r);
        begin = m[0].second;
    }
    begin = s.cbegin();
    while (std::regex_search(begin, end, m, circleRe)) {
        SVGCircle c;
        c.cx = std::stof(m[1]); c.cy = std::stof(m[2]);
        c.r = std::stof(m[3]);
        c.color = parseColor(m[4]);
        circles.push_back(c);
        begin = m[0].second;
    }
    begin = s.cbegin();
    while (std::regex_search(begin, end, m, lineRe)) {
        SVGLine l;
        l.x1 = std::stof(m[1]); l.y1 = std::stof(m[2]);
        l.x2 = std::stof(m[3]); l.y2 = std::stof(m[4]);
        l.color = parseColor(m[5]);
        lines.push_back(l);
        begin = m[0].second;
    }
}

const std::string& SVGImage::data() const { return m_svgData; }

} // namespace ui
} // namespace lambdaOS
