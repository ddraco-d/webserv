#include "AllServers.h"

int		AllServers::setup(void)
{
	std::vector<std::string> IPs; //= _config.getAll_IPs(); ПОЛУЧАЕМ СПИСОК IP ИЗ КОНФИГА
	std::vector<unsigned int> ports; //= _config.getAllPorts(); ПОЛУЧАЕМ СПИСОК ПОРТОВ ИЗ КОНФИГА
	IPs.push_back("127.0.0.1");
	ports.push_back(8080);
	IPs.push_back("127.0.0.1");
	ports.push_back(8081);

	FD_ZERO(&_masterFD);
	// _fd_size = vect.size();
	_fdMax = 0;

	for (int i = 0; i < IPs.size(); i++)
	{
		Server		serv(ports[i], IPs[i]);
		long		fd;

		if (serv.setup() != -1)
		{
			fd = serv.getFD();
			FD_SET(fd, &_masterFD);
			_servers.insert(std::make_pair(fd, serv));
			if (fd > _fdMax)
				_fdMax = fd;
			std::cout << "Setting up " << IPs[i] << ":" << ports[i] << "..." << std::endl;
		}
	}

	if (_fdMax == 0)
	{
		std::cout << "Could not setup cluster !" << std::endl;
		return (-1);
	}
	else
		return (0);
}

int		AllServers::select()
{
		int rVal;
		struct timeval tv;
		tv.tv_sec  = 1;
		tv.tv_usec = 0;
		_read_fds = _masterFD;
		FD_ZERO(&_write_fds);
		for (std::vector<int>::iterator it = _ready.begin() ; it != _ready.end() ; it++)
			FD_SET(*it, &_write_fds);

		std::cout << "\rWaiting on a connection" << std::flush;
		rVal = ::select(_fdMax + 1, &_read_fds, &_write_fds, NULL, &tv);
		return (rVal);
}

void			AllServers::send()
{
	std::string hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
	for (std::vector<int>::iterator it = _ready.begin(); it != _ready.end(); it++)
	{
		if (FD_ISSET(*it, &_write_fds))
		{
			long ret;
			
			ret = ::send(*it, hello.c_str(), strlen(hello.c_str()), 0);
			_ready.erase(it);
			if (ret == -1)
			{
				perror("send");
				FD_CLR(*it, &_masterFD);
				FD_CLR(*it, &_read_fds);
			}
			break;
		}
	}
}


void	AllServers::run(void)
{
	int i, rVal;

	while (1)
	{
		rVal = select();
		if (rVal == 0)
			continue;
		else if (rVal > 0)
		{
			send();
			rVal = 0;
			for(i = 0; i <= _fdMax; i++)
			{
				if (FD_ISSET(i, &_read_fds)) 
				{ // we got one!!
					if (_servers.find(i) != _servers.end()) 
					{
						rVal = _servers.find(i)->second.accept();
						if (rVal != -1)
						{
							FD_SET(rVal, &_masterFD); // add to master set
							if (rVal > _fdMax)   // keep track of the max
								_fdMax = rVal;
						}
					}
					else 
					{
						rVal = _servers.find(i)->second.recv(i);
						if (rVal == 1)
						{
							_ready.push_back(i);
							// std::cout << buf << std::endl;
						}
						else
							FD_CLR(i, &_masterFD);
							
					} // END handle data from client
				} // END got new incoming connection
			} // END looping through file descriptors
		}
		else if (rVal == -1)
		{
			perror("select");
			exit(4);
		//	std::cerr << RED << "Problem with select !" << RESET << std::endl;
		// 	for (std::map<long, Server *>::iterator it = _sockets.begin() ; it != _sockets.end() ; it++)
		// 		it->second->close(it->first);
		// 	_sockets.clear();
		// 	_ready.clear();
		// 	FD_ZERO(&_fd_set);
		// 	for (std::map<long, Server>::iterator it = _servers.begin() ; it != _servers.end() ; it++)
		// 		FD_SET(it->first, &_fd_set);
		}
	}
}


			// for (std::map<long, Server>::iterator it = _servers.begin(); it != _servers.end(); it++)
			// {
			// 	long	fd = it->first;
			// 	if (FD_ISSET(fd, &read_fds))
			// 	{
			// 		long	socket = it->second.accept();

			// 		if (socket != -1)
			// 		{
			// 			FD_SET(socket, &_masterFD);
			// 			_sockets.insert(std::make_pair(socket, &(it->second)));
			// 			if (socket > _max_fd)
			// 				_max_fd = socket;
			// 		}
			// 		ret = 0;
			// 		break;
			// 	}
			// }
			// for (std::map<long, Server *>::iterator it = _sockets.begin() ; ret && it != _sockets.end() ; it++)
			// {
			// 	long	socket = it->first;

			// 	if (FD_ISSET(socket, &reading_set))
			// 	{
			// 		long	ret = it->second->recv(socket);

			// 		if (ret == 0)
			// 		{
			// 			it->second->process(socket, _config);
			// 			_ready.push_back(socket);
			// 		}
			// 		else if (ret == -1)
			// 		{
			// 			FD_CLR(socket, &_fd_set);
			// 			FD_CLR(socket, &reading_set);
			// 			_sockets.erase(socket);
			// 			it = _sockets.begin();
			// 		}
			// 		ret = 0;
			// 		break;
			// 	}
			// }

