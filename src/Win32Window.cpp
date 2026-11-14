#include "Win32Window.h"

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
        HWND hwnd = window.getNativeHandle();

        LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
        exStyle |= WS_EX_LAYERED;
        exStyle |= WS_EX_TOOLWINDOW;
        exStyle &= ~WS_EX_APPWINDOW;

        ShowWindow(hwnd, SW_HIDE);
        SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle);

        SetLayeredWindowAttributes(hwnd, RGB(colorKey.r, colorKey.g, colorKey.b), 0, LWA_COLORKEY);

        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);

        ShowWindow(hwnd, SW_SHOW);
    }

    sf::FloatRect getWorkArea()
    {
        RECT rect{};
        SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);

        return sf::FloatRect(
            {static_cast<float>(rect.left), static_cast<float>(rect.top)},
            {static_cast<float>(rect.right - rect.left), static_cast<float>(rect.bottom - rect.top)});
    }

    std::filesystem::path getExecutableDir()
    {
        wchar_t buffer[MAX_PATH];
        const DWORD length = GetModuleFileNameW(nullptr, buffer, MAX_PATH);
        return std::filesystem::path(std::wstring(buffer, length)).parent_path();
    }

}   // namespace win32  ← 세 함수 모두 이 괄호 "안"에 있어야 함