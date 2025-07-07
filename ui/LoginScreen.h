#pragma once
#include "graphics/GraphicsContext.h"
#include <string>

namespace lambdaOS {
namespace ui {

class LoginScreen {
public:
    LoginScreen();
    ~LoginScreen();

    void draw(GraphicsContext& ctx, int x, int y, int width, int height);
    void setUsername(const std::string& username);
    void setPassword(const std::string& password);
    void setError(const std::string& error);
    bool onMouseEvent(int mouseX, int mouseY, int button, bool pressed);
    bool onKeyEvent(int key, bool pressed);
    const std::string& getUsername() const;
    const std::string& getPassword() const;
    bool isSubmitted() const;

private:
    std::string m_username;
    std::string m_password;
    std::string m_error;
    bool m_submitted = false;
    int m_focusField = 0; // 0: username, 1: password
};

} // namespace ui
} // namespace lambdaOS
