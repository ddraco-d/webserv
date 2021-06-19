#ifndef SERVER_H
# define SERVER_H
# include <stdio.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <unistd.h>
# include <stdlib.h>
# include <netinet/in.h>
# include <string.h>
# include <iostream>
#include "../request/Request.hpp"
#include "../Config/Parcer.hpp"
#include "../response/Response.hpp"
# define RECV_SIZE 65536


class Server {
public:
	Server(unsigned int port, std::string host, ServerConfig serverConfig);
	// Server(const Server & src);
	// ~Server(void);

	// Server & operator=(const Server & src);

	long	getFD(void);

	int		setup(void);
	void	setAddr(void);
	long	accept(void);
	void	process(long socket); //, Config & conf); нужно!
	// void	processChunk(long socket); ??
	int		recv(long socket);
	int	send(long socket);
	// void	close(int socket);
	// void	clean(void);
private:
	ServerConfig				_serverConfig;
	std::map<long, std::string>	_requests;
	std::map<long, std::string>	_response;
	unsigned int				_port;
	std::string					_host;
	long						_serverFd;
	struct sockaddr_in			_addr;

	Server(void);

};

#endif