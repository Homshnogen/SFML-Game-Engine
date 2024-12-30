
#include "bullet.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include "player.h"

// init static member

Bullet::Bullet(sf::Vector2f pos, bool isPlayer) : shape(5.0f), isPlayer(isPlayer), velocity({0.0f, isPlayer ? -200.0f : 80.0f}) {
    setShape(&shape);
    shape.setPosition(pos);
    shape.setFillColor(isPlayer ? sf::Color::Red : sf::Color::Yellow);
    setActive(true);
    setVisible(true);
}


void Bullet::gameMove(Timeline &timeline) {
    float tick = getTick(timeline);
    if (!isActive()) return;
    if (shape.getPosition().y > 600.0f || shape.getPosition().y < 0.0f) setActive(false);

    shape.move(tick*velocity);
}

void Bullet::pushPlayer(Player &player) {
    if (!isActive() || isPlayer) return;
    if (shape.getGlobalBounds().intersects(player.getShape().getGlobalBounds())) {
        player.killEvent();
    }
}

sf::CircleShape &Bullet::getShape() {return shape;}

