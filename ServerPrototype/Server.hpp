#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <codecvt>
#include <locale>

#include "SFML/Network.hpp"

using namespace std;
using namespace std::filesystem;
using namespace sf;

class Server
{
    private:
        TcpListener listener;
        SocketSelector selector;
        vector<TcpSocket*> clients;

    public:
        Server();

        void update();
        void handleCommand(const string& raw, TcpSocket* client);
        void registerUser(const string& username, const string& password, TcpSocket* client);
        void loginUser(const string& username, const string& password, TcpSocket* client);
        void sendError(TcpSocket* client, const string& error_message);
        void acceptNewClient();
        void broadcastMessage(const string& message, TcpSocket* sender);
        void disconnectClient(vector<TcpSocket*>::iterator& it);
        bool handleClientPacket(TcpSocket* client);

        void run();

        //virtual ~Server();

};