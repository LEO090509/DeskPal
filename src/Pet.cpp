#include "Pet.h"
#include <algorithm>
#include <cmath>

namespace
{
    constexpr float kGravity      = 1500.f;   // 중력 가속도 (px/초²)
    constexpr float kMaxFallSpeed = 1500.f;   // 최대 낙하 속도 (px/초)
    constexpr float kWalkSpeed    = 60.f;     // 걷는 속도 (px/초)
}

Pet::Pet(sf::Vector2f startFeetPosition)
    : m_position(startFeetPosition)
    , m_velocity({0.f, 0.f})
    , m_onGround(false)
    , m_radius(30.f)
    , m_shape(m_radius)
{
    m_shape.setFillColor(sf::Color(255, 248, 240));
    m_shape.setOrigin({m_radius, m_radius});

    // 창 안에서의 위치: 가로 가운데, 바닥에 붙도록
    m_shape.setPosition({kWindowSize.x / 2.f, kWindowSize.y - m_radius});
}

void Pet::update(float dt, const sf::FloatRect& area)
{
    // 펫이 갈 수 있는 경계 (발 위치 기준)
    const float halfWidth = kWindowSize.x / 2.f;
    const float leftLimit  = area.position.x + halfWidth;
    const float rightLimit = area.position.x + area.size.x - halfWidth;
    const float floorY     = area.position.y + area.size.y;

    // 1. 중력: 공중에 있으면 아래쪽 속도 증가
    if (!m_onGround)
    {
        m_velocity.y = std::min(m_velocity.y + kGravity * dt, kMaxFallSpeed);
    }

    // 2. 이동
    m_position += m_velocity * dt;

    // 3. 바닥 충돌: 착지하면 멈추고 걷기 시작
    if (m_position.y >= floorY)
    {
        m_position.y = floorY;
        m_velocity.y = 0.f;

        if (!m_onGround)
        {
            m_onGround = true;
            m_velocity.x = kWalkSpeed;   // 착지 후 오른쪽으로 걷기
        }
    }

    // 4. 좌우 화면 끝: 방향 전환
    if (m_position.x < leftLimit)
    {
        m_position.x = leftLimit;
        m_velocity.x = std::abs(m_velocity.x);
    }
    else if (m_position.x > rightLimit)
    {
        m_position.x = rightLimit;
        m_velocity.x = -std::abs(m_velocity.x);
    }
}

void Pet::render(sf::RenderTarget& target) const
{
    target.draw(m_shape);
}

sf::Vector2f Pet::getFeetPosition() const
{
    return m_position;
}