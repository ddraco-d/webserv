/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ddraco <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/11 17:13:02 by ddraco            #+#    #+#             */
/*   Updated: 2021/06/20 14:31:48 by ddraco           ###   ########.fr       */
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
	try {
		if (ac > 1)
			servers.config(av[1]);
		else
			servers.config();
		if (servers.setup() == -1)
			return (1);
		servers.run();
	}
	catch (const char *str)
	{
		std::cerr << "ERROR: " << str << std::endl;
		return (1);
	}
	return (0);
}
