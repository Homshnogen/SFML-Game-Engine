#include "eventManager.h"
#include <iostream>
#include <cstring>
#include "v8helpers.h"

EventManager::EventManager(): listeners(), events(), eventTime(), localTime(nullptr), writeEvents(), playerIndex(-1) {
}
EventManager& EventManager::get() {
    static EventManager instance;
    return instance;
}
void EventManager::raiseEvent(NetworkEvent event) { // add an event to net queue
    if (event.playerIndex != playerIndex) return; // do not trigger implicit events from other pcs (for now; see canNetwork)
    writeEvents.lock();
    for (auto i = eventsOutbox.begin(); i != eventsOutbox.end(); i++) {
        if (event.time < i->time) { // most should be equal, maybe inefficient
            eventsOutbox.insert(i, event);
            writeEvents.unlock();
            return;
        }
    }

    eventsOutbox.push_back(event); // maintain time-rank URGENTLY
    writeEvents.unlock();
    //return;
}
void EventManager::handleEventQueue() { // handle all raised events; call after eventsForNetwork
    //std::cout << eventTime << std::endl;
    while(!events.empty() && events.front().time <= eventTime) {
        handleEvent(events.front());
        events.pop_front();
    }
}
void EventManager::handleEvent(NetworkEvent &event) { // send event to registered objects
    //std::cout << event.type << std::endl;
    for (GameObject *listener : listeners) {
        listener->lock();
        listener->handleEvent(event);
        listener->unlock();
    }
}
void EventManager::addListener(GameObject *obj) { // adds an object to listeners
    listeners.push_back(obj); // don't add a listener twice pls
}
void EventManager::removeListener(GameObject *obj) { // removes an object from listeners
    listeners.remove(obj);
}
void EventManager::setTimeline(Timeline *t) {
    localTime = t;
}
Timeline *EventManager::getLocalTimeline() {
    return localTime;
}
void EventManager::setPlayerIndex(int i) {
    playerIndex = i;
}
float EventManager::getLocalTime() {
    return localTime ? localTime->getTime() : 0.0f; // will cause bad things if timeline is not set
}
float EventManager::getEventTime() {
    return eventTime;
}
void EventManager::setEventTime(float t) {
    eventTime = t;
}
bool EventManager::canNetwork(NetworkEvent &event) { // select which events are sent to server
    return true;
    //return (event.type == eventTypes::characterDeath) || (event.type == eventTypes::characterSpawn) || (event.type == eventTypes::characterCollision) || (event.type == eventTypes::playerDisconnect);
}
std::vector<NetworkEvent> EventManager::eventsForNetwork() {
    std::vector<NetworkEvent> ret;
    for (auto i = events.begin(); i != events.end(); i++) {
        while (!eventsOutbox.empty() && eventsOutbox.front().time < i->time) {
            events.insert(i, eventsOutbox.front());
            if (canNetwork(eventsOutbox.front())) ret.push_back(eventsOutbox.front());
            eventsOutbox.pop_front();
        }
        if (eventsOutbox.empty()) break;
    }
    while (!eventsOutbox.empty()) {
        events.push_back(eventsOutbox.front());
        if (canNetwork(eventsOutbox.front())) ret.push_back(eventsOutbox.front());
        eventsOutbox.pop_front();
    }
    return ret;
}
void EventManager::addEventsInbox(int numEvents, NetworkEvent *eventsInbox) {
    int j = 0;
    for (auto i = events.begin(); i != events.end(); i++) {
        while (j < numEvents && eventsInbox[j].time < i->time) {
            events.insert(i, eventsInbox[j]);
            j++;
        }
        if (j == numEvents) break;
    }
    while (j < numEvents) {
        events.push_back(eventsInbox[j]);
        j++;
    }
}


void EventManager::ScriptedRaiseEvent(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	//std::string context_name("default");
	if(args.Length() == 1) {// type
        v8::Isolate *isolate = args.GetIsolate();
        v8::Local<v8::Context> context = isolate->GetCurrentContext();
        v8::EscapableHandleScope handle_scope(args.GetIsolate());
        v8::Context::Scope context_scope(context);

        NetworkEvent new_event;
        new_event.time = get().getLocalTime();
        new_event.playerIndex = get().playerIndex;
        strncpy(new_event.type, v8helpers::ToCString(v8::String::Utf8Value(isolate, args[0])), 16);
        //strncpy(new_event.guid, v8helpers::ToCString(v8::String::Utf8Value(isolate, args[3])), 16);
        //new_event.location.x = args[1]->NumberValue();
        //new_event.location.y = args[2]->NumberValue();
        //new_event.key;

        get().raiseEvent(new_event);
	} else if(args.Length() == 2) {// type, guid
        v8::Isolate *isolate = args.GetIsolate();
        v8::Local<v8::Context> context = isolate->GetCurrentContext();
        v8::EscapableHandleScope handle_scope(args.GetIsolate());
        v8::Context::Scope context_scope(context);

        NetworkEvent new_event;
        new_event.time = get().getLocalTime();
        new_event.playerIndex = get().playerIndex;
        strncpy(new_event.type, v8helpers::ToCString(v8::String::Utf8Value(isolate, args[0])), 16);
        strncpy(new_event.guid, v8helpers::ToCString(v8::String::Utf8Value(isolate, args[3])), 16);
        //new_event.location.x = args[1]->NumberValue();
        //new_event.location.y = args[2]->NumberValue();
        //new_event.key;

        get().raiseEvent(new_event);
	} else if(args.Length() == 3) { // type, location.x, location.y
        v8::Isolate *isolate = args.GetIsolate();
        v8::Local<v8::Context> context = isolate->GetCurrentContext();
        v8::EscapableHandleScope handle_scope(args.GetIsolate());
        v8::Context::Scope context_scope(context);

        NetworkEvent new_event;
        new_event.time = get().getLocalTime();
        new_event.playerIndex = get().playerIndex;
        strncpy(new_event.type, v8helpers::ToCString(v8::String::Utf8Value(isolate, args[0])), 16);
        //strncpy(new_event.guid, v8helpers::ToCString(v8::String::Utf8Value(isolate, args[3])), 16);
        new_event.location.x = args[1]->NumberValue();
        new_event.location.y = args[2]->NumberValue();
        //new_event.key;

        get().raiseEvent(new_event);
	} else if(args.Length() == 4) { // type, guid, location.x, location.y
        v8::Isolate *isolate = args.GetIsolate();
        v8::Local<v8::Context> context = isolate->GetCurrentContext();
        v8::EscapableHandleScope handle_scope(args.GetIsolate());
        v8::Context::Scope context_scope(context);

        NetworkEvent new_event;
        new_event.time = get().getLocalTime();
        new_event.playerIndex = get().playerIndex;
        strncpy(new_event.type, v8helpers::ToCString(v8::String::Utf8Value(isolate, args[0])), 16);
        strncpy(new_event.guid, v8helpers::ToCString(v8::String::Utf8Value(isolate, args[3])), 16);
        new_event.location.x = args[1]->NumberValue();
        new_event.location.y = args[2]->NumberValue();
        //new_event.key;

        get().raiseEvent(new_event);
	}
}