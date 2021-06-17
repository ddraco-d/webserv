#ifndef PARCER_HPP
#define PARCER_HPP

#define NO_FIND std::string::npos

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>

#include <sys/types.h>
#include <sys/stat.h>
class Location
{
public:
	std::string name;
	std::map<std::string, std::string> more_info;
	
	void print(void)
	{
		std::cout << "MORE INFO LOCATION: " << name << ":\n";
		for (std::map<std::string, std::string>::iterator it = more_info.begin(); it != more_info.end(); ++it)
		{
			std::cout << "|" << it->first << ":" << it->second << "|\n";
		}
	}
private:
};

class Server
{
public:
	std::string name;
	std::map<std::string, std::string> more_info;
	std::map<std::string, Location> locations;
	
	uint32_t host;
	std::string host_string;
	uint16_t port;
	

	void print(void)
	{
		std::cout << "MORE INFO SERVER " << name << ":\n";
		for (std::map<std::string, std::string>::iterator it = more_info.begin(); it != more_info.end(); ++it)
		{
			std::cout << "|" << it->first << ":" << it->second << "|\n";
		}
		for (std::map<std::string, Location>::iterator it = locations.begin(); it != locations.end(); ++it)
		{
			it->second.print();
		}

	}
private:
};

class Config
{
	public:
		Config(){};
		Config(const std::string &path);
		std::vector<Server> servers;
	private:
		void init(std::string const &str);
		void pars_server(std::string const &str);
		void pars_location(std::string const &str, Server *server, std::string const &ln);
		size_t find_location(std::string const &str, int i);
		size_t find_server(std::string const &str, int i);
		void server_init(std::string const &str, Server *server);
		void location_init(std::string const &str, Location *location);
		bool check_brace(std::string const &str);
		bool find_in_set(char c, std::string const &set);
		std::string remove_delim(std::string item, std::string const &set);
		std::vector<std::string> split_line(const std::string &buffer);
		std::map<std::string, std::string> more_info_init(std::vector<std::string> info);

		void server_init_listen(std::string info, Server *server);

		void check_valid_server(void);
		void check_valid_location(Server *server);
		int dir_exists(const char* const path);
};

#endif