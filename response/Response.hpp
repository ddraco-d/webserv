/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: efumiko <efumiko@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/14 16:39:39 by efumiko           #+#    #+#             */
/*   Updated: 2021/06/17 11:23:57 by efumiko          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Request.hpp"
#include "../Config.hpp"
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/time.h>
#include <dirent.h>
#include <fcntl.h>

#include <sstream>
#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

#define DRCT 0
#define FILE 1
#define NON_EXIST -1

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
	
	std::map<std::size_t, std::string> _reason_phrases;
	std::map<std::string, std::string> _mime;

	
	std::string _host;
	size_t 		_port;
	bool		_autoindex;

	std::string _request_content;
	std::string _upload_path;
	std::string _name_file;
	std::string _res;
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
	std::string getListing();
	std::string read_file(std::string filepath);
	void initReasonPhrases();
	void initMIME();


	int getTypeFile(std::string filepath);
	//bool isCGI();

	std::string get_method();
	std::string post_method();
	std::string delete_method();

	std::string replace(std::string source, std::string to_replace, std::string new_value);
	//Location getLocation(const Request &request_conf, const Config &serv_conf);
	~Response();
};