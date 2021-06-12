// Server side C program to demonstrate HTTP Server programming
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <fcntl.h>
#include <vector>

#define PORT 8080
int main(int argc, char const *argv[])
{
	int server_fd, new_socket; long valread;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	
	// Only this line has been changed. Everything is same.
	char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
	
	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("In socket");
		exit(EXIT_FAILURE);
	}
    // fcntl(server_fd, F_SETFL, O_NONBLOCK); ???


	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );
	
	memset(address.sin_zero, '\0', sizeof address.sin_zero);
	int yes = 1;
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

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

	fd_set master;    // master file descriptor list
	fd_set read_fds;  // temp file descriptor list for select()
	fd_set writing_set;
	int fdmax;        // maximum file descriptor number

	FD_SET(server_fd, &master);

	fdmax = server_fd;
	int i, j;
	int nbytes;
	char buf[1000];

	struct timeval tv;


	std::vector<int> _ready;

	while(1)
	{
		printf("\n+++++++ Select work++++++++\n\n");
		read_fds = master;

		// timeout
		tv.tv_sec = 10;
		tv.tv_usec = 0;
		
		// writing_fds
		FD_ZERO(&writing_set);
		for (std::vector<int>::iterator it = _ready.begin() ; it != _ready.end() ; it++)
			FD_SET(*it, &writing_set);

		if (select(fdmax+1, &read_fds, &writing_set, NULL, &tv) == -1) 
		{
			perror("select");
			exit(4);
		}

		for (std::vector<int>::iterator it = _ready.begin(); it != _ready.end(); it++)
		{
			if (FD_ISSET(*it, &writing_set))
			{
				long ret;
				
				ret = send(*it, hello, strlen(hello), 0);
				_ready.erase(it);
				if (ret == -1)
				{
					perror("send");
				 	FD_CLR(*it, &master);
				 	FD_CLR(*it, &read_fds);
				// 	_sockets.erase(*it);
				}
				break;
			}
		}

		for(i = 0; i <= fdmax; i++)
		{
			if (FD_ISSET(i, &read_fds)) 
			{ // we got one!!
				if (i == server_fd) 
				{
					// handle new connections
					new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
					if (new_socket == -1) 
						perror("accept");
					else 
					{
						fcntl(new_socket, F_SETFL, O_NONBLOCK);
						FD_SET(new_socket, &master); // add to master set
						if (new_socket > fdmax) {    // keep track of the max
							fdmax = new_socket;
						}
					}
				}
				else 
				{
					// handle data from a client
					if ((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0) 
					{
						// got error or connection closed by client
						if (nbytes == 0) 
							printf("selectserver: socket %d hung up\n", i); // connection closed
						else 
							perror("recv");
						close(i); // bye!
						FD_CLR(i, &master); // remove from master set
					} 
					else 
					{
						_ready.push_back(i);
						std::cout << buf << std::endl;
						// if (send(i, hello, strlen(hello), 0) == -1) 
						// 	perror("send");
					}
				} // END handle data from client
			} // END got new incoming connection
		} // END looping through file descriptors
	}
	return 0;
}

