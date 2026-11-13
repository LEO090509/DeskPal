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

    // 펫의 발 위치에 맞춰 창을 이동
    void syncWindowToPet();

    sf::RenderWindow m_window;
    sf::Clock m_clock;
    sf::FloatRect m_workArea;     // 돌아다닐 수 있는 화면 영역
    sf::Vector2i m_lastWindowPos; // 마지막으로 옮긴 창 위치
    Pet m_pet;
};