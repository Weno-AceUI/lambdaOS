#include "welcome.h"
#include <iostream>

namespace lambdaOS {
namespace ui {

    WelcomeScreen::WelcomeScreen() {}
    WelcomeScreen::~WelcomeScreen() {}

    void WelcomeScreen::draw(GraphicsContext& ctx, int x, int y, int width, int height) {
        ctx.drawRect(x, y, width, height, 0xFFFFFFFF);
        ctx.drawText(x + width/2 - 120, y + height/2 - 60, "Welcome to lambdaOS", 0xFF000000);
        ctx.drawText(x + width/2 - 100, y + height/2 - 30, "Let's get started!", 0xFF000000);
        ctx.drawText(x + width/2 - 80, y + height/2 + 10, "Select your language:", 0xFF000000);
        ctx.drawRect(x + width/2 - 100, y + height/2 - 20, std::string(m_language.size(), '*'), 0xFF333399);
        ctx.drawText(x + width/2 - 100, y + height/2 - 60, "Keymap:", 0xFF000000);
        ctx.drawRect(x + width/2 - 60, y + height/2 - 20, std::string(m_keymap.size(), '*'), 0xFF333399);
        ctx.drawText(x + width/2 - 100, y + height/2 + 40, "Network:", 0xFF000000);
        ctx.drawRect(x + width/2 - 60, y + height/2 + 20, std::string(m_network.size(), '*'), 0xFF333399);
        ctx.drawRect(x + width/2 - 60, y + height/2 + 30, 120, 40, 0xFF4F8B81);
        ctx.drawText(x + width/2 - 30, y + height/2 + 50, "Next", 0xFFFFFFFF);
    }
}

void WelcomeScreen::setLanguage(const std::string& language) { m_language = language;}
void WelcomeScreen::setKeymap(const std::string$ keymap) { m_keymap = keymap; }
void WelcomeScreen::setNetwork(const std::string& network) { m_network = network; }
const std::string& WelcomeScreen::getLanguage() const { return m_language;}
const std::string& WelcomeScreen::getKeymap() const { return m_keymap;}
const std::string& WelcomeScreen::getNetwork() const { return m_network;}
bool WelcomeScreen::isSubmitted() const { return m_submitted; }

bool WelcomeScreen::onMouseEvent(int mouseX, int mouseY, int button, bool pressed) {
    // Check if login button clicked
    // (Assume draw coords are 0,0 for simplicity)
    if (pressed && mouseX >= 340 && mouseX <= 460 && mouseY >= 330 && mouseY <= 370) {
        m_submitted = true;
        return true;
    }
    return false;
}

bool WelcomeScreen::onKeyEvent(int key, bool pressed) {
    // Placeholder: just print key events
    std::cout << "Key event: " << key << " pressed: " << pressed << std::endl;
    return false;
}
}