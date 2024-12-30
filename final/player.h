#pragma once
#include "player.fwd.h"
#include "gameObject.fwd.h"
#include <SFML/Graphics.hpp>
#include "gameObject.h"

class Player : public GameObject {
private:
    sf::Vector2f upVelocity;
    sf::Vector2f gravity;
    sf::Vector2f spawn;
    sf::CircleShape shape;
    float speed;
    int playerIndex;
public:
    bool freefall;
    bool leftMove;
    bool rightMove;
    
    //Player();
    Player(int id);
    sf::CircleShape &getShape();
    //const sf::CircleShape &getShape();

    void jump();
    sf::Vector2f getVelocity();
    int getID();
    void setVelocity(const sf::Vector2f &v);
    void gameMove(Timeline &timeline);
    void land();
    void setSpawn(sf::Vector2f sp);
    void respawnEvent();
    void killEvent();
    void collideEvent();
    void handleEvent(NetworkEvent &event) override;
};