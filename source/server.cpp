#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <functional>

//назавание сокета сервера
#define PATH "my_socket"
//код выхода
#define CLOSE_MSG "exit"
void handler(int socket)
{
    std::cout << "User #" << socket << " connected!" << std::endl;
    //читаем в бесконечном цикле
    while (true) {
        char buf[512];
        //прием
        size_t bytes_read = recv(socket, buf, 512, 0);

        std::string msg(buf, bytes_read);

        std::cout << socket << ": " << msg << std::endl;
        //отправка
        send(socket, msg.c_str(), msg.size(), 0);
        if (msg == CLOSE_MSG) {
            break;
        }
    }
    std::cout << "User #" << socket << " disconnected!" << std::endl;
    //закрываем при отключении
    close(socket);
}

int main(int argc, char** argv)
{
        //слушающий сокет
    int listener = socket(AF_UNIX, SOCK_STREAM, 0);
    if (listener < 0) {
        perror("socket");
        exit(1);
    }
    
    //структура сервера
    struct sockaddr_un server_sock_addr{};
    server_sock_addr.sun_family = AF_UNIX;
    strcpy(server_sock_addr.sun_path, PATH);
    if (bind(listener, (struct sockaddr*) &server_sock_addr, sizeof(server_sock_addr)) < 0) {
        perror("bind");
        exit(2);
    }
    
    //слушаем
    listen(listener, 5);

    for (size_t i = 0; i < 5; ++i) {
        int socket = accept(listener, nullptr, nullptr);
        if (socket < 0) {
            perror("accept");
            exit(3);
        }
        //обработка в разных потоках
        std::thread(handler, socket).detach();
    }
    return 0;
}

