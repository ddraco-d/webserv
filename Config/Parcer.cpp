#include "Parcer.hpp"

void Config::config_start()
{
	std::ifstream conf("defolt.conf");
	if (!conf.is_open())
		throw "no defolt.conf";
	conf.close();
	config_start("defolt.conf");
}

void Config::config_start(const std::string &path)
{
	std::ifstream conf(path);
	std::stringstream ss;
	if (!conf.is_open())
		throw "config does not open";
	ss << conf.rdbuf();
	init(ss.str());
	check_valid_server();
	conf.close();
}

std::vector<ServerConfig> Config::getServers()
{
	return (servers);
}

void Config::check_valid_server(void)
{
	for (int i = 0; i < servers.size(); ++i)
	{
		std::string path_ser;
		if (servers[i].more_info.count("root") == 0)
			throw "no root in server";
		else
			path_ser = servers[i].more_info["root"];
		if (servers[i].more_info["root"][0] != '.')
				throw "no valid root server expected ./";
		else if (dir_exists(servers[i].more_info["root"].c_str()) != 1)
			throw "no valid path in root server";
		if (servers[i].more_info.count("index") == 1)
		{
			path_ser += "/" + (servers[i]).more_info["index"];
			if (dir_exists(path_ser.c_str()) != 2)
				throw "bad index in server";
		}
		if (servers[i].more_info.count("cgi_bin") == 1)
			throw "cgi_bin only in location";
		check_valid_location(&(servers[i]));
	}
}

void Config::check_valid_location(ServerConfig *server)
{
	for (std::map<std::string, Location>::iterator it = server->locations.begin(); it != server->locations.end(); ++it)
	{
		std::string path_loc;
		if (it->second.more_info.count("root") == 1)
		{
			path_loc = it->second.more_info["root"];
			if (path_loc[0] != '.')
				throw "no valid root location expected ./";
			if (dir_exists(path_loc.c_str()) != 1)
				throw "no valid path in root location";
		}
		else
			path_loc = "." + it->first;
		
		if (it->second.more_info.count("index") == 1 && it->second.more_info.count("cgi_bin") == 1)
				throw "available only index or cgi in location";
		if (it->second.more_info.count("index") == 1)
		{
			path_loc += "/" + it->second.more_info["index"];
			if (dir_exists(path_loc.c_str()) != 2)
				throw "bad index in location";
		}
		if (it->second.more_info.count("cgi_bin") == 1)
		{
			path_loc += "/" + it->second.more_info["cgi_bin"];
			if (dir_exists(path_loc.c_str()) != 2)
				throw "bad cgi_bin in location";
		}
	}
}

int Config::dir_exists(const char* const path)
{
    struct stat info;

    int statRC = stat(path, &info);
    if( statRC != 0 )
    {
        if (errno == ENOENT)
			return (0); // something along the path does not exist
        if (errno == ENOTDIR)
			return (0); // something in path prefix is not a dir
        return -1;
    }
    return ((info.st_mode & S_IFDIR ) ? 1 : 2);
}

void Config::init(std::string const &str)
{
	size_t i = 0;
	size_t idx = 0;
	size_t idx2 = 0;
	if (find_server(str, i) == NO_FIND)
		throw "no server in config";
	while ((idx = find_server(str, i)) != NO_FIND)
	{
		if ((idx2 = find_server(str, idx+1)) != NO_FIND)
			pars_server(std::string(str.begin()+idx+6, str.begin()+idx2));
		else
			pars_server(std::string(str.begin()+idx+6, str.end()));
		i = idx + 1;
	}
}

void Config::pars_server(std::string const &str)
{
	size_t i = 0;
	size_t idx = 0;
	size_t idx2 = 0;
	ServerConfig svr;
	check_brace(str);
	std::string str2 = std::string(str.begin()+str.find("{")+1, str.begin()+str.rfind("}"));
	int g = (find_location(str2, 0) == NO_FIND) ? str2.length() : find_location(str2, 0);
	server_init(std::string(str2.begin(), str2.begin()+ g), &svr);
	while ((idx = find_location(str2, i)) != NO_FIND)
	{
		int count_brace = 0;
		for (idx2 = idx; idx2 < str2.length() && str2[idx2] != '{'; ++idx2);
		if  (str2[idx2] == '{')
		{
			count_brace++;
			idx2++;
			for ( ; idx2 < str2.length() && count_brace != 0; ++idx2)
				count_brace += (str2[idx2] == '{') ? 1 : ((str2[idx2] == '}') ? -1 : 0);
			if (count_brace == 0)
				pars_location(std::string(str2.begin()+idx+9, str2.begin() + idx2), &svr, "");
		}
		i = idx2;
	}
	servers.push_back(svr);
	//svr.print();
}

