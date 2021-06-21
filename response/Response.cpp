/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: efumiko <efumiko@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/15 18:42:33 by efumiko           #+#    #+#             */
/*   Updated: 2021/06/21 01:110:00 by efumiko          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
/*
Response::Response(void)
{
	initReasonPhrases();
	initMIME();

	// =====for autoindex=====
	_host = "localhost";
	_port = 8000;
    _code = 200;
	
	// =====GET=====
    // _method = "GET";
    // _path_to_res = "./test/www/index.html";

	// =====autoindex=====
	//_autoindex = true;
    // _res = "www/";
	// _path_to_res = "./test/www/";

	// =====POST create=====
	_method = "POST";
    _path_to_res = "./test/www/new_file";
	_upload_path = "./test/www/tmp/";
	_name_file = "new_file.html";
	_request_content = "Hello world123123";
	_res = "/www/tmp/new_file";

	// =====DELETE=====
	// _method = "DELETE";
	// _path_to_res = "./test/www/tmp/new_file";
	// _res = "/www/tmp/new_file";
}
*/
//Response::Response(const Request &request_conf, const Config &serv_conf)
Response::Response(Request request)
{
	// ====не факт, что эти штуки нужны)) возможно, только как параметры для cgi===
	//_req_conf = request_conf;  
	//_serv_conf = serv_conf; // используется в getErrorPage()

	// ===первая строка Request===
	_code = request.status_code;
	_res = request.req_path;
	_method = request.method;
	
	// ===итоговый путь к папке или файлу===
	_path_to_res = request.path;

	// ===имя файла, если есть===
	// _name_file = request.getNameFile();
	
	// ===контент, который отправляем с помощью POST===
	_request_content = request.body;
	//std::cout << "BODY: " << _request_content << "|\n";
	// ===путь к папке, в которой будем сохранять файлы для POST===
	if (getTypeFile(_path_to_res) == DRCT)
		_upload_path = _path_to_res + "/POST.txt";
	else
		_upload_path = "";
	// параметры сервера
	_host = request.host_string;
	_port = request.port;
	_autoindex = request.autoindex;

	_is_cgi = request.is_cgi;
	_cgi_arg = request.cgi_arg;
	if ((_methods = request.getAllowMethods()) == "")
		_methods = "GET";
	initReasonPhrases();
	initMIME();
}


std::string Response::getResponse()
{
	if ((_code == 413) || (_code == 404) || (_code == 505))
		return (createResponse(getErrorPage()));
	if (_code == 405)
	{
		char **methods_c = ft_split(_methods.c_str(), ' ');
		std::vector<std::string> methods;
		std::string methods_str;
		int i = 0;
		while (methods_c[i] != NULL)
		{
			methods.push_back(std::string(methods_c[i]));
			++i;
		}
		ft_free_array(&methods_c);
		for (size_t i = 0; i < methods.size(); ++i)
		{
			methods_str += methods[i];
			if (i < methods.size() - 1)
				methods_str += ", ";
		}
		_headers["Allow"] = methods_str;
		return (createResponse(getErrorPage()));
	}

	// автоиндекс и путь это директория.
	if ((getTypeFile(_path_to_res) == DRCT) && _autoindex && _method == "GET") {
        return (createResponse(getListing()));
    }
	if (_is_cgi)
		return run_cgi();

	if (_method == "GET")
		return get_method();
	if (_method == "POST")
		return post_method();
	if (_method == "DELETE")
		return delete_method();
	return NULL;
}

std::string Response::run_cgi()
{
	int		pid;
	pid = fork();
	if (pid == 0)
	{
		char *argv[2];
		char *envp[4];
		argv[0] = (char *)_path_to_res.c_str();
		argv[1] = (char *)NULL;
		envp[0] = (char *)("REQUEST_METHOD="+_method).c_str();
		envp[1] = (char *)("SERVER_PROTOCOL=HTTP/1.1");
		envp[2] = (char *)("PATH_INFO="+_path_to_res).c_str();
		envp[3] = (char *)NULL;
		if (!freopen("./cgi_in.html", "w", stdout))
			std::cout << "NO OPEN OUT\n";
		std::cout << _cgi_arg;
		if (!freopen("./cgi_in.html", "r", stdin))
			std::cout << "NO OPEN IN\n";
		if (!freopen("./cgi_out.html", "w", stdout))
			std::cout << "NO OPEN OUT\n";
		if (execve(_path_to_res.c_str(), argv, envp) == -1)
		{
			std::cout << "CGI ERROR\n";
		}
		exit(0);
	}
	wait(&pid);
	std::stringstream answer;
	std::ifstream out("./cgi_out.html");
	if (out.is_open())
	{
		answer << "<body><h3><span style=\"color: #5ba865; font-size: 1em\">" << out.rdbuf() << "</span></h3></body>";
		out.close();
		remove("./cgi_out.html");
		remove("./cgi_in.html");
	}
	return (createResponse(answer.str()));
}

std::string Response::getListing()
{
	std::string base;
	std::string listing;
	std::string link_base;
	struct dirent *en;
	DIR *dr;

	base = read_file("./html/www/listing.html");
	base = replace(base, "$1", _res);
	if ((dr = opendir(_path_to_res.c_str())) == NULL)
		return ("ERROR OPENDIR");
    while ((en = readdir(dr)) != 0)
		listing += "<li><a href=\""  + _res + "/" + std::string(en->d_name) +  "\">" + std::string(en->d_name) + "</a></li>\n";
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
	if ((getTypeFile(_path_to_res) == FILE) && (remove(_path_to_res.c_str()) == 0))
	{
        _code = 204;
		return (createResponse(""));
	}
    _code = 404;
	return (createResponse(getErrorPage()));
}

