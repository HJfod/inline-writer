#include <iostream>
#include <Windows.h>
#include "../SDL/include/SDL.h"
#include "app.h"

#define HANDLED 1

namespace hooks {
    HHOOK KeyHook;
    HHOOK MouseHook;

    LRESULT CALLBACK MouseProc(const int nCode, WPARAM wParam, LPARAM lParam) {
        switch (wParam) {
            case WM_RBUTTONDOWN:
                {
                    bool req = true;
                    for (int i = 0; i < app::settings::control_keys.size(); i++)
                        if (!(GetKeyState(app::settings::control_keys[i]) >> 15))
                            req = false;
                    if (req) {
                        POINT c;
                        GetCursorPos(&c);
                        
                        wind::corrector::box.x = c.x;
                        wind::corrector::box.y = c.y;
                        wind::corrector::SetVisible(true);

                        return HANDLED;
                    }
                }
                break;
        }
        return CallNextHookEx(MouseHook, nCode, wParam, lParam);
    }

    LRESULT CALLBACK KeyboardProc(const int nCode, WPARAM wParam, LPARAM lParam) {
        switch (wParam) {
            case WM_KEYDOWN:
                switch (((PKBDLLHOOKSTRUCT)lParam)->vkCode) {
                    case IW_DEBUG_KEY:
                        wind::corrector::SetVisible(!wind::corrector::visible);
                        break;
                    case VK_BACK:
                        wind::corrector::SetVisible(false);
                        break;
                    case IW_QUIT_KEY:
                        app::quit = true;
                        break;
                    case IW_OPEN_KEY:
                        {
                            bool req = true;
                            for (int i = 0; i < app::settings::control_keys.size(); i++)
                                if (!(GetKeyState(app::settings::control_keys[i]) >> 15))
                                    req = false;
                            if (req) {
                                POINT c = methods::GetCaretPosition();
                                
                                wind::corrector::box.x = c.x;
                                wind::corrector::box.y = c.y;
                                wind::corrector::SetVisible(true);

                                return HANDLED;
                            }
                        }
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