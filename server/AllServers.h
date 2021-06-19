#ifndef ALLSERVER_H
#define ALLSERVER_H
#include "server.h"

class AllServers {
public:
	AllServers(void) {};
	// AllServers(const AllServers & src);
	// ~AllServers(void);

	// AllServers & operator=(const AllServers & src);
	void	config();
	void	config(std::string file);
	int		setup(void);
	void	run(void);
	// void	clean(void);
private:
	Config						_config;
	std::map<long, Server>		_servers;
	std::map <long, Server *>	_accepted_sockets;
	std::vector<int>			_ready;
	fd_set						_masterFD;
	fd_set 						_read_fds;  // temp file descriptor list for select()
	fd_set 						_write_fds; // temp file descriptor list for select()
	unsigned int				_fd_size;
	long						_fdMax;
	int							select();
	void						send();
	void						recvv();
	void						acceptt();
};

#endif