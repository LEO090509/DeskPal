#pragma once
#include <SFML/Graphics.hpp>
#include <random>
#include "Animator.h"

// 데스크톱 펫 한 마리 (지금은 아기 고양이)
class Pet
{
public:
    static constexpr sf::Vector2u kWindowSize{64u, 64u};

    // sheet: 캐릭터 스프라이트 시트 (App이 소유, Pet은 빌려 씀)
    Pet(sf::Vector2f startFeetPosition, const sf::Texture& sheet);

    void update(float dt, const sf::FloatRect& area);
    void render(sf::RenderTarget& target) const;

    // --- 사용자 상호작용 ---
    void grab();
    void moveHeld(sf::Vector2f feetPosition);
    void release(sf::Vector2f throwVelocity);
    void poke();

    sf::Vector2f getFeetPosition() const;
    bool isHeld() const;

private:
    enum class State { Idle, Walk, Sit, Sleep, Fall, Held };

    void changeState(State next);
    bool isOnGround() const;

    void updateIdle(float dt);
    void updateWalk(float dt, const sf::FloatRect& area);
    void updateSit(float dt);
    void updateSleep(float dt);
    void updateFall(float dt, const sf::FloatRect& area);

    // 상태 → 애니메이션 연결
    static const Animation& animationFor(State state);
    void updateSprite();

    float randomRange(float min, float max);
    bool chance(float probability);
    static const char* toString(State state);

    State m_state;
    float m_stateTimer;

    sf::Vector2f m_position;   // 발 위치 (화면 좌표)
    sf::Vector2f m_velocity;
    float m_facing;            // +1 오른쪽, -1 왼쪽

    sf::Sprite m_sprite;       // 화면에 그릴 그림
    Animator m_animator;       // 시트의 어느 칸을 보여줄지

    std::mt19937 m_rng;
};