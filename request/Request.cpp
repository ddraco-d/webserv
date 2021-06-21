#include "Request.hpp"

std::vector<std::string> Request::split_line(std::string buffer)
{
	std::vector<std::string> table(0);
	std::stringstream ss(buffer);
	std::string item;
	while (std::getline(ss, item, '\n'))
	{
		table.push_back(remove_delim(item, "\t\r\v"));
		std::cout << remove_delim(item, "\t\r\v") << "|\n";
	}
	return (table);
}

std::vector<std::string> Request::split_first(std::string buffer)
{
	std::vector<std::string> table;
	std::stringstream ss(buffer);
	std::string item;
	while (std::getline(ss, item, ' '))
		table.push_back(item);
	return (table);
}

std::vector<std::string> Request::split_headers(std::string buffer)
{
	std::vector<std::string> table;
	std::stringstream ss(buffer);
	std::string item;
	char delim = ':';
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
	for (unsigned long i = 0; i < set.size(); ++i)
	{
		std::string::iterator end_pos = std::remove(item.begin(), item.end(), set[i]);
		item.erase(end_pos, item.end());
	}
	return (item);
}

Request::Request(char *buffer, ServerConfig *server)
{
	std::vector<std::string> table(0);
	std::vector<std::string> first_line;
	std::vector<std::string> other_line;
	_allow_methods = "";
	table = split_line(std::string(buffer));
	unsigned long i = 0;
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
		for (i = 1; table[i] != "" && i < table.size(); ++i)
		{
			//std::cout << "TAB: " << table[i] << "|\n";;
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
	//std::cout << "\nBODY: " << body << "|\n";
	host = server->host;
	host_string = server->host_string;
	port = server->port;

	path_post = "./";
	autoindex = false;
	is_cgi = false;

	status_code = check_valid(server);
	std::cout << "REQ  PATH: " << req_path << "\n";
	std::cout << "ITOG PATH: " << path << "\n";
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

int Request::check_valid_path(ServerConfig *server)
{
	std::string url = "/";
	for (std::map<std::string, Location>::iterator it = server->locations.begin(); it != server->locations.end(); ++it)
	{
		if (path.find(it->first, 0) == 0)
			if (it->first.length() >= url.length())
				url = it->first;
	}
	//зная где мы валидируем и меняем путь если все ок
	if (server->more_info.count("autoindex") == 1)
		if (server->more_info["autoindex"] == "on")
			autoindex = true;
	if (url == "/")
	{
		//остаемся в сервере
		_allow_methods = server->more_info["allow_methods"];
		if (path == url || path == url + "/") 
		{
			if (server->more_info.count("index") == 1)
				path = path + "/" + server->more_info["index"];
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
			if (std::stoi(server->more_info["client_body_buffer_size"]) < (int)body.size())
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
		_allow_methods = server->locations[url].more_info["allow_methods"];
		if (path == url || path == url + "/")
		{
			if (server->locations[url].more_info.count("index") == 1)
				path = path + "/" + server->locations[url].more_info["index"];
		}
		std::string  ending = std::string(path.begin() + url.size(), path.end());
		if (server->locations[url].more_info.count("root") == 1)
				path = server->locations[url].more_info["root"] + ending;
		if (server->locations[url].more_info.count("cgi_bin") == 0)
			if (dir_exists(path.c_str()) <= 0)
				return (404);
		if (server->locations[url].more_info.count("allow_methods") == 1)
			if (server->locations[url].more_info["allow_methods"].find(method) == NO_FIND)
				return (405);
		if (server->locations[url].more_info.count("client_body_buffer_size") == 1)
		{
			if (std::stoi(server->locations[url].more_info["client_body_buffer_size"]) < (int)body.size())
				return (413);
		}
		else
		{
			if (12656974 < body.size())
				return (413);
		}
		if (server->locations[url].more_info.count("cgi_bin") == 1)
		{
			if (!(method == "GET" || method == "POST"))
				return (405);
			if (method == "GET")
				cgi_arg = ending;
			if (method == "POST")
				cgi_arg = body;
			is_cgi = true;
			if (server->locations[url].more_info.count("root") == 1)
				path = server->locations[url].more_info["root"] + "/" + remove_delim(server->locations[url].more_info["cgi_bin"], " \t\v\r");
			if (dir_exists(path.c_str()) <= 0)
				return (404);
			std::cout << "ARG_CGI:" << cgi_arg << "|\n";
		}
	}
	remove_duplicate_slash();
	return (0);
}

void  Request::remove_duplicate_slash()
{
	unsigned long idx;
	while((idx = path.find("//")) != NO_FIND)
	{
		path.erase(idx, 1);
	}
	while((idx = req_path.find("//")) != NO_FIND)
	{
		req_path.erase(idx, 1);
	}
	if (path[path.size()-1] == '/')
		path.erase(path.size()-1, 1);
	if (req_path[req_path.size()-1] == '/')
		req_path.erase(req_path.size()-1, 1);
}

int Request::check_valid(ServerConfig *server)
{
	if (!(method == "GET" || method == "POST" || method == "DELETE"))
		return (405);
	if (!(version == "HTTP/1.1"))
		return (505);
	if (!(path[0] == '/'))
		return (505);
	return (check_valid_path(server));
}


std::string Request::getAllowMethods() 
{
	return _allow_methods;
}