/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: efumiko <efumiko@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/15 18:42:33 by efumiko           #+#    #+#             */
/*   Updated: 2021/06/16 20:21:34 by efumiko          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(void)
{
	_host = "localhost";
	_port = 8000;

    _res = "www/";
    _code = 200;
    _method = "GET";
    //_path_to_res = "./test/www/index.html";
    _path_to_res = "./test/www/";
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
	
	_path_to_res = "test/www/index.html";
	//_path_to_res = "./test/www/index.html";
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

	// автоиндекс и путь это директория.
	if (isFile(_path_to_res) == false) {
        return (createResponse(getListing()));
    }
	//if (isCGI()) {}

	if (_method == "GET") {
		return get_method();
	}
	if (_method == "POST") {
		return post_method();
	}
	if (_method == "DELETE") {
		return delete_method();
	}
	return NULL;
}

std::string Response::getListing()
{
	std::string base;
	std::string listing;
	std::string link_base;
	struct dirent *en;
	DIR *dr;

	base = read_file("./test/www/listing.html");
	base = replace(base, "$1", _res);
	if ((dr = opendir(_path_to_res.c_str())) == NULL)
		return ("ERROR OPENDIR");
    while ((en = readdir(dr)) != 0)
		listing += "<li><a href=\"" + _host + ":" + SSTR(_port) + "/" + _res + std::string(en->d_name) +  "\">" + std::string(en->d_name) + "</a></li>\n";
	closedir(dr);
	base = replace(base, "$2", listing);
	return (base);
}

std::string Response::replace(std::string source, std::string to_replace, std::string new_value) 
{
	size_t start_pos = 0;
	while((start_pos = source.find(to_replace, start_pos)) != std::string::npos) {
		source.replace(start_pos, to_replace.length(), new_value);
		start_pos += new_value.length();
	}
	return (source);
}

std::string Response::delete_method()
{
    int type;

	if (isFile(_path_to_res) && (remove(_path_to_res.c_str()) == 0))
	{
        _code = 204;
		return (createResponse(""));
	}
    _code = 404;
	return (createResponse(getErrorPage()));
}

std::string Response::get_method()
{
    time_t file_date;
	_body_content = read_file(_path_to_res);
	if (_code == 403 || _code == 404)
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

std::string Response::post_method()
{
	int fd = -1;
	int rtn = 0;
	int type;
	std::string path;

	if (_upload_path.size() > 0)
	{
		path = _upload_path;
	}
	return "";
}
	// else
	// 	path = ressource_path;
	// DEBUG("POST path: " + path);
	// type = pathType(path, NULL);
	// try
	// {
	// 	if (type == 1)
	// 	{
	// 		if ((fd = open(path.c_str(), O_WRONLY | O_TRUNC, 0644)) == -1)
	// 			throw(throwMessageErrno("TO CHANGE"));
	// 		write(fd, _header_block.getContent().c_str(), _header_block.getContent().length());
	// 		close(fd);
	// 		rtn = 200;
	// 		headers["Content-Location"] = _header_block.getRequestLine()._request_target;
	// 	}
	// 	else if (type == 0)
	// 	{
	// 		if ((fd = open(path.c_str(), O_WRONLY | O_APPEND | O_CREAT, 0644)) == -1)
	// 			return (_generateResponse(500, headers, _getErrorHTMLPage(500)));
	// 		write(fd, _header_block.getContent().c_str(), _header_block.getContent().length());
	// 		close(fd);
	// 		rtn = 201;
	// 		headers["Location"] = _header_block.getRequestLine()._request_target;
	// 	}
	// 	else
	// 		return (_generateResponse(500, headers, _getErrorHTMLPage(500)));
	// }
	// catch (std::exception & ex)
	// {
	// 	throwError(ex);
	// }
	// return (_generateResponse(rtn, headers, ""));
//}

// todo: переписать функцию
std::string Response::read_file(std::string filepath)
{
	std::ifstream		f;
	std::stringstream	buf;
	std::string			res;

	if (isFile(filepath))
	{
		f.open(filepath.c_str(), std::ifstream::in);
		if (f.is_open() == false)
		{
			_code = 403;
			return ("");
		}
		buf << f.rdbuf();
		res = buf.str();
		f.close();
		return (res);
	}
	_code = 404;
	return ("");
}

bool	Response::isFile(std::string filepath)
{
	struct stat s;
	if (stat(filepath.c_str(), &s) == 0 )
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

void Response::initReasonPhrases() 
{
	_reason_phrases[200] = "OK";
	_reason_phrases[201] = "Created";
	_reason_phrases[202] = "Accepted";
	_reason_phrases[204] = "No Content";
	_reason_phrases[300] = "Multiple Choices";
	_reason_phrases[301] = "Moved Permanently";
	_reason_phrases[302] = "Found";
	_reason_phrases[303] = "See Other";
	_reason_phrases[307] = "Temporary Redirect";
	_reason_phrases[400] = "Bad Request";
	_reason_phrases[401] = "Unauthorized";
	_reason_phrases[403] = "Forbidden";
	_reason_phrases[404] = "Not Found";
	_reason_phrases[405] = "Method Not Allowed";
	//_reason_phrases[408] = "Request Timeout";
	//_reason_phrases[409] = "Conflict";
	//_reason_phrases[410] = "Gone";
	//_reason_phrases[411] = "Length Required";
	//_reason_phrases[412] = "Precondition Failed";
	_reason_phrases[413] = "Payload Too Large";
	_reason_phrases[414] = "URI Too Long";
	//_reason_phrases[415] = "Unsupported Media Type";
	//_reason_phrases[416] = "Range Not Satisfiable";
	//_reason_phrases[417] = "Expectation Failed";
	//_reason_phrases[426] = "Upgrade Required";
	_reason_phrases[500] = "Internal Server Error";
	_reason_phrases[501] = "Not Implemented";
	_reason_phrases[502] = "Bad Gateway";
	_reason_phrases[503] = "Service Unavailable";
	_reason_phrases[504] = "Gateway Timeout";
	_reason_phrases[505] = "HTTP Version Not Supported";
}

std::string Response::getReasonPhrase()
{
	return _reason_phrases[_code];
}

std::string Response::createResponse(std::string body)
{
	_response += "HTTP/1.1 ";
	_response += SSTR(_code << " ");
	_response += getReasonPhrase() + "\r\n";
	
	_headers["Content-Length"] = SSTR(body.size());
	_headers["Server"] = "MissionImpossible";
	_headers["Date"] = getCurrentDate();
	
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