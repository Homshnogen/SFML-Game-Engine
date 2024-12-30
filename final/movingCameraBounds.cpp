#include "movingCameraBounds.h"

MovingCameraBounds::MovingCameraBounds() : MovingCameraBounds(0.0f, 0.0f, sf::Vector2f(0.0f, 0.0f)) {

}
MovingCameraBounds::MovingCameraBounds(float width, float height, sf::Vector2f position) : Platform(width, position), camera(nullptr) {
    // todo: give these relative positions
    shape.setSize(sf::Vector2f(width, height));
    setActive(true);
    setVisible(false);
}
void MovingCameraBounds::setCamera(MovingCamera *c) {
    camera = c;
}
void MovingCameraBounds::move(sf::Vector2f mv) {
    shape.move(mv);
}
void MovingCameraBounds::pushPlayer(Player &player) {
    if (camera == nullptr) return; // shouldn't happen
    player.lock();
    if (shape.getGlobalBounds().intersects(player.getShape().getGlobalBounds())) {
        // closest edge-based de-colliding, move camera
        float bottom, top, left, right;
        bottom = shape.getSize().y + shape.getPosition().y - player.getShape().getPosition().y;
        top = player.getShape().getPosition().y + 2*player.getShape().getRadius() - shape.getPosition().y;
        right = shape.getSize().x + shape.getPosition().x - player.getShape().getPosition().x;
        left = player.getShape().getPosition().x + 2*player.getShape().getRadius() - shape.getPosition().x;
        // min positive between top and bottom
        if (top < bottom) {
            if (left < right) {
                if (left < top) {
                    // move left (move camera right)
                    camera->moveAmount(sf::Vector2f(left, 0.0f));
                } else {
                    // move top (move camera down)
                    camera->moveAmount(sf::Vector2f(0.0f, top));
                }
            } else {
                if (right < top) {
                    // move right (move camera left)
                    camera->moveAmount(sf::Vector2f(-right, 0.0f));
                } else {
                    // move top (move camera down)
                    camera->moveAmount(sf::Vector2f(0.0f, top));
                }
            }
        } else {
            // bottom > 0.0f
            if (left < right) {
                if (left < bottom) {
                    // move left (move camera right)
                    camera->moveAmount(sf::Vector2f(left, 0.0f));
                } else {
                    // move bottom (move camera up)
                    camera->moveAmount(sf::Vector2f(0.0f, -bottom));
                }
            } else {
                if (right < bottom) {
                    // move right (move camera left)
                    camera->moveAmount(sf::Vector2f(-right, 0.0f));
                } else {
                    // move bottom (move camera up)
                    camera->moveAmount(sf::Vector2f(0.0f, -bottom));
                }
            }
        }
    }
    player.unlock();
}