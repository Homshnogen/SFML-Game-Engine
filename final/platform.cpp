#include "platform.h"
#include <SFML/Graphics.hpp>
#include <iostream>

// init static member
int Platform::platformID = 0;
Platform::Platform() : Platform::Platform(150.0f, sf::Vector2f()) {
}

Platform::Platform(float width, sf::Vector2f position) : Platform(width, position, sf::Color::Red) {
}

Platform::Platform(float width, sf::Vector2f position, sf::Color color) : shape(sf::Vector2f(width, 50.0f)), GameObject("platform" + std::to_string(platformID++)) {
    shape.setFillColor(color);
    shape.setPosition(position);
    GameObject::setShape(&shape);
    setActive(true);
    setVisible(true);
}

sf::RectangleShape &Platform::getShape() {return shape;}

void Platform::pushPlayer(Player &player) {
    player.lock();
    
    bool leftright = false;
    if (shape.getGlobalBounds().intersects(player.getShape().getGlobalBounds())) {
        // closest edge-based de-colliding
        float bottom, top, left, right;
        bottom = shape.getSize().y + shape.getPosition().y - player.getShape().getPosition().y;
        top = player.getShape().getPosition().y + 2*player.getShape().getRadius() - shape.getPosition().y;
        right = shape.getSize().x + shape.getPosition().x - player.getShape().getPosition().x;
        left = player.getShape().getPosition().x + 2*player.getShape().getRadius() - shape.getPosition().x;
        // min positive between top and bottom
        if (top < bottom) {
            if (left < right) {
                if (left < top) {
                    // move left
                    player.getShape().setPosition(shape.getPosition().x - 2*player.getShape().getRadius(), player.getShape().getPosition().y);
                    leftright = true;
                } else {
                    // move top
                    player.getShape().setPosition(player.getShape().getPosition().x, shape.getPosition().y - 2*player.getShape().getRadius());
                    // player has landed on a platform and can jump
                    if (player.getVelocity().y >= 0) player.land(); // use this to fix half-jump
                }
            } else {
                if (right < top) {
                    // move right
                    player.getShape().setPosition(shape.getSize().x + shape.getPosition().x, player.getShape().getPosition().y);
                    leftright = true;
                } else {
                    // move top
                    player.getShape().setPosition(player.getShape().getPosition().x, shape.getPosition().y - 2*player.getShape().getRadius());
                    // player has landed on a platform and can jump
                    if (player.getVelocity().y >= 0) player.land();
                }
            }
        } else {
            // bottom > 0.0f
            if (left < right) {
                if (left < bottom) {
                    // move left
                    player.getShape().setPosition(shape.getPosition().x - 2*player.getShape().getRadius(), player.getShape().getPosition().y);
                    leftright = true;
                } else {
                    // move bottom
                    player.getShape().setPosition(player.getShape().getPosition().x, shape.getSize().y + shape.getPosition().y);
                    if (player.getVelocity().y < 0) player.setVelocity(sf::Vector2f(player.getVelocity().x, 0.0f));
                }
            } else {
                if (right < bottom) {
                    // move right
                    player.getShape().setPosition(shape.getSize().x + shape.getPosition().x, player.getShape().getPosition().y);
                    leftright = true;
                } else {
                    // move bottom
                    player.getShape().setPosition(player.getShape().getPosition().x, shape.getSize().y + shape.getPosition().y);
                    if (player.getVelocity().y < 0) player.setVelocity(sf::Vector2f(player.getVelocity().x, 0.0f));
                }
            }
        }
        if (leftright) player.collideEvent(); // left-right collision
    }
    player.unlock();
}