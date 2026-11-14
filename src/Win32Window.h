#pragma once
#include <SFML/Graphics.hpp>
#include <filesystem>
#include <string>
#include <vector>

// Windows 전용 기능 모음 (Win32 API는 Win32Window.cpp 안에서만 사용)
namespace win32
{
    // 팝업 메뉴 항목 하나
    //  - text가 비어 있고 children도 없으면 → 구분선
    //  - children이 있으면 → 하위 메뉴 (▶)
    struct MenuItem
    {
        int id = 0;                        // 선택했을 때 돌려받을 번호 (1 이상)
        std::wstring text;                 // 표시할 글자 (한글 OK)
        bool enabled = true;               // false면 회색 (선택 불가)
        bool checked = false;              // true면 ✓ 표시
        std::vector<MenuItem> children;    // 하위 메뉴
    };

    void makeDesktopPetWindow(sf::RenderWindow& window, sf::Color colorKey);
    sf::FloatRect getWorkArea();
    std::filesystem::path getExecutableDir();

    // 마우스 위치에 팝업 메뉴를 띄우고, 선택된 항목의 id를 반환 (취소하면 0)
    // ※ 메뉴가 떠 있는 동안은 이 함수가 반환되지 않음 (게임 루프 일시정지)
    int showPopupMenu(sf::RenderWindow& window, const std::vector<MenuItem>& items);
}