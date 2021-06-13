#ifndef MINISERV_H
# define MINISERV_H
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <fcntl.h>
#include <vector>


class Server {
public:
	Server(unsigned int port, std::string host);
	// Server(const Server & src);
	~Server(void);

	// Server & operator=(const Server & src);

	long	getFD(void);

	int		setup(void);
	void	setAddr(void);
	long	accept(void);
	// void	process(long socket, Config & conf); нужно!
	// void	processChunk(long socket);
	int		recv(long socket);
	int		send(long socket);
	void	close(int socket);
	void	clean(void);

private:
	// std::map<long, std::string>	_requests;
	// t_listen					_listen;
	unsigned int				_port;
	std::string					_host;
	long						_serverFd;
	struct sockaddr_in			_addr;

	Server(void);

};

#endif