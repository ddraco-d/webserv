/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: efumiko <efumiko@student.21-school.ru>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/15 18:42:33 by efumiko           #+#    #+#             */
/*   Updated: 2021/06/15 21:11:16 by efumiko          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(void)
{
    _code = 200;
    _method = "GET";
    _path_to_res = "./test/www/index.html";
}

Response::Response(const Request &request_conf, const Config &serv_conf)
{
	//_req_conf = request_conf;
	//_serv_conf = serv_conf;
	//_code = request_conf.getCode();
	_code = 200;
	//_location = getLocation(request_conf, serv_conf);
	//_method = request_conf.getMethod();
	_method = "GET";
	//_path_to_res = request.conf.getPath();
	_path_to_res = "./test/www/index.html";
}

Response::~Response()
{}

std::string Response::getResponse()
{
	if (_code == 413)
		return (createResponse(getErrorPage()));
	if (_code == 405)
	{
		//std::vector<std::string> methods = _location.getMethods();
		std::vector<std::string> methods;
        methods.push_back("GET");
        methods.push_back("POST");
        methods.push_back("DELETE");
         
        std::string methods_str;

		for (size_t i = 0; i < methods.size(); ++i)
		{
			methods_str += methods[i];
			if (i < methods.size() - 1)
				methods_str += ", ";
		}
		_headers["Allow"] = methods_str;
		return (createResponse(getErrorPage()));
	}
	if (_code == 404)
		return (createResponse(getErrorPage()));
	
	// путь к ресурсу
	//path_to_res = _location.getRoot(); // путь к корню из конфига
	//if (path_to_res[path_to_res.size() - 1] == '/') // убираем /
	//	path_to_res = std::string(path_to_res, 0, path_to_res.size() - 1);
	//path_to_res += request_conf.getPath();

	// автоиндекс и путь это директория.
	//if (isDirectory()) {}
	//if (isCGI()) {}

	if (_method == "GET") {
		return get_method();
	}
	// if (_method == "POST") {
	// 	return post_method();
	// }
	// if (_method == "DELETE") {
	// 	return delete_method();
	//}
	return NULL;
}

std::string Response::get_method()
{
    time_t file_date;
	int code_read = read_file();
	if (code_read == 403 || code_read == 404)
		return (createResponse(getErrorPage()));
	_headers["Content-Type"] = getMIME();
	_headers["Last-Modified"] = getLastModif();
	_code = 200;
    return createResponse(_body_content);
}

std::string Response::getLastModif()
{
	struct stat buffer;
	struct timezone tz;
	struct timeval t;
    time_t last_date;

	gettimeofday(&t, &tz);
	int exist = stat(_path_to_res.c_str(), &buffer);
    last_date = buffer.st_mtime + tz.tz_minuteswest * 60;
    return (formatDate(last_date));
}

int Response::read_file()
{
	std::ifstream		file;
	std::stringstream	buffer;

	file.open(_path_to_res.c_str(), std::ifstream::in);
	if (isFile())
	{
		if (file.is_open() == false)
		{
			_code = 403;
			return 403;
		}
		buffer << file.rdbuf();
		_body_content = buffer.str();
		file.close();
	} // todo: autoindex
	else
	{
		_code = 404;
		return (404);
	}
	return (200);
}

bool	Response::isFile()
{
	struct stat s;
	if (stat(_path_to_res.c_str(), &s) == 0 )
	{
		if (s.st_mode & S_IFDIR)
			return false;
		else if (s.st_mode & S_IFREG)
			return true;
		else
			return false;
	}
	else
		return false;
}

std::string Response::getMIME()
{
	std::map<std::string, std::string> m;
	std::string ext;
	size_t i;

	i = _path_to_res.size() - 1;
	while (i > 0 && _path_to_res[i] != '.')
		--i;
	if (i == 0)
		return ("text/plain");
	ext = std::string(_path_to_res, i + 1, _path_to_res.size() - i);
	m["aac"] = "audio/aac";
	m["abw"] = "application/x-abiword";
	m["arc"] = "application/octet-stream";
	m["avi"] = "video/x-msvideo";
	m["azw"] = "application/vnd.amazon.ebook";
	m["bin"] = "application/octet-stream";
	m["bmp"] = "image/bmp";
	m["bz"] = "application/x-bzip";
	m["bz2"] = "application/x-bzip2";
	m["csh"] = "application/x-csh";
	m["css"] = "text/css";
	m["csv"] = "text/csv";
	m["doc"] = "application/msword";
	m["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	m["eot"] = "application/vnd.ms-fontobject";
	m["epub"] = "application/epub+zip";
	m["gif"] = "image/gif";
	m["htm"] = "text/html";
	m["html"] = "text/html";
	m["ico"] = "image/x-icon";
	m["ics"] = "text/calendar";
	m["jar"] = "application/java-archive";
	m["jpeg"] = "image/jpeg";
	m["jpg"] = "image/jpeg";
	m["js"] = "application/javascript";
	m["json"] = "application/json";
	m["mid"] = "audio/midi";
	m["midi"] = "audio/midi";
	m["mpeg"] = "video/mpeg";
	m["mpkg"] = "application/vnd.apple.installer+xml";
	m["odp"] = "application/vnd.oasis.opendocument.presentation";
	m["ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	m["odt"] = "application/vnd.oasis.opendocument.text";
	m["oga"] = "audio/ogg";
	m["ogv"] = "video/ogg";
	m["ogx"] = "application/ogg";
	m["otf"] = "font/otf";
	m["png"] = "image/png";
	m["pdf"] = "application/pdf";
	m["ppt"] = "application/vnd.ms-powerpoint";
	m["pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	m["rar"] = "application/x-rar-compressed";
	m["rtf"] = "application/rtf";
	m["sh"] = "application/x-sh";
	m["svg"] = "image/svg+xml";
	m["swf"] = "application/x-shockwave-flash";
	m["tar"] = "application/x-tar";
	m["tif"] = "image/tiff";
	m["tiff"] = "image/tiff";
	m["ts"] = "application/typescript";
	m["ttf"] = "font/ttf";
	m["vsd"] = "application/vnd.visio";
	m["wav"] = "audio/x-wav";
	m["weba"] = "audio/webm";
	m["webm"] = "video/webm";
	m["webp"] = "image/webp";
	m["woff"] = "font/woff";
	m["woff2"] = "font/woff2";
	m["xhtml"] = "application/xhtml+xml";
	m["xls"] = "application/vnd.ms-excel";
	m["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	m["xml"] = "application/xml";
	m["xul"] = "application/vnd.mozilla.xul+xml";
	m["zip"] = "application/zip";
	m["3gp"] = "audio/3gpp";
	m["3g2"] = "audio/3gpp2";
	m["7z"] = "application/x-7z-compressed";
	if (m.count(ext))
		return (m[ext]);
	return ("application/octet-stream");
}

std::string Response::getReasonPhrase()
{
	std::map<std::size_t, std::string> m;

	m[100] = "Continue";
	m[101] = "Switching Protocols";
	m[200] = "OK";
	m[201] = "Created";
	m[202] = "Accepted";
	m[203] = "Non-Authoritative Information";
	m[204] = "No Content";
	m[205] = "Reset Content";
	m[206] = "Partial Content";
	m[300] = "Multiple Choices";
	m[301] = "Moved Permanently";
	m[302] = "Found";
	m[303] = "See Other";
	m[304] = "Not Modified";
	m[305] = "Use Proxy";
	m[307] = "Temporary Redirect";
	m[400] = "Bad Request";
	m[401] = "Unauthorized";
	m[402] = "Payment Required";
	m[403] = "Forbidden";
	m[404] = "Not Found";
	m[405] = "Method Not Allowed";
	m[406] = "Not Acceptable";
	m[407] = "Proxy Authentication Required";
	m[408] = "Request Timeout";
	m[409] = "Conflict";
	m[410] = "Gone";
	m[411] = "Length Required";
	m[412] = "Precondition Failed";
	m[413] = "Payload Too Large";
	m[414] = "URI Too Long";
	m[415] = "Unsupported Media Type";
	m[416] = "Range Not Satisfiable";
	m[417] = "Expectation Failed";
	m[426] = "Upgrade Required";
	m[500] = "Internal Server Error";
	m[501] = "Not Implemented";
	m[502] = "Bad Gateway";
	m[503] = "Service Unavailable";
	m[504] = "Gateway Timeout";
	m[505] = "HTTP Version Not Supported";
	// добавить проверку
	return m[_code];
}

std::string Response::createResponse(std::string body)
{
	_headers["Content-Length"] = SSTR(body.size());
	_headers["Server"] = "MissionImpossible";
	_headers["Date"] = getCurrentDate();
	_response += "HTTP/1.1 ";
	_response += SSTR(_code << " ");
	_response += getReasonPhrase() + "\r\n";
	
	std::map<std::string, std::string>::iterator it;
	it = _headers.begin();
	while (it != _headers.end())
	{
		_response += it->first + ": " + it->second + "\r\n";
		++it;
	}
	_response += "\r\n";
	_response += body;
	return (_response);
}

std::string Response::getCurrentDate(void)
{
	struct timeval now;
	struct timezone tz;

	gettimeofday(&now, &tz);
	return (formatDate(now.tv_sec + tz.tz_minuteswest * 60));
}

std::string Response::formatDate(time_t date)
{
	char buffer[33];
	struct tm *ts;
	size_t last;

	ts   = localtime(&date);
	last = strftime(buffer, 32, "%a, %d %b %Y %T GMT", ts);
	buffer[last] = '\0';
	return (std::string(buffer));
}

std::string Response::getErrorPage()
{
    return ("ERROR PAGE");
}

int main() {
    Response test;
    std::cout << test.getResponse() << std::endl;
}