#pragma once
#include <SFML/Graphics.hpp>
#include <random>
#include "Animator.h"

// 데스크톱 펫 한 마리
class Pet
{
public:
    static constexpr sf::Vector2u kWindowSize{64u, 64u};

    Pet(sf::Vector2f startFeetPosition, const sf::Texture& sheet);

    void update(float dt, const sf::FloatRect& area);
    void render(sf::RenderTarget& target) const;

    // --- 사용자 상호작용 ---
    void grab();
    void moveHeld(sf::Vector2f feetPosition);
    void release(sf::Vector2f throwVelocity);
    void poke();
    void sleep();     // 메뉴: 재우기 (바닥에 있을 때만)
    void wakeUp();    // 메뉴: 깨우기
    void setCanGlide(bool canGlide);   // 활강 가능한 캐릭터인가 (날다람쥐)

    sf::Vector2f getFeetPosition() const;
    bool isHeld() const;
    bool isSleeping() const;
    bool isOnGround() const;

private:
    enum class State { Idle, Walk, Sit, Sleep, Fall, Held };

    void changeState(State next);

    void updateIdle(float dt);
    void updateWalk(float dt, const sf::FloatRect& area);
    void updateSit(float dt);
    void updateSleep(float dt);
    void updateFall(float dt, const sf::FloatRect& area);

    static const Animation& animationFor(State state);
    void updateSprite();

    float randomRange(float min, float max);
    bool chance(float probability);
    static const char* toString(State state);

    State m_state;
    float m_stateTimer;

    sf::Vector2f m_position;
    sf::Vector2f m_velocity;
    float m_facing;

    sf::Sprite m_sprite;
    Animator m_animator;

    bool m_canGlide = false;
    std::mt19937 m_rng;
};