#include "Animator.h"

Animator::Animator(sf::Vector2i frameSize)
    : m_frameSize(frameSize)
{
}

void Animator::play(const Animation& animation)
{
    if (m_playing && animation.row == m_current.row)
        return;   // 이미 재생 중

    m_current = animation;
    m_frame = 0;
    m_time = 0.f;
    m_playing = true;
}

void Animator::update(float dt)
{
    if (!m_playing)
        return;

    m_time += dt;
    while (m_time >= m_current.frameDuration)
    {
        m_time -= m_current.frameDuration;
        m_frame = (m_frame + 1) % m_current.frameCount;   // 마지막 다음은 처음으로 (반복)
    }
}

sf::IntRect Animator::getFrameRect() const
{
    return sf::IntRect(
        {m_frame * m_frameSize.x, m_current.row * m_frameSize.y},   // 위치
        m_frameSize);                                                // 크기
}