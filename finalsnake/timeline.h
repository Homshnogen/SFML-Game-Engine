#pragma once
#include <vector>

class Timeline {
private:
    float time;
    float speed;
    bool paused;
    std::vector<Timeline*> children;
public:
    Timeline();
    void update(float elapsed);
    float getTime();
    void setTime(float newTime);
    void setSpeed(float newSpeed);
    void pause();
    bool isPaused();
    void unpause();
    void addChild(Timeline* child);
};