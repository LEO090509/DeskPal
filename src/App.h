#pragma once
#include <SFML/Graphics.hpp>
#include <cstddef>
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

    // --- 우클릭 메뉴 ---
    void openContextMenu();
    bool loadCharacter(std::size_t index);   // 실패하면 false (기존 캐릭터 유지)

    sf::RenderWindow m_window;
    sf::Clock m_clock;
    sf::FloatRect m_workArea;
    sf::Vector2i m_lastWindowPos;
    std::size_t m_characterIndex = 0;   // 현재 캐릭터 번호 (m_sheet보다 먼저!)
    sf::Texture m_sheet;                // 현재 캐릭터 시트 (m_pet보다 먼저!)
    Pet m_pet;

    bool m_isPressing = false;
    bool m_isDragging = false;
    sf::Vector2f m_pressMousePos;
    sf::Vector2f m_grabOffset;
    sf::Vector2f m_prevMousePos;
    sf::Vector2f m_mouseVelocity;
};