#pragma once
#include <SFML/Graphics.hpp>
#include "Pet.h"

class App
{
public:
    App();
    void run();

private:
    void processEvents();
    void update(float dt);
    void render();

    void syncWindowToPet();

    // --- 마우스 상호작용 ---
    void beginPress();               // 펫 위에서 왼쪽 버튼을 누름
    void updateMouse(float dt);      // 누르고 있는 동안 매 프레임 처리

    sf::RenderWindow m_window;
    sf::Clock m_clock;
    sf::FloatRect m_workArea;
    sf::Vector2i m_lastWindowPos;
    Pet m_pet;

    // --- 마우스 드래그 상태 ---
    bool m_isPressing = false;       // 펫을 누르고 있는가
    bool m_isDragging = false;       // 일정 거리 이상 끌었는가 (클릭과 구분)
    sf::Vector2f m_pressMousePos;    // 누른 순간의 마우스 위치 (화면 좌표)
    sf::Vector2f m_grabOffset;       // 마우스 → 펫 발 위치까지의 차이
    sf::Vector2f m_prevMousePos;     // 지난 프레임의 마우스 위치
    sf::Vector2f m_mouseVelocity;    // 마우스 이동 속도 (던지기용)
};