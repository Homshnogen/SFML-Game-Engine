#include "enemyGroup.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstring>

// init static member


int EnemyGroup::platformID = 0;
std::mutex EnemyGroup::livingLock;

EnemyGroup::EnemyGroup() : EnemyGroup::EnemyGroup(sf::Vector2f(700.0f, 500.0f), sf::Vector2f(50.0f, 50.0f)) {
}

EnemyGroup::EnemyGroup(sf::Vector2f size, sf::Vector2f startPos) : bulletTimer(0.0f), bullet(nullptr), velocity(20.0f, 0.0f), startPos(startPos), shape(size), GameObject("platform" + std::to_string(platformID++)) {
    shape.setFillColor(sf::Color(0x00000000));
    GameObject::setShape(&shape);
    //createEnemies();
    reset();
    setActive(true);
    setVisible(true);
}
EnemyGroup::~EnemyGroup() {
    livingLock.lock();
    if (bullet != nullptr) {
        delete bullet;
    }
    livingLock.unlock();
}

void EnemyGroup::reset() {
    shape.setPosition(startPos);
    if (bullet != nullptr) {
        bullet->lock();
        bullet->setActive(false);
        if (!bullet->isPlayer) {
            bullet->isPlayer = true;
        }
        bullet->unlock();
    }
    velocity = {60.0f, 0.0f}; // maybe change to speed
}

void EnemyGroup::shoot() {
    // randomly choose an enemy to shoot
    livingLock.lock();
    if (!bullet) {
        int randomX = 50 + std::rand() % 700;
        int randomY = 50 + std::rand() % 500;
        int randomC = std::rand();
        sf::Color randomColor(((0xfff00000 >> (randomX&15))+(0x3579aabd << (randomY&11))+(0x59483726 >> (randomC&13)))|(0x5050a0ff));
        // TODO: dist from player
        bullet = new Bullet({float(randomX), float(randomY)}, false);
        bullet->getShape().setFillColor(randomColor);
        syncTime(bullet);
    }
    livingLock.unlock();
}

void EnemyGroup::draw(sf::RenderWindow &window) {
    GameObject::draw(window);
    livingLock.lock();
    if (bullet != nullptr) {
        bullet->lock();
        bullet->draw(window);
        bullet->unlock();
    }
    livingLock.unlock();
}

void EnemyGroup::pushPlayer(Player &player) {
    player.lock();
    livingLock.lock();
    if (bullet) {
        bullet->lock();
        bullet->pushPlayer(player);
        if (bullet->isPlayer) { // has been collected
            bullet->unlock();
            bullet = nullptr;
        } else {
            bullet->unlock();
        }
    }
    livingLock.unlock();
    player.unlock();
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
        junk.erase(del);
        //delete del;
    }
    livingLock.lock();
    if (bullet != nullptr) {
        bullet->lock();
        if (bullet->isActive()) {
            bullet->gameMove(timeline);
            bullet->unlock();
        } else {
            bullet->unlock();
            bullet = nullptr;
        }
    }
    livingLock.unlock();
}


sf::RectangleShape &EnemyGroup::getShape() {return shape;}

void EnemyGroup::handleEvent(NetworkEvent &event) {
    if (strncmp(event.type, "resetgame", 16) == 0) { // case eventTypes::characterDeath:
        std::cout << "reset " << event.playerIndex << std::endl;
        reset();
    }
}
