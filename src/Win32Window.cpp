#include "Win32Window.h"

// windows.h의 불필요한 부분과 min/max 매크로 충돌을 막는 설정
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

namespace win32
{
    void makeDesktopPetWindow(sf::RenderWindow& window, sf::Color colorKey)
    {
        // SFML 창의 Windows 핸들(HWND) 얻기
        HWND hwnd = window.getNativeHandle();

        // 확장 스타일 변경
        LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
        exStyle |= WS_EX_LAYERED;       // 투명 처리가 가능한 창
        exStyle |= WS_EX_TOOLWINDOW;    // 작업표시줄, Alt+Tab에서 숨김
        exStyle &= ~WS_EX_APPWINDOW;    // 작업표시줄 강제 표시 해제

        // 작업표시줄 반영을 위해 잠깐 숨겼다가 다시 표시
        ShowWindow(hwnd, SW_HIDE);
        SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle);

        // colorKey 색을 완전 투명으로 지정
        SetLayeredWindowAttributes(hwnd, RGB(colorKey.r, colorKey.g, colorKey.b), 0, LWA_COLORKEY);

        // 항상 위에 표시 (위치/크기는 그대로)
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);

        ShowWindow(hwnd, SW_SHOW);
    }
}