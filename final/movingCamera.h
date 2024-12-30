#pragma once
#include "movingCamera.fwd.h"
#include "movingCameraBounds.fwd.h"
#include <SFML/Graphics.hpp>
#include "player.h"
#include "gameObject.h"
#include "movingCameraBounds.h"

class MovingCamera : public GameObject, public sf::View {
private:
    std::vector<MovingCameraBounds *> bounds;
    //bool active;
public:
    MovingCamera();
    MovingCamera(unsigned int width, unsigned int height);
    MovingCamera(unsigned int width, unsigned int height, std::vector<MovingCameraBounds *> &list);
    void moveAmount(sf::Vector2f move);
    void registerBounds(MovingCameraBounds *b);
    void registerBounds(std::vector<MovingCameraBounds *> &list);
    void pushPlayer(Player &player);
    void draw(sf::RenderWindow &window) override;
    //void draw(sf::RenderWindow &window);
};