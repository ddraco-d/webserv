#include "Request.hpp"

std::vector<std::string> Request::split_line(const std::string &buffer)
{
	std::vector<std::string> table;
	std::stringstream ss;
	std::string item;
	ss.str(buffer);
	while (std::getline(ss, item, '\n'))
		table.push_back(item);
	return (table);
}

std::vector<std::string> Request::split_first(const std::string &buffer)
{
	std::vector<std::string> table;
	std::stringstream ss;
	std::string item;
	ss.str(buffer);
	while (std::getline(ss, item, ' '))
		table.push_back(item);
	return (table);
}

std::vector<std::string> Request::split_headers(const std::string &buffer)
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

std::string Request::remove_delim(std::string item, std::string const &set)
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
			req_path = remove_delim(first_line[1], "\t\v\r");
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
	host = server->host;
	host_string = server->host_string;
	port = server->port;

	path_post = "./";
	autoindex = false;
	status_code = check_valid(server);
	std::cout << "PATH: " << path << "\n";
	if (autoindex == true)
		std::cout << "autoindex: on\n";
	else
		std::cout << "autoindex: off\n";
}

int Request::dir_exists(const char* const path)
{
    struct stat info;

    int statRC = stat(path, &info);
    if( statRC != 0 )
    {
        if (errno == ENOENT)
			return (0); // something along the path does not exist
        if (errno == ENOTDIR)
			return (0); // something in path prefix is not a dir
        return (-1);
    }
    return ((info.st_mode & S_IFDIR ) ? 1 : 2);
}

int Request::check_valid_path(Server *server)
{
	std::string url = "/";
	for (std::map<std::string, Location>::iterator it = server->locations.begin(); it != server->locations.end(); ++it)
	{
		if (path.find(it->first, 0) == 0)
			if (it->first.length() >= url.length())
				url = it->first;
	}
	//зная где мы валидируем и меняем путь если все ок
	if (url == "/")
	{
		//остаемся в сервере
		if (path == url)
		{
			if (server->more_info.count("index") == 1)
				path = path + server->more_info["index"];
			else
			{
				if (server->more_info.count("autoindex") == 1)
					if (server->more_info["autoindex"] == "on")
						autoindex = true;
			}
		}
		if (server->more_info.count("root") == 1)
			path = server->more_info["root"] + path;
		if (dir_exists(path.c_str()) <= 0)
			return (404);

		if (server->more_info.count("allow_methods") == 1)
			if (server->more_info["allow_methods"].find(method) == NO_FIND)
				return (405);
		
		if (server->more_info.count("client_body_buffer_size") == 1)
		{
			if (std::stoi(server->more_info["client_body_buffer_size"]) < body.size())
				return (413);
		}
		else
		{
			if (12656974 < body.size())
				return (413);
		}		
	}
	else
	{
		if (path == url)
		{
			if (server->locations[url].more_info.count("index") == 1)
				path = path + server->locations[url].more_info["index"];
			else
			{
				if (server->locations[url].more_info.count("autoindex") == 1)
					if (server->locations[url].more_info["autoindex"] == "on")
						autoindex = true;
			}
		}
		std::string  ending = std::string(path.begin() + url.size(), path.end());
		if (server->locations[url].more_info.count("root") == 1)
				path = server->locations[url].more_info["root"] + ending;
		
		if (dir_exists(path.c_str()) <= 0)
			return (404);

		if (server->locations[url].more_info.count("allow_methods") == 1)
			if (server->locations[url].more_info["allow_methods"].find(method) == NO_FIND)
				return (405);
		
		if (server->locations[url].more_info.count("client_body_buffer_size") == 1)
		{
			if (std::stoi(server->locations[url].more_info["client_body_buffer_size"]) < body.size())
				return (413);
		}
		else
		{
			if (12656974 < body.size())
				return (413);
		}
		

				/*
		if (server->locations[url].more_info.count("cgi_bin") == 1)
		{
			cgi_arg = std::string(path.begin() + url.size(), path.end());
			if (server->locations[url].more_info.count("root") == 1)
				path = server->locations[url].more_info["root"] + "/" + remove_delim(server->locations[url].more_info["cgi_bin"], " \t\v\r");
			std::cout << "ARG_CGI:" << cgi_arg << "\n";
			return(404);
		}
		else
		{
			if (server->locations[url].more_info.count("root") == 1)
				path = server->locations[url].more_info["root"] + ending;
		}
		*/
	}
	return (0);
}



int Request::check_valid(Server *server)
{
	if (!(method == "GET" || method == "POST" || method == "DELETE"))
		return (405);
	if (!(version == "HTTP/1.1"))
		return (505);
	return (check_valid_path(server));
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