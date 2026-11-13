#include <SFML/Graphics.hpp>

int main()
{
    // 400x300 크기의 일반 창 생성 (STEP 3에서 투명 펫 창으로 바뀔 예정)
    sf::RenderWindow window(sf::VideoMode({400u, 300u}), "DeskPal - STEP 1");
    window.setFramerateLimit(60);

    // 모찌 햄스터 자리 표시용 동그라미 (STEP 7에서 스프라이트로 교체)
    sf::CircleShape mochi(50.f);
    mochi.setFillColor(sf::Color(255, 248, 240));   // 떡 같은 하얀색
    mochi.setOrigin({50.f, 50.f});                  // 기준점을 원의 중심으로
    mochi.setPosition({200.f, 150.f});              // 창 가운데

    while (window.isOpen())
    {
        // SFML 3 방식 이벤트 처리
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear(sf::Color(40, 44, 52));
        window.draw(mochi);
        window.display();
    }
}