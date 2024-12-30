#include <SFML/Graphics.hpp>
#include "player.h"
#include "timeline.h"
#include "eventManager.h"
#include <iostream>
#include <vector>
#include <bits/stdc++.h>
#include <thread>

#include <zmq.hpp>
#include <string>
#include "typedefs.h"

#include <v8.h>
#include <libplatform/libplatform.h>
#include <cstdio>
#include "v8helpers.h"
#include "ScriptManager.h"
#include "gameObject.h"
#include "scriptListener.h"
#include "enemyGroup.h"

#include <filesystem>

int main(int argc, char** argv)
{
	if (argc >= 2) {
		std::cout << argv[1] << std::endl;
		
		// Path to the directory
		std::string path = std::string(argv[1]) + "/";
		std ::cout << std::filesystem::exists(path) << std::endl;
	
		// Looping until all the items of the directory are
		// exhausted
		for (const auto& entry : std::filesystem::directory_iterator(path)) {
	
			// Converting the path to const char * in the
			// subsequent lines
			std::filesystem::path outfilename = entry.path();
			std::cout << entry.path().filename().string() << std::endl;;
			std::string outfilename_str = outfilename.string();
			const char* path = outfilename_str.c_str();
	
			std ::cout << path << std::endl;
		}
	}
  //  Prepare our context and socket
  zmq::context_t context(1);
  zmq::socket_t socket(context, zmq::socket_type::req);

  std::string serverAddress = "tcp://localhost:5555";
  std::string tcpThing = "tcp://localhost:";
  socket.connect (serverAddress);

  // Globals for game loop
  int clientID;
  Timeline localTime, serverTime;
  //TimeListener *timeListener = new TimeListener();
  GameState localState;

  //  Send a request to connect
  zmq::message_t request (sizeof(MessageType));
  MessageType type{"connect"};
  memcpy (request.data(), &type, sizeof(MessageType));
  std::cout << "Sending (request) connect ..." << std::endl;
  socket.send (request, zmq::send_flags::none);
  
  //  Get the reply.
  zmq::message_t reply;
  socket.recv(reply, zmq::recv_flags::none);
  MessageType *replyType = (MessageType *) reply.data();
  if (strncmp(replyType->type, "init", sizeof(MessageType)) == 0) {
      socket.disconnect(serverAddress);
      ClientInitialize *ci = (ClientInitialize *) (reply.data() + sizeof(MessageType));
      clientID = ci->playerIndex;
      EventManager::get().setPlayerIndex(clientID);

      tcpThing = "tcp://localhost:";
      tcpThing.append(ci->port);
      
      localState = ci->initState;
      std::srand(localState.time);
      serverTime.setTime(localState.time);
      EventManager::get().setEventTime(localState.time);
      
      EventManager::get().setTimeline(&serverTime);
      /*
      timeListener->setPlayerIndex(clientID);
      timeListener->setTimeline(&localTime);
      EventManager::get().addListener(timeListener);
      */

      socket.connect(tcpThing);
      std::cout << tcpThing << std::endl;
  } else {
      // there was an error, give up
      std::cout << "bad connection, quitting" << std::endl;
      
      socket.disconnect(serverAddress);
      socket.close();
      return 0;
  }



  // Render window used for high-level render functions
  sf::RenderWindow window(sf::VideoMode(800, 600), "A round snake... a caterpillar");
  std::vector<GameObject *> allObjects;
  {
    EnemyGroup *eg = new EnemyGroup();
    allObjects.push_back(eg);
    EventManager::get().addListener(eg);
  }
  /*
  { // set all platforms
    allObjects.push_back(new Platform(150, sf::Vector2f(30.0f, 260.0f), sf::Color::Yellow));
    allObjects.push_back(new Platform(150, sf::Vector2f(180.0f, 360.0f)));
    allObjects.push_back(new Platform(400, sf::Vector2f(380.0f, 460.0f)));
    allObjects.push_back(new MovingPlatform(150, sf::Vector2f(180.0f, 200.0f), sf::Vector2f(380.0f, 200.0f)));
    allObjects.push_back(new Platform(150, sf::Vector2f(-500.0f, 380.0f), sf::Color::Blue));
    allObjects.push_back(new Platform(150, sf::Vector2f(-330.0f, 240.0f), sf::Color::Green));
    allObjects.push_back(new Platform(150, sf::Vector2f(-150.0f, 140.0f), sf::Color::White));
    allObjects.push_back(new MovingPlatformVector(150, std::vector<sf::Vector2f>{sf::Vector2f(-600.0f, 450.0f), sf::Vector2f(-800.0f, 400.0f), sf::Vector2f(-950.0f, 500.0f), sf::Vector2f(-750.0f, 400.0f)}));
    allObjects.push_back(new KillPlane(1000.0f));
    MovingCamera *cameraView = new MovingCamera(800, 600);
    cameraView->registerBounds(new MovingCameraBounds(100.0f, 600.0f, sf::Vector2f(0.0f, 0.0f)));
    cameraView->registerBounds(new MovingCameraBounds(100.0f, 600.0f, sf::Vector2f(700.0f, 0.0f)));
    cameraView->registerBounds(new MovingCameraBounds(800.0f, 100.0f, sf::Vector2f(0.0f, 0.0f)));
    cameraView->registerBounds(new MovingCameraBounds(800.0f, 100.0f, sf::Vector2f(0.0f, 500.0f)));
    cameraView->setActive(true);
    window.setView(*cameraView);
    allObjects.push_back(cameraView);
  } */
  sf::Vector2f spawn(60.0f, 500.0f);
  Player* players[8];
  {
    for (int i = 0; i < 8; i++) {
      players[i] = new Player(i);
      players[i]->setActive(localState.playerConnected[i] && localState.playerAlive[i]);
      EventManager::get().addListener(players[i]); // every player is a listener
    }
    sf::Texture *playerTexture = new sf::Texture();
    if (!playerTexture->loadFromFile("orng.png")){
      std::cout << "File unable to be loaded: \"orng.png\"" << std::endl;
      return 1;
    }

    players[clientID]->getShape().setTexture(playerTexture);
    players[clientID]->setSpawn(spawn + sf::Vector2f(clientID*90, 0));
    players[clientID]->respawnEvent(); // broke it last time
  }

  const auto loopPhysics = [&localTime, &serverTime, &players, &clientID, &allObjects]() { // make into its own v8 script
    // update player position
    players[clientID]->lock();
    players[clientID]->gameMove(localTime);
    players[clientID]->pushPlayer(*players[clientID]); // removed earlier?
    players[clientID]->unlock();
    // Draw/update shapes and collide with new player position
    for (GameObject* obj : allObjects) {
      //std::cout << "loopphys" << std::endl;
      obj->lock();
      obj->gameMove(localTime); // in progress FIX
      obj->pushPlayer(*players[clientID]); // removed earlier?
      obj->unlock();
    }
  };

  // draw net players
  sf::CircleShape sp(30.0f);
  sp.setFillColor(sf::Color::Yellow);
  const auto loopDraw = [&sp, &window, &players, &localTime, &serverTime, &localState, &clientID, &allObjects]() {
    window.setActive(true);
    // draw everything
    window.clear(sf::Color(0x407020ff));
    // draw player
    players[clientID]->lock();
    players[clientID]->draw(window);
    players[clientID]->unlock();
    // Draw/update shapes and collide with new player position
    for (auto *obj : allObjects) {
      obj->lock();
      obj->draw(window);
      obj->unlock();
    }
    /*
    for (int i = 0; i < 8; i++) {
    // draw local player
      players[i]->lock();
      players[i]->draw(window);
      players[i]->unlock();
    } */
    // done drawing
    window.display();

    if (window.hasFocus()) {
      // check all the window's events that were triggered since the last iteration of the loop
      sf::Event event;
      while (window.pollEvent(event)) // go through every queued event
      {
        // "close requested" event: we close the window
        if (event.type == sf::Event::Closed) 
        {
          std::cout << "goodbye!" << std::endl;
          window.close();
        }
        else if (event.type == sf::Event::KeyPressed) {
          {
          NetworkEvent ev{};
          strncpy(ev.type, "keypress", 16);
          //ev.type = eventTypes::keyPress;
          ev.playerIndex = clientID;
          ev.time = EventManager::get().getLocalTime();
          ev.key = event.key.code;
          EventManager::get().raiseEvent(ev);
          }
        }
        else if (event.type == sf::Event::KeyReleased) {
          {
          NetworkEvent ev{};
          strncpy(ev.type, "keyrelease", 16);
          //ev.type = eventTypes::keyRelease;
          ev.playerIndex = clientID;
          ev.time = EventManager::get().getLocalTime();
          ev.key = event.key.code;
          EventManager::get().raiseEvent(ev);
          }
        }
      }
    }
    window.setActive(false);
  };

  float fixedFrametime = 0.02f;
  const auto loopWait = [&fixedFrametime]() {
    sf::Clock clock;
    float now = clock.getElapsedTime().asSeconds();
    while (clock.getElapsedTime().asSeconds() < now + fixedFrametime);
    return;
  };

  sf::Thread physicsThread(loopPhysics), drawThread(loopDraw), waitThread(loopWait);
  window.setActive(false);

  // init v8
  std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
  v8::V8::InitializePlatform(platform.release());
  v8::V8::InitializeICU();
  v8::V8::Initialize();
  v8::Isolate::CreateParams create_params;
  create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
  v8::Isolate *isolate = v8::Isolate::New(create_params);

  { // anonymous scope for managing handle scope
    v8::Isolate::Scope isolate_scope(isolate); // must enter the virtual machine to do stuff
    v8::HandleScope handle_scope(isolate);

    // Best practice to isntall all global functions in the context ahead of time.
    v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
    global->Set(isolate, "print", v8::FunctionTemplate::New(isolate, v8helpers::Print));
    global->Set(isolate, "gameobjectfactory", v8::FunctionTemplate::New(isolate, GameObject::ScriptedGameObjectFactory));
    global->Set(isolate, "GameObject", v8::FunctionTemplate::New(isolate, GameObject::ScriptedGameObjectFinder));
    global->Set(isolate, "gameobjecttest", v8::FunctionTemplate::New(isolate, GameObject::ScriptedMethodCallTest));
    global->Set(isolate, "raiseevent", v8::FunctionTemplate::New(isolate, EventManager::ScriptedRaiseEvent));
    //global->Set(isolate, "GameObjects", v8::Object::New(isolate)); // maybe this will work (it didn't work)
    global->Set(isolate, "ClientID", v8::Number::New(isolate, clientID)); // maybe this will work
    //default_context->Global()->Set(v8::String::NewFromUtf8(isolate, "ClientID"), v8::Number::New(isolate, clientID));
    v8::Local<v8::Context> default_context =  v8::Context::New(isolate, NULL, global);
    v8::Context::Scope default_context_scope(default_context); // enter the context
    ScriptManager *sm = new ScriptManager(isolate, default_context); 


    // add scripts
    sm->addLocalScript("handle_event", "scripts/handle_event.js");
    sm->addLocalScript("setup", "scripts/setup.js");
    // run setup
    sm->runOne("setup", false);
    // add scripted event listener
    ScriptListener *sl = new ScriptListener(sm, "handle_event", clientID);
    //sl->setPlayerIndex(clientID);
    EventManager::get().addListener(sl);


    // run the program as long as the window is open : MAIN LOOP
    int profileCounter = 0;
    float profileTime = 0.0f;
    std::vector<NetworkEvent> netQueue;
    
    while (window.isOpen())
    {
          profileCounter++;
          if (profileCounter >= 1000) {
            profileTime = localTime.getTime() ;
            profileCounter -= 1000;
          }
      waitThread.wait(); // keeps at fixedFrametime second long frames
      waitThread.launch();
      physicsThread.launch();
      drawThread.launch();
      
          zmq::message_t request (sizeof(MessageType)+sizeof(ClientUpdate) + sizeof(int)+sizeof(NetworkEvent)*netQueue.size());
          //if (netQueue.size() != 0) std::cout << "upd " << request.size() << " " << sizeof(MessageType) + sizeof(ClientUpdate) + sizeof(int) + sizeof(NetworkEvent)*netQueue.size() << " " << netQueue.size() << std::endl;
          MessageType type{"update"};
          memcpy (request.data(), &type, sizeof(MessageType));
          ClientUpdate cu{};
          players[clientID]->lock(); // maybe unnecessary
          cu.position = players[clientID]->getShape().getPosition();
          cu.active = players[clientID]->isActive();
          players[clientID]->unlock();
          cu.localTime = EventManager::get().getLocalTime();
          cu.eventTime = EventManager::get().getEventTime();
          memcpy (request.data() + sizeof(MessageType), &cu, sizeof(ClientUpdate));
          *(int*)(request.data() + sizeof(MessageType) + sizeof(ClientUpdate)) = netQueue.size();
          NetworkEvent *out = (NetworkEvent*)(request.data() + sizeof(MessageType) + sizeof(ClientUpdate) + sizeof(int));
          for (int i = netQueue.size() - 1; i >= 0; i--) {
            out[i] = netQueue.back();
            netQueue.pop_back();
          }
          //std::cout << "Sending (request) update ..." << std::endl;
          socket.send (request, zmq::send_flags::none);

          //  Get the reply.
          zmq::message_t reply;
          socket.recv (reply, zmq::recv_flags::none);
          MessageType *replyType = (MessageType *) reply.data();

          ServerUpdate *su = nullptr;
          int numEvents = 0;
          NetworkEvent *in;
          if (strncmp(replyType->type, "update2", sizeof(MessageType)) == 0) {
            // we have proper update data!
            su = (ServerUpdate *) (reply.data() + sizeof(MessageType));
            numEvents = *(int*)(reply.data() + sizeof(MessageType) + sizeof(ServerUpdate));
            in = (NetworkEvent*)(reply.data() + sizeof(MessageType) + sizeof(ServerUpdate) + sizeof(int));
            
            //std::cout << "Received \"" << gs->time << "\" " << std::endl;
          } else {
            // there was an error, do something
            zmq::message_t replyError (sizeof(MessageType));
            MessageType replyType{"error"};
            memcpy (replyError.data(), &replyType, sizeof(MessageType));
            socket.send (replyError, zmq::send_flags::none);
            // abort client
            std::cout << "server error, aborting client ..." << std::endl;
            drawThread.wait();
            physicsThread.wait();
            window.close();
            break;
          }
          //if (numEvents != 0) std::cout << "upd2 " << reply.size() << " " << sizeof(MessageType) + sizeof(ServerUpdate) + sizeof(int) + sizeof(NetworkEvent)*numEvents << " " << numEvents << std::endl;


      physicsThread.wait();
      drawThread.wait();
      //waitThread.wait(); // keeps at fixedFrametime second long frames
      //waitThread.launch();
      /* happens in init now
      if (localState.time <= 0) {
        localState = *serverState;
      }
      */
      float tick = -localState.time;
      if (su != nullptr) { // should always happen
        localState.time = su->localTime;
        EventManager::get().setEventTime(su->eventTime);
      }
      // time management
      tick += localState.time;
      localTime.update(tick);
      serverTime.setTime(localState.time); // synchronize with server
      if (localState.playerConnected[0]) {
        //std::cout << localState.playerPositions[0].x << ", " << localState.playerPositions[0].x << std::endl;

      }
      EventManager::get().addEventsInbox(numEvents, in);
      std::vector<NetworkEvent> temp = EventManager::get().eventsForNetwork();
      for (NetworkEvent &e : temp) {
        netQueue.push_back(e);
      }
      EventManager::get().handleEventQueue();
      //physicsThread.launch(); // moved up
      //drawThread.launch();
    }

    // closing client, disconnect
    zmq::message_t replyDisco (sizeof(MessageType));
    MessageType discoType{"disco"};
    memcpy (replyDisco.data(), &discoType, sizeof(MessageType));
    socket.send (replyDisco, zmq::send_flags::none);

    socket.disconnect(tcpThing);
    socket.close();
    //initSocket.close();

    std::cout << "last goodbye!" << std::endl;

      /*
      else if(c == 'e')
      {
        // TODO: make js template for event and use NewInstance
        v8::Local<v8::Object> js_event = v8::Object::New(isolate);
        // Populate event
        js_event->Set(v8::String::NewFromUtf8(isolate, "type"), v8::String::New(isolate, "playerspawn")); // changed this from number
        js_event->Set(v8::String::NewFromUtf8(isolate, "positionX"), v8::Number::New(isolate, 0));
        js_event->Set(v8::String::NewFromUtf8(isolate, "positionY"), v8::Number::New(isolate, 4));

        // inject event as global
        default_context->Global()->Set(v8::String::NewFromUtf8(isolate, "GameEvent"), js_event);

        sm->runOne("handle_event", false);
      }
      */
  }
  // clean v8
  isolate->Dispose();
  v8::V8::Dispose();
  v8::V8::ShutdownPlatform();

  return 0;
}
