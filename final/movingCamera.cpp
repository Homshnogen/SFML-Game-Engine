#include "movingCamera.h"

MovingCamera::MovingCamera() : MovingCamera(800, 600) {
}

MovingCamera::MovingCamera(unsigned int width, unsigned int height) : sf::View(sf::FloatRect(0, 0, width, height)), bounds() {
    setActive(true);
    setVisible(false);
}

MovingCamera::MovingCamera(unsigned int width, unsigned int height, std::vector<MovingCameraBounds *> &list) : MovingCamera(width, height) {
    // todo: give these relative positions
    registerBounds(list);
}
void MovingCamera::moveAmount(sf::Vector2f move) {
    sf::View::setCenter(move + sf::View::getCenter());
    for (MovingCameraBounds *b : bounds) {
        b->move(move);
    }
}
void MovingCamera::registerBounds(MovingCameraBounds *b) { // try to do this once per thing
    b->setCamera(this);
    bounds.push_back(b);
}
void MovingCamera::registerBounds(std::vector<MovingCameraBounds *> &list) {
    for (MovingCameraBounds *b : list) {
        registerBounds(b);
    }
}
void MovingCamera::pushPlayer(Player &player) {

    sf::FloatRect cameraRect = sf::FloatRect(getCenter()-sf::Vector2f(0.5*getSize().x, 0.5*getSize().y), getSize());
    if (!player.getShape().getGlobalBounds().intersects(cameraRect)) {
        moveAmount(player.getShape().getPosition()-getCenter()); // move camera to off-screen player
    }
    for (MovingCameraBounds *b : bounds) {
        b->pushPlayer(player);
    }
}

void MovingCamera::draw(sf::RenderWindow &window) {
    // set active if active
    if (isActive()) {
        window.setView(*this);
    }
}