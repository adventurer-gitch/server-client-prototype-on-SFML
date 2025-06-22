#include "Server.hpp"

Server::Server()
{
    if (this->listener.listen(5000) != Socket::Done)
    {
        cerr << "[-] Error starting server!\n";
    }
    else
    {
        cout << "[+] The server is running on port 5000\n";
        this->selector.add(this->listener);
    }
}

void Server::update()
{
    if (!this->selector.wait(milliseconds(100)))
    {
        return;
    }

    if (this->selector.isReady(this->listener))
    {
        this->acceptNewClient();
    }
        
    for (auto it = this->clients.begin(); it != this->clients.end(); )
    {
        TcpSocket* client = *it;

        if (this->selector.isReady(*client))
        {
            if (!this->handleClientPacket(client))
            {
                this->disconnectClient(it);
                continue;
            }
                
        }

        ++it;
    }
}

void Server::handleCommand(const string& raw, TcpSocket* client)
{
    istringstream stream(raw);
    string segment;
    vector<string> parts;

    while (getline(stream, segment, '|'))
    {
        parts.push_back(segment);
    }

    if (parts.empty())
    {
        return;
    }

    const string cmd = parts[0];

    if (cmd == "#registration" && parts.size() == 3)
    {
        string username = parts[1];
        string password = parts[2];
        //Регистрация клиента
        this->registerUser(parts[1], parts[2], client);
    }
    else if (cmd == "#login" && parts.size() == 3)
    {
        string username = parts[1];
        string password = parts[2];
        //Вход клиента
        this->loginUser(parts[1], parts[2], client);
    }
    else
    {
        //Неизвестная команда
        this->sendError(client, "FAIL");
    }
    
    
}

void Server::registerUser(const string& username, const string& password, TcpSocket* client)
{
    if (!exists("user_database/" + username + ".ini"))
    {
        //Создание учётной записи клиента
        ofstream reg_file;

        reg_file.open("user_database/" + username + ".ini");
        
        if (reg_file.is_open())
        {
            reg_file << username << endl << password;
        }

        reg_file.close();

        //------->
        Packet packet;
        packet << "OK";
        client->send(packet);
        cout << "[+] " << username << " подключился!\n";
        
        Packet p;
        p << "[+] Добро пожаловать, " + username;
        client->send(packet);
    }
    else
    {
        //Отказ в регистрации по причине наличии клиента с таким же именем пользователя
        Packet packet;
        packet << "FAIL";
        client->send(packet);
    }
}

void Server::loginUser(const string& username, const string& password, TcpSocket* client)
{
    if (exists("user_database/" + username + ".ini"))
    {
        ifstream reed_file;
        string storedUsername;
        string storedPassword;

        reed_file.open("user_database/" + username + ".ini");

        if (reed_file.is_open())
        {
            getline(reed_file, storedUsername);
            getline(reed_file, storedPassword);
        }

        reed_file.close();

        if (storedPassword == password)
        {
            Packet packet;
            packet << "OK";
            client->send(packet);

            cout << "[+] " << username << " connected!\n";

            packet.clear();

            packet << "[+] Welcome, " + username + "!";
            client->send(packet);
            
        }
        else
        {
            Packet packet;
            packet << "FAIL";
            client->send(packet);
        }  
    }
    else
    {
        Packet packet;
        packet << "FAIL";
        client->send(packet);
    }
    
}

void Server::sendError(TcpSocket* client, const string& error_message)
{
    Packet error;
    error << "ERROR|" + error_message;
    client->send(error);
}

void Server::run()
{
    while (true)
    {
        this->update();
    }
}

void Server::acceptNewClient()
{
    TcpSocket* newClient = new TcpSocket();

    if (this->listener.accept(*newClient) == Socket::Done)
    {
        cout << "[+] New client: " << newClient->getRemoteAddress() << ":" << newClient->getRemotePort() << endl;
        newClient->setBlocking(false);
        this->clients.push_back(newClient);
        this->selector.add(*newClient);
    }
    else
    {
        delete newClient;
    }
    
}

bool Server::handleClientPacket(TcpSocket* client)
{
    Packet packet;

    if (client->receive(packet) != Socket::Done)
    {
        cout << "[-] The client has disconnected!\n";

        return false;
    }

    if (packet.getDataSize() == 0)
    {
        cout << "[-] Received an empty package!\n";

        return false;
    }

    string message;

    if (!(packet >> message))
    {
        cout << "[-] Failed to unpack message!\n";

        return false;
    }
    
    if (message.rfind("#", 0) == 0)
    {
        this->handleCommand(message, client);
    }
    else
    {
        //wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;

        //wstring wide = converter.from_bytes(message);

        

        cout << "[>] Received: " << message << endl;

        // for (char ch : message)
        // {
        //     cout << hex << (int)(unsigned char)ch << " ";
        // }
        // cout << dec << endl;

        // this->broadcastMessage(message, client);
    }

    return true;
    
}

void Server::broadcastMessage(const string& message, TcpSocket* sender)
{
    for (TcpSocket* client : this->clients)
    {
        Packet out;

        // if (client == sender)
        // {
        //     out << "Вы: " + message;
        // }
        // else
        // {
        //     out << message;
        // }

        if (client != sender)
        {
            out << "[<] " + message;
            client->send(out);
        }
        
        //client->send(out);
    }
}

void Server::disconnectClient(vector<TcpSocket*>::iterator& it)
{
    TcpSocket* client = *it;

    cout << "[-] Client disabled: " << client->getRemoteAddress() << ":" << client->getRemotePort() << endl;

    this->selector.remove(*client);
    client->disconnect();
    delete client;

    it = this->clients.erase(it);
}