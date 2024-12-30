#include "player.h"
#include <SFML/Graphics.hpp>

#include "eventManager.h"
#include <iostream>
#include <cstring>
#include <vector>
#include <cmath>

/*
Player::Player() : Player(-1){
}*/
Player::Player(int id) : velocity(), shootCooldown(0.0f), children(), shape(30.0f), gravity(0.0f, 300.0f), spawn(), playerIndex(id), 
                        leftMove(false), downMove(false), upMove(false), rightMove(false), speed(160.0f), GameObject("player" + std::to_string(id)){
    shape.setFillColor(sf::Color(((0xf00f0000 >> (5*(id%3))) + (0x00aafaa0 << id) + (0x0003a7cf << (2*id))) | 0x000000ff));
    setShape(&shape);
    setActive(true);
    setVisible(true);
    gameover = false;
    //freefall = true;
}
Player::~Player() {
    for (auto *child : children) {
        delete child;
    }
}

sf::CircleShape &Player::getShape() {return shape;}

void Player::draw(sf::RenderWindow &window) {
    for (int i = 0; i < children.size() && i < 2; i++) { // draw friends under player
        auto *child = children[i];
        child->lock();
        child->draw(window);
        child->unlock();
    }
    GameObject::draw(window);
    for (int i = 2; i < children.size(); i++) { // draw enemies over player
        auto *child = children[i];
        child->lock();
        child->draw(window);
        child->unlock();
    }
}

sf::Vector2f Player::getVelocity(float tick) {
    sf::Vector2f power = sf::Vector2f();
    if (leftMove) power += sf::Vector2f(-speed, 0.0f);
    if (rightMove) power += sf::Vector2f(speed, 0.0f);
    if (upMove) power += sf::Vector2f(0.0f, -speed);
    if (downMove) power += sf::Vector2f(0.0f, speed);
    if (tick > 1.0f) {
        velocity = power;
    } else if (tick < 0.0f) {
        // velocity = velocity
    } else {
        velocity = velocity*(1.0f-tick) + power*tick;
    }
    return velocity*tick;
}

void Player::gameMove(Timeline &timeline) {
    float tick = getTick(timeline);
    if (gameover) {
        timeline.pause();
        return;
    } else {
        timeline.unpause();
    }
    if (!isActive()) { return; }
    if (shootCooldown > 0.0f) {shootCooldown -= tick;};
    for (auto *del : junk) {
        junk.erase(del);
        //delete del;
    }
    for (auto *child : children) {
        child->lock();
        child->gameMove(timeline);
        child->unlock();
    }
    for (int i = 0; i < children.size(); i++) {
        auto *child = children[i];
        child->lock();
        sf::Vector2f distV;
        if (i == 0) { // math simplified when children are the same shape
            distV = shape.getPosition()-child->getShape().getPosition();
        } else {
            distV = children[i-1]->getShape().getPosition()-child->getShape().getPosition();
        }
        float dist = sqrtf(distV.x*distV.x + distV.y*distV.y);
        static float maxDist = 50.0f; // a little smaller
        if (dist > maxDist) {
            sf::Vector2f move = distV * ((dist-maxDist)/dist);
            child->getShape().move(move);
        }
        //child->gameMove(timeline);
        child->unlock();
    }
    shape.move(getVelocity(tick));
}

void Player::pushPlayer(Player &player) {
    if (!isActive() || player.gameover || gameover) { return; }
    if (shape.getPosition().y > 535.0f || shape.getPosition().y < 5.0f || shape.getPosition().x > 735.0f || shape.getPosition().x < 5.0f) {
        killEvent();
    }
    for (int i = 0; i < children.size(); i++) {
        if (i < 2) continue; // based on snake; can't contact the first 3 children of head. //changed to 2 because of circle tangency and fancy math
        auto *child = children[i];
        child->lock();
        child->pushPlayer(player);
        child->unlock();
    }
}

