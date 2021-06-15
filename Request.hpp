#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include "Parcer.hpp"

class Request
{
private:
	std::string method;
	std::string path;
	std::string version;
	std::map<std::string, std::string> headers;
	std::string body;

	int check_valid(Server *server);
public:
	Request(char *buffer, Server *server);
	std::string get(void);
	int		status_code;
};

std::vector<std::string> split_line(const std::string &buffer)
{
	std::vector<std::string> table;
	std::stringstream ss;
	std::string item;
	ss.str(buffer);
	while (std::getline(ss, item, '\n'))
		table.push_back(item);
	return (table);
}

std::vector<std::string> split_first(const std::string &buffer)
{
	std::vector<std::string> table;
	std::stringstream ss;
	std::string item;
	ss.str(buffer);
	while (std::getline(ss, item, ' '))
		table.push_back(item);
	return (table);
}

std::vector<std::string> split_headers(const std::string &buffer)
{
	std::vector<std::string> table;
	std::stringstream ss;
	std::string item;
	char delim = ':';
	ss.str(buffer);
	while (std::getline(ss, item, delim))
	{
		std::string::iterator end_pos = std::remove(item.begin(), item.end(), ' ');
		item.erase(end_pos, item.end());
		table.push_back(item);
		delim = '\0';
	}
	return (table);
}

std::string remove_delim(std::string item, std::string const &set)
{
	for (int i = 0; i < set.size(); ++i)
	{
		std::string::iterator end_pos = std::remove(item.begin(), item.end(), set[i]);
		item.erase(end_pos, item.end());
	}
	return (item);
}

Request::Request(char *buffer, Server *server)
{
	std::vector<std::string> table;
	std::vector<std::string> first_line;
	std::vector<std::string> other_line;
	table = split_line(buffer);
	int i = 0;
	if (!table.empty())
	{
		first_line = split_first(table[i].data());
		if (first_line.size() == 3)
		{
			method = remove_delim(first_line[0], "\t\v\r");
			path = remove_delim(first_line[1], "\t\v\r");
			version = remove_delim(first_line[2], "\t\v\r");
		}
		for (i = 1; i < table.size() && table[i] != ""; ++i)
		{
			other_line = split_headers(table[i].data());
			if (other_line.size() == 2)
				headers[other_line[0]] = other_line[1];
		}
		if (table[i] == "")
		{
			i++;
			for ( ; i < table.size(); ++i)
			{
				body += table[i] + "\n";
			}
		}
	}
	status_code = check_valid(server);
}

int Request::check_valid(Server *server)
{
	if (!(method == "GET" || method == "POST" || method == "DELETE"))
		return (471);
	if (!(version == "HTTP/1.1"))
		return (400);
	if (server->locations.count(path) == 0 && path != "/")
		return (404);
	return (0);
}

std::string Request::get(void) 
{
	std::string request;
	request = "METHOD:" + method + "\n" 
	+ "PATH:" + path + "\n"
	+ "VERSION:" + version + "\n";

	for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
		request += it->first + ":" + it->second + "\n";
	
	request += "BODY: " + body;
	return (request);
}
