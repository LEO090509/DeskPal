#include "Pet.h"
#include <cmath>

Pet::Pet(sf::Vector2f startPosition)
    : m_position(startPosition)
    , m_velocity({120.f, 80.f})   // 오른쪽으로 초당 120px, 아래로 초당 80px
    , m_radius(30.f)
    , m_shape(m_radius)
{
    m_shape.setFillColor(sf::Color(255, 248, 240));   // 떡 같은 하얀색
    m_shape.setOrigin({m_radius, m_radius});          // 기준점을 원의 중심으로
    m_shape.setPosition(m_position);
}

void Pet::update(float dt, sf::Vector2f bounds)
{
    // 1. 이동: 위치 += 속도 × 시간
    m_position += m_velocity * dt;

    // 2. 벽 충돌 처리 (왼쪽/오른쪽)
    if (m_position.x - m_radius < 0.f)
    {
        m_position.x = m_radius;
        m_velocity.x = std::abs(m_velocity.x);    // 오른쪽으로
    }
    else if (m_position.x + m_radius > bounds.x)
    {
        m_position.x = bounds.x - m_radius;
        m_velocity.x = -std::abs(m_velocity.x);   // 왼쪽으로
    }

    // 3. 벽 충돌 처리 (위/아래)
    if (m_position.y - m_radius < 0.f)
    {
        m_position.y = m_radius;
        m_velocity.y = std::abs(m_velocity.y);    // 아래로
    }
    else if (m_position.y + m_radius > bounds.y)
    {
        m_position.y = bounds.y - m_radius;
        m_velocity.y = -std::abs(m_velocity.y);   // 위로
    }

    // 4. 계산된 위치를 외형에 반영
    m_shape.setPosition(m_position);
}

void Pet::render(sf::RenderTarget& target) const
{
    target.draw(m_shape);
}