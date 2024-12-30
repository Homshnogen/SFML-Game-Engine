#pragma once
#include <SFML/Graphics.hpp>
#include "gameObject.h"

class Bullet : public GameObject{
private:
    sf::CircleShape shape;
    sf::Vector2f velocity;
public:
    bool isPlayer;
    Bullet(sf::Vector2f pos, bool isPlayer);
    void gameMove(Timeline &timeline);
    void pushPlayer(Player &player);
    sf::CircleShape &getShape();
};