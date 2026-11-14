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

    // --- 스프라이트 ---
    constexpr int   kFrameSize   = 32;    // 시트 한 칸 크기 (px)
    constexpr float kSpriteScale = 2.f;   // 32px → 64px

    // --- 애니메이션: {행, 프레임 수, 프레임당 시간(초)} ---
    constexpr Animation kIdleAnim  {0, 4, 0.5f};
    constexpr Animation kWalkAnim  {1, 4, 0.15f};
    constexpr Animation kSitAnim   {2, 2, 0.8f};
    constexpr Animation kSleepAnim {3, 2, 1.0f};
    constexpr Animation kFallAnim  {4, 2, 0.12f};
    constexpr Animation kHeldAnim  {5, 2, 0.25f};

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

Pet::Pet(sf::Vector2f startFeetPosition, const sf::Texture& sheet)
    : m_state(State::Fall)
    , m_stateTimer(0.f)
    , m_position(startFeetPosition)
    , m_velocity({0.f, 0.f})
    , m_facing(1.f)
    , m_sprite(sheet)
    , m_animator({kFrameSize, kFrameSize})
    , m_rng(std::random_device{}())
{
    // 기준점 = 프레임의 아래쪽 가운데(발): 좌우 반전해도 제자리에서 뒤집힘
    m_sprite.setOrigin({kFrameSize / 2.f, static_cast<float>(kFrameSize)});
    m_sprite.setPosition({kWindowSize.x / 2.f, static_cast<float>(kWindowSize.y)});

    m_animator.play(animationFor(m_state));
    updateSprite();
}

// ───────────────────────── 메인 업데이트 ─────────────────────────

void Pet::update(float dt, const sf::FloatRect& area)
{
    switch (m_state)
    {
    case State::Idle:  updateIdle(dt);        break;
    case State::Walk:  updateWalk(dt, area);  break;
    case State::Sit:   updateSit(dt);         break;
    case State::Sleep: updateSleep(dt);       break;
    case State::Fall:  updateFall(dt, area);  break;
    case State::Held:  break;
    }

    m_animator.play(animationFor(m_state));
    m_animator.update(dt);
    updateSprite();
}

// ───────────────────────── 상태 전환 ─────────────────────────

void Pet::changeState(State next)
{
    if constexpr (kDebugLog)
        std::cout << "[Pet] " << toString(m_state) << " -> " << toString(next) << '\n';

    m_state = next;

    switch (next)
    {
    case State::Idle:
        m_stateTimer = randomRange(kIdleTime.min, kIdleTime.max);
        break;
    case State::Walk:
        m_stateTimer = randomRange(kWalkTime.min, kWalkTime.max);
        if (chance(0.5f))
            m_facing = -m_facing;
        break;
    case State::Sit:
        m_stateTimer = randomRange(kSitTime.min, kSitTime.max);
        break;
    case State::Sleep:
        m_stateTimer = randomRange(kSleepTime.min, kSleepTime.max);
        break;
    case State::Fall:
    case State::Held:
        m_stateTimer = 0.f;
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

    const float roll = randomRange(0.f, 1.f);
    if (roll < 0.50f)      changeState(State::Walk);
    else if (roll < 0.85f) changeState(State::Sit);
    else                   changeState(State::Sleep);
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

    if (chance(0.4f)) changeState(State::Sleep);
    else              changeState(State::Idle);
}

void Pet::updateSleep(float dt)
{
    m_stateTimer -= dt;
    if (m_stateTimer <= 0.f)
        changeState(State::Idle);
}

void Pet::updateFall(float dt, const sf::FloatRect& area)
{
    const Limits lim = computeLimits(area);

    m_velocity.y = std::min(m_velocity.y + kGravity * dt, kMaxFallSpeed);
    m_velocity.x -= m_velocity.x * kAirDrag * dt;

    m_position += m_velocity * dt;

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

    if (m_position.y < lim.top)
    {
        m_position.y = lim.top;
        m_velocity.y = std::abs(m_velocity.y) * kWallBounce;
    }

    if (std::abs(m_velocity.x) > 1.f)
        m_facing = (m_velocity.x > 0.f) ? 1.f : -1.f;

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

// ───────────────────────── 애니메이션 / 그리기 ─────────────────────────

const Animation& Pet::animationFor(State state)
{
    switch (state)
    {
    case State::Idle:  return kIdleAnim;
    case State::Walk:  return kWalkAnim;
    case State::Sit:   return kSitAnim;
    case State::Sleep: return kSleepAnim;
    case State::Fall:  return kFallAnim;
    case State::Held:  return kHeldAnim;
    }
    return kIdleAnim;
}

void Pet::updateSprite()
{
    m_sprite.setTextureRect(m_animator.getFrameRect());
    m_sprite.setScale({kSpriteScale * m_facing, kSpriteScale});
}

void Pet::render(sf::RenderTarget& target) const
{
    target.draw(m_sprite);
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

void Pet::sleep()
{
    if (!isOnGround() || m_state == State::Sleep)
        return;   // 공중에 있거나 이미 자고 있으면 무시

    changeState(State::Sleep);
}

void Pet::wakeUp()
{
    if (m_state == State::Sleep)
        changeState(State::Idle);
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

bool Pet::isSleeping() const
{
    return m_state == State::Sleep;
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