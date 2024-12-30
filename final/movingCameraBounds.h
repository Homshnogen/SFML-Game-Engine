#pragma once
#include "movingCamera.fwd.h"
#include "movingCameraBounds.fwd.h"
#include "player.h"
#include <SFML/Graphics.hpp>
#include "platform.h"
#include "movingCamera.h"

class MovingCameraBounds : public Platform {
private:
    MovingCamera *camera;
public:
    MovingCameraBounds();
    MovingCameraBounds(float width, float height, sf::Vector2f position);
    void setCamera(MovingCamera *c);
    void pushPlayer(Player &player);
    void move(sf::Vector2f mv);
    //void draw(sf::RenderWindow &window) override;
    //void draw(sf::RenderWindow &window);
};