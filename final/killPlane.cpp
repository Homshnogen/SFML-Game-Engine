#include "killPlane.h"
#include <SFML/Graphics.hpp>

KillPlane::KillPlane(float y) : kill_y(y) {
}

void KillPlane::pushPlayer(Player &player) {
    if (player.isActive() && player.getShape().getPosition().y + 2.0f * player.getShape().getRadius() > kill_y) {
        player.killEvent();
    }
}

