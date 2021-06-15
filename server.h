#ifndef SERVER_H
# define SERVER_H
# define PORT 8080
# include <stdio.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <unistd.h>
# include <stdlib.h>
# include <netinet/in.h>
# include <string.h>
# include <iostream>
# include <fcntl.h>
# include <vector>
# include <map>
#include <sstream>
#include <fstream>
#include <set>

class Server {
public:
	Server(unsigned int port, std::string host);
	// Server(const Server & src);
	// ~Server(void);

	// Server & operator=(const Server & src);

	long	getFD(void);

	int		setup(void);
	void	setAddr(void);
	long	accept(void);
	// void	process(long socket, Config & conf); нужно!
	// void	processChunk(long socket);
	int		recv(long socket);
	int	send(long socket);
	// void	close(int socket);
	// void	clean(void);
private:
	// std::map<long, std::string>	_requests;

	unsigned int				_port;
	std::string					_host;
	long						_serverFd;
	struct sockaddr_in			_addr;

	Server(void);

};

#endif