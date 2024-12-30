#include "timeListener.h"

TimeListener::TimeListener() : localTime(nullptr), playerIndex(-1) {
}
void TimeListener::setTimeline(Timeline *t) {
    localTime = t;
}
void TimeListener::setPlayerIndex(int i) {
    playerIndex = i;
}
void TimeListener::handleEvent(NetworkEvent &event) {
    if (localTime && (event.playerIndex == playerIndex)) {
        if (event.type == "timeslow") {
            localTime->setSpeed(0.5f);
        } else if (event.type == "timenormal") {
            localTime->setSpeed(1.0f);
        } else if (event.type == "timefast") {
            localTime->setSpeed(2.0f);
        } else if (event.type == "timepause") {
            if (localTime->isPaused()) {
                localTime->unpause();
            } else {
                localTime->pause();
            }
        }
    }
    
    /*
    //if (localTime && (event.playerIndex == playerIndex) && (event.type == eventTypes::keyPress)) {
    if (localTime && (event.playerIndex == playerIndex) && (event.type == "keypress")) {
        switch (event.key) {
        case sf::Keyboard::Key::Q:
            localTime->setSpeed(0.5f);
        break;
        case sf::Keyboard::Key::W:
            localTime->setSpeed(1.0f);
        break;
        case sf::Keyboard::Key::E:
            localTime->setSpeed(2.0f);
        break;
        case sf::Keyboard::Key::P:
            if (localTime->isPaused()) {
                localTime->unpause();
            } else {
                localTime->pause();
            }
        break;
        }
    }
    */
}