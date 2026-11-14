#include "App.h"
#include "Win32Window.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <iterator>
#include <stdexcept>

namespace
{
    constexpr float kMaxDeltaTime = 0.1f;
    const sf::Color kTransparentColor(255, 0, 255);

    constexpr float kDragThreshold     = 4.f;
    constexpr float kVelocitySmoothing = 0.5f;

    // 선택할 수 있는 캐릭터 목록 {메뉴 이름, 시트 파일}
    struct Character
    {
        const wchar_t* name;
        const char* file;
    };
    constexpr Character kCharacters[] = {
        {L"고양이", "kitten_sheet.png"},
        {L"햄스터", "mochi_hamster_sheet.png"},
    };
    constexpr std::size_t kCharacterCount = std::size(kCharacters);

    // 메뉴 명령 번호 (0은 "선택 안 함"이라 사용하지 않음)
    enum MenuCommand : int
    {
        kCmdSleep = 1,
        kCmdWake,
        kCmdQuit,
        kCmdCharacterBase = 100,   // 100 + 캐릭터 번호
    };

    std::filesystem::path sheetPath(std::size_t index)
    {
        return win32::getExecutableDir() / "assets" / kCharacters[index].file;
    }

    sf::Texture loadSheet(const std::filesystem::path& path)
    {
        sf::Texture texture;
        if (!texture.loadFromFile(path))
            throw std::runtime_error("Failed to load: " + path.string());
        return texture;
    }

    sf::Vector2f getMouseScreenPosition()
    {
        return sf::Vector2f(sf::Mouse::getPosition());
    }
}

App::App()
    : m_window(sf::VideoMode(Pet::kWindowSize), "DeskPal", sf::Style::None)
    , m_workArea(win32::getWorkArea())
    , m_lastWindowPos({-1, -1})
    , m_sheet(loadSheet(sheetPath(m_characterIndex)))
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
        else if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (mouse->button == sf::Mouse::Button::Left)
                beginPress();
            else if (mouse->button == sf::Mouse::Button::Right && !m_isPressing)
                openContextMenu();
        }
    }
}

// ───────────────────────── 우클릭 메뉴 ─────────────────────────

void App::openContextMenu()
{
    using win32::MenuItem;

    // 하위 메뉴: 캐릭터 목록 (현재 캐릭터에 ✓)
    std::vector<MenuItem> characterMenu;
    for (std::size_t i = 0; i < kCharacterCount; ++i)
    {
        MenuItem item;
        item.id = kCmdCharacterBase + static_cast<int>(i);
        item.text = kCharacters[i].name;
        item.checked = (i == m_characterIndex);
        characterMenu.push_back(item);
    }

    // 전체 메뉴: {id, 글자, 선택 가능 여부, ✓ 여부, 하위 메뉴}
    const std::vector<MenuItem> menu = {
        {kCmdSleep, L"재우기", m_pet.isOnGround() && !m_pet.isSleeping()},
        {kCmdWake,  L"깨우기", m_pet.isSleeping()},
        {},                                              // 구분선
        {0, L"캐릭터 바꾸기", true, false, characterMenu},
        {},                                              // 구분선
        {kCmdQuit,  L"종료"},
    };

    const int command = win32::showPopupMenu(m_window, menu);

    if (command == kCmdSleep)
        m_pet.sleep();
    else if (command == kCmdWake)
        m_pet.wakeUp();
    else if (command == kCmdQuit)
        m_window.close();
    else if (command >= kCmdCharacterBase &&
             command < kCmdCharacterBase + static_cast<int>(kCharacterCount))
        loadCharacter(static_cast<std::size_t>(command - kCmdCharacterBase));

    // 메뉴가 떠 있던 시간만큼 dt가 튀지 않도록 시계 초기화
    m_clock.restart();
}

bool App::loadCharacter(std::size_t index)
{
    if (index == m_characterIndex)
        return true;

    try
    {
        // 같은 m_sheet 자리에 새 텍스처를 덮어씀 → Pet의 스프라이트가 자동으로 새 그림 사용
        m_sheet = loadSheet(sheetPath(index));
        m_characterIndex = index;
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "[Error] " << e.what() << '\n';   // 실패 시 기존 캐릭터 유지
        return false;
    }
}

// ───────────────────────── 마우스 드래그 ─────────────────────────

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

// ───────────────────────── 루프 ─────────────────────────

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