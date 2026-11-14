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

    void beginPress();
    void updateMouse(float dt);

    sf::RenderWindow m_window;
    sf::Clock m_clock;
    sf::FloatRect m_workArea;
    sf::Vector2i m_lastWindowPos;
    sf::Texture m_sheet;            // 캐릭터 스프라이트 시트 (Pet보다 먼저 선언!)
    Pet m_pet;

    bool m_isPressing = false;
    bool m_isDragging = false;
    sf::Vector2f m_pressMousePos;
    sf::Vector2f m_grabOffset;
    sf::Vector2f m_prevMousePos;
    sf::Vector2f m_mouseVelocity;
};