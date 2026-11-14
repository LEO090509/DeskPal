#include "Pet.h"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace
{
    // --- 물리 ---
    constexpr float kGravity        = 1500.f;
    constexpr float kMaxFallSpeed   = 1500.f;
    constexpr float kWalkSpeed      = 60.f;
    constexpr float kJumpSpeed      = 450.f;
    constexpr float kMaxThrowSpeed  = 2500.f;
    constexpr float kAirDrag        = 0.8f;
    constexpr float kWallBounce     = 0.5f;
    constexpr float kFloorBounce    = 0.35f;
    constexpr float kMinBounceSpeed = 400.f;

    // --- 상태별 지속 시간 (초) ---
    struct Range { float min, max; };
    constexpr Range kIdleTime  {1.5f, 4.f};
    constexpr Range kWalkTime  {2.f, 6.f};
    constexpr Range kSitTime   {3.f, 8.f};
    constexpr Range kSleepTime {10.f, 20.f};

    // --- 겉모습 ---
    const sf::Color kBodyColor (255, 248, 240);   // 떡 같은 하얀색
    const sf::Color kSleepColor(225, 230, 245);   // 잘 때 살짝 푸르스름

    // --- 디버그: 상태 전환을 콘솔에 출력 ---
    constexpr bool kDebugLog = true;

    struct Limits
    {
        float left, right, top, floor;
    };

    Limits computeLimits(const sf::FloatRect& area)
    {
        const float halfWidth = Pet::kWindowSize.x / 2.f;
        return {
            area.position.x + halfWidth,
            area.position.x + area.size.x - halfWidth,
            area.position.y + static_cast<float>(Pet::kWindowSize.y),
            area.position.y + area.size.y
        };
    }
}

Pet::Pet(sf::Vector2f startFeetPosition)
    : m_state(State::Fall)
    , m_stateTimer(0.f)
    , m_animTime(0.f)
    , m_position(startFeetPosition)
    , m_velocity({0.f, 0.f})
    , m_facing(1.f)
    , m_radius(26.f)
    , m_shape(m_radius)
    , m_rng(std::random_device{}())
{
    m_shape.setFillColor(kBodyColor);

    // 기준점을 원의 "맨 아래 가운데(발)"로: 찌그러져도 발이 바닥에 붙어 있게
    m_shape.setOrigin({m_radius, m_radius * 2.f});
    m_shape.setPosition({kWindowSize.x / 2.f, static_cast<float>(kWindowSize.y)});
}

// ───────────────────────── 메인 업데이트 ─────────────────────────

void Pet::update(float dt, const sf::FloatRect& area)
{
    m_animTime += dt;

    switch (m_state)
    {
    case State::Idle:  updateIdle(dt);        break;
    case State::Walk:  updateWalk(dt, area);  break;
    case State::Sit:   updateSit(dt);         break;
    case State::Sleep: updateSleep(dt);       break;
    case State::Fall:  updateFall(dt, area);  break;
    case State::Held:  break;   // 들려 있는 동안은 App이 위치를 정해줌
    }

    updateAppearance();
}

// ───────────────────────── 상태 전환 ─────────────────────────

void Pet::changeState(State next)
{
    if constexpr (kDebugLog)
        std::cout << "[Pet] " << toString(m_state) << " -> " << toString(next) << '\n';

    m_state = next;
    m_animTime = 0.f;

    // 새 상태에 들어갈 때 할 일 (지속 시간 정하기 등)
    switch (next)
    {
    case State::Idle:
        m_stateTimer = randomRange(kIdleTime.min, kIdleTime.max);
        break;
    case State::Walk:
        m_stateTimer = randomRange(kWalkTime.min, kWalkTime.max);
        if (chance(0.5f))
            m_facing = -m_facing;   // 절반 확률로 반대 방향
        break;
    case State::Sit:
        m_stateTimer = randomRange(kSitTime.min, kSitTime.max);
        break;
    case State::Sleep:
        m_stateTimer = randomRange(kSleepTime.min, kSleepTime.max);
        break;
    case State::Fall:
    case State::Held:
        m_stateTimer = 0.f;   // 시간이 아니라 사건(착지, 놓기)으로 끝나는 상태
        break;
    }
}

bool Pet::isOnGround() const
{
    return m_state == State::Idle || m_state == State::Walk
        || m_state == State::Sit  || m_state == State::Sleep;
}

// ───────────────────────── 상태별 업데이트 ─────────────────────────

void Pet::updateIdle(float dt)
{
    m_stateTimer -= dt;
    if (m_stateTimer > 0.f)
        return;

    // 다음 행동 고르기
    const float roll = randomRange(0.f, 1.f);
    if (roll < 0.50f)      changeState(State::Walk);    // 50%
    else if (roll < 0.85f) changeState(State::Sit);     // 35%
    else                   changeState(State::Sleep);   // 15%
}

