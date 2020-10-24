#include <string>
#include <vector>
#include <Windows.h>
#include "methods.h"

#define IW_QUIT_KEY     VK_ESCAPE
#define IW_OPEN_KEY     VK_SPACE
#define IW_DEBUG_KEY    VK_F9

namespace settings_paths {
    std::string folder = "settings/";
    std::string settings = folder + "settings.txt";
    std::string emoji = folder + "emoji.txt";
}

namespace app {
    bool quit = false;
    std::string key_collector = "";
    namespace settings {
        std::vector<int> control_keys;
        std::string emoji_prefix = "";
        char emoji_prefix_char;
    }
}

namespace wind {
    bool update = true;
    namespace corrector {
        POINT pos = { 0, 0 };
        SIZE size = { 100, 150 };
        SDL_Rect box = { pos.x, pos.y, size.cx, size.cy };
        int window = 0;
        bool visible = false;
        void SetVisible(bool v = false) {
            visible = v;
            update = true;
        };
    }
}
