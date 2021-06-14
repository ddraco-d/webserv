/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ddraco <ddraco@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/11 17:13:02 by ddraco            #+#    #+#             */
/*   Updated: 2021/06/14 19:54:10 by ddraco           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "webserv.hpp"
// #include "Server.hpp"
// #include "Request.hpp"
// #include "Response.hpp"
// #include "Cluster.hpp"

#include "AllServers.h"

int		main(int ac, char **av)
{
	AllServers	servers;
	if (servers.setup() == -1)
		return (1);
	servers.run();
	// if (ac == 2)
	// {
	// 	AllServers	servers;
	// 	try {
	// 		// cluster.config(av[1]);
	// 		if (servers.setup() == -1)
	// 			return (1);
	// 		servers.run();
	// 		// cluster.clean();			
	// 	}
	// 	catch (std::exception &e) {
	// 		std::cerr << e.what() << std::endl;
	// 	}
	// }
	// else
	// {
	// 	// std::cerr << RED << "Invalid number of arguments." << RESET << std::endl;
	// 	return (1);
	// }
	// return (0);
}


// int main(int argc, char const *argv[])
// {
// 	int server_fd, new_socket; 
// 	long valread;




// 	while(1)
// 	{
// 		printf("\n+++++++ Select work++++++++\n\n");
// 		read_fds = master;
// 		rVal = 0;

// 		// timeout
// 		tv.tv_sec = 10;
// 		tv.tv_usec = 0;
		
// 		// writing_fds
// 		FD_ZERO(&writing_set);
// 		for (std::vector<int>::iterator it = _ready.begin() ; it != _ready.end() ; it++)
// 			FD_SET(*it, &writing_set);

// 		rVal = select(fdmax+1, &read_fds, &writing_set, NULL, &tv);
// 		if (rVal == -1) 
// 		{
// 			perror("select");
// 			exit(4);
// 		}
// 		else if (rVal == 0)
// 			continue;


// 		for (std::vector<int>::iterator it = _ready.begin(); it != _ready.end(); it++)
// 		{
// 			if (FD_ISSET(*it, &writing_set))
// 			{
// 				long ret;
				
// 				ret = send(*it, hello.c_str(), strlen(hello.c_str()), 0);
// 				_ready.erase(it);
// 				if (ret == -1)
// 				{
// 					perror("send");
// 				 	FD_CLR(*it, &master);
// 				 	FD_CLR(*it, &read_fds);
// 				// 	_sockets.erase(*it);
// 				}
// 				break;
// 			}
// 		}

// 		rVal = 0;
// 		for(i = 0; i <= fdmax; i++)
// 		{
// 			if (FD_ISSET(i, &read_fds)) 
// 			{ // we got one!!
// 				if (i == serv.getFD()) 
// 				{
// 					rVal = serv.accept();
// 					if (rVal != -1)
// 					{
// 						FD_SET(rVal, &master); // add to master set
// 						if (rVal > fdmax)   // keep track of the max
// 							fdmax = rVal;
// 					}
// 				}
// 				else 
// 				{
// 					rVal = serv.recv(i);
// 					if (rVal == 1)
// 					{
// 						_ready.push_back(i);
// 						// std::cout << buf << std::endl;
// 					}
// 					else
// 						FD_CLR(i, &master);
						
// 				} // END handle data from client
// 			} // END got new incoming connection
// 		} // END looping through file descriptors
// 	}
// 	return 0;
// }



// int main(int argc, char const *argv[])
// {
// 	int server_fd, new_socket; 
// 	long valread;
// 	std::string hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";


// 	/****Cluster variables****/

// 	fd_set master;    // master file descriptor list
// 	fd_set read_fds;  // temp file descriptor list for select()
// 	fd_set writing_set;
// 	int fdmax;        // maximum file descriptor number

// 	Server serv(PORT, "0.0.0.0");
// 	serv.setup();
// 	std::map<long, Server> servers;

// 	FD_SET(serv.getFD(), &master);
// 	fdmax = serv.getFD();
// 	int i, j;
// 	struct timeval tv;
// 	std::vector<int> _ready;
// 	int rVal;

// 	while(1)
// 	{
// 		printf("\n+++++++ Select work++++++++\n\n");
// 		read_fds = master;
// 		rVal = 0;

// 		// timeout
// 		tv.tv_sec = 10;
// 		tv.tv_usec = 0;
		
// 		// writing_fds
// 		FD_ZERO(&writing_set);
// 		for (std::vector<int>::iterator it = _ready.begin() ; it != _ready.end() ; it++)
// 			FD_SET(*it, &writing_set);

// 		rVal = select(fdmax+1, &read_fds, &writing_set, NULL, &tv);
// 		if (rVal == -1) 
// 		{
// 			perror("select");
// 			exit(4);
// 		}
// 		else if (rVal == 0)
// 			continue;


// 		for (std::vector<int>::iterator it = _ready.begin(); it != _ready.end(); it++)
// 		{
// 			if (FD_ISSET(*it, &writing_set))
// 			{
// 				long ret;
				
// 				ret = send(*it, hello.c_str(), strlen(hello.c_str()), 0);
// 				_ready.erase(it);
// 				if (ret == -1)
// 				{
// 					perror("send");
// 				 	FD_CLR(*it, &master);
// 				 	FD_CLR(*it, &read_fds);
// 				// 	_sockets.erase(*it);
// 				}
// 				break;
// 			}
// 		}

// 		rVal = 0;
// 		for(i = 0; i <= fdmax; i++)
// 		{
// 			if (FD_ISSET(i, &read_fds)) 
// 			{ // we got one!!
// 				if (i == serv.getFD()) 
// 				{
// 					rVal = serv.accept();
// 					if (rVal != -1)
// 					{
// 						FD_SET(rVal, &master); // add to master set
// 						if (rVal > fdmax)   // keep track of the max
// 							fdmax = rVal;
// 					}
// 				}
// 				else 
// 				{
// 					rVal = serv.recv(i);
// 					if (rVal == 1)
// 					{
// 						_ready.push_back(i);
// 						// std::cout << buf << std::endl;
// 					}
// 					else
// 						FD_CLR(i, &master);
						
// 				} // END handle data from client
// 			} // END got new incoming connection
// 		} // END looping through file descriptors
// 	}
// 	return 0;
// }