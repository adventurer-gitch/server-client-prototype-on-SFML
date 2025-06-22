#pragma once

#include <iostream>
#include <string>

using namespace std;

class User
{
private:
	string username;
	string password;

public:
	void setUsername(const string& username);
	void setPassword(const string& password);

	const string& getUsername() const;
	const string& getPassword() const;

};