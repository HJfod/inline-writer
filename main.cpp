#include <iostream>
#include <string>
#include <Windows.h>
#include <thread>
#include <fstream>
#include "SDL/include/SDL.h"
#include "SDL/include/SDL_syswm.h"
#include "main.h"
#include "style.h"
#undef main

// all my love to this so post
// https://stackoverflow.com/questions/23048993/sdl-fullscreen-translucent-background/51956224#51956224

constexpr unsigned int $H(const char* str, int h = 0) {
    return !str[h] ? 5381 : ($H(str, h+1) * 33) ^ str[h];
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

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Rect box;
    box.w = 80;
    box.h = 70;
    box.x = 40;
    box.y = 20;

    SDL_DisplayMode DM;
    if (SDL_GetDesktopDisplayMode(0, &DM) != 0) {
        SDL_Log("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, DM.w, DM.h,
        SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP);
    renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);

    SDL_SetRenderDrawColor(renderer,
        style::colors::chroma_key.R, style::colors::chroma_key.G, style::colors::chroma_key.B, 255);
    SDL_RenderClear(renderer);

    //SDL_SetWindowOpacity(window, .5);
    MakeWindowTransparent(window, style::colors::chroma_key_ref);
    SDL_RenderPresent(renderer);

    SDL_Event event;

    hooks::KeyHook = SetWindowsHookEx(WH_KEYBOARD_LL, hooks::KeyboardProc, NULL, 0);

    while (!app::quit) {
        if (wind::update) {
            SDL_SetRenderDrawColor(renderer,
                style::colors::chroma_key.R, style::colors::chroma_key.G, style::colors::chroma_key.B, 255);
            SDL_RenderClear(renderer);
            if (wind::corrector::visible) {
                SDL_SetRenderDrawColor(renderer,
                    style::colors::main_bg.R, style::colors::main_bg.G, style::colors::main_bg.B, 255);
                SDL_RenderFillRect(renderer, &box);
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

    UnhookWindowsHookEx(hooks::KeyHook);

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    SDL_Quit();

    return 0;
}