
#include <zmq.hpp>
#include <zmq_addon.hpp>
#include <string>
#include <iostream>
#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>

#define sleep(n)	Sleep(n)
#endif

#include "typedefs.h"
#include <thread>
#include <mutex>
#include <list>


std::mutex gsLock;
GameState gs;

class ServerEventManager {
private:
    float eventTimes[8]; // give events after this time
    float localTimes[8]; // raise client local time for min
    std::list<NetworkEvent> events;
    std::mutex write;
    ServerEventManager() : events(), write() {
    }
public:
    static ServerEventManager &get() {
        static ServerEventManager instance;
        return instance;
    }
    void updateLocalTime(int playerIndex, float localTime) { // 1
        write.lock();
        localTimes[playerIndex] = localTime;
        write.unlock();
    }
    void raiseEvent(NetworkEvent event) {
        write.lock();
        for (auto i = events.begin(); i != events.end(); i++) { // forward iteration, may be less efficient
            if (event.time < i->time) { // sort by time
                events.insert(i, event);
                write.unlock();
                return;
            }
        }
        events.push_back(event);
        write.unlock();
        //return;
    }
    void addEvents(int numEvents, NetworkEvent *updateEvents) { // 2
        write.lock();
        // updateEvents is sorted
        int j = 0;
        for (auto i = events.begin(); i != events.end(); i++) { // forward iteration, may be less efficient
            while (updateEvents[j].time < i->time && j < numEvents) { // sort by time
                events.insert(i, updateEvents[j]);
                j++;
            }
            if (j == numEvents) break;
        }
        while (j < numEvents) {
            events.push_back(updateEvents[j]);
            j++;
        }
        write.unlock();
    }
    std::vector<NetworkEvent> getEvents(int playerIndex, float eventTime) { // 3
        write.lock();
        //std::cout << "size " << events.size() << std::endl;

        std::vector<NetworkEvent> ret;
        float newEventTime = minLocalTime();
        //std::cout << "local " << newEventTime << std::endl;
        float deadEventTime = minEventTime() - 0.05;
        //std::cout << "event " << deadEventTime << std::endl;
        while (!events.empty() && events.front().time < deadEventTime) {
            //std::cout << "bye" << std::endl;
            events.pop_front();
        }
        for (auto i = events.begin(); i != events.end(); i++) {
            if (i->time <= eventTime) {
            } else if (i->time <= newEventTime) {
                if (i->playerIndex != playerIndex) ret.push_back(*i); // don't send back events
            } else {
                break;
            }
        }
        eventTimes[playerIndex] = newEventTime;
        write.unlock();
        return ret;
    }
    float minLocalTime() { // 4
        int i;
        float min;

        gsLock.lock();
        for (i = 0; i < 8; i++) {
            if (gs.playerConnected[i]) break;
        }
        if (i == 8) min = 0.0f; // severe error
        min = localTimes[i];
        for (int i = i + 1; i < 8; i++) {
            if (gs.playerConnected[i] && min > localTimes[i]) min = localTimes[i];
        }
        gsLock.unlock();
        
        return min;
    }
    float minEventTime() {
        int i;
        float min;

        gsLock.lock();
        for (i = 0; i < 8; i++) {
            if (gs.playerConnected[i]) break;
        }
        if (i == 8) {
            min = 0.0f; // severe error
            //std::cout << "oh noooo" << std::endl;
        }
        min = eventTimes[i];
        for (int i = i + 1; i < 8; i++) {
            if (gs.playerConnected[i] && min > eventTimes[i]) min = eventTimes[i];
        }
        gsLock.unlock();

        return min;
    }
};

