
#include <stdio.h>
#include <sys/socket.h>

#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <unistd.h>
#include "Request.hpp"
#include "Parcer.hpp"
#include "Response.hpp"

#define PORT 8000

class Web_server
{
private:

public:
	Web_server(Server server, char **envp);
	int 				server_fd;
	int					new_socket;
	struct sockaddr_in 	address;
	int 				addrlen;
};

Web_server::Web_server(Server server, char **envp)
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
		std::vector<char> request(14000);
        long valread = recv(new_socket, request.data(), request.size(), 0);
        printf("REQUEST:\n%s", request.data());
		if(valread < 0)
		{
			printf("Нет байт для чтения\n");
		}
		Request req(request.data(), &server);

		Response res(req);
		std::string response = res.getResponse();
		
		send(new_socket , response.c_str(), strlen(response.c_str()), 0);
		printf("RESPONSE:\n%s", response.c_str());
		close(new_socket);
	}

/*
		std::stringstream response;
		response << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";

		if (req.status_code == 0)
		{
			std::stringstream h;
			std::ifstream html("index.html");
			h << html.rdbuf();
			response << h.str().length() << "\n\n" << h.str();
			send(new_socket , response.str().c_str(), strlen(response.str().c_str()), 0);
			response <<  "\n\n" << req.get();
		}
		else if (req.status_code == 1)
		{

				std::string command;
				command = req.path + " \"" + req.cgi_arg + "\"";
				std::cout << "COMMAND: " << command.c_str() << "|\n";
				char *argv[3];
				argv[0] = (char *)req.path.c_str();
				argv[1] = (char *)req.cgi_arg.c_str();
				argv[2] = NULL;
			
			int		pid;
			pid = fork();
			if (pid == 0)
			{
				if (!freopen("./cgi_tester/test.txt", "r", stdin))
					std::cout << "NO OPEN\n";
				if (!freopen("./cgi_tester/resalt.txt", "w", stdout))
					std::cout << "NO OPEN\n";
				if (execve(req.path.c_str(), argv, envp) == -1)
				{
					std::cout << "CGI ERROR\n";
				}
				exit(0);
			}
			wait(&pid);
			
			std::stringstream answer;
			std::ifstream html_answer("./cgi_tester/resalt.txt");
			answer << html_answer.rdbuf();
			std::cout << "ANSWER:" << answer.str() << "|\n";
			response << answer.str().length() << "\n\n" << answer.str();
			send(new_socket , response.str().c_str(), strlen(response.str().c_str()), 0);
			
		}
		else
		{
			std::stringstream bed;
			bed << "Bed request :(\n";
			response << bed.str().length() << "\n\n" << bed.str();
			send(new_socket , response.str().c_str(), strlen(response.str().c_str()), 0);
			response << "\n\n" << req.get();
		}
		printf("\n\nRESPONS:\n%s", response.str().c_str());
	*/
}

int main(int argc, char **argv, char **envp)
{
	if (argc == 2)
	{
		Config config(argv[1]);
		Web_server web(config.servers[0], envp);
	}
    return 0;
}