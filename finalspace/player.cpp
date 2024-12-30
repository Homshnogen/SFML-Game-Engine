#include "player.h"
#include <SFML/Graphics.hpp>

#include "eventManager.h"
#include "bullet.h"
#include <iostream>
#include <cstring>
#include <vector>

/*
Player::Player() : Player(-1){
}*/
Player::Player(int id) : shootCooldown(0.0f), children(), shape(30.0f), gravity(0.0f, 300.0f), spawn(), playerIndex(id), leftMove(false), rightMove(false), speed(160.0f), GameObject("player" + std::to_string(id)){
    shape.setFillColor(sf::Color(((0xf00f0000 >> (5*(id%3))) + (0x00aafaa0 << id) + (0x0003a7cf << (2*id))) | 0x000000ff));
    setShape(&shape);
    setActive(true);
    setVisible(true);
    //freefall = true;
}
Player::~Player() {
    for (auto *child : children) {
        delete child;
    }
}

sf::CircleShape &Player::getShape() {return shape;}

void Player::draw(sf::RenderWindow &window) {
    GameObject::draw(window);
    for (auto *child : children) {
        child->lock();
        child->draw(window);
        child->unlock();
    }
}

sf::Vector2f Player::getVelocity() {
    sf::Vector2f ret = sf::Vector2f();
    if (leftMove && shape.getPosition().x > 20) ret += sf::Vector2f(-speed, 0.0f);
    if (rightMove && shape.getPosition().x < 720) ret += sf::Vector2f(speed, 0.0f);
    if (rightMove && leftMove) {ret = sf::Vector2f(0.0f, 0.0f);}
    return ret;
}

void Player::gameMove(Timeline &timeline) {
    float tick = getTick(timeline);
    if (!isActive()) {
        timeline.pause();
        return;
    } else {
        timeline.unpause();
    }
    if (shootCooldown > 0.0f) {shootCooldown -= tick;};
    for (auto *del : junk) {
        junk.erase(del); // probably fine
        break; // just in case
        //delete del;
    }
    for (auto *child : children) {
        if (!child->isActive()) {
            child->lock();
            children.erase(child);
            junk.insert(child);
            child->unlock();
            break; // iterator is damaged, catch it next tick
            //delete child;
            //std::cout << "bye bullet!!" << std::endl;
        } else {
            child->gameMove(timeline);
        }
    }
    shape.move(tick*getVelocity());
}

void Player::shoot() {
    if (shootCooldown <= 0.1f) {
        shootCooldown = 0.7f;
       children.insert(syncTime(new Bullet({shape.getPosition().x+25.0f, shape.getPosition().y-15.0f}, true)));
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
        setActive(false);
    } else if (strncmp(event.type, "playerwin", 16) == 0) { // case eventTypes::characterDeath:
        std::cout << "You win! press 'S' to restart " << std::endl;
        setActive(false);
    } else if (strncmp(event.type, "disconnect", 16) == 0) { // case eventTypes::playerDisconnect:
        std::cout << "dc " << event.playerIndex << std::endl;
        setActive(false);
    } else if (strncmp(event.type, "playerspawn", 16) == 0) { // case eventTypes::characterSpawn:
        std::cout << "spn " << event.playerIndex << std::endl;
        spawn = event.location;
        for (auto *child : children) {
            child->setActive(false);
        }
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
        leftMove = true;
    } else if (strncmp(event.type, "playerright", 16) == 0) {
        // start move right
        rightMove = true;
    } else if (strncmp(event.type, "playershoot", 16) == 0) {
        shoot();
    } else if (strncmp(event.type, "playerleftstop", 16) == 0) {
        // stop move left
        leftMove = false;
    } else if (strncmp(event.type, "playerrightstop", 16) == 0) {
        // stop move right
        rightMove = false;
        /*
    } else if (strncmp(event.type, "keyrelease", 16) == 0) { // case eventTypes::keyRelease:
        //std::cout << "krelease " << event.playerIndex << std::endl;
        switch(event.key) {
        case sf::Keyboard::Key::Left:
            // stop move left
            leftMove = false;
        break;
        case sf::Keyboard::Key::Right:
            // stop move right
            rightMove = false;
        break;
        case sf::Keyboard::Key::Up:
            // start jump
            shoot();
        break;
        }
        */
    }
}