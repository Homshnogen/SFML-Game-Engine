#pragma once
#include "player.h"
#include <SFML/Graphics.hpp>
#include "gameObject.h"
#include "bullet.h"
#include <unordered_set>
#include <vector>

class EnemyGroup : public GameObject {
private:
    static std::mutex livingLock;
    static int platformID;

    sf::Vector2f startPos;
    sf::Vector2f velocity;
    float bulletTimer;
    Bullet* bullet;
    std::unordered_set<GameObject*> junk;
protected:
    sf::RectangleShape shape;
public:
    EnemyGroup();
    EnemyGroup(sf::Vector2f size, sf::Vector2f startPos);
    ~EnemyGroup();
    void gameMove(Timeline &timeline) override;
    sf::RectangleShape &getShape();
    void pushPlayer(Player &player) override;
    
    void shoot();
    void reset();
    void handleEvent(NetworkEvent &event) override;
    void draw(sf::RenderWindow &window) override;

};