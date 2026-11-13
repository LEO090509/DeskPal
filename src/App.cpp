#include "App.h"
#include "Win32Window.h"
#include <algorithm>

namespace
{
    constexpr unsigned int kWindowWidth  = 400;
    constexpr unsigned int kWindowHeight = 300;
    constexpr float kMaxDeltaTime = 0.1f;

    // 이 색으로 칠해진 부분은 투명해진다 (펫 그림에는 이 색을 쓰면 안 됨!)
    const sf::Color kTransparentColor(255, 0, 255);
}

App::App()
    : m_window(sf::VideoMode({kWindowWidth, kWindowHeight}), "DeskPal", sf::Style::None)
    , m_pet({200.f, 150.f})
{
    m_window.setFramerateLimit(60);

    // 화면 가운데에 배치
    const sf::Vector2u desktop = sf::VideoMode::getDesktopMode().size;
    m_window.setPosition({
        static_cast<int>(desktop.x - kWindowWidth) / 2,
        static_cast<int>(desktop.y - kWindowHeight) / 2
    });

    win32::makeDesktopPetWindow(m_window, kTransparentColor);
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
        // 닫기 버튼이 없으므로 임시 종료 방법 ①: ESC 키
        else if (const auto* key = event->getIf<sf::Event::KeyPressed>())
        {
            if (key->code == sf::Keyboard::Key::Escape)
                m_window.close();
        }
        // 임시 종료 방법 ②: 펫을 우클릭 (STEP 8에서 우클릭 메뉴로 교체)
        else if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (mouse->button == sf::Mouse::Button::Right)
                m_window.close();
        }
    }
}

void App::update(float dt)
{
    const sf::Vector2f bounds(m_window.getSize());
    m_pet.update(dt, bounds);
}

void App::render()
{
    m_window.clear(kTransparentColor);   // 배경 = 투명색
    m_pet.render(m_window);
    m_window.display();
}