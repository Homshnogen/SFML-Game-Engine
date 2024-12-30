#include "enemyGroup.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstring>
#include "bullet.h"

// init static member


typedef EnemyGroup::Enemy Enemy;
int EnemyGroup::platformID = 0;
int Enemy::numLiving = 0;
std::mutex Enemy::livingLock;
Enemy::Enemy(sf::Vector2f pos) : shape({30.0f, 50.0f}){
    setShape(&shape);
    shape.setPosition(pos);
    setActive(false);
    setVisible(true);
}

void Enemy::pushPlayer(Player &player) {
    if (!isActive()) return;
    if (player.isActive() && shape.getGlobalBounds().intersects(player.getShape().getGlobalBounds())) {
        player.killEvent();
    } else if (shape.getPosition().y + shape.getSize().y > 550.0f) {
        player.killEvent();
    } else {
        for (auto *child : player.children) {
            if (Bullet *bullet = dynamic_cast<Bullet *>(child)) {
                if (bullet->isPlayer && bullet->isActive() && shape.getGlobalBounds().intersects(bullet->getShape().getGlobalBounds())) {
                    setActive(false);
                    bullet->setActive(false);
                    livingLock.lock();
                    numLiving--;
                    if(numLiving == 0) {
                        player.winEvent();
                    }
                    livingLock.unlock();
                }
            }
        }
    }
}
void Enemy::gameMove(Timeline &timeline) {
    getTick(timeline);
}
sf::RectangleShape &Enemy::getShape() {return shape;}


EnemyGroup::EnemyGroup() : EnemyGroup::EnemyGroup(sf::Vector2f(380.0f, 280.0f), sf::Vector2f(20.0f, 20.0f)) {
}

EnemyGroup::EnemyGroup(sf::Vector2f size, sf::Vector2f startPos) : bulletTimer(0.0f), enemies(), bullets(), velocity(20.0f, 0.0f), startPos(startPos), shape(size), GameObject("platform" + std::to_string(platformID++)) {
    shape.setFillColor(sf::Color(0x00000000));
    GameObject::setShape(&shape);
    createEnemies();
    reset();
    setActive(true);
    setVisible(true);
}
EnemyGroup::~EnemyGroup() {
    for (auto *enemy : enemies) {
        delete enemy;
    }
    for (auto *bullet : bullets) {
        delete bullet;
    }
}

void EnemyGroup::reset() {
    moveTo(startPos); // will move the enemies
    for (auto *bullet : bullets) {
        bullet->setActive(false);
    }
    // spawn enemies, move() to start position, and set move direction
    for (auto *enemy : enemies) {
        enemy->getShape().setFillColor(sf::Color::Cyan);
        if (!enemy->isActive()) {
            enemy->setActive(true);
            Enemy::numLiving++;
        }
    }
    velocity = {60.0f, 0.0f}; // maybe change to speed
}

void EnemyGroup::draw(sf::RenderWindow &window) {
    GameObject::draw(window);
    for (auto *bullet : bullets) {
        bullet->lock();
        bullet->draw(window);
        bullet->unlock();
    }
    for (auto *enemy : enemies) {
        enemy->lock();
        enemy->draw(window);
        enemy->unlock();
    }
}

void EnemyGroup::pushPlayer(Player &player) {
    player.lock();
    //moveTo({0.0f, 0.0f});
    for (auto *enemy : enemies) {
        enemy->lock();
        enemy->pushPlayer(player);
        enemy->unlock();
    }
    for (auto *bullet : bullets) {
        bullet->lock();
        bullet->pushPlayer(player);
        bullet->unlock();
    }
    player.unlock();
}
void EnemyGroup::createEnemies() {
    if (enemies.size() > 0) {return;}
    shape.setPosition({0.0f, 0.0f});
    float spacing = 30.0f;
    int i = 0;
    for (float yoff = 0.0f; yoff < shape.getSize().y; yoff += 2.5f*spacing) {
        for (float xoff = 1.5f*spacing*(i%2); xoff < shape.getSize().x; xoff += 3.0f*spacing) {
            enemies.push_back(new Enemy(sf::Vector2f({xoff, yoff}))); // parent should call this from position (0, 0) // maybe use std::move
        }
        i+=1;
    }
}

void EnemyGroup::moveTo(sf::Vector2f pos) {
    move(pos-shape.getPosition());
}
void EnemyGroup::move(sf::Vector2f dist) {
    for (auto *enemy : enemies) {
        enemy->getShape().move(dist);
    }
    shape.move(dist);
}
void EnemyGroup::shoot() {
    // randomly choose an enemy to shoot
    Enemy::livingLock.lock();
    if (Enemy::numLiving == 0) return;
    int random = std::rand() % Enemy::numLiving;
    for (auto *enemy : enemies) {
        if (enemy->isActive()) {
            if (random-- == 0) {
                bullets.insert(syncTime(new Bullet({enemy->getShape().getPosition().x+10.0f, enemy->getShape().getPosition().y+55.0f}, false)));
            }
        }
    }
    Enemy::livingLock.unlock();
}

void EnemyGroup::gameMove(Timeline &timeline) {
    float tick = getTick(timeline);
    if (!isActive() || timeline.isPaused()) return;
    bulletTimer += tick;
    if (bulletTimer > 1.0f) {
        shoot();
        bulletTimer -= 1;
    }
    sf::Vector2f dist = tick*velocity;
    if ((velocity.x > 0.0f && shape.getSize().x + shape.getPosition().x > 780.0f) || (velocity.x < 0.0f && shape.getPosition().x < 20.0f)) {
        velocity = -velocity;
        dist.y += 60.0f;
    }
    for (auto *del : junk) {
        junk.erase(del); // probably fine
        break; // just in case
        //delete del;
    }
    for (auto *enemy : enemies) {
        enemy->gameMove(timeline);
    }
    for (auto *bullet : bullets) {
        if (!bullet->isActive()) {
            bullet->lock();
            bullets.erase(bullet);
            junk.insert(bullet);
            bullet->unlock();
            break; // iterator is damaged; catch it next tick
            //delete bullet;
        } else {
            bullet->gameMove(timeline);
        }
    }
    // TODO: shoot
    move(dist);

}


sf::RectangleShape &EnemyGroup::getShape() {return shape;}

void EnemyGroup::handleEvent(NetworkEvent &event) {
    if (strncmp(event.type, "resetgame", 16) == 0) { // case eventTypes::characterDeath:
        std::cout << "reset " << event.playerIndex << std::endl;
        reset();
    }
}
