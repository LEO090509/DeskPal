#pragma once
#include <SFML/Graphics.hpp>

// 모찌 햄스터 한 마리를 표현하는 클래스
class Pet
{
public:
    explicit Pet(sf::Vector2f startPosition);

    // 상태 계산 (dt: 지난 프레임으로부터 흐른 시간, 초 단위)
    void update(float dt, sf::Vector2f bounds);

    // 화면에 그리기 (상태를 바꾸지 않으므로 const)
    void render(sf::RenderTarget& target) const;

private:
    sf::Vector2f m_position;   // 위치 (px)
    sf::Vector2f m_velocity;   // 속도 (px/초)
    float m_radius;            // 몸 반지름 (px)

    sf::CircleShape m_shape;   // 임시 외형 (STEP 7에서 스프라이트로 교체)
};