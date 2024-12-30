Compilation: Compile with the make command (using the given makefile)
Execution: Run by calling the client with (./client) or the server with (./server)
    The server must be active for the client to connect. 8 clients can be connected at once, however they are unable to interact with eachother, so the game can be considered singleplayer
    In this game, the player snake head cannot collide with the first two directly connected body pieces. This is because in snake, the snake cannot collide with its closest three body parts, and circles have a different tangency from squares which makes 2 a more reasonable number.

Controls:
    Left, Right, Up, and Down arrows: Set the direction of movement. The character does not stop moving (per snake rules)
    S: Restart the game
    L: Hold to suspend the player in an inactive state and pause the game. This is just to test the scripting capabilities for event management
