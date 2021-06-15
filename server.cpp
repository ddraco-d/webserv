#include "server.h"


void		Server::setAddr(void)
{
	_addr.sin_family = AF_INET;
	if (_host == "")
		_addr.sin_addr.s_addr = INADDR_ANY;
	else
		_addr.sin_addr.s_addr = inet_addr(_host.c_str());
	_addr.sin_port = htons(_port);
	memset(_addr.sin_zero, '\0', sizeof _addr.sin_zero);
}

Server::Server(unsigned int port, std::string host)
{
	this->_port = port;
	this->_host = host;
	_serverFd = -1;
	this->setAddr();
}

int		Server::setup(void)
{
	int yes = 1;
	if ((_serverFd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("In socket");
		exit(EXIT_FAILURE);
	}
	setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

	if (_serverFd == -1)
	{
		std::cerr <<  "Could not create server."  << std::endl;
		return (-1);
	}
	this->setAddr(); //???????
	if (bind(_serverFd, (struct sockaddr *)&_addr, sizeof(_addr))<0)
	{
		perror("In bind");
		exit(EXIT_FAILURE);
	}
	if (listen(_serverFd, 10) < 0)
	{
		perror("In listen");
		exit(EXIT_FAILURE);
	}
	return (0);
}


long Server::getFD()
{
	return (_serverFd);
}

long		Server::accept(void)
{
	long	new_socket;
	int addrlen = sizeof(_addr);

	// handle new connections
	new_socket = ::accept(_serverFd, (struct sockaddr *)&_addr, (socklen_t*)&addrlen);
	if (new_socket == -1) 
		perror("accept");
	else 
	{
		fcntl(new_socket, F_SETFL, O_NONBLOCK);
		// _requests.insert(std::make_pair(socket, "")); ?
	}
	return (new_socket);
}

int			Server::recv(long socket)
{
	char buf[1000];
	int nbytes;

	// handle data from a client
	if ((nbytes = ::recv(socket, buf, sizeof(buf), 0)) <= 0) 
	{
		if (nbytes == 0) 
			std::cout << "\rConnection closed by client, socket number" << socket << std::endl;
		else if (nbytes == -1)
			perror("\r recv");
		::close(socket); // bye!
		return (-1);
	} 
	else
		return (1);
}

int			Server::send(long socket)
{
	// std::string hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";

	std::stringstream response, h;
	response << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";
	std::ifstream html("my.html");
	h << html.rdbuf();
	response << h.str().length() << "\n\n" << h.str();
	int rVal = ::send(socket , response.str().c_str(), strlen(response.str().c_str()), 0);
	// int rVal = ::send(socket, hello.c_str(), strlen(hello.c_str()), 0);
	if (rVal == -1)
	{
		close(socket);
		perror("send");
		return (-1);
	}
	return (rVal);
}

