#ifndef ALLSERVER_H
#define ALLSERVER_H
#include "server.h"

class AllServers {
public:
	AllServers(void) {};
	AllServers(const AllServers & src);
	// ~AllServers(void);

	AllServers & operator=(const AllServers & src);

	// void	config(std::string fileconf);
	int		setup(void);
	void	run(void);
	// void	clean(void);
private:
	// Config						_config;
	std::map<long, Server>		_servers;
	// std::map<long, Server *>	_sockets; нужно для другого варианта реализации
	std::vector<int>			_ready;
	fd_set						_masterFD;
	fd_set 						_read_fds;  // temp file descriptor list for select()
	fd_set 						_write_fds;
	unsigned int				_fd_size;
	long						_fdMax;
	int							select();
	void						send();
};

#endif