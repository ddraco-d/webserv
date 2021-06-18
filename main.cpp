/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ddraco <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/11 17:13:02 by ddraco            #+#    #+#             */
/*   Updated: 2021/06/18 18:06:54 by ddraco           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <sys/socket.h>

#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <unistd.h>
#include "request/Request.hpp"
#include "Config/Parcer.hpp"
#include "response/Response.hpp"
#include "server/AllServers.h"


int		main(int ac, char **av, char **envp)
{
	AllServers	servers;
	
	servers.config(av[1]);
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
