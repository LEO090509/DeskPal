#pragma once
#include <SFML/Graphics.hpp>

// 애니메이션 하나의 정보
struct Animation
{
    int row;              // 스프라이트 시트의 몇 번째 행인가
    int frameCount;       // 프레임 수
    float frameDuration;  // 한 프레임을 보여주는 시간 (초)
};

// 스프라이트 시트에서 "지금 보여줄 칸"을 계산하는 클래스
class Animator
{
public:
    explicit Animator(sf::Vector2i frameSize);

    // 애니메이션 재생 (이미 재생 중인 것과 같으면 무시 → 매 프레임 호출해도 안전)
    void play(const Animation& animation);

    void update(float dt);

    // 시트에서 잘라낼 사각형
    sf::IntRect getFrameRect() const;

private:
    sf::Vector2i m_frameSize;
    Animation m_current{0, 1, 1.f};
    bool m_playing = false;
    int m_frame = 0;      // 현재 프레임 번호 (열)
    float m_time = 0.f;   // 현재 프레임이 표시된 시간
};