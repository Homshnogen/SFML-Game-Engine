#pragma once
#include "gameObject.h"
#include "player.h"

class KillPlane : public GameObject {
private:
    float kill_y;
public:
    KillPlane(float y);
    void pushPlayer(Player &player);
};