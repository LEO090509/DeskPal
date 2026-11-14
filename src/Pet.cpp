#include "Pet.h"
#include <algorithm>
#include <cmath>

namespace
{
    constexpr float kGravity        = 1200.f;  // 중력 가속도 (px/초²)
    constexpr float kMaxFallSpeed   = 1500.f;  // 최대 낙하 속도
    constexpr float kWalkSpeed      = 60.f;    // 걷는 속도
    constexpr float kJumpSpeed      = 450.f;   // 점프 순간 위쪽 속도
    constexpr float kMaxThrowSpeed  = 2500.f;  // 던지기 최대 속도
    constexpr float kAirDrag        = 0.7f;    // 공기 저항 (클수록 가로 속도가 빨리 줄어듦)
    constexpr float kWallBounce     = 0.5f;    // 벽에 튕길 때 남는 속도 비율
    constexpr float kFloorBounce    = 0.45f;   // 바닥에 튕길 때 남는 속도 비율
    constexpr float kMinBounceSpeed = 400.f;   // 이보다 빠르게 떨어져야 바닥에서 튕김

    // 발 위치 기준으로 펫이 갈 수 있는 경계
    struct Limits
    {
        float left, right, top, floor;
    };

    Limits computeLimits(const sf::FloatRect& area)
    {
        const float halfWidth = Pet::kWindowSize.x / 2.f;
        return {
            area.position.x + halfWidth,                               // left
            area.position.x + area.size.x - halfWidth,                 // right
            area.position.y + static_cast<float>(Pet::kWindowSize.y),  // top (머리가 화면 위에 닿는 지점)
            area.position.y + area.size.y                              // floor
        };
    }
}

Pet::Pet(sf::Vector2f startFeetPosition)
    : m_mode(Mode::Air)
    , m_position(startFeetPosition)
    , m_velocity({0.f, 0.f})
    , m_facing(1.f)
    , m_radius(30.f)
    , m_shape(m_radius)
{
    m_shape.setFillColor(sf::Color(255, 248, 240));
    m_shape.setOrigin({m_radius, m_radius});
    m_shape.setPosition({kWindowSize.x / 2.f, kWindowSize.y - m_radius});
}

void Pet::update(float dt, const sf::FloatRect& area)
{
    switch (m_mode)
    {
    case Mode::Ground: updateGround(dt, area); break;
    case Mode::Air:    updateAir(dt, area);    break;
    case Mode::Held:   break;   // 들려 있는 동안은 물리 계산 안 함
    }
}

void Pet::updateGround(float dt, const sf::FloatRect& area)
{
    const Limits lim = computeLimits(area);

    m_position.x += m_facing * kWalkSpeed * dt;
    m_position.y = lim.floor;

    // 화면 끝에 닿으면 뒤돌기
    if (m_position.x < lim.left)
    {
        m_position.x = lim.left;
        m_facing = 1.f;
    }
    else if (m_position.x > lim.right)
    {
        m_position.x = lim.right;
        m_facing = -1.f;
    }
}

void Pet::updateAir(float dt, const sf::FloatRect& area)
{
    const Limits lim = computeLimits(area);

    // 1. 중력 + 공기 저항
    m_velocity.y = std::min(m_velocity.y + kGravity * dt, kMaxFallSpeed);
    m_velocity.x -= m_velocity.x * kAirDrag * dt;

    // 2. 이동
    m_position += m_velocity * dt;

    // 3. 좌우 벽: 속도를 줄이며 튕기기
    if (m_position.x < lim.left)
    {
        m_position.x = lim.left;
        m_velocity.x = std::abs(m_velocity.x) * kWallBounce;
    }
    else if (m_position.x > lim.right)
    {
        m_position.x = lim.right;
        m_velocity.x = -std::abs(m_velocity.x) * kWallBounce;
    }

    // 4. 천장(화면 위쪽 끝)
    if (m_position.y < lim.top)
    {
        m_position.y = lim.top;
        m_velocity.y = std::abs(m_velocity.y) * kWallBounce;
    }

    // 5. 바닥: 빠르면 통 튕기고, 느리면 착지
    if (m_position.y >= lim.floor && m_velocity.y >= 0.f)
    {
        m_position.y = lim.floor;

        if (m_velocity.y > kMinBounceSpeed)
        {
            m_velocity.y = -m_velocity.y * kFloorBounce;
        }
        else
        {
            m_mode = Mode::Ground;
            m_velocity = {0.f, 0.f};
        }
    }

    // 날아가는 방향을 바라보게 (착지 후 그 방향으로 걸어감)
    if (std::abs(m_velocity.x) > 1.f)
        m_facing = (m_velocity.x > 0.f) ? 1.f : -1.f;
}

void Pet::grab()
{
    m_mode = Mode::Held;
    m_velocity = {0.f, 0.f};
}

void Pet::moveHeld(sf::Vector2f feetPosition)
{
    m_position = feetPosition;
}

void Pet::release(sf::Vector2f throwVelocity)
{
    // 너무 빠르면 최대 속도로 제한
    if (throwVelocity.length() > kMaxThrowSpeed)
        throwVelocity = throwVelocity.normalized() * kMaxThrowSpeed;

    m_velocity = throwVelocity;
    m_mode = Mode::Air;
}

void Pet::jump()
{
    if (m_mode != Mode::Ground)
        return;

    m_velocity = {m_facing * kWalkSpeed, -kJumpSpeed};   // 위쪽은 y가 음수
    m_mode = Mode::Air;
}

void Pet::render(sf::RenderTarget& target) const
{
    target.draw(m_shape);
}

sf::Vector2f Pet::getFeetPosition() const
{
    return m_position;
}

bool Pet::isHeld() const
{
    return m_mode == Mode::Held;
}