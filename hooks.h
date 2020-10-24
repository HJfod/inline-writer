#include <iostream>
#include <Windows.h>
#include "app.h"

namespace hooks {
    HHOOK KeyHook;

    LRESULT CALLBACK KeyboardProc(const int nCode, WPARAM wParam, LPARAM lParam) {
        switch (wParam) {
            case WM_KEYDOWN:
                switch (((PKBDLLHOOKSTRUCT)lParam)->vkCode) {
                    case VK_F10:
                        wind::corrector::SetVisible(!wind::corrector::visible);
                        break;
                    case VK_BACK:
                        wind::corrector::SetVisible(false);
                        break;
                    case VK_ESCAPE:
                        app::quit = true;
                        break;
                    default:
                        SDL_Event e;
                        e.type = SDL_TEXTINPUT;
                        e.key.keysym.scancode = (SDL_Scancode)lParam;
                }
        }
        return CallNextHookEx(KeyHook, nCode, wParam, lParam);
    }
}