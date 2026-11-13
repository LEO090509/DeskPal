#include "App.h"
#include <algorithm>

namespace
{
    constexpr unsigned int kWindowWidth  = 400;
    constexpr unsigned int kWindowHeight = 300;

    // 프레임 사이 시간이 이보다 길면 잘라냄 (아래 설명 참고)
    constexpr float kMaxDeltaTime = 0.1f;
}

App::App()
    : m_window(sf::VideoMode({kWindowWidth, kWindowHeight}), "DeskPal - STEP 2")
    , m_pet({200.f, 150.f})
{
    m_window.setFramerateLimit(60);
}

void App::run()
{
    m_clock.restart();

    while (m_window.isOpen())
    {
        // Delta Time: 지난 프레임부터 지금까지 흐른 시간(초)
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
            m_window.close();
    }
}

void App::update(float dt)
{
    const sf::Vector2f bounds(m_window.getSize());   // 창 크기 (unsigned → float 변환)
    m_pet.update(dt, bounds);
}

void App::render()
{
    m_window.clear(sf::Color(40, 44, 52));
    m_pet.render(m_window);
    m_window.display();
}