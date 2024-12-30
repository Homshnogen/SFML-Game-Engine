#pragma once
#include <SFML/System.hpp>

enum eventTypes { // outdated
    nullEvent,
    characterCollision,
    characterDeath,
    characterSpawn,
    keyPress,
    keyRelease,
    characterAt,
    playerConnect,
    playerDisconnect,
    clientInit
};

struct GameState {
    float time;
    bool playerConnected[8];
    bool playerAlive[8];
    sf::Vector2f playerPositions[8];
};
struct ClientUpdate {
    //int playerIndex;
    sf::Vector2f position; // maintains server state
    bool active; // new, maintains server state
    float eventTime; // new
    float localTime; // new
};
struct ClientInitialize {
    int playerIndex;
    char port[6];
    GameState initState; // new
};
struct MessageType {
    char type[8];
};
struct ServerUpdate { // new
    float eventTime;
    float localTime;
};
struct NetworkEvent { // new
    float time;
    int playerIndex;
    char type[16];
    char guid[16];
    //union {
        int key; // sf::Keyboard::Key
        sf::Vector2f location;
    //};
};