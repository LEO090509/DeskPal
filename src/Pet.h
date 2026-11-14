#pragma once
#include <SFML/Graphics.hpp>
#include <random>

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
    void poke();                                // 클릭: 자고 있으면 깨우기, 아니면 폴짝

    sf::Vector2f getFeetPosition() const;
    bool isHeld() const;

private:
    // 펫의 행동 상태
    enum class State { Idle, Walk, Sit, Sleep, Fall, Held };

    // 상태 전환 (모든 전환은 반드시 이 함수를 거친다)
    void changeState(State next);
    bool isOnGround() const;

    // 상태별 업데이트
    void updateIdle(float dt);
    void updateWalk(float dt, const sf::FloatRect& area);
    void updateSit(float dt);
    void updateSleep(float dt);
    void updateFall(float dt, const sf::FloatRect& area);

    // 상태에 맞게 겉모습 갱신 (STEP 7에서 애니메이션으로 교체)
    void updateAppearance();

    // 랜덤 도우미
    float randomRange(float min, float max);
    bool chance(float probability);   // probability 확률로 true

    static const char* toString(State state);

    State m_state;
    float m_stateTimer;        // 현재 상태의 남은 시간 (초)
    float m_animTime;          // 현재 상태에 들어온 뒤 흐른 시간 (흔들림 효과용)

    sf::Vector2f m_position;   // 발 위치 (화면 좌표)
    sf::Vector2f m_velocity;   // 공중에서의 속도 (px/초)
    float m_facing;            // +1 오른쪽, -1 왼쪽

    float m_radius;
    sf::CircleShape m_shape;   // 임시 외형

    std::mt19937 m_rng;        // 난수 생성기
};