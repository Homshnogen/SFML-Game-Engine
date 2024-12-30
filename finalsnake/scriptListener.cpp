#include "scriptListener.h"
#include <v8.h>

ScriptListener::ScriptListener(ScriptManager *sm, std::string sc, int clientID) : sm(sm), handleScript(sc), clientID(clientID){
}
/*
void ScriptListener::setPlayerIndex(int i) { // init in constructor
    playerIndex = i;
} */
void ScriptListener::handleEvent(NetworkEvent &event) {
    if (event.playerIndex != clientID) {return;}

    v8::Isolate *isolate = sm->getIsolate();
	// TODO: make js template for event and use NewInstance
	v8::Local<v8::Object> js_event = v8::Object::New(isolate);
    // Populate event
    js_event->Set(v8::String::NewFromUtf8(isolate, "time"), v8::Number::New(isolate, event.time));
    js_event->Set(v8::String::NewFromUtf8(isolate, "playerIndex"), v8::Number::New(isolate, event.playerIndex));
	js_event->Set(v8::String::NewFromUtf8(isolate, "type"), v8::String::NewFromUtf8(isolate, event.type)); // changed this from number
	js_event->Set(v8::String::NewFromUtf8(isolate, "guid"), v8::String::NewFromUtf8(isolate, event.guid));
	js_event->Set(v8::String::NewFromUtf8(isolate, "key"), v8::Number::New(isolate, event.key));
    {
        v8::Local<v8::Object> location = v8::Object::New(isolate);
        location->Set(v8::String::NewFromUtf8(isolate, "x"), v8::Number::New(isolate, event.location.x));
        location->Set(v8::String::NewFromUtf8(isolate, "y"), v8::Number::New(isolate, event.location.y));
        js_event->Set(v8::String::NewFromUtf8(isolate, "location"), location);
    }
    // inject event as global
    //default_context->Global()->Set(v8::String::NewFromUtf8(isolate, "GameEvent"), js_event);
    sm->getContext()->Global()->Set(v8::String::NewFromUtf8(isolate, "GameEvent"), js_event);

	sm->runOne(handleScript, false);
}