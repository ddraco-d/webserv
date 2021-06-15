#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#define NO_FIND std::string::npos


class Location
{
public:
	// std::vector<std::string> getMethods(void); // todo: create method that return vector of allowed methods in location
	// std::string getRoot(); // написать
	// std::string getIndex(); // написать

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
	std::string GetName(void)
	{
		return (name);
	}
	void print(void)
	{
		std::cout << "MORE INFO SERVER:\n";
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
		Config(void);
		Config(const std::string &path);
		std::map<std::string, Server> servers;
		
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
};

Config::Config(void)
{}

Config::Config(const std::string &path)
{
	std::ifstream html(path);
	std::stringstream ss;
	ss << html.rdbuf();
	init(ss.str());
}

void Config::init(std::string const &str)
{
	size_t i = 0;
	size_t idx = 0;
	size_t idx2 = 0;
	
	while ((idx = find_server(str, i)) != std::string::npos)
	{
		if ((idx2 = find_server(str, idx+1)) != std::string::npos)
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
	Server svr;
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
	servers[svr.GetName()] = svr;
	svr.print();
}

bool Config::check_brace(std::string const &str)
{
	int idx_begin = str.find("{");
	int idx_end = str.rfind("}");
	if (idx_begin == NO_FIND || idx_end == NO_FIND)
		std::cout << "---------ERROR--------------\n";
	for (int i = 0; i < idx_begin; ++i)
		if (!find_in_set(str[i], " \n\t\v\r"))
			std::cout << str[i] << "---------ERROR--------------\n";
	for (int i = idx_end + 1; i < str.length(); ++i)
		if (!find_in_set(str[i], " \n\t\v\r"))
			std::cout << str[i] << "---------ERROR--------------\n";
	int count_brace = 0;
	for (int i = idx_begin + 1; i < idx_end; ++i)
	{
		if (count_brace < 0)
			exit(-1);
		count_brace += (str[i] == '{') ? 1 : ((str[i] == '}') ? -1 : 0);
	}
	if (count_brace != 0)
	{
		std::cout << "brace---------ERROR--------------\n";
		return (0);
	}
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
	while ((idx = str.find("location ", i)) != std::string::npos)
	{
		if (idx == 0)
			return (idx);
		if (find_in_set(str[idx-1], " \n\t\v\r"))
			return (idx);
		i = idx + 1;
	}
	return (idx);
}

void Config::server_init(std::string const &str, Server *server)
{
	std::vector<std::string> info = split_line(remove_delim(std::string(str), "\t\v\r"));
	server->more_info = more_info_init(info);
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
		{
			more_info[std::string(info[i].begin(), info[i].begin()+g)] = std::string(info[i].begin()+g+1, info[i].end());
		}
		else
			std::cout << "NO_INFO:" << info[i] << "\n";
	}
	return (more_info);
}

void Config::location_init(std::string const &str, Location *location)
{
	std::vector<std::string> info = split_line(remove_delim(std::string(str), "\t\v\r"));
	location->more_info = more_info_init(info);
}

void Config::pars_location(std::string const &str, Server *server, std::string const &ln)
{
	int i = 0;
	int idx = 0;
	int idx2 = 0;
	Location loc;
	loc.name = std::string(str.begin(), str.begin()+str.find("{"));
	loc.name = remove_delim(std::string(ln+loc.name), " \n\t\v\r");
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
	while ((idx = str.find("server", i)) != std::string::npos)
	{
		if ((idx == 0) && find_in_set(str[idx+6], " \n\t\v\r{"))
			return (idx);
		if (find_in_set(str[idx-1], " \n\t\v\r}") && find_in_set(str[idx+6], " \n\t\v\r{"))
			return (idx);
		i = idx + 1;
	}
	return (idx);
}