#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
// #include <algorithm>


std::vector<std::string> split_arg(const std::string &buffer)
{
	std::vector<std::string> table;
	std::stringstream ss;
	std::string item;
	char delim = '&';
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

int main(int argc, char **argv, char **envp)
{
	std::string argument;
	std::cin >> argument;
	argument = remove_delim(argument, "?");
	std::vector<std::string> table = split_arg(argument);
	std::string fn, ln;
	fn = std::string(table[0].begin() + table[0].find("=") + 1, table[0].end());
	ln = std::string(table[1].begin() + table[1].find("=") + 1, table[1].end());
	std::cout << "<head>\n<style>\nbody {\ncolor: #ffffff;\nbackground: linear-gradient(#151866, #156b51);\npadding: 250px;\n}\n</style>\n<body>\n" << "Hello, " << fn << " " << ln << " from our webservice!;)" << "\n</head>";
	std::ofstream cookie("cookie.txt");
	if (cookie.is_open())
		cookie << "Set-Cookie:fn=" << fn << "\nSet-Cookie:ln=" << ln << "\n";
	return (0);
}