void Pet::updateWalk(float dt, const sf::FloatRect& area)
{
    const Limits lim = computeLimits(area);

    m_position.x += m_facing * kWalkSpeed * dt;
    m_position.y = lim.floor;

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

    m_stateTimer -= dt;
    if (m_stateTimer <= 0.f)
        changeState(State::Idle);
}

void Pet::updateSit(float dt)
{
    m_stateTimer -= dt;
    if (m_stateTimer > 0.f)
        return;

    if (chance(0.4f)) changeState(State::Sleep);   // 앉아 있다가 스르륵 잠들기
    else              changeState(State::Idle);
}

void Pet::updateSleep(float dt)
{
    m_stateTimer -= dt;
    if (m_stateTimer <= 0.f)
        changeState(State::Idle);   // 푹 자고 일어남
}

void Pet::updateFall(float dt, const sf::FloatRect& area)
{
    const Limits lim = computeLimits(area);

    // 1. 중력 + 공기 저항
    m_velocity.y = std::min(m_velocity.y + kGravity * dt, kMaxFallSpeed);
    m_velocity.x -= m_velocity.x * kAirDrag * dt;

    // 2. 이동
    m_position += m_velocity * dt;

    // 3. 좌우 벽
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

    // 4. 천장
    if (m_position.y < lim.top)
    {
        m_position.y = lim.top;
        m_velocity.y = std::abs(m_velocity.y) * kWallBounce;
    }

    // 5. 날아가는 방향 바라보기
    if (std::abs(m_velocity.x) > 1.f)
        m_facing = (m_velocity.x > 0.f) ? 1.f : -1.f;

    // 6. 바닥: 빠르면 튕기고, 느리면 착지 → Idle
    if (m_position.y >= lim.floor && m_velocity.y >= 0.f)
    {
        m_position.y = lim.floor;

        if (m_velocity.y > kMinBounceSpeed)
        {
            m_velocity.y = -m_velocity.y * kFloorBounce;
        }
        else
        {
            m_velocity = {0.f, 0.f};
            changeState(State::Idle);
        }
    }
}

// ───────────────────────── 겉모습 ─────────────────────────

void Pet::updateAppearance()
{
    sf::Vector2f scale{1.f, 1.f};
    sf::Color color = kBodyColor;

    switch (m_state)
    {
    case State::Idle:
        break;
    case State::Walk:
    {
        // 걸을 때 말랑말랑 (가로/세로가 번갈아 늘었다 줄었다)
        const float wobble = 0.05f * std::sin(m_animTime * 12.f);
        scale = {1.f + wobble, 1.f - wobble};
        break;
    }
    case State::Sit:
        scale = {1.12f, 0.88f};
        break;
    case State::Sleep:
    {
        // 납작하게 퍼져서 천천히 숨쉬기
        const float breath = 0.03f * std::sin(m_animTime * 2.f);
        scale = {1.2f - breath, 0.75f + breath};
        color = kSleepColor;
        break;
    }
    case State::Fall:
        scale = {0.92f, 1.08f};
        break;
    case State::Held:
        scale = {0.85f, 1.15f};   // 잡혀서 쭉 늘어남
        break;
    }

    m_shape.setScale(scale);
    m_shape.setFillColor(color);
}

void Pet::render(sf::RenderTarget& target) const
{
    target.draw(m_shape);
}

// ───────────────────────── 사용자 상호작용 ─────────────────────────

void Pet::grab()
{
    m_velocity = {0.f, 0.f};
    changeState(State::Held);
}

void Pet::moveHeld(sf::Vector2f feetPosition)
{
    m_position = feetPosition;
}

void Pet::release(sf::Vector2f throwVelocity)
{
    if (throwVelocity.length() > kMaxThrowSpeed)
        throwVelocity = throwVelocity.normalized() * kMaxThrowSpeed;

    m_velocity = throwVelocity;
    changeState(State::Fall);
}

void Pet::poke()
{
    // 자고 있으면 폴짝 대신 깨어나기만
    if (m_state == State::Sleep)
    {
        changeState(State::Idle);
        return;
    }

    if (!isOnGround())
        return;

    m_velocity = {m_facing * kWalkSpeed, -kJumpSpeed};
    changeState(State::Fall);
}

// ───────────────────────── 기타 ─────────────────────────

sf::Vector2f Pet::getFeetPosition() const
{
    return m_position;
}

bool Pet::isHeld() const
{
    return m_state == State::Held;
}

float Pet::randomRange(float min, float max)
{
    std::uniform_real_distribution<float> dist(min, max);
    return dist(m_rng);
}

bool Pet::chance(float probability)
{
    return randomRange(0.f, 1.f) < probability;
}

const char* Pet::toString(State state)
{
    switch (state)
    {
    case State::Idle:  return "Idle";
    case State::Walk:  return "Walk";
    case State::Sit:   return "Sit";
    case State::Sleep: return "Sleep";
    case State::Fall:  return "Fall";
    case State::Held:  return "Held";
    }
    return "?";
}