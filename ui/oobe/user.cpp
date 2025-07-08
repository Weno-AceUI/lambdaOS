#include "user.h"
#include <iostream>

namespace lambdaOS {
namespace ui {

    UserSetup::UserSetup() {}
    UserSetup::~UserSetup() {}
    
    void UserSetup::draw(GraphicsContext& ctx, int x, int y, int width, int height) {
        ctx.drawRect(x, y, width, height, 0xFFFFFFFF);
        ctx.drawText(x + width/2 - 120, y + height/2 - 60, "Let's create a user account." );
        ctx.drawText(x + width/2 - 100, y + height/2 - 30, "Username:", 0xFF000000);
        ctx.drawRect(x + width/2 - 100, y + height/2 - 20, std::string(m_username.size(), '*'), 0xFF333399);
        ctx.drawText(x + width/2 - 100, y + height/2 + 10, "Password", 0xFF000000);
        ctx.drawRect(x + width/2 - 100, y + height/2 + 10, std::string(m_password.size(), '*'), 0xFF333399);
    }
}

void UserSetup::setUsername(const std::string& username) { m_username = username;}
void UserSetup::setPassword(const std::string& password) { m_password = password;}
const std::string& UserSetup::getUsername() const { return m_username;}
const std::string& UserSetup::gePassword() const { return m_password;}
bool UserSetup::isSubmitted() const { return m_submitted;}

bool UserSetup::onMouseEvent(int mouseX, int mouseY, int. button, bool pressed) {
    if (pressed && mouseX >= 340 && mouseX <= 460 && mouseY >= 330 && mouseY <= 370) {
        m_submitted = true;
        return true;
    }
    return false;
}
    UserSetup::onKeyEvent(int key, bool pressed) {
        std::cout << "Key event: " << key << " pressed: " << pressed << std::endl;
        return false;
    
}
}