#pragma once
#include "player.h"
#include <SFML/Graphics.hpp>
#include "gameObject.h"
#include <unordered_set>
#include <vector>

class EnemyGroup : public GameObject {
    public: class Enemy : public GameObject {
    protected:
        sf::RectangleShape shape;
    public:
        static int numLiving;
        static std::mutex livingLock;
        Enemy(sf::Vector2f pos);
        void pushPlayer(Player &player) override;
        void gameMove(Timeline &timeline) override;
        sf::RectangleShape &getShape();
    };
private:
    static int platformID;

    sf::Vector2f startPos;
    sf::Vector2f velocity;
    float bulletTimer;
    std::vector<Enemy*> enemies;
    std::unordered_set<GameObject*> bullets;
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
    void createEnemies();
    void moveTo(sf::Vector2f pos);
    void move(sf::Vector2f dist);
    void handleEvent(NetworkEvent &event) override;
    void draw(sf::RenderWindow &window) override;

};