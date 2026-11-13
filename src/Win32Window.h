#pragma once
#include <SFML/Graphics.hpp>

// Windows 전용 창 기능 모음
// (Win32 API는 이 파일들 안에서만 사용한다)
namespace win32
{
    // 일반 SFML 창을 데스크톱 펫용 창으로 바꾼다.
    //  - colorKey 색으로 칠해진 픽셀은 투명 + 클릭 통과
    //  - 항상 다른 창 위에 표시
    //  - 작업표시줄과 Alt+Tab 목록에서 숨김
    void makeDesktopPetWindow(sf::RenderWindow& window, sf::Color colorKey);
    // 주 모니터의 작업 영역 (화면에서 작업표시줄을 뺀 영역, 화면 좌표)
    sf::FloatRect getWorkArea();
}