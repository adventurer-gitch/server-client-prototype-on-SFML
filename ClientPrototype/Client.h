#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <filesystem>
#include <thread>
#include <mutex>
#include <chrono>
#include <codecvt>
#include <locale>

#include "SFML/Network.hpp"

#include "User.h"

using namespace std;
using namespace sf;
using namespace std::filesystem;

const IpAddress IP_ADDRESS = "192.168.137.26";
const unsigned short PORT = 5000;

class Client
{
private:
	TcpSocket socket;
	User user;
	std::mutex socketMutex;
	bool exit_the_program = false;

public:
	Client();

	void registrationOrLogin();
	void registration();
	void login();

	void update();
	
	//string readUtf8Line();

	//bool sendUtf8Message(TcpSocket& socket, const string& username, const string& message);

	void run();

	virtual ~Client();

};