void Player::addChild(Bullet *child) { // no locks?
    syncTime(child); // redundant
    child->getShape().setRadius(30.0f);
    if (children.size() < 2) {
        child->getShape().setFillColor(sf::Color(0x77aa44ff));
    }
    child->isPlayer = true;
    if (children.empty()) {
        child->getShape().setPosition(shape.getPosition());
    } else {
        child->getShape().setPosition(children.back()->getShape().getPosition());
    }
    children.push_back(child);
}
void Player::removeChildren() { // TODO: animated remove
    while (!children.empty()) {
        children.back()->setActive(false);
        junk.insert(children.back());
        children.pop_back();
    }
}
void Player::setSpawn(sf::Vector2f sp) {
    spawn = sp;
}
void Player::respawnEvent() {
    NetworkEvent ev{};
    strncpy(ev.type, "playerspawn", 16);
    //ev.type = eventTypes::characterSpawn;
    ev.playerIndex = playerIndex;
    ev.time = EventManager::get().getEventTime();
    ev.location = spawn;
    EventManager::get().raiseEvent(ev);

    strncpy(ev.type, "resetgame", 16);
    ev.playerIndex = playerIndex;
    ev.time = EventManager::get().getEventTime();
    ev.location = spawn;
    EventManager::get().raiseEvent(ev);
}
void Player::killEvent() { // freeze time and change texture on handle
    if (isActive()) {
        NetworkEvent ev{};
        strncpy(ev.type, "playerdeath", 16);
        //ev.type = eventTypes::characterDeath;
        ev.playerIndex = playerIndex;
        ev.time = EventManager::get().getEventTime();
        ev.location = shape.getPosition();
        EventManager::get().raiseEvent(ev);
    }
}
void Player::winEvent() { // freeze time and change texture on handle
    if (isActive()) {
        NetworkEvent ev{};
        strncpy(ev.type, "playerwin", 16);
        ev.playerIndex = playerIndex;
        ev.time = EventManager::get().getEventTime();
        ev.location = shape.getPosition();
        EventManager::get().raiseEvent(ev);
    }
}
void Player::collideEvent() {
    if (isActive()) {
        NetworkEvent ev{};
        strncpy(ev.type, "playercollide", 16);
        //ev.type = eventTypes::characterCollision;
        ev.playerIndex = playerIndex;
        ev.time = EventManager::get().getEventTime();
        ev.location = shape.getPosition();
        EventManager::get().raiseEvent(ev);
    }
}
int Player::getID() {
    return playerIndex;
}
void Player::handleEvent(NetworkEvent &event) {
    if (event.playerIndex != playerIndex) return;
    if (strncmp(event.type, "playerdeath", 16) == 0) { // case eventTypes::characterDeath:
        std::cout << "You died! press 'S' to restart " << std::endl;
        gameover = true;
        //setActive(false);
    } else if (strncmp(event.type, "playerwin", 16) == 0) { // case eventTypes::characterDeath:
        std::cout << "You win! press 'S' to restart " << std::endl;
        gameover = true;
        //setActive(false);
    } else if (strncmp(event.type, "disconnect", 16) == 0) { // case eventTypes::playerDisconnect:
        std::cout << "dc " << event.playerIndex << std::endl;
        setActive(false);
    } else if (strncmp(event.type, "resetgame", 16) == 0) { // case eventTypes::playerDisconnect:
        shape.setPosition(spawn);
        setActive(true);
        gameover = false;
    } else if (strncmp(event.type, "playerspawn", 16) == 0) { // case eventTypes::characterSpawn:
        std::cout << "spn " << event.playerIndex << std::endl;
        spawn = event.location;
        removeChildren();
        velocity = {0.0f, 0.0f};
        leftMove = false;
        rightMove = false;
        upMove = false;
        downMove = false;
        gameover = false;
        //shape.setPosition(spawn);
        //setActive(true);
        {
            NetworkEvent ev2;
            ev2.time = event.time; // should work
            ev2.playerIndex = event.playerIndex;
            strncpy(ev2.type, "moveobject", 16);
            strncpy(ev2.guid, guid.c_str(), 16);
            ev2.location = event.location;

            EventManager::get().raiseEvent(ev2);
        }
    } else if (strncmp(event.type, "connect", 16) == 0) { // case eventTypes::playerConnect:
        std::cout << "join " << event.playerIndex << std::endl;
        shape.setPosition(spawn);
        setActive(true);
    } else if (strncmp(event.type, "characterAt", 16) == 0) { // case eventTypes::characterAt:
        shape.setPosition(event.location);
    } else if (strncmp(event.type, "keypress", 16) == 0) { // case eventTypes::keyPress:
        //std::cout << "kpress " << event.playerIndex << std::endl;
        switch(event.key) {
        case sf::Keyboard::Key::S:
            // launch respawn event
            respawnEvent();
        break;
        /*
        case sf::Keyboard::Key::Left:
            // start move left
            leftMove = true;
        break;
        case sf::Keyboard::Key::Right:
            // start move right
            rightMove = true;
        break;
        case sf::Keyboard::Key::Up:
            // start jump
            shoot();
        break;
            */
        }
    } else if (strncmp(event.type, "playerleft", 16) == 0) {
        // start move left
        rightMove = false;
        leftMove = true;
    } else if (strncmp(event.type, "playerright", 16) == 0) {
        // start move right
        leftMove = false;
        rightMove = true;
    } else if (strncmp(event.type, "playerup", 16) == 0) {
        downMove = false;
        upMove = true;
    } else if (strncmp(event.type, "playerdown", 16) == 0) {
        upMove = false;
        downMove = true;
    }
}