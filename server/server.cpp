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

Server::Server(unsigned int port, std::string host, ServerConfig serverConfig)
{
	_serverConfig = serverConfig;
	this->_port = port;
	this->_host = host;
	_serverFd = -1;
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
	this->setAddr();
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
		_requests.insert(std::make_pair(new_socket, ""));
	}
	return (new_socket);
}

void		Server::process(long socket) //, Config & conf)
{
	std::string str = _requests[socket].c_str();
	char *cstr = new char[str.length() + 1];
	strcpy(cstr, str.c_str());
	Request req(cstr, &_serverConfig);
	Response res(req);
	// if (_requests[socket].find("Transfer-Encoding: chunked") != std::string::npos &&
	// 	_requests[socket].find("Transfer-Encoding: chunked") < _requests[socket].find("\r\n\r\n"))
	// 	this->processChunk(socket);

	if (_requests[socket].size() < 1000)
		std::cout << "\nRequest :" << std::endl << "[" << _requests[socket]  << "]" << std::endl;
	else
		std::cout << "\nRequest :" << std::endl << "["  << _requests[socket].substr(0, 1000) << "..." << _requests[socket].substr(_requests[socket].size() - 10, 15) << "]" << std::endl;

	if (_requests[socket] != "")
	{
		_requests.erase(socket);

		_response.insert(std::make_pair(socket, res.getResponse()));


		//hardcode
		// std::stringstream response, h;
		// response << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";
		// std::ifstream html("my.html");
		// h << html.rdbuf();
		// response << h.str().length() << "\n\n" << h.str();
		// _response.insert(std::make_pair(socket, response.str()));
		//
	}
	delete [] cstr;
}

int			Server::recv(long socket)
{
	int nbytes;
	char	buf[RECV_SIZE] = {0};
	// handle data from a client
	if ((nbytes = ::recv(socket, buf, RECV_SIZE - 1, 0)) <= 0) 
	{
		if (nbytes == 0) 
			std::cout << "\rConnection closed by client, socket number" << socket << std::endl;
		else if (nbytes == -1)
			perror("\r recv");
		::close(socket); // bye!
		return (-1);
	}
	_requests[socket] += std::string(buf);
	return (1);
}

int			Server::send(long socket)
{
	static std::map<long, size_t>	sent_data;
	if (sent_data.find(socket) == sent_data.end())
		sent_data[socket] = 0;

	if (_response[socket].size() < 1000 && sent_data[socket] == 0)
		std::cout << "\rResponse :                " << std::endl << "[" << _response[socket] << "]\n" << std::endl;
	else
		std::cout << "\rResponse :                " << std::endl << "["  << _response[socket].substr(0, 1000) << "..." << _response[socket].substr(_response[socket].size() - 10, 15) << "]\n" << std::endl;

	std::string s = _response[socket].substr(sent_data[socket], RECV_SIZE);
	int rVal = ::send(socket, s.c_str(), s.size(), 0);

	if (rVal == -1)
	{
		close(socket);
		sent_data[socket] = 0;
		perror("send");
		return (-1);
	}
	else
	{
		sent_data[socket] += rVal;
		if (sent_data[socket] >= _response[socket].size())
		{
			_response.erase(socket);
			sent_data[socket] = 0;
			return (0);
		}
		return (1);
	}
}

