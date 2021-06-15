#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>

class Request
{
private:
	std::string method;
	std::string path;
	std::string version;
	std::map<std::string, std::string> headers;
public:
    std::string getPath() const;
    std::string getMethod() const;

	Request(void);
	Request(char * buffer);
	std::string get(void);
	~Request();
};

Request::Request(void) 
{}

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

Request::Request(char * buffer)
{
	std::vector<std::string> table;
	std::vector<std::string> first_line;
	std::vector<std::string> other_line;
	table = split_line(buffer);
	if (!table.empty())
	{
		first_line = split_first(table[0].data());
		if (first_line.size() == 3)
		{
			method = first_line[0];
			path = first_line[1];
			version = first_line[2];
		}
		for (int i = 1; i < table.size(); ++i)
		{
			other_line = split_headers(table[i].data());
			if (other_line.size() == 2)
				headers[other_line[0]] = other_line[1];
		}
	}
}
std::string Request::get(void) 
{
	std::string request;
	request = "METHOD:" + method + "\n" 
	+ "PATH:" + path + "\n"
	+ "VERSION:" + version + "\n";

	for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
		request += it->first + ":" + it->second + "\n";
	return (request);
}
 Request::~Request()
 {

 }