#include "movingPlatformVector.h"
#include "platform.h"
#include <SFML/Graphics.hpp>
#include <bits/stdc++.h>
#include <iostream>

MovingPlatformVector::MovingPlatformVector() : MovingPlatformVector(150.0f, std::vector<sf::Vector2f>{sf::Vector2f()}) {
}

MovingPlatformVector::MovingPlatformVector(float width, std::vector<sf::Vector2f> pos) : MovingPlatformVector(width, sf::Color::Magenta, pos)  {
}

MovingPlatformVector::MovingPlatformVector(float width, sf::Color color, std::vector<sf::Vector2f> pos) : positions(pos), Platform(width, sf::Vector2f(), color)  {
    // literally just assume the vector has an element
    // nevermind, i fixed that
    setActive(true);
    setVisible(true);
}

void MovingPlatformVector::gameMove(Timeline &timeline) {
    // move the platform linearly between a lot of points
    // don't even worry about there being no points

    float tick = getTick(timeline); // this call synchronizes time for this object
    float tween = std::fmod(getTime(), (float)positions.size());
    
    int index = (int)tween; // should be floor
    tween -= (float)index; // get remainder


    sf::Vector2f newPosition = tween*positions[(index+1)%positions.size()] + (1.0f-tween)*positions[index];
    
    shape.setPosition(newPosition);
}