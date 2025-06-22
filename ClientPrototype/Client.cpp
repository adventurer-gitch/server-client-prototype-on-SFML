#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include "Client.h"

Client::Client()
{

}

void Client::registrationOrLogin()
{
	//Сообщение о запуске клиента
	cout << "[+] The client is running!\n";

	//Проверка на наличие аккаунта
	if (exists("Data/user_data.ini")) //Если имеется аккаунт и он подключён
	{
		string username;
		string password;

		ifstream reed_file;

		reed_file.open("Data/user_data.ini");
		
		if (reed_file.is_open())
		{
			getline(reed_file, username);
			getline(reed_file, password);
		}

		reed_file.close();

		this->user.setUsername(username);
		this->user.setPassword(password);
		
		if (this->socket.getRemotePort() == 0)
		{
			if (this->socket.connect(IP_ADDRESS, PORT) != Socket::Done)
			{
				cerr << "[-] Failed to connect to the server!\n";
				return;
			}
			else
			{
				string cmd = "#login|" + this->user.getUsername() + "|" + this->user.getPassword();
				Packet packet;
				packet << cmd;
				this->socket.send(packet);

				packet.clear();

				if (this->socket.receive(packet) == Socket::Done)
				{
					string answer;
					packet >> answer;

					if (answer == "OK")
					{
						//Добро пожаловать

						cout << "[+] Login was successful!\n";

						packet.clear();
						if (this->socket.receive(packet) == Socket::Done)
						{
							string w;
							packet >> w;
							cout << w << endl;
						}
						this->socket.setBlocking(false);
						
						//Одно из предпологаемых начало ошибки
						this->update();
					}
					else if (answer == "FAIL")
					{
						this->registrationOrLogin();
					}
				}
			}
		}
	}
	else //Если аккаунт не подключён или его нет
	{
		//Запрос на действия
		cout << "[+] Select action: \n";
		cout << "[0] Exit\n";
		cout << "[1] Registration\n";
		cout << "[2] Login\n";
		cout << "[:]";

		//Ответ на запрос на действие
		unsigned short action_ROL;
		cin >> action_ROL;

		switch (action_ROL)
		{
		case 0:
			//Выход и завершение работы программы клиента
			this->exit_the_program = true;
			break;

		case 1:
			//Регистрация
			this->registration();
			break;

		case 2:
			//Вход
			this->login();
			break;

		default:
			//Неверное действие
			cout << "[-] This action is unknown or not possible!\n";
			this->registrationOrLogin();
			break;
		}
	}
}

void Client::registration()
{
	//Регистрация
	//Создание имени пользователя
	cout << "[+] Enter your username: ";
	string username;
	cin >> username;
	this->user.setUsername(username);
	
	//Создание пароля
	string password1;
	string password2;

	do
	{
		cout << "[+] Create a password: ";
		cin >> password1;

		cout << "[+] Confirm the created password: ";
		cin >> password2;

		if (password1 == password2)
		{
			this->user.setPassword(password1);
		}
		else
		{
			cout << "[-} Passwords do not match!\n";
		}
	} while (password1 != password2);

	password1 = "";
	password2 = "";

	if (this->socket.getRemotePort() == 0)
	{
		if (this->socket.connect(IP_ADDRESS, PORT) != Socket::Done)
		{
			cerr << "[-] Failed to connect to the server!\n";
			return;
		}
		else
		{
			string cmd = "#registration|" + this->user.getUsername() + "|" + this->user.getPassword();
			Packet packet;
			packet << cmd;
			this->socket.send(packet);

			packet.clear();

			if (this->socket.receive(packet) == Socket::Done)
			{
				string answer;
				packet >> answer;

				if (answer == "OK")
				{
					ofstream registration_user_data_file;

					registration_user_data_file.open("Data/user_data.ini");

					if (registration_user_data_file.is_open())
					{
						registration_user_data_file << this->user.getUsername() << endl;
						registration_user_data_file << this->user.getPassword() << endl;
					}
					registration_user_data_file.close();

					this->socket.setBlocking(false);
					this->update();
					cout << "[+] Welcome!\n";
				}
				else if (answer == "FAIL")
				{
					this->registrationOrLogin();
				}
			}
		}
	}
}

//Вход в аккаунт
void Client::login()
{
	string username;
	string password;

	cout << "[+] Enter your username: ";
	cin >> username;
	this->user.setUsername(username);

	cout << "[+] Enter password: ";
	cin >> password;
	this->user.setPassword(password);

	if (this->socket.getRemotePort() == 0)
	{
		if (this->socket.connect(IP_ADDRESS, PORT) != Socket::Done)
		{
			cerr << "[-] Failed to connect to the server!\n";
			return;
		}
		else
		{
			string cmd = "#login|" + this->user.getUsername() + "|" + this->user.getPassword();
			Packet packet;
			packet << cmd;
			this->socket.send(packet);

			packet.clear();

			if (this->socket.receive(packet) == Socket::Done)
			{
				string answer;
				packet >> answer;

				if (answer == "OK")
				{
					cout << "[+] Welcome!\n";
					this->socket.setBlocking(false);
					this->update();
				}
				else if (answer == "FAIL")
				{
					this->registrationOrLogin();
				}
			}
		}
	}
}

void Client::update()
{
	cin.ignore();

	// Поток приёма сообщений
	std::thread receiverThread([&]() {
		while (!this->exit_the_program) {
			std::lock_guard<std::mutex> lock(this->socketMutex);

			Packet recv_packet;
			Socket::Status status = this->socket.receive(recv_packet);

			if (status == Socket::Done) {
				std::string reply;
				if (recv_packet >> reply) {
					std::cout << reply << std::endl;
				}
				else {
					std::cout << "[-] Empty or damaged package received!\n";
				}
			}
			else if (status == Socket::Disconnected) {
				std::cout << "[-] The server has disconnected!\n";
				this->exit_the_program = true;
				break;
			}
			else {
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
			}
		}
	});

	// Поток отправки сообщений
	while (!this->exit_the_program)
	{
		std::string message;
		getline(cin, message);
		
		Packet packet;

		if (message == "#exit") {
			this->exit_the_program = true;
			break;
		}

		packet << this->user.getUsername() + ": " + message;

		this->socket.send(packet);
	}

	receiverThread.join();
}




void Client::run()
{
	this->registrationOrLogin();
}

Client::~Client()
{
	//this->socket.disconnect();
}