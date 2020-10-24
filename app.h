#include <string>

namespace settings_paths {
    std::string folder = "settings/";
    std::string settings = folder + "settings.txt";
    std::string emoji = folder + "emoji.txt";
}

namespace app {
    bool quit = false;
    std::string key_collector = "";
    namespace settings {
        std::string emoji_prefix = "";
        char emoji_prefix_char;
    }
}

namespace wind {
    bool update = true;
    namespace corrector {
        bool visible = true;
        void SetVisible(bool v = false) {
            visible = v;
            update = true;
        };
    }
}
