#pragma once
#include "player.fwd.h"
#include "gameObject.fwd.h"

#include <mutex>
//#include "player.h"
#include "timeline.h"
#include "typedefs.h"
#include <SFML/Graphics.hpp>

#include <utility>
#include <v8.h>
#include "ScriptManager.h"
#include <map>

class GameObject{
private:
    float localTime;
    std::mutex innerLock;
    sf::Shape *shape;
    bool active;
    bool visible;

    
    v8::Isolate* isolate;
    v8::Global<v8::Context>* context;

    /** Static variable to keep count of current number of GameObject instances */
    static int current_guid;
    
    /**
     * NOTE: These "Accessors" have to be **static**
     *
     * You will need to implement a setter and getter for every class
     * member variable you want accessible to javascript.
     */
    static void setGameObjectX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void getGameObjectX(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info); // note return type
    static void setGameObjectY(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void getGameObjectY(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info); // note return type
    static void setGameObjectActive(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void getGameObjectActive(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info); // note return type
    //static void setGameObjectGUID(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    //static void getGameObjectGUID(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info); // note return type
protected:
    void setShape(sf::Shape *s);
public:
    GameObject();
    GameObject(std::string name);
    ~GameObject();

    std::string guid;

    virtual void gameMove(Timeline &timeline); // remove, move to script (actually don't)
    virtual void pushPlayer(Player &player); // actually don't remove them
    float getTick(Timeline &timeline); // was to be called once per tick

    void setTime(float time);
    float getTime();

    virtual void draw(sf::RenderWindow &window);
    virtual void handleEvent(NetworkEvent &event);
    //sf::Shape* getShape();
    // make these script-accessible
    void setActive(bool a);
    bool isActive();
    void setVisible(bool a);
    bool isVisible();
    // c++ only
    inline void lock() {innerLock.lock();};
    inline void unlock() {innerLock.unlock();};
    
    /**
     * This function will make this class instance accessible to scripts in
     * the given context. 
     *
     * IMPORTANT: Please read this definition of this function in
     * GameObject.cpp. The helper function I've provided expects certain
     * parameters which you must use in order to take advance of this
     * convinience. 
     */
    v8::Local<v8::Object> exposeToV8(v8::Isolate *isolate, v8::Local<v8::Context> &context);

    /**
     * Static function to keep track of current total number of
     * gameobjects, to facilitate creating GUIDs by incrementing a counter.
     */
    static int getCurrentGUID();

    /**
     * Shared list of all instances of game objects...auto populated by the
     * constructor.
     */
    //static std::vector<GameObject*> game_objects;
    static std::map<std::string, GameObject*> game_objects;

    /**
     * Factor methods for creating new instances of GameObjects.
     *
     * This is primarily a demonstration of how you can create new objects
     * in javascript. NOTE: this is embedded in this GameObject class, but
     * could (and likely should) be used for other functionality as well
     * (i.e., Events). 
     */
    //static GameObject* GameObjectFactory(std::string context_name="default");
    static void ScriptedGameObjectFactory(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void ScriptedGameObjectFinder(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void ScriptedMethodCallTest(const v8::FunctionCallbackInfo<v8::Value>& args);
};