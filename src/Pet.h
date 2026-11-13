#pragma once
#include <SFML/Graphics.hpp>

// 모찌 햄스터 한 마리를 표현하는 클래스
class Pet
{
public:
    // 펫 창의 크기 (펫 그림이 들어가는 칸)
    static constexpr sf::Vector2u kWindowSize{64u, 64u};

    explicit Pet(sf::Vector2f startFeetPosition);

    // area: 펫이 돌아다닐 수 있는 화면 영역 (작업 영역)
    void update(float dt, const sf::FloatRect& area);
    void render(sf::RenderTarget& target) const;

    // 발 위치 (화면 좌표, 창의 아래쪽 가운데)
    sf::Vector2f getFeetPosition() const;

private:
    sf::Vector2f m_position;   // 발 위치 (화면 좌표)
    sf::Vector2f m_velocity;   // 속도 (px/초)
    bool m_onGround;           // 바닥에 서 있는가

    float m_radius;
    sf::CircleShape m_shape;   // 임시 외형 (STEP 7에서 스프라이트로 교체)
};