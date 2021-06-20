#ifndef REQUEST_HPP
#define REQUEST_HPP
#include "../Config/Parcer.hpp"

class Request
{
private:
	int check_valid(ServerConfig *server);
	int check_valid_path(ServerConfig *server);
	int dir_exists(const char* const path);
	void  remove_duplicate_slash();
	std::vector<std::string> split_line(const std::string &buffer);
	std::vector<std::string> split_first(const std::string &buffer);
	std::vector<std::string> split_headers(const std::string &buffer);
	std::string remove_delim(std::string item, std::string const &set);
public:
	Request(){};
	Request(char *buffer, ServerConfig *server);
<<<<<<< HEAD:request/Request.hpp
	std::string get(void);
	
=======
>>>>>>> ashea:Request.hpp
	int		status_code;
	bool autoindex;
	std::string method;
	std::string req_path;
	std::string path;
	std::string version;
	std::string body;

	std::string cgi_arg;
	bool		is_cgi;

	std::map<std::string, std::string> headers;

	uint32_t host;
	uint16_t port;
	std::string host_string;

	std::string path_post;
};

#endif