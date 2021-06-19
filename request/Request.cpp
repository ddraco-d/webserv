#include "Request.hpp"

// std::vector<std::string> Request::split_line(const std::string &buffer)
// {
// 	std::vector<std::string> table;
// 	std::stringstream ss;
// 	std::string item;
// 	ss.str(buffer);
// 	while (std::getline(ss, item, '\n'))
// 		table.push_back(item);
// 	return (table);
// }

// std::vector<std::string> Request::split_first(const std::string &buffer)
// {
// 	std::vector<std::string> table;
// 	std::stringstream ss;
// 	std::string item;
// 	ss.str(buffer);
// 	while (std::getline(ss, item, ' '))
// 		table.push_back(item);
// 	return (table);
// }

// std::vector<std::string> Request::split_headers(const std::string &buffer)
// {
// 	std::vector<std::string> table;
// 	std::stringstream ss;
// 	std::string item;
// 	char delim = ':';
// 	ss.str(buffer);
// 	while (std::getline(ss, item, delim))
// 	{
// 		std::string::iterator end_pos = std::remove(item.begin(), item.end(), ' ');
// 		item.erase(end_pos, item.end());
// 		table.push_back(item);
// 		delim = '\0';
// 	}
// 	return (table);
// }

// std::string Request::remove_delim(std::string item, std::string const &set)
// {
// 	for (int i = 0; i < set.size(); ++i)
// 	{
// 		std::string::iterator end_pos = std::remove(item.begin(), item.end(), set[i]);
// 		item.erase(end_pos, item.end());
// 	}
// 	return (item);
// }

// Request::Request(char *buffer, ServerConfig *server)
// {
// 	std::vector<std::string> table;
// 	std::vector<std::string> first_line;
// 	std::vector<std::string> other_line;
// 	table = split_line(buffer);
// 	int i = 0;
// 	if (!table.empty())
// 	{
// 		first_line = split_first(table[i].data());
// 		if (first_line.size() == 3)
// 		{
// 			method = remove_delim(first_line[0], "\t\v\r");
// 			path = remove_delim(first_line[1], "\t\v\r");
// 			req_path = remove_delim(first_line[1], "\t\v\r");
// 			version = remove_delim(first_line[2], "\t\v\r");
// 		}
// 		for (i = 1; i < table.size() && table[i] != ""; ++i)
// 		{
// 			other_line = split_headers(table[i].data());
// 			if (other_line.size() == 2)
// 				headers[other_line[0]] = other_line[1];
// 		}
// 		if (table[i] == "")
// 		{
// 			i++;
// 			for ( ; i < table.size(); ++i)
// 			{
// 				body += table[i] + "\n";
// 			}
// 		}
// 	}
// 	host = server->getHost();
// 	host_string = server->getHostString();
// 	port = server->getPort();

// 	path_post = "./";
// 	autoindex = false;
// 	is_cgi = false;

// 	status_code = check_valid(server);
// 	std::cout << "REQ  PATH: " << req_path << "\n";
// 	std::cout << "ITOG PATH: " << path << "\n";
// 	if (autoindex == true)
// 		std::cout << "autoindex: on\n";
// 	else
// 		std::cout << "autoindex: off\n";
// }

// int Request::dir_exists(const char* const path)
// {
//     struct stat info;

//     int statRC = stat(path, &info);
//     if( statRC != 0 )
//     {
//         if (errno == ENOENT)
// 			return (0); // something along the path does not exist
//         if (errno == ENOTDIR)
// 			return (0); // something in path prefix is not a dir
//         return (-1);
//     }
//     return ((info.st_mode & S_IFDIR ) ? 1 : 2);
// }

// int Request::check_valid_path(ServerConfig *server)
// {
// 	std::string url = "/";
// 	std::map<std::string, Location> locs = server->getLocation();
// 	for (std::map<std::string, Location>::iterator it = locs.begin(); it != locs.end(); ++it)
// 	{
// 		if (path.find(it->first, 0) == 0)
// 			if (it->first.length() >= url.length())
// 				url = it->first;
// 	}
// 	//зная где мы валидируем и меняем путь если все ок
// 	if (url == "/")
// 	{
// 		//остаемся в сервере
// 		if (path == url)
// 		{
// 			if (server->getMoreInfo().count("index") == 1)
// 				path = path + server->getMoreInfo()["index"];
// 			else
// 			{
// 				if (server->getMoreInfo().count("autoindex") == 1)
// 					if (server->getMoreInfo()["autoindex"] == "on")
// 						autoindex = true;
// 			}
// 		}
// 		if (server->getMoreInfo().count("root") == 1)
// 			path = server->getMoreInfo()["root"] + path;
// 		if (dir_exists(path.c_str()) <= 0)
// 			return (404);

