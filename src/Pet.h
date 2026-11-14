#pragma once
#include <SFML/Graphics.hpp>

// 모찌 햄스터 한 마리를 표현하는 클래스
class Pet
{
public:
    static constexpr sf::Vector2u kWindowSize{64u, 64u};

    explicit Pet(sf::Vector2f startFeetPosition);

    void update(float dt, const sf::FloatRect& area);
    void render(sf::RenderTarget& target) const;

    // --- 사용자 상호작용 ---
    void grab();                                // 집어 들기
    void moveHeld(sf::Vector2f feetPosition);   // 들고 있는 동안 위치 지정
    void release(sf::Vector2f throwVelocity);   // 놓기 (던지기)
    void jump();                                // 폴짝 (바닥에 있을 때만)

    sf::Vector2f getFeetPosition() const;
    bool isHeld() const;

private:
    // 지금 펫이 어떤 물리 상황에 있는가
    enum class Mode { Ground, Air, Held };

    void updateGround(float dt, const sf::FloatRect& area);
    void updateAir(float dt, const sf::FloatRect& area);

    Mode m_mode;
    sf::Vector2f m_position;   // 발 위치 (화면 좌표)
    sf::Vector2f m_velocity;   // 공중에서의 속도 (px/초)
    float m_facing;            // 바라보는 방향: +1 오른쪽, -1 왼쪽

    float m_radius;
    sf::CircleShape m_shape;   // 임시 외형 (STEP 7에서 스프라이트로 교체)
};