std::string Response::get_method()
{
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
	stat(_path_to_res.c_str(), &buffer);
    last_date = buffer.st_mtime + tz.tz_minuteswest * 60;
    return (formatDate(last_date));
}

std::string Response::post_method()
{
	int fd = -1;
	std::string path;
	std::string name_header;

	if (_upload_path != "")
		path = _upload_path;
	else
		path = _path_to_res;
	
	int typeFile;
	typeFile = getTypeFile(path);
	if (typeFile != DRCT)
	{
		if ((fd = open(path.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0644)) == -1)
		{
			_code = 500;
			return (createResponse(getErrorPage()));
		}
		write(fd, _request_content.c_str(), _request_content.length());
		close(fd);
		_code = (typeFile == FILE ? 200 : 201); 
		name_header = (typeFile == FILE ? "Content-Location" : "Location");
		_headers[name_header] = _res;
		return (typeFile == FILE ? createResponse(_request_content) : createResponse(""));
	}
	_code = 500;
	return (createResponse((getErrorPage())));
}

std::string Response::read_file(std::string filepath)
{
	std::stringstream	buf;
	std::string			res;

	if (getTypeFile(filepath) == FILE)
	{
		std::ifstream f(filepath, std::ifstream::binary);
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

int	Response::getTypeFile(std::string filepath)
{
	struct stat s;
	if (stat(filepath.c_str(), &s) == 0 )
	{
		if (s.st_mode & S_IFREG)
			return FILE;
		else
			return DRCT;
	}
	else
		return NON_EXIST;
}

void Response::initMIME()
{
	_mime["aac"] = "audio/aac";
	_mime["abw"] = "application/x-abiword";
	_mime["arc"] = "application/octet-stream";
	_mime["avi"] = "video/x-msvideo";
	_mime["azw"] = "application/vnd.amazon.ebook";
	_mime["bin"] = "application/octet-stream";
	_mime["bmp"] = "image/bmp";
	_mime["bz"] = "application/x-bzip";
	_mime["bz2"] = "application/x-bzip2";
	_mime["csh"] = "application/x-csh";
	_mime["css"] = "text/css";
	_mime["csv"] = "text/csv";
	_mime["doc"] = "application/msword";
	_mime["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	_mime["eot"] = "application/vnd.ms-fontobject";
	_mime["epub"] = "application/epub+zip";
	_mime["gif"] = "image/gif";
	_mime["htm"] = "text/html";
	_mime["html"] = "text/html";
	_mime["ico"] = "image/x-icon";
	_mime["ics"] = "text/calendar";
	_mime["jar"] = "application/java-archive";
	_mime["jpeg"] = "image/jpeg";
	_mime["jpg"] = "image/jpeg";
	_mime["js"] = "application/javascript";
	_mime["json"] = "application/json";
	_mime["mid"] = "audio/midi";
	_mime["midi"] = "audio/midi";
	_mime["mpeg"] = "video/mpeg";
	_mime["mpkg"] = "application/vnd.apple.installer+xml";
	_mime["odp"] = "application/vnd.oasis.opendocument.presentation";
	_mime["ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	_mime["odt"] = "application/vnd.oasis.opendocument.text";
	_mime["oga"] = "audio/ogg";
	_mime["ogv"] = "video/ogg";
	_mime["ogx"] = "application/ogg";
	_mime["otf"] = "font/otf";
	_mime["png"] = "image/png";
	_mime["pdf"] = "application/pdf";
	_mime["ppt"] = "application/vnd.ms-powerpoint";
	_mime["pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	_mime["rar"] = "application/x-rar-compressed";
	_mime["rtf"] = "application/rtf";
	_mime["sh"] = "application/x-sh";
	_mime["svg"] = "image/svg+xml";
	_mime["woff"] = "font/woff";
	_mime["woff2"] = "font/woff2";
	_mime["xhtml"] = "application/xhtml+xml";
	_mime["xls"] = "application/vnd.ms-excel";
	_mime["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	_mime["xml"] = "application/xml";
	_mime["xul"] = "application/vnd.mozilla.xul+xml";
	_mime["zip"] = "application/zip";
	_mime["3gp"] = "audio/3gpp";
	_mime["3g2"] = "audio/3gpp2";
	_mime["7z"] = "application/x-7z-compressed";
	
	_mime["txt"] = "text/plain";
	_mime["cpp"] = "text/plain";
	_mime["hpp"] = "text/plain";
}

std::string Response::getMIME()
{
	std::string ext;
	size_t i;

	i = _path_to_res.size() - 1;
	while (i > 0 && _path_to_res[i] != '.')
		--i;
	ext = std::string(_path_to_res, i + 1, _path_to_res.size() - i);
	if (_mime.count(ext))
		return (_mime[ext]);
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
	std::ifstream cookie("cookie.txt");
	if (cookie.is_open())
	{
		std::stringstream ss;
		ss << cookie.rdbuf();
		if (ss.str().length() > 0)
			_response += ss.str();
		cookie.close();
		remove("cookie.txt");
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
	std::string errorPage;

	// если в конфиге указаны error_page
	// error_pages - это map<int, string> определенному коду ошибки соответствует своя error_page страница
	//if (_serv_conf.error_pages.count(_code) > 0)
	//	return (read_file(_serv_conf.error_pages[_code]));
	errorPage = read_file("./html/www/error.html");
	errorPage = replace(errorPage, "$1", SSTR(_code));
	errorPage = replace(errorPage, "$2", getReasonPhrase());
	return (errorPage);
}
