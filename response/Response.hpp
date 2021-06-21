/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: efumiko <efumiko@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/14 16:39:39 by efumiko           #+#    #+#             */
/*   Updated: 2021/06/21 22:13:53 by efumiko          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../request/Request.hpp"
#include "../Config/Parcer.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include "string.h"
#include "stdlib.h"
//#include "wait.h"

#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

#define DRCT 0
#define FILE 1
#define NON_EXIST -1

void		ft_free_array(char ***ar);
char		**ft_split(char const *s, char c);
class Response
{
private:
	std::string _response;
	std::string _first_line;
	std::map<std::string, std::string> _headers;

	std::string _body_content;
	
	size_t		_code;
	std::string	_method;
	std::string	_path_to_res;
	
	std::map<std::size_t, std::string> _reason_phrases;
	std::map<std::string, std::string> _mime;

	std::string _host;
	uint16_t 	_port;
	bool		_autoindex;

	std::string _request_content;
	std::string _upload_path;
	std::string _name_file;
	std::string _res;

	bool		_is_cgi;
	std::string	_cgi_arg;
	std::string _methods;
	
	std::string _error_path;
public:
//	Response();
//	Response(const Request &request_conf, const Config &serv_conf);
	Response(Request request);
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
	std::string getMethod() const;
	void initReasonPhrases();
	void initMIME();


	int getTypeFile(std::string filepath);
	//bool isCGI();

	std::string get_method();
	std::string post_method();
	std::string delete_method();

	std::string replace(std::string source, std::string to_replace, std::string new_value);
	//Location getLocation(const Request &request_conf, const Config &serv_conf);
	std::string run_cgi();
};

#endif