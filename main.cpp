#include <iostream>
#include <string>
#include <iterator>
#include <Windows.h>
#include <thread>
#include <fstream>
#include "SDL/include/SDL.h"
#include "SDL/include/SDL_syswm.h"
#include <sstream>
#include "headers/main.h"
#include "headers/style.h"
#undef main

// all my love to this so post
// https://stackoverflow.com/questions/23048993/sdl-fullscreen-translucent-background/51956224#51956224

constexpr unsigned int $H(const char* str, int h = 0) {
    return !str[h] ? 5381 : ($H(str, h+1) * 33) ^ str[h];
}

template<char delimiter> class WordDelimitedBy : public std::string {};

std::vector<std::string> split(std::string _str, char _split) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(_str);
    while (std::getline(tokenStream, token, _split))
        tokens.push_back(token);
    return tokens;
}

int ReplaceKeyStringWithKeyID(std::string _key) {
    int res = 0;
    switch ($H(_key.c_str())) {
        case $H("SPACE"):   res = VK_SPACE; break;
        case $H("CTRL"):    res = VK_CONTROL; break;
        case $H("SHIFT"):   res = VK_SHIFT; break;
    }
    return res;
}

void LoadSettings() {
    std::ifstream settings_file (settings_paths::settings);

    std::string line;
    if (settings_file.is_open()) {
        while ( getline (settings_file, line) ) {
            std::string val = line.substr(line.find(" ") + 1);

            switch ($H(line.substr(0, line.find(" ")).c_str())) {
                case $H("emoji_prefix"):
                    app::settings::emoji_prefix = val;
                    app::settings::emoji_prefix_char = val.at(0);
                    break;
                case $H("control_keys"):
                    std::vector<std::string> len = split(val, ' ');
                    for (int i = 0; i < len.size(); i++)
                        app::settings::control_keys.push_back(ReplaceKeyStringWithKeyID(len[i]));
                    break;
            }
        }
        settings_file.close();
    } else std::cout << "Error: Settings not found!" << std::endl;
}

bool MakeWindowTransparent(SDL_Window* window, COLORREF colorKey) {
    // Get window handle (https://stackoverflow.com/a/24118145/3357935)
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);  // Initialize wmInfo
    SDL_GetWindowWMInfo(window, &wmInfo);
    HWND hWnd = wmInfo.info.win.window;

    // Change window type to layered (https://stackoverflow.com/a/3970218/3357935)
    SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);

    // Set transparency color
    return SetLayeredWindowAttributes(hWnd, colorKey, 0, LWA_COLORKEY);
}

int main() {
    int SDL_Init(SDL_INIT_VIDEO);
    SDL_VideoInit(NULL);

    std::cout << "Bootup" << std::endl;

    LoadSettings();

    std::cout << "emoji_prefix: " << app::settings::emoji_prefix << ";" << std::endl;

    int display_count = SDL_GetNumVideoDisplays();

    std::vector<SDL_Rect> display_bounds;
    std::vector<SDL_Window*> windows;
    SDL_Renderer* renderer = NULL;

    for (int i = 0; i < display_count; i++) {
        display_bounds.push_back( SDL_Rect() );
        SDL_GetDisplayBounds( i, &display_bounds.back() );
        
        SDL_Window* window;
        windows.push_back(window);

        SDL_DisplayMode DM;
        if (SDL_GetDesktopDisplayMode(i, &DM) != 0) {
            SDL_Log("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
            return 1;
        }

        window = SDL_CreateWindow("", display_bounds[i].x, display_bounds[i].y, DM.w, DM.h,
            SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP);
        renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);

        SDL_SetRenderDrawColor(renderer,
            style::colors::chroma_key.R, style::colors::chroma_key.G, style::colors::chroma_key.B, 255);
        SDL_RenderClear(renderer);

        SDL_SetWindowOpacity(window, .5);
        //MakeWindowTransparent(window, style::colors::chroma_key_ref);
        SDL_RenderPresent(renderer);
    }

    SDL_Event event;

    hooks::KeyHook = SetWindowsHookEx(WH_KEYBOARD_LL, hooks::KeyboardProc, NULL, 0);
    hooks::MouseHook = SetWindowsHookEx(WH_MOUSE_LL, hooks::MouseProc, NULL, 0);

    while (!app::quit) {
        if (wind::update) {
            SDL_SetRenderDrawColor(renderer,
                style::colors::chroma_key.R, style::colors::chroma_key.G, style::colors::chroma_key.B, 255);
            SDL_RenderClear(renderer);
            if (wind::corrector::visible) {
                SDL_SetRenderDrawColor(renderer,
                    style::colors::main_bg.R, style::colors::main_bg.G, style::colors::main_bg.B, 255);

                SDL_RenderFillRect(renderer, &wind::corrector::box);
            }
            wind::update = false;
            SDL_RenderPresent(renderer);
        }

        if (SDL_PollEvent(&event))
            if (event.type == SDL_TEXTINPUT)
                std::cout << event.text.text;
            if (event.type == SDL_KEYDOWN)
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        app::quit = true;
                        break;
                }
    };

    UnhookWindowsHookEx(hooks::MouseHook);
    UnhookWindowsHookEx(hooks::KeyHook);

    for (int i = 0; i < display_count; i++)
        SDL_DestroyWindow(windows[i]);
    SDL_DestroyRenderer(renderer);

    SDL_Quit();

    return 0;
}