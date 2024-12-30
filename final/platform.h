#pragma once
#include "player.h"
#include <SFML/Graphics.hpp>
#include "gameObject.h"

class Platform : public GameObject {
private:
    static int platformID;
protected:
    sf::RectangleShape shape;
public:
    Platform();
    Platform(float width, sf::Vector2f position);
    Platform(float width, sf::Vector2f position, sf::Color color);
    sf::RectangleShape &getShape();
    void pushPlayer(Player &player);

};