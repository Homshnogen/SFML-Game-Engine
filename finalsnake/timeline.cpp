#include "timeline.h"
#include <iostream>

Timeline::Timeline() : time(0), speed(1), paused(false), children(){

}
void Timeline::update(float elapsed) {
    if (paused) return;
    elapsed *= speed;
    // do update things
    time += elapsed;
    for (Timeline *child : children) {
        child->update(elapsed);
    }
}
float Timeline::getTime() {return time;}
void Timeline::setTime(float newTime) {time = newTime;}
void Timeline::setSpeed(float newSpeed) {speed = newSpeed;}
void Timeline::pause() {paused = true;}
bool Timeline::isPaused() {return paused;}
void Timeline::unpause() {paused = false;}
void Timeline::addChild(Timeline *child) {children.push_back(child);}