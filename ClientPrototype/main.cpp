#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#ifdef _WIN32

#include <Windows.h>

#include "Client.h"

int main()
{
	//system("chcp 1251");

	//SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	locale::global(locale("ru_RU.UTF-8"));
	setlocale(LC_ALL, "ru_RU.UTF-8");

	Client client;
	
	client.run();

	return 0;
}

#endif