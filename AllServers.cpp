#include "AllServers.h"

void	AllServers::config(std::string fileconf)
{
	// _config.parse(fileconf.c_str()); запускаем парсинг
}

int		AllServers::setup(void)
{
	std::vector<std::string> IPs; //= _config.getAll_IPs(); ПОЛУЧАЕМ СПИСОК IP ИЗ КОНФИГА
	std::vector<unsigned int> ports; //= _config.getAllPorts(); ПОЛУЧАЕМ СПИСОК ПОРТОВ ИЗ КОНФИГА
	IPs.push_back("127.0.0.1");
	ports.push_back(8080);
	IPs.push_back("127.0.0.1");
	ports.push_back(8081);

	FD_ZERO(&_masterFD);
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
		FD_ZERO(&_masterFD);
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
		std::cout << "\rWaiting on a connection " <<  std::flush;
		rVal = ::select(_fdMax + 1, &_read_fds, &_write_fds, NULL, &tv);
		return (rVal);
}

void	AllServers::send()
{
	for (std::vector<int>::iterator it = _ready.begin(); it != _ready.end(); it++)
	{
		if (FD_ISSET(*it, &_write_fds))
		{
			long rVal;

			rVal = _servers.find(*it)->second.send(*it);
			_ready.erase(it); //точно ли всегда нужно это делать?
			if (rVal == -1)
			{
				FD_CLR(*it, &_masterFD);
				FD_CLR(*it, &_read_fds);
				_accepted_sockets.erase(*it);
				_ready.erase(it);
			}
			break;
		}
	}
}

void	AllServers::recvv()
{
	int rVal;

	for (std::set<long>::iterator it = _accepted_sockets.begin(); it != _accepted_sockets.end() ; it++)
	{
		if (FD_ISSET(*it, &_read_fds))
		{
			rVal = _servers.find(*it)->second.recv(*it);
			if (rVal == 1)
			{
				std::cout << "\nHere should be request\n";
				// _servers.find(i)->second.process();
				_ready.push_back(*it);
			}
			else
			{
				FD_CLR(*it, &_read_fds);
				FD_CLR(*it, &_masterFD);
				_accepted_sockets.erase(*it);
			}
			break;
		}
	}
}

void	AllServers::acceptt()
{
	for (std::map<long, Server>::iterator it = _servers.begin(); it != _servers.end() ; it++)
	{
		long serv_socket = it->first;
		if (FD_ISSET(serv_socket, &_read_fds))
		{
			long accepted_socket = it->second.accept();
			if (accepted_socket != -1)
			{
				FD_SET(accepted_socket, &_masterFD); // add to master set
				_accepted_sockets.insert(accepted_socket);
				if (accepted_socket > _fdMax)   // keep track of the max
					_fdMax = accepted_socket;
			}
			break;
		}
	}
}


void	AllServers::run(void)
{
	int rVal;

	while (1)
	{
		rVal = select();
		if (rVal == 0)
			continue;
		else if (rVal > 0)
		{
			send();
			std::cout << "\r-Received a connection-!   " << std::flush;
			recvv();
			
			acceptt();
		}
		else if (rVal == -1)
		{
			perror("select");
			for (std::set<long>::iterator it = _accepted_sockets.begin(); it != _accepted_sockets.end() ; it++)
				::close(*it);
			_accepted_sockets.clear();
			_ready.clear();
			FD_ZERO(&_masterFD);
			for (std::map<long, Server>::iterator it = _servers.begin() ; it != _servers.end() ; it++)
				FD_SET(it->first, &_masterFD);
		}
	}
}


