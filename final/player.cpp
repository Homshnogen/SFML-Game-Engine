#include "player.h"
#include <SFML/Graphics.hpp>

#include "eventManager.h"

#include <iostream>
#include <cstring>

/*
Player::Player() : Player(-1){
}*/
Player::Player(int id) : shape(30.0f), gravity(0.0f, 300.0f), upVelocity(0.0f, 0.0f), spawn(), playerIndex(id), leftMove(false), rightMove(false), speed(100.0f), GameObject("player" + std::to_string(id)){
    shape.setFillColor(sf::Color(0xff9900ff));
    setShape(&shape);
    setActive(true);
    setVisible(true);
    //freefall = true;
}

sf::CircleShape &Player::getShape() {return shape;}

sf::Vector2f Player::getVelocity() {
    sf::Vector2f ret = upVelocity;
    if (leftMove) ret += sf::Vector2f(-speed, 0.0f);
    if (rightMove) ret += sf::Vector2f(speed, 0.0f);
    return ret;
}
void Player::setVelocity(const sf::Vector2f &v) {
    upVelocity.y = v.y;
}

void Player::gameMove(Timeline &timeline) {
    float tick = getTick(timeline);
    if (!isActive()) return;
    shape.move(tick*getVelocity());
    setVelocity(getVelocity() + tick*gravity);
    // set freefall true before collisions
    freefall = true;
}

void Player::jump() {
    if (freefall) return;
    freefall = true;
    setVelocity(sf::Vector2f(0.0f, -280.0f));
}
void Player::land() {
    freefall = false;
    setVelocity(sf::Vector2f(0.0f, 1.0f));
}
void Player::setSpawn(sf::Vector2f sp) {
    spawn = sp;
}
void Player::respawnEvent() {
    NetworkEvent ev{};
    strncpy(ev.type, "playerspawn", 16);
    //ev.type = eventTypes::characterSpawn;
    ev.playerIndex = playerIndex;
    ev.time = EventManager::get().getLocalTime();
    ev.location = spawn;
    EventManager::get().raiseEvent(ev);
}
void Player::killEvent() {
    NetworkEvent ev{};
    strncpy(ev.type, "playerdeath", 16);
    //ev.type = eventTypes::characterDeath;
    ev.playerIndex = playerIndex;
    ev.time = EventManager::get().getLocalTime();
    ev.location = shape.getPosition();
    EventManager::get().raiseEvent(ev);
}
void Player::collideEvent() {
    NetworkEvent ev{};
    strncpy(ev.type, "playercollide", 16);
    //ev.type = eventTypes::characterCollision;
    ev.playerIndex = playerIndex;
    ev.time = EventManager::get().getLocalTime();
    ev.location = shape.getPosition();
    EventManager::get().raiseEvent(ev);
}
int Player::getID() {
    return playerIndex;
}
void Player::handleEvent(NetworkEvent &event) {
    if (event.playerIndex != playerIndex) return;
    if (strncmp(event.type, "playerdeath", 16) == 0) { // case eventTypes::characterDeath:
        std::cout << "kill " << event.playerIndex << std::endl;
        setActive(false);
    } else if (strncmp(event.type, "disconnect", 16) == 0) { // case eventTypes::playerDisconnect:
        std::cout << "dc " << event.playerIndex << std::endl;
        setActive(false);
    } else if (strncmp(event.type, "playerspawn", 16) == 0) { // case eventTypes::characterSpawn:
        std::cout << "spn " << event.playerIndex << std::endl;
        spawn = event.location;
        setVelocity(sf::Vector2f(0.0f, 0.0f));
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
        setVelocity(sf::Vector2f(0.0f, 0.0f));
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
            jump();
        break;
            */
        }
    } else if (strncmp(event.type, "playerleft", 16) == 0) {
        // start move left
        leftMove = true;
    } else if (strncmp(event.type, "playerright", 16) == 0) {
        // start move right
        rightMove = true;
    } else if (strncmp(event.type, "playerjump", 16) == 0) {
        // start jump
        jump();
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
            jump();
        break;
        }
        */
    }
}