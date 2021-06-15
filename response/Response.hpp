/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: efumiko <efumiko@student.21-school.ru>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/14 16:39:39 by efumiko           #+#    #+#             */
/*   Updated: 2021/06/15 19:06:45 by efumiko          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Request.hpp"
#include "../Config.hpp"
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/time.h>

#include <sstream>
#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

class Request;
class Config;

class Response
{
private:
	std::string _response;
	std::string _first_line;
	std::map<std::string, std::string> _headers;
	Location 	_location;
	std::string _body_content;
	
	Request 	_req_conf;
	Config 		_serv_conf;
	
	size_t		_code;
	std::string	_method;
	std::string	_path_to_res;
	
public:
	Response();
	Response(const Request &request_conf, const Config &serv_conf);

	std::string getResponse();
	std::string createResponse(std::string body);
	std::string getErrorPage();
	std::string getMIME();
	std::string getLastModif();
	std::string getCurrentDate(void);
	std::string formatDate(time_t date);
	std::string getReasonPhrase();
	int read_file();


	bool isDirectory();
	bool isCGI();
	bool isFile();

	std::string get_method();
	//std::string post_method();
	//std::string delete_method();


	//Location getLocation(const Request &request_conf, const Config &serv_conf);
	~Response();
};