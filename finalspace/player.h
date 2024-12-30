#pragma once
#include "player.fwd.h"
#include "gameObject.fwd.h"
#include <SFML/Graphics.hpp>
#include "gameObject.h"
#include <unordered_set>

class Player : public GameObject {
private:
    sf::Vector2f gravity;
    sf::Vector2f spawn;
    sf::CircleShape shape;
    float speed;
    float shootCooldown;
    int playerIndex;
    std::unordered_set<GameObject*> junk;
public:
    bool freefall;
    bool leftMove;
    bool rightMove;
    std::unordered_set<GameObject*> children;
    
    //Player();
    Player(int id);
    ~Player();
    sf::CircleShape &getShape();
    //const sf::CircleShape &getShape();

    void shoot();
    sf::Vector2f getVelocity();
    int getID();
    void gameMove(Timeline &timeline);
    void setSpawn(sf::Vector2f sp);
    void respawnEvent();
    void killEvent();
    void winEvent();
    void collideEvent();
    void handleEvent(NetworkEvent &event) override;
    void draw(sf::RenderWindow &window) override;
};