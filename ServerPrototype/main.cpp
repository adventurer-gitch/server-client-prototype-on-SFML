#include "Server.hpp"

int main()
{

    setlocale(LC_ALL, "Russian");

    setlocale(LC_ALL, "UTF-8");

    locale::global(locale("ru_RU.UTF-8"));

    //system("chcp 65001");

    Server server;

    server.run();

    return 0;
}