#include "App.h"
#include "Win32Window.h"
#include <algorithm>
#include <cmath>

namespace
{
    constexpr float kMaxDeltaTime = 0.1f;
    const sf::Color kTransparentColor(255, 0, 255);

    constexpr float kDragThreshold     = 4.f;
    constexpr float kVelocitySmoothing = 0.5f;

    // 캐릭터 시트 파일 (햄스터로 바꾸려면 "mochi_hamster_sheet.png")
    constexpr const char* kCharacterSheet = "kitten_sheet.png";

    sf::Vector2f getMouseScreenPosition()
    {
        return sf::Vector2f(sf::Mouse::getPosition());
    }
}

App::App()
    : m_window(sf::VideoMode(Pet::kWindowSize), "DeskPal", sf::Style::None)
    , m_workArea(win32::getWorkArea())
    , m_lastWindowPos({-1, -1})
    // exe 옆 assets 폴더에서 시트 불러오기 (실패하면 예외 발생 → main에서 처리)
    , m_sheet(win32::getExecutableDir() / "assets" / kCharacterSheet)
    , m_pet({m_workArea.position.x + m_workArea.size.x / 2.f,
             m_workArea.position.y + static_cast<float>(Pet::kWindowSize.y)},
            m_sheet)
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
                m_window.close();
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
    m_grabOffset = m_pet.getFeetPosition() - mouse;
    m_mouseVelocity = {0.f, 0.f};
}

void App::updateMouse(float dt)
{
    if (!m_isPressing)
        return;

    const sf::Vector2f mouse = getMouseScreenPosition();

    if (!m_isDragging && (mouse - m_pressMousePos).length() > kDragThreshold)
    {
        m_isDragging = true;
        m_pet.grab();
    }

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

    if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    {
        if (m_isDragging)
            m_pet.release(m_mouseVelocity);
        else
            m_pet.poke();

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