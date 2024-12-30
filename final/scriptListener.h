#pragma once
#include "gameObject.h"
#include "ScriptManager.h"

class ScriptListener : public GameObject {
private:
    ScriptManager *sm;
    std::string handleScript;
    //int playerIndex;
public:
    ScriptListener(ScriptManager *sm, std::string sc);
    //void setPlayerIndex(int i);
    void handleEvent(NetworkEvent &event) override;
};