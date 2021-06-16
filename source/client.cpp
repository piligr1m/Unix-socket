#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <string>
#include <iostream>

//назавание сокета сервера
#define PATH "my_socket"
//код выхода
#define CLOSE_MSG "exit"

int main()
{
    //создаем сокет
    int client_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    //ловим ошибку при создании
    if (client_socket < 0) {
        perror("socket");
        exit(1);
    }
    //создаем структуру сервера
    struct sockaddr_un server_sock_addr{};
    server_sock_addr.sun_family = AF_UNIX;
    strcpy(server_sock_addr.sun_path, PATH);
    //подключаемся к серверу
    if (connect(client_socket, (struct sockaddr*) &server_sock_addr, sizeof(server_sock_addr)) < 0) {
        perror("connect");
        exit(2);
    }
    std::cout << "Connected!" << std::endl;
    
    //в бесконечном цикле отправляемя сообщения, пока не будет передано сообщение с кодом выхода
    while (true) {
        std::string msg;
	    std::getline(std::cin, msg);
        //отправка
        send(client_socket, msg.c_str(), msg.size(), 0);
        char buf[512];
        //получение
        size_t bytes_read = recv(client_socket, buf, 512, 0);
        std::string answer(buf, bytes_read);
        //пишем ответ сервера
        std::cout << "Server answer: " << answer << std::endl;
        
        //выход
        if (answer == CLOSE_MSG) {
            break;
        }
    }
    //закрываем сокет
    close(client_socket);
    return 0;
}

