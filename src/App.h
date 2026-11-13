#pragma once
#include <SFML/Graphics.hpp>
#include "Pet.h"

// 프로그램 전체를 조립하고 게임 루프를 돌리는 클래스
class App
{
public:
    App();
    void run();   // 게임 루프 시작 (창이 닫힐 때까지 반환되지 않음)

private:
    void processEvents();     // ① 입력 처리
    void update(float dt);    // ② 상태 계산
    void render();            // ③ 화면 그리기

    sf::RenderWindow m_window;
    sf::Clock m_clock;
    Pet m_pet;
};