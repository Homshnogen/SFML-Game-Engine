#pragma once
#include "gameObject.h"
#include "timeline.h"

class TimeListener : public GameObject {
private:
    Timeline *localTime;
    int playerIndex;
public:
    TimeListener();
    void setTimeline(Timeline *t);
    void setPlayerIndex(int i);
    void handleEvent(NetworkEvent &event) override;
};