
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>

#include "Request.hpp"

//Создайте сокет int server_fd = socket(домен, тип, протокол);
//Определите сокет
//На сервере дождитесь входящего соединения
//Отправляйте и получайте сообщения
//Закройте розетку
/*
struct sockaddr_in 
{ 
    __uint8_t         sin_len; 
    sa_family_t       sin_family; 
    in_port_t         sin_port; 
    struct in_addr    sin_addr; 
    char              sin_zero[8]; 
};
*/
#define PORT 8080
int main(int argc, char const *argv[])
{
    int server_fd;
	int new_socket;
	long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
	address.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(address.sin_zero, '\0', sizeof(address.sin_zero));
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
	
    if (listen(server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
	
    while(1)
    {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        std::vector<char> request(1400);
        valread = recv(new_socket, request.data(), request.size(), 0);
        printf("REQUEST:\n%s", request.data());
		if(valread < 0)
		{
			printf("Нет байт для чтения\n");
		}
		std::stringstream response, h;
		response << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";
		std::ifstream html("my.html");
		h << html.rdbuf();
		response << h.str().length() << "\n\n" << h.str();
		//send(new_socket , response.str().c_str(), strlen(response.str().c_str()), 0);

		std::stringstream response2;
		response2 << "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: ";
		Request req(request.data());
		response2 << req.get().length() << "\n\n" << req.get();
		send(new_socket , response2.str().c_str(), strlen(response2.str().c_str()), 0);
		printf("\n\nRESPONS:\n%s", response2.str().c_str());
        close(new_socket);
    }
    return 0;
}