// 		if (server->getMoreInfo().count("allow_methods") == 1)
// 			if (server->getMoreInfo()["allow_methods"].find(method) == NO_FIND)
// 				return (405);
		
// 		if (server->getMoreInfo().count("client_body_buffer_size") == 1)
// 		{
// 			if (std::stoi(server->getMoreInfo()["client_body_buffer_size"]) < body.size())
// 				return (413);
// 		}
// 		else
// 		{
// 			if (12656974 < body.size())
// 				return (413);
// 		}		
// 	}
// 	else
// 	{
// 		if (path == url)
// 		{
// 			if (server->getLocation()[url].more_info.count("index") == 1)
// 				path = path + server->getLocation()[url].more_info["index"];
// 			else
// 			{
// 				if (server->getLocation()[url].more_info.count("autoindex") == 1)
// 					if (server->getLocation()[url].more_info["autoindex"] == "on")
// 						autoindex = true;
// 			}
// 		}
// 		std::string  ending = std::string(path.begin() + url.size(), path.end());
// 		if (server->getLocation()[url].more_info.count("root") == 1)
// 				path = server->getLocation()[url].more_info["root"] + ending;
		
// 		if (server->getLocation()[url].more_info.count("cgi_bin") == 0)
// 			if (dir_exists(path.c_str()) <= 0)
// 				return (404);

// 		if (server->getLocation()[url].more_info.count("allow_methods") == 1)
// 			if (server->getLocation()[url].more_info["allow_methods"].find(method) == NO_FIND)
// 				return (405);
		
// 		if (server->getLocation()[url].more_info.count("client_body_buffer_size") == 1)
// 		{
// 			if (std::stoi(server->getLocation()[url].more_info["client_body_buffer_size"]) < body.size())
// 				return (413);
// 		}
// 		else
// 		{
// 			if (12656974 < body.size())
// 				return (413);
// 		}
		
// 		if (server->getLocation()[url].more_info.count("cgi_bin") == 1)
// 		{
// 			cgi_arg = ending;
// 			is_cgi = true;
// 			if (server->getLocation()[url].more_info.count("root") == 1)
// 				path = server->getLocation()[url].more_info["root"] + "/" + remove_delim(server->getLocation()[url].more_info["cgi_bin"], " \t\v\r");
// 			if (dir_exists(path.c_str()) <= 0)
// 				return (404);
// 			std::cout << "ARG_CGI:" << cgi_arg << "|\n";
// 		}
// 	}
// 	return (0);
// }



// int Request::check_valid(ServerConfig *server)
// {
// 	if (!(method == "GET" || method == "POST" || method == "DELETE"))
// 		return (405);
// 	if (!(version == "HTTP/1.1"))
// 		return (505);
// 	if (!(path[0] == '/'))
// 		return (505);
// 	return (check_valid_path(server));
// }

// std::string Request::get(void) 
// {
// 	std::string request;
// 	request = "METHOD:" + method + "\n" 
// 	+ "PATH:" + path + "\n"
// 	+ "VERSION:" + version + "\n";

// 	for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
// 		request += it->first + ":" + it->second + "\n";
	
// 	request += "BODY: " + body;
// 	return (request);
// }

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

Request::Request(char *buffer, ServerConfig *server)
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
	is_cgi = false;

	status_code = check_valid(server);
	std::cout << "\nREQ  PATH: " << req_path << "\n";
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
	if (url == "/")
	{
		//остаемся в сервере
		if (path == url || path == url + "/") 
		{
			if (server->more_info.count("index") == 1)
				path = path + "/" + server->more_info["index"];
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
		if (path == url || path == url + "/")
		{
			if (server->locations[url].more_info.count("index") == 1)
				path = path + "/" + server->locations[url].more_info["index"];
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
		
		if (server->locations[url].more_info.count("cgi_bin") == 0)
			if (dir_exists(path.c_str()) <= 0)
				return (404);

		if (server->locations[url].more_info.count("allow_methods") == 1)
			if (server->locations[url].more_info["allow_methods"].find(method) == NO_FIND)
			{
				std::cout << "!!!!!!!tut!!!!!!!!\n";
				return (405);
			}
		
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
		
		if (server->locations[url].more_info.count("cgi_bin") == 1)
		{
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
	return (0);
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