#pragma once
#include "../graphics/GraphicsContext.h"
#include <string>

namespace lambdaOS {
namespace ui {

    class UserSetup {
        public:
        UserSetup();
        ~UserSetup();

        void draw(GraphicsContext& ctx, int x, int y, int width, int height);
        void setUsername(const std::string& username)
        void setPassword(const std::string& password);
        void setError(const std::string& error);
        bool onMouseEvent(int mouseX, int mouseY, int button, bool pressed);
        bool onKeyEvent(int key, bool pressed);
        const std::string& getUser() const;
        const std::string& getPassword() const;
        bool isSubmitted() const;

        private:
        std::string m_username;
        std::string m_password;
        bool m_submitted = false;
        int m_focusField = 0;
    };
}
}