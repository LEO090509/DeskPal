#include "App.h"
#include "Win32Window.h"
#include <algorithm>
#include <cmath>

namespace
{
    constexpr float kMaxDeltaTime = 0.1f;
    const sf::Color kTransparentColor(255, 0, 255);
}

App::App()
    : m_window(sf::VideoMode(Pet::kWindowSize), "DeskPal", sf::Style::None)
    , m_workArea(win32::getWorkArea())
    , m_lastWindowPos({-1, -1})
    // 시작 위치: 화면 위쪽 가운데 (여기서 떨어지기 시작)
    , m_pet({m_workArea.position.x + m_workArea.size.x / 2.f,
             m_workArea.position.y + static_cast<float>(Pet::kWindowSize.y)})
{
    m_window.setFramerateLimit(60);
    win32::makeDesktopPetWindow(m_window, kTransparentColor);
    syncWindowToPet();
}

void App::run()
{
    m_clock.restart();

    while (m_window.isOpen())
    {
        float dt = m_clock.restart().asSeconds();
        dt = std::min(dt, kMaxDeltaTime);

        processEvents();
        update(dt);
        render();
    }
}

void App::processEvents()
{
    while (const std::optional event = m_window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            m_window.close();
        }
        else if (const auto* key = event->getIf<sf::Event::KeyPressed>())
        {
            if (key->code == sf::Keyboard::Key::Escape)
                m_window.close();
        }
        else if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (mouse->button == sf::Mouse::Button::Right)
                m_window.close();
        }
    }
}

void App::update(float dt)
{
    m_pet.update(dt, m_workArea);
    syncWindowToPet();
}

void App::syncWindowToPet()
{
    // 발 위치(아래쪽 가운데) → 창의 왼쪽 위 좌표로 변환
    const sf::Vector2f feet = m_pet.getFeetPosition();
    const sf::Vector2i newPos(
        static_cast<int>(std::round(feet.x - Pet::kWindowSize.x / 2.f)),
        static_cast<int>(std::round(feet.y - Pet::kWindowSize.y)));

    // 위치가 바뀌었을 때만 창 이동 (불필요한 Windows 호출 줄이기)
    if (newPos != m_lastWindowPos)
    {
        m_window.setPosition(newPos);
        m_lastWindowPos = newPos;
    }
}

void App::render()
{
    m_window.clear(kTransparentColor);
    m_pet.render(m_window);
    m_window.display();
}