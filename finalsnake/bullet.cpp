
#include "bullet.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include "player.h"

// init static member

Bullet::Bullet(sf::Vector2f pos, bool isPlayer) : shape(5.0f), isPlayer(isPlayer) {
    setShape(&shape);
    shape.setPosition(pos);
    shape.setFillColor(isPlayer ? sf::Color::Red : sf::Color::Yellow);
    setActive(true);
    setVisible(true);
}


void Bullet::gameMove(Timeline &timeline) {
    float tick = getTick(timeline);
    if (!isActive()) return;
}

void Bullet::pushPlayer(Player &player) {
    if (!isActive() || player.gameover) return;
    if (isPlayer) {
        sf::Vector2f distV = shape.getPosition()-player.getShape().getPosition();
        float dist2 = distV.x*distV.x + distV.y*distV.y;
        static float minDist2 = 55.0f*55.0f; // a little smaller
        //if (shape.getGlobalBounds().intersects(player.getShape().getGlobalBounds())) {
        if (dist2 < minDist2) {
            player.killEvent();
        }
    } else {
        sf::Vector2f distV = shape.getPosition()-player.getShape().getPosition() - sf::Vector2f(25.0f, 25.0f); // should account for different radius (r1-r2)
        float dist2 = distV.x*distV.x + distV.y*distV.y;
        static float minDist2 = 35.0f*35.0f; // (r1+r2)
        //if (shape.getGlobalBounds().intersects(player.getShape().getGlobalBounds())) {
        if (dist2 < minDist2) {
            player.addChild(this);
        }
    }
}

sf::CircleShape &Bullet::getShape() {return shape;}

