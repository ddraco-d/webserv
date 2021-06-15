
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
#include "Parcer.hpp"

#define PORT 8000

class Web_server
{
private:

public:
	Web_server(Server server);
	int 				server_fd;
	int					new_socket;
	struct sockaddr_in 	address;
	int 				addrlen;
};

Web_server::Web_server(Server server)
{
	addrlen = sizeof(address);
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
	address.sin_family = AF_INET;
	address.sin_port = htons(server.port);
	address.sin_addr.s_addr = htonl(server.host);
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
		printf("\n%d: +++++++ Waiting for new connection ++++++++\n\n", server.port);
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
		std::vector<char> request(1400);
        long valread = recv(new_socket, request.data(), request.size(), 0);
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

		std::stringstream bed;
		bed << "Bed request :(\n";
		std::stringstream response2;
		response2 << "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: ";
		Request req(request.data(), &server);
		if (req.status_code == 0)
			response2 << req.get().length() << "\n\n" << req.get();
		else
			response2 << bed.str().length() << "\n\n" << bed.str();
		send(new_socket , response2.str().c_str(), strlen(response2.str().c_str()), 0);
		printf("\n\nRESPONS:\n%s", response2.str().c_str());
        close(new_socket);
	}
}

int main(int argc, char **argv)
{
	if (argc == 2)
	{
		Config config(argv[1]);
		Web_server web(config.servers[0]);
	}
    return 0;
}