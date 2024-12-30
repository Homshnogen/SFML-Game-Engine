#pragma once
#include "gameObject.h"
#include "timeline.h"
#include "typedefs.h"
#include <list>
#include <mutex>
#include <v8.h>

class EventManager {
private:
    EventManager();
    std::list<GameObject*> listeners;
    std::list<NetworkEvent> events;
    std::list<NetworkEvent> eventsOutbox;
    float eventTime;
    Timeline *serverTime;
    int playerIndex;
    std::mutex writeEvents;
    bool canNetwork(NetworkEvent &event);
public:
    static EventManager& get();
    void setTimeline(Timeline *t);
    void setPlayerIndex(int i);
    float getLocalTime();
    float getEventTime();
    void setEventTime(float t);
    std::vector<NetworkEvent> eventsForNetwork();
    void addEventsInbox(int numEvents, NetworkEvent *eventsInbox);
    void raiseEvent(NetworkEvent event); // add an event to net queue
    void handleEventQueue(); // handle all raised events until eventTime
    void handleEvent(NetworkEvent &event); // send event to registered objects
    void addListener(GameObject *obj); // adds an object to listeners
    void removeListener(GameObject *obj); // removes an object from listeners

    static void ScriptedRaiseEvent(const v8::FunctionCallbackInfo<v8::Value>& args);
};