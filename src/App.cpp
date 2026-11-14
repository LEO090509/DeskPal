#include "App.h"
#include "Win32Window.h"
#include <algorithm>
#include <cmath>

namespace
{
    constexpr float kMaxDeltaTime = 0.1f;
    const sf::Color kTransparentColor(255, 0, 255);

    constexpr float kDragThreshold     = 4.f;   // 이만큼(px) 움직여야 드래그로 판정
    constexpr float kVelocitySmoothing = 0.5f;  // 던지기 속도를 부드럽게 (0~1)

    // 마우스 위치 (화면 좌표)
    sf::Vector2f getMouseScreenPosition()
    {
        return sf::Vector2f(sf::Mouse::getPosition());
    }
}

App::App()
    : m_window(sf::VideoMode(Pet::kWindowSize), "DeskPal", sf::Style::None)
    , m_workArea(win32::getWorkArea())
    , m_lastWindowPos({-1, -1})
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
            if (mouse->button == sf::Mouse::Button::Left)
                beginPress();
            else if (mouse->button == sf::Mouse::Button::Right)
                m_window.close();   // 임시 종료 (STEP 8에서 메뉴로 교체)
        }
    }
}

void App::beginPress()
{
    const sf::Vector2f mouse = getMouseScreenPosition();

    m_isPressing = true;
    m_isDragging = false;
    m_pressMousePos = mouse;
    m_prevMousePos = mouse;
    m_grabOffset = m_pet.getFeetPosition() - mouse;   // 잡은 지점 유지용
    m_mouseVelocity = {0.f, 0.f};
}

void App::updateMouse(float dt)
{
    if (!m_isPressing)
        return;

    const sf::Vector2f mouse = getMouseScreenPosition();

    // 1. 일정 거리 이상 움직이면 드래그 시작
    if (!m_isDragging && (mouse - m_pressMousePos).length() > kDragThreshold)
    {
        m_isDragging = true;
        m_pet.grab();
    }

    // 2. 드래그 중: 펫을 마우스에 붙이고, 마우스 속도 측정
    if (m_isDragging)
    {
        m_pet.moveHeld(mouse + m_grabOffset);

        if (dt > 0.f)
        {
            const sf::Vector2f instant = (mouse - m_prevMousePos) / dt;
            m_mouseVelocity += (instant - m_mouseVelocity) * kVelocitySmoothing;
        }
    }
    m_prevMousePos = mouse;

    // 3. 버튼을 뗐는지 확인 (창 밖에서 떼도 감지되도록 실시간 상태로 확인)
    if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    {
        if (m_isDragging)
            m_pet.release(m_mouseVelocity);   // 던지기
        else
            m_pet.poke();                     // 그냥 클릭 → 폴짝

        m_isPressing = false;
        m_isDragging = false;
    }
}

void App::update(float dt)
{
    updateMouse(dt);
    m_pet.update(dt, m_workArea);
    syncWindowToPet();
}

void App::syncWindowToPet()
{
    const sf::Vector2f feet = m_pet.getFeetPosition();
    const sf::Vector2i newPos(
        static_cast<int>(std::round(feet.x - Pet::kWindowSize.x / 2.f)),
        static_cast<int>(std::round(feet.y - Pet::kWindowSize.y)));

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