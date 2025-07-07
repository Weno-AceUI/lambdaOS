#pragma once
#include "../graphics/GraphicsContext.h"
#include <string>

namespace lambdaOS {
namespace ui {

    class WelcomeScreen {
        public:
        WelcomeScreen();
        ~WelcomeScreen();

        void draw(GraphicsContext& ctx, int x, int y, int width, int height);
        void setLanguage(const std::string& language);
        void setKeymap(const std::string& keymap);
        void setNetwork(const std::string& network);
        void setError(const std::string& error);
        bool onMouseEvent(int mouseX, int mouseY, int button, bool pressed);
        bool onKeyEvent(int key, bool pressed);
        const std::string& getLanguage() const;
        const std::string& getKeymap() const;
        const std::string& getNetwork() const;
        bool isSubmitted() const;

        private:
        std::string m_language;
        std::string m_keymap;
        std::string m_network;
        bool m_submitted = false;
        int m_focusField = 0;
    };
}
}