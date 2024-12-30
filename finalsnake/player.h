#pragma once
#include "player.fwd.h"
#include "gameObject.fwd.h"
#include <SFML/Graphics.hpp>
#include "gameObject.h"
#include "bullet.h"
#include <unordered_set>

class Player : public GameObject {
private:
    sf::Vector2f gravity;
    sf::Vector2f spawn;
    sf::Vector2f velocity;
    sf::CircleShape shape;
    float speed;
    float shootCooldown;
    int playerIndex;
    std::unordered_set<GameObject*> junk;
    bool leftMove, rightMove, upMove, downMove;
public:
    bool freefall;
    bool gameover;
    std::vector<Bullet*> children;
    
    //Player();
    Player(int id);
    ~Player();
    sf::CircleShape &getShape();
    //const sf::CircleShape &getShape();

    void shoot();
    sf::Vector2f getVelocity(float tick);
    int getID();
    void gameMove(Timeline &timeline) override;
    void pushPlayer(Player &player) override;
    void setSpawn(sf::Vector2f sp);
    void addChild(Bullet *child);
    void removeChildren();
    void respawnEvent();
    void killEvent();
    void winEvent();
    void collideEvent();
    void handleEvent(NetworkEvent &event) override;
    void draw(sf::RenderWindow &window) override;
};