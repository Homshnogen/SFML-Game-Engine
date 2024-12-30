#pragma once
#include "platform.h"

class MovingPlatformVector : public Platform {
private:
    std::vector<sf::Vector2f> positions;
public:
    MovingPlatformVector();
    MovingPlatformVector(float width, std::vector<sf::Vector2f> pos);
    MovingPlatformVector(float width, sf::Color color, std::vector<sf::Vector2f> pos);
    void gameMove(Timeline &timeline) override;
};