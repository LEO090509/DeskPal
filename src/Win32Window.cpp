#include "Win32Window.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

namespace
{
    // MenuItem 목록 → Windows 메뉴(HMENU)로 변환 (하위 메뉴는 재귀로)
    HMENU buildMenu(const std::vector<win32::MenuItem>& items)
    {
        HMENU menu = CreatePopupMenu();

        for (const auto& item : items)
        {
            if (!item.children.empty())
            {
                HMENU sub = buildMenu(item.children);
                AppendMenuW(menu, MF_POPUP | MF_STRING,
                            reinterpret_cast<UINT_PTR>(sub), item.text.c_str());
            }
            else if (item.text.empty())
            {
                AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
            }
            else
            {
                UINT flags = MF_STRING;
                if (!item.enabled) flags |= MF_GRAYED;
                if (item.checked)  flags |= MF_CHECKED;
                AppendMenuW(menu, flags, static_cast<UINT_PTR>(item.id), item.text.c_str());
            }
        }
        return menu;
    }
}

namespace win32
{
    void makeDesktopPetWindow(sf::RenderWindow& window, sf::Color colorKey)
    {
        HWND hwnd = window.getNativeHandle();

        LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
        exStyle |= WS_EX_LAYERED;       // 투명 처리
        exStyle |= WS_EX_TOOLWINDOW;    // 작업표시줄, Alt+Tab에서 숨김
        exStyle |= WS_EX_NOACTIVATE;    // ★ 클릭해도 포커스를 뺏지 않음 (STEP 8)
        exStyle &= ~WS_EX_APPWINDOW;

        ShowWindow(hwnd, SW_HIDE);
        SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle);

        SetLayeredWindowAttributes(hwnd, RGB(colorKey.r, colorKey.g, colorKey.b), 0, LWA_COLORKEY);

        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);

        ShowWindow(hwnd, SW_SHOWNOACTIVATE);   // 표시할 때도 포커스 안 가져감
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

    int showPopupMenu(sf::RenderWindow& window, const std::vector<MenuItem>& items)
    {
        HWND hwnd = window.getNativeHandle();
        HMENU menu = buildMenu(items);

        POINT cursor{};
        GetCursorPos(&cursor);

        // Windows 규칙: 이걸 해야 메뉴 바깥을 클릭했을 때 메뉴가 닫힘
        SetForegroundWindow(hwnd);

        const int selected = TrackPopupMenu(
            menu,
            TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_NONOTIFY,   // 선택한 id를 바로 반환
            cursor.x, cursor.y, 0, hwnd, nullptr);

        PostMessageW(hwnd, WM_NULL, 0, 0);   // 메뉴 정리용 (Windows 권장 사항)
        DestroyMenu(menu);                   // 하위 메뉴까지 함께 해제

        return selected;
    }

}   // namespace win32