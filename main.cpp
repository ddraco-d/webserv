/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ddraco <ddraco@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/11 17:13:02 by ddraco            #+#    #+#             */
/*   Updated: 2021/06/11 17:13:20 by ddraco           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "webserv.hpp"
// #include "Server.hpp"
// #include "Request.hpp"
// #include "Response.hpp"
// #include "Cluster.hpp"

int		main(int ac, char **av)
{
	if (ac == 2)
	{
		Cluster		cluster;
		try {
			cluster.config(av[1]);
			if (cluster.setup() == -1)
				return (1);
			cluster.run();
			cluster.clean();			
		}
		catch (std::exception &e) {
			std::cerr << e.what() << std::endl;
		}
	}
	else
	{
		std::cerr << RED << "Invalid number of arguments." << RESET << std::endl;
		return (1);
	}
	return (0);
}
