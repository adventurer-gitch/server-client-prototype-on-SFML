#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include "User.h"

void User::setUsername(const string& username)
{
	this->username = username;
}

void User::setPassword(const string& password)
{
	this->password = password;
}

const string& User::getUsername() const
{
	return this->username;
}

const string& User::getPassword() const
{
	return this->password;
}
