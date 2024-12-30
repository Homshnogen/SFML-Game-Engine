
if (GameEvent.type == "keypress") {
    switch(GameEvent.key) {
    case Key.Q:
        raiseevent("timeslow");
    break;
    case Key.W:
        raiseevent("timenormal");
    break;
    case Key.E:
        raiseevent("timefast");
    break;
    case Key.P:
        raiseevent("timepause");
    break;
    case Key.Left:
        raiseevent("playerleft");
    break;
    case Key.Right:
        raiseevent("playerright");
    break;
    case Key.Up:
        raiseevent("playerjump");
    break;
    case Key.L: // suspends the player in inactive state, for testing purposes
        let obj = GameObject("player"+ClientID);
        obj.active = false;
    break;
    }
} else if (GameEvent.type == "keyrelease") {
    switch(GameEvent.key) {
    case Key.Left:
        raiseevent("playerleftstop");
    break;
    case Key.Right:
        raiseevent("playerrightstop");
    break;
    case Key.Up:
        raiseevent("playerjump");
    break;
    case Key.L:
        let obj = GameObject("player"+ClientID);
        obj.active = true;
    break;
    }
} else if (GameEvent.type == "moveobject") {
    //let obj = gameobjectfactory(GameEvent.guid);
    let obj = GameObject(GameEvent.guid);
    obj.x = GameEvent.location.x;
    obj.y = GameEvent.location.y;
    obj.active = true;
    print("moved "+GameEvent.guid+" to "+GameEvent.location.x+","+GameEvent.location.y);
    /*
    print("succes c:");
    GameObjects.gameobject0.x = GameEvent.positionX;
    GameObjects.gameobject0.y = GameEvent.positionY;
    print("moved gameobject0 to "+GameEvent.positionX+","+GameEvent.positionY);
    */
}
/*
else {
    print("failure :c");
    for (let i in GameEvent) {
        print(i);
    }
}
*/