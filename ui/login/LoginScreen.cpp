#include "LoginScreen.h"
#include <iostream>

namespace lambdaOS {
namespace ui {

LoginScreen::LoginScreen() {}
LoginScreen::~LoginScreen() {}

void LoginScreen::draw(GraphicsContext& ctx, int x, int y, int width, int height) {
    ctx.drawRect(x, y, width, height, 0xFF222A36); // background
    ctx.drawRect(x + width/2 - 150, y + height/2 - 100, 300, 200, 0xFFFFFFFF); // login box
    ctx.drawText(x + width/2 - 120, y + height/2 - 60, "Username:", 0xFF000000);
    ctx.drawText(x + width/2 - 20, y + height/2 - 60, m_username, 0xFF333399);
    ctx.drawText(x + width/2 - 120, y + height/2 - 20, "Password:", 0xFF000000);
    ctx.drawText(x + width/2 - 20, y + height/2 - 20, std::string(m_password.size(), '*'), 0xFF333399);
    ctx.drawRect(x + width/2 - 60, y + height/2 + 30, 120, 40, 0xFF4F8B81); // login button
    ctx.drawText(x + width/2 - 30, y + height/2 + 50, "Login", 0xFFFFFFFF);
    if (!m_error.empty()) {
        ctx.drawText(x + width/2 - 120, y + height/2 + 80, m_error, 0xFFFF3333);
    }
}

void LoginScreen::setUsername(const std::string& username) { m_username = username; }
void LoginScreen::setPassword(const std::string& password) { m_password = password; }
void LoginScreen::setError(const std::string& error) { m_error = error; }
const std::string& LoginScreen::getUsername() const { return m_username; }
const std::string& LoginScreen::getPassword() const { return m_password; }
bool LoginScreen::isSubmitted() const { return m_submitted; }

bool LoginScreen::onMouseEvent(int mouseX, int mouseY, int button, bool pressed) {
    // Check if login button clicked
    // (Assume draw coords are 0,0 for simplicity)
    if (pressed && mouseX >= 340 && mouseX <= 460 && mouseY >= 330 && mouseY <= 370) {
        m_submitted = true;
        return true;
    }
    return false;
}

bool LoginScreen::onKeyEvent(int key, bool pressed) {
    // Placeholder: just print key events
    std::cout << "Key event: " << key << " pressed: " << pressed << std::endl;
    return false;
}

} // namespace ui
} // namespace lambdaOS