int main () {
    //  Prepare our context and socket
    zmq::context_t context (2);
    zmq::socket_t socket (context, zmq::socket_type::rep);
    sf::Clock clock;
    {
        gs.time = 0;
        for (int i = 0; i < 8; i++) {
            gs.playerConnected[i] = false;
        }
    }
    socket.bind ("tcp://*:5555");
    const char ports[8][6] = {"5556", "5557", "5558", "5559", "5560", "5561", "5562", "5563"};
    std::mutex ccActive[8];
    sf::Thread *ccThreads[8];
    const auto clientConnection = [&context, &ccActive, &ports, &clock](int clientIndex) {
        ccActive[clientIndex].lock();
        // make socket
        zmq::socket_t socket(context, zmq::socket_type::rep);
        //socket.setsockopt(ZMQ_RCVTIMEO, 2000); // SHOULD set timeout to 2 seconds
        std::string tcpThing = "tcp://*:";
        tcpThing.append(ports[clientIndex]);
                    std::cout << tcpThing << std::endl;
        socket.bind(tcpThing);
        std::cout << "connection " << clientIndex << " created!" << std::endl;

        gsLock.lock();
        gs.playerConnected[clientIndex] = true;
        gs.playerAlive[clientIndex] = false;
        gsLock.unlock();
        { // moved player connect here
            NetworkEvent event{};
            strncpy(event.type, "connect", 16);
            //event.type = eventTypes::playerConnect;
            event.playerIndex = clientIndex;
            event.time = gs.time;
            ServerEventManager::get().raiseEvent(event);
        }

        bool connected = true;
        float timeout = clock.getElapsedTime().asSeconds();
        while (connected) {
            zmq::message_t request;
            float time = clock.getElapsedTime().asSeconds();
            socket.recv(request, zmq::recv_flags::dontwait); // no block
            if (request.size() == 0) { // no message received
                if (time > timeout + 5.0f) { // timeout expired, disconnect
                    /*
                    // send a disconnect message, just in case
                    zmq::message_t reply (sizeof(MessageType));
                    MessageType replyType{"disco"};
                    memcpy (reply.data(), &replyType, sizeof(MessageType));
                    std::cout << "tries to send before receive" << std::endl;
                    socket.send (reply, zmq::send_flags::none); // tries to send before receive
                    */
                    // end thread
                    std::cout << "connection " << clientIndex << " timed out, disconnecting ..." << std::endl;
                    
                    connected = false;
                }
                continue;
            }
            // message received!
            timeout = time;

            MessageType *type = (MessageType *) request.data();
            // share game state info
            if (strncmp(type->type, "update", sizeof(MessageType)) == 0) {
                // update server state
                ClientUpdate *cu = (ClientUpdate *) (request.data() + sizeof(MessageType));
                int numEvents = *(int*)(request.data() + sizeof(MessageType) + sizeof(ClientUpdate));
                //if (numEvents != 0) std::cout << "upd " << request.size() << " " << sizeof(MessageType) + sizeof(ClientUpdate) + sizeof(int) + sizeof(NetworkEvent)*numEvents << " " << numEvents << std::endl;
                NetworkEvent *in = (NetworkEvent*)(request.data() + sizeof(MessageType) + sizeof(ClientUpdate) + sizeof(int));

                gsLock.lock();
                gs.time = time; // time in seconds
                gs.playerPositions[clientIndex] = cu->position;
                gs.playerAlive[clientIndex] = cu->active;
                gsLock.unlock();
                {
                    NetworkEvent event{};
                    strncpy(event.type, "characterAt", 16);
                    //event.type = eventTypes::characterAt;
                    event.playerIndex = clientIndex;
                    event.time = gs.time;
                    event.location = cu->position;
                    ServerEventManager::get().raiseEvent(event);
                }
                ServerEventManager::get().updateLocalTime(clientIndex, cu->localTime); // 1
                ServerEventManager::get().addEvents(numEvents, in); // 2
                std::vector<NetworkEvent> outbox = ServerEventManager::get().getEvents(clientIndex, cu->eventTime); // 3

                //  Send state back to client

                zmq::message_t reply (sizeof(MessageType)+sizeof(ServerUpdate) + sizeof(int)+sizeof(NetworkEvent)*outbox.size());
                //if (outbox.size() != 0) std::cout << "upd2 " << reply.size() << " " << sizeof(MessageType) + sizeof(ServerUpdate) + sizeof(int) + sizeof(NetworkEvent)*outbox.size() << " " << outbox.size() << std::endl;
                MessageType replyType{"update2"};
                memcpy (reply.data(), &replyType, sizeof(MessageType));
                ServerUpdate su{};
                su.eventTime = ServerEventManager::get().minLocalTime(); // 4
                su.localTime = time;
                memcpy (reply.data()+sizeof(MessageType), &su, sizeof(ServerUpdate));
                *(int*)(reply.data()+sizeof(MessageType)+sizeof(ServerUpdate)) = outbox.size();
                NetworkEvent *out = (NetworkEvent*)(reply.data() + sizeof(MessageType) + sizeof(ServerUpdate) + sizeof(int));
                for (int i = outbox.size()-1; i >= 0; i--) {
                    out[i] = outbox.back();
                    outbox.pop_back();
                }

                socket.send (reply, zmq::send_flags::none);
            } else if (strncmp(type->type, "disco", sizeof(MessageType)) == 0) {
                // disconnect
                connected = false;
                // send a disconnect message, just in case
                zmq::message_t reply (sizeof(MessageType));
                MessageType replyType{"disco"};
                memcpy (reply.data(), &replyType, sizeof(MessageType));
                socket.send (reply, zmq::send_flags::none);
            } else {
                // don't know how to handle message / bad data
                // send an error message to indicate error on receipt
                zmq::message_t reply (sizeof(MessageType));
                MessageType replyType{"error"};
                memcpy (reply.data(), &replyType, sizeof(MessageType));
                socket.send (reply, zmq::send_flags::none);
            }
        }
        // disconnect player
        gsLock.lock();
        gs.playerConnected[clientIndex] = false;
        {
            NetworkEvent event{};
            strncpy(event.type, "disconnect", 16);
            //event.type = eventTypes::playerDisconnect;
            event.playerIndex = clientIndex;
            event.time = clock.getElapsedTime().asSeconds();
            ServerEventManager::get().raiseEvent(event);
        }
        gsLock.unlock();
        socket.close();
        std::cout << "connection " << clientIndex << " broken!" << std::endl;
        ccActive[clientIndex].unlock();
    };
    for (int i = 0; i < 8; i++) {
        ccThreads[i] = new sf::Thread(clientConnection, i);
    }

    std::cout << "Starting server..." << std::endl;
    while (true) {
        zmq::message_t request;
        socket.recv(request, zmq::recv_flags::dontwait); // no block
        if (request.size() == 0) continue; // no message received

        MessageType *type = (MessageType *) request.data();
        if (strncmp(type->type, "connect", sizeof(MessageType)) == 0) {
            int clientIndex;
            for (clientIndex = 0; clientIndex < 8; clientIndex++) {
                // check all of the ports
                if (ccActive[clientIndex].try_lock()) { // connection available!
                    //  Send connection data back to client
                    zmq::message_t reply (sizeof(MessageType)+sizeof(ClientInitialize));
                    MessageType replyType{"init"};
                    memcpy (reply.data(), &replyType, sizeof(MessageType));
                    ClientInitialize *init = (ClientInitialize *) (reply.data()+sizeof(MessageType));
                    init->playerIndex = clientIndex;
                    strncpy(init->port, ports[clientIndex], sizeof(init->port));
                    gs.time = clock.getElapsedTime().asSeconds();
                    init->initState = gs;
                    socket.send (reply, zmq::send_flags::none);
                    // start thread for connection
                    ccThreads[clientIndex]->launch();
                    ccActive[clientIndex].unlock(); // pass over lock
                    break;
                }
            } 
            if (clientIndex >= 8) { // all connections are busy
                // send error
                zmq::message_t reply (sizeof(MessageType));
                MessageType replyType{"error"};
                memcpy (reply.data(), &replyType, sizeof(MessageType));
                socket.send (reply, zmq::send_flags::none);
            }

        } else {
            // message in bad format, send error
            zmq::message_t reply (sizeof(MessageType));
            MessageType replyType{"error"};
            memcpy (reply.data(), &replyType, sizeof(MessageType));
            socket.send (reply, zmq::send_flags::none);

        }
    }
    for (int i = 0; i < 8; i++) {
        ccThreads[i]->wait();
    }
    return 0;
}
