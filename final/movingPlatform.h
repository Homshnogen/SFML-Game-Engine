#pragma once
#include "platform.h"

class MovingPlatform : public Platform {
private:
    sf::Vector2f startPosition;
    sf::Vector2f endPosition;
public:
    MovingPlatform();
    MovingPlatform(float width, sf::Vector2f start, sf::Vector2f end);
    MovingPlatform(float width, sf::Color color, sf::Vector2f start, sf::Vector2f end);
    void gameMove(Timeline &timeline) override;
};