bool Config::check_brace(std::string const &str)
{
	int idx_begin = str.find("{");
	int idx_end = str.rfind("}");
	if (idx_begin == NO_FIND || idx_end == NO_FIND)
		throw "bad brace";
	for (int i = 0; i < idx_begin; ++i)
		if (!find_in_set(str[i], " \n\t\v\r"))
			throw "extra characters between brace";
	for (int i = idx_end + 1; i < str.length(); ++i)
		if (!find_in_set(str[i], " \n\t\v\r"))
			throw "extra characters between brace";
	int count_brace = 0;
	for (int i = idx_begin + 1; i < idx_end; ++i)
	{
		if (count_brace < 0)
			throw "bad brace";
		count_brace += (str[i] == '{') ? 1 : ((str[i] == '}') ? -1 : 0);
	}
	if (count_brace != 0)
		throw "bad brace";
	return (1);
}

bool Config::find_in_set(char c, std::string const &set)
{
	for (int i = 0; set[i]; ++i)
		if (set[i] == c)
			return (true);
	return(false);
}

size_t Config::find_location(std::string const &str, int i)
{
	size_t idx = 0;
	while ((idx = str.find("location ", i)) != NO_FIND)
	{
		if (idx == 0)
			return (idx);
		if (find_in_set(str[idx-1], " \n\t\v\r"))
			return (idx);
		i = idx + 1;
	}
	return (idx);
}

void Config::server_init_listen(std::string info, ServerConfig *server)
{
	int idx = info.find(":", 0);
	server->host_string = std::string(info.begin(), info.begin()+idx);
	server->host = std::stoi(std::string(info.begin(), info.begin()+idx));
	server->port = std::stoi(std::string(info.begin()+idx+1, info.end()));
}

void Config::server_init(std::string const &str, ServerConfig *server)
{
	std::vector<std::string> info = split_line(remove_delim(std::string(str), "\t\v\r"));
	server->more_info = more_info_init(info);
	
	if (server->more_info.count("listen"))
		server_init_listen(server->more_info["listen"], server);
	else
		throw "no listen in server";
}

std::string Config::remove_delim(std::string item, std::string const &set)
{
	for (int i = 0; i < set.size(); ++i)
	{
		std::string::iterator end_pos = std::remove(item.begin(), item.end(), set[i]);
		item.erase(end_pos, item.end());
	}
	return (item);
}

std::vector<std::string> Config::split_line(const std::string &buffer)
{
	std::vector<std::string> table;
	std::stringstream ss;
	std::string item;
	ss.str(buffer);
	while (std::getline(ss, item, '\n'))
	{
		if (item.size() > 0)
			table.push_back(item);
	}
	return (table);
}

std::map<std::string, std::string> Config::more_info_init(std::vector<std::string> info)
{
	std::map<std::string, std::string> more_info;
	for (int i = 0; i < info.size(); ++i)
	{
		int g = info[i].find(" ");
		if (g != NO_FIND)
			more_info[std::string(info[i].begin(), info[i].begin()+g)] = std::string(info[i].begin()+g+1, info[i].end());
		else
			throw "argument without value";
	}
	return (more_info);
}

void Config::location_init(std::string const &str, Location *location)
{
	std::vector<std::string> info = split_line(remove_delim(std::string(str), "\t\v\r"));
	location->more_info = more_info_init(info);
}

void Config::pars_location(std::string const &str, ServerConfig *server, std::string const &ln)
{
	int i = 0;
	int idx = 0;
	int idx2 = 0;
	Location loc;
	loc.name = remove_delim(std::string(str.begin(), str.begin()+str.find("{")), " \n\t\v\r");
	if	(loc.name[0] == '/')
		loc.name =  ln + loc.name;
	else
		throw "bed location name, expected /*";
	std::string str2 = std::string(str.begin()+str.find("{"), str.begin()+str.rfind("}")+1);
	check_brace(str2);
	str2 = std::string(str2.begin()+str2.find("{")+1, str2.begin()+str2.rfind("}"));
	int g = (find_location(str2, 0) == NO_FIND) ? str2.length() : find_location(str2, 0);
	location_init(std::string(str2.begin(), str2.begin()+ g), &loc);
	while ((idx = find_location(str2, i)) != NO_FIND)
	{
		idx2 = 0;
		int count_brace = 0;
		for (idx2 = idx; idx2 < str2.length() && str2[idx2] != '{'; ++idx2);
		if  (str2[idx2] == '{')
		{
			count_brace++;
			idx2++;
			for ( ; idx2 < str2.length() && count_brace != 0; ++idx2)
				count_brace += (str2[idx2] == '{') ? 1 : ((str2[idx2] == '}') ? -1 : 0);
			if (count_brace == 0)
				pars_location(std::string(str2.begin()+idx+9, str2.begin() + idx2), server, loc.name);	
		}
		i = idx2;
	}
	server->locations[loc.name] = loc;
}

size_t Config::find_server(std::string const &str, int i)
{
	size_t idx = 0;
	while ((idx = str.find("server", i)) != NO_FIND)
	{
		if ((idx == 0) && find_in_set(str[idx+6], " \n\t\v\r{"))
			return (idx);
		if (find_in_set(str[idx-1], " \n\t\v\r}") && find_in_set(str[idx+6], " \n\t\v\r{"))
			return (idx);
		i = idx + 1;
	}
	return (idx);
}