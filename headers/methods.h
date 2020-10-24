#include <iostream>
#include <Windows.h>

namespace methods {
    RECT GetCoordinates(HWND hWnd) {
        RECT rc;
        GetWindowRect(hWnd, &rc);
        return rc;
    }

    POINT GetCaretPosition() {
        GUITHREADINFO guiInfo;
        guiInfo.cbSize = sizeof(GUITHREADINFO);

        HWND wnd = GetForegroundWindow();
        DWORD dwThreadID = GetWindowThreadProcessId(wnd, NULL);
        GetGUIThreadInfo(dwThreadID, &guiInfo);

        if (GetGUIThreadInfo(dwThreadID, &guiInfo))
            if (guiInfo.hwndCaret)
                return { GetCoordinates(wnd).left + guiInfo.rcCaret.left, GetCoordinates(wnd).top + guiInfo.rcCaret.top };
        return { 0, 0 };
    }
}