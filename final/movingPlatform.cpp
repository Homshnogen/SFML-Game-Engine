#include "movingPlatform.h"
#include "platform.h"
#include <SFML/Graphics.hpp>
#include <bits/stdc++.h>
#include <iostream>

MovingPlatform::MovingPlatform() : MovingPlatform(150.0f, sf::Vector2f(), sf::Vector2f()) {
}

MovingPlatform::MovingPlatform(float width, sf::Vector2f start, sf::Vector2f end) : MovingPlatform(width, sf::Color::Magenta, start, end)  {
}

MovingPlatform::MovingPlatform(float width, sf::Color color, sf::Vector2f start, sf::Vector2f end) : startPosition(start), endPosition(end), Platform(width, start, color)  {
    setActive(true);
    setVisible(true);
}

void MovingPlatform::gameMove(Timeline &timeline) {
    // move the platform linearly between start and end points
    float tick = getTick(timeline);
    float tween = std::fmod(getTime(), 2.0f);
    if (tween > 1.0f) {
        tween = 2.0f - tween;
    }
    sf::Vector2f newPosition = tween*endPosition + (1.0f-tween)*startPosition;
    
    shape.setPosition(newPosition);
}