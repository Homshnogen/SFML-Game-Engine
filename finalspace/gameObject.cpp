#include "gameObject.h"
#include <iostream>

#include <filesystem>
#include <cstring>
#include "v8helpers.h"

/** Definitions of static class members */
int GameObject::current_guid = 0;
std::map<std::string, GameObject*> GameObject::game_objects;

GameObject::GameObject() : GameObject("gameobject" + std::to_string(current_guid++)) {

}
GameObject::GameObject(std::string name) : localTime(0), innerLock(), guid(name), shape(nullptr){
    //game_objects.push_back(this);
	game_objects[guid] = this;
}
GameObject::~GameObject() {
	this->lock();
    std::cout << "bye!" << this->guid << std::endl;
	game_objects.erase(this->guid);
	this->unlock();
}

void GameObject::gameMove(Timeline &timeline) {
 // make this not exist eventually, move to script
}
void GameObject::pushPlayer(Player &player) {
 // make this not exist eventually, move to script
}
float GameObject::getTick(Timeline &timeline) {
    float tick = timeline.getTime() - localTime;
    setTime(timeline.getTime());
    return tick;
}
void GameObject::setTime(float time) {
    localTime = time;
}
float GameObject::getTime() {
    return localTime;
}
void GameObject::draw(sf::RenderWindow &window) {
    if (!(visible && active && shape)) return;
    //std::cout << guid << std::endl;
    window.draw(*shape);
}
void GameObject::handleEvent(NetworkEvent &event) {

}
void GameObject::setShape(sf::Shape *s) {
    shape = s;
}
/*
sf::Shape* GameObject::getShape() {
    return shape;
}*/
void GameObject::setActive(bool a) {
    active = a;
}
bool GameObject::isActive() {
    return active;
}
void GameObject::setVisible(bool a) {
    visible = a;
}
bool GameObject::isVisible() {
    return visible;
}
GameObject *GameObject::syncTime(GameObject *child) {
	child->localTime = localTime;
	game_objects.erase(child->guid);
    return child;
}

v8::Local<v8::Object> GameObject::exposeToV8(v8::Isolate *isolate, v8::Local<v8::Context> &context)
{
	std::vector<v8helpers::ParamContainer<v8::AccessorGetterCallback, v8::AccessorSetterCallback>> v;
	v.push_back(v8helpers::ParamContainer("x", getGameObjectX, setGameObjectX));
	v.push_back(v8helpers::ParamContainer("y", getGameObjectY, setGameObjectY));
	v.push_back(v8helpers::ParamContainer("active", getGameObjectActive, setGameObjectActive));
	//v.push_back(v8helpers::ParamContainer("guid", getGameObjectGUID, setGameObjectGUID));
	return v8helpers::exposeToV8("GameObjects", guid, this, v, isolate, context);
}

void GameObject::setGameObjectX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	GameObject* obj = static_cast<GameObject*> (wrap->Value());
	obj->shape->setPosition(value->Int32Value(), obj->shape->getPosition().y);
}

void GameObject::getGameObjectX(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	GameObject* obj = static_cast<GameObject*> (wrap->Value());
	int x_val = obj->shape->getPosition().x;
	info.GetReturnValue().Set(x_val);
}

void GameObject::setGameObjectY(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	GameObject* obj = static_cast<GameObject*> (wrap->Value());
	obj->shape->setPosition(obj->shape->getPosition().x, value->Int32Value());
}

void GameObject::getGameObjectY(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	GameObject* obj = static_cast<GameObject*> (wrap->Value());
	int y_val = obj->shape->getPosition().y;
	info.GetReturnValue().Set(y_val);
}

void GameObject::setGameObjectActive(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	GameObject* obj = static_cast<GameObject*> (wrap->Value());
	obj->setActive(value->BooleanValue());
}

void GameObject::getGameObjectActive(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	GameObject* obj = static_cast<GameObject*> (wrap->Value());
	info.GetReturnValue().Set(obj->isActive());
}

/*
void GameObject::setGameObjectGUID(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	GameObject* obj = static_cast<GameObject*> (wrap->Value());
	v8::String::Utf8Value utf8_str(info.GetIsolate(), value->ToString());
	obj->guid = *utf8_str;
}

void GameObject::getGameObjectGUID(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	GameObject* obj = static_cast<GameObject*> (wrap->Value());
	v8::Local<v8::String> v8_guid = v8::String::NewFromUtf8(info.GetIsolate(), obj->guid.c_str());
	info.GetReturnValue().Set(v8_guid);
}
*/


/**
 * Factory method for allowing javascript to create instances of native game
 * objects
 *
 * NOTE: Like with the setters above, this static function does have a return
 * type (and object), but the return value is placed in the function callback
 * parameter, not the native c++ return type.
 */
void GameObject::ScriptedGameObjectFactory(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	ScriptedGameObjectFinder(args);
	if (args.GetReturnValue().Get()->IsNullOrUndefined()) {
		v8::Isolate *isolate = args.GetIsolate();
		v8::Local<v8::Context> context = isolate->GetCurrentContext();
		v8::EscapableHandleScope handle_scope(args.GetIsolate());
		v8::Context::Scope context_scope(context);

		GameObject *new_object;

		//std::string context_name("default");
		if(args.Length() == 1) { // the name of the object
			v8::String::Utf8Value str(isolate, args[0]);
			new_object = new GameObject(std::string(v8helpers::ToCString(str)));
		} else {
			new_object = new GameObject();
		}
		v8::Local<v8::Object> v8_obj = new_object->exposeToV8(isolate, context);
		args.GetReturnValue().Set(handle_scope.Escape(v8_obj));
	}
}
/*
void GameObject::ScriptedGameObjectFactory(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::Isolate *isolate = args.GetIsolate();
	v8::Local<v8::Context> context = isolate->GetCurrentContext();
	v8::EscapableHandleScope handle_scope(args.GetIsolate());
	v8::Context::Scope context_scope(context);

	GameObject *new_object;

	//std::string context_name("default");
	if(args.Length() == 1) // the name of the object
	{
		v8::String::Utf8Value str(isolate, args[0]);
		v8::Local<v8::String> go_key = v8::String::NewFromUtf8(isolate, "GameObjects");
		if (context->Global()->Has(go_key) && context->Global()->Get(go_key)->ToObject()->Has(args[0])) { // redundant check
			// guid is not unique, return existing object
			std::cout << "GameObject '" << v8helpers::ToCString(str) << "' already exists inside v8, returning reference" << std::endl;
			v8::Local<v8::Value> v8_obj = context->Global()->Get(go_key)->ToObject()->Get(args[0]);
			args.GetReturnValue().Set(handle_scope.Escape(v8_obj));
			return;
		} else {
			auto it = game_objects.find(v8helpers::ToCString(str));
			if (it != game_objects.end()) {
				// guid is not unique but hasn't been exposed yet
				std::cout << "GameObject '" << v8helpers::ToCString(str) << "' already exists outside v8, creating internal reference" << std::endl;
				new_object = it->second;
				v8::Local<v8::Value> v8_obj = new_object->exposeToV8(isolate, context);
				//context->Global()->Get(go_key)->ToObject()->Set(args[0], v8_obj); // always redundant to expose
				args.GetReturnValue().Set(handle_scope.Escape(v8_obj));
				return;
			}
		}
		new_object = new GameObject(std::string(v8helpers::ToCString(str)));
		//context_name = std::string(v8helpers::ToCString(str));
	} else {
		new_object = new GameObject();
	}
#if GO_DEBUG
		std::cout << "Created new object in context " << context_name << std::endl;
#endif

	v8::Local<v8::Object> v8_obj = new_object->exposeToV8(isolate, context);
	args.GetReturnValue().Set(handle_scope.Escape(v8_obj));
} */

void GameObject::ScriptedMethodCallTest(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	if(args.Length() != 1) return; // improper arg
	v8::Isolate *isolate = args.GetIsolate();
	v8::Local<v8::Context> context = isolate->GetCurrentContext();
	v8::EscapableHandleScope handle_scope(args.GetIsolate());
	v8::Context::Scope context_scope(context);

	GameObject *obj = static_cast<GameObject*>(v8::Local<v8::External>::Cast(v8::Local<v8::Object>::Cast(args[0])->GetInternalField(0))->Value());
	std::cout << obj << std::endl;

	//args.GetReturnValue().Set(handle_scope.Escape(v8_obj));
}

void GameObject::ScriptedGameObjectFinder(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::Isolate *isolate = args.GetIsolate();
	v8::Local<v8::Context> context = isolate->GetCurrentContext();
	v8::EscapableHandleScope handle_scope(args.GetIsolate());
	v8::Context::Scope context_scope(context);

	//std::string context_name("default");
	if(args.Length() == 1) // the name of the object
	{
		v8::String::Utf8Value str(isolate, args[0]);
		auto it = game_objects.find(v8helpers::ToCString(str));
		if (it != game_objects.end() && it->second != nullptr) {
			// guid is not unique but hasn't been exposed yet
			std::cout << "GameObject '" << v8helpers::ToCString(str) << "' found" << std::endl;
			GameObject *found_object = it->second;
			v8::Local<v8::Value> v8_obj = found_object->exposeToV8(isolate, context);
			args.GetReturnValue().Set(handle_scope.Escape(v8_obj));
			return;
		} else {
			std::cout << "GameObject '" << v8helpers::ToCString(str) << "' not found" << std::endl;
		}
	}
}