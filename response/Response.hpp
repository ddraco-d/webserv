/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: efumiko <efumiko@student.21-school.ru>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/14 16:39:39 by efumiko           #+#    #+#             */
/*   Updated: 2021/06/14 23:17:47 by efumiko          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Request.hpp"
#include "../Config.hpp"
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

class Request;
class Config;

class Response
{
private:
    Response(void);

    std::string _response;
    std::string _first_line;
    std::map<std::string, std::string> _headers;
    size_t _code;
	Location _location;
    std::string _method;
    std::string _body_content;
public:
    Response();

    std::string getResponse(const Request &request_conf, const Config &serv_conf);
    std::string createResponse(std::string body);
    std::string getErrorPage();
    std::string getMIME(std::string filename);
    std::string getLastModif(std::string path_to_res);
    int read_file(std::string path_to_res);


    bool isDirectory(std::string path_to_res);
    bool isCGI(std::string path_to_res);
    bool isFile(std::string path_to_res);

    std::string get_method(std::string path_to_res);
    std::string post_method(std::string path_to_res);
    std::string delete_method(std::string path_to_res);


    Location getLocation(const Request &request_conf, const Config &serv_conf);
    ~Response();
};

Response::Response(void)
{}

Response::~Response()
{}

std::string Response::getResponse(const Request &request_conf, const Config &serv_conf)
{
    //_code = request_conf.getCode();
    _code = 200;
    _location = getLocation(request_conf, serv_conf);
    _method = request_conf.getMethod();
    std::string path_to_res;

    // коды ошибок
    if (_code == 413)
        return (createResponse(getErrorPage()));
    if (_code == 405)
    {
        std::vector<std::string> methods = _location.getMethods();
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
    path_to_res = _location.getRoot(); // путь к корню из конфига
	if (path_to_res[path_to_res.size() - 1] == '/') // убираем /
		path_to_res = std::string(path_to_res, 0, path_to_res.size() - 1);
	path_to_res += request_conf.getPath();

    // автоиндекс и путь это директория.
    if (isDirectory(path_to_res)) {}
    if (isCGI(path_to_res)) {}

    if (_method == "GET") {
        return get_method(path_to_res);
    }
    if (_method == "POST") {
        return post_method(path_to_res);
    }
    if (_method == "DELETE") {
        return delete_method(path_to_res);
    }
    return NULL;
}

std::string Response::get_method(std::string path_to_res)
{
    //std::vector<unsigned char> content_bytes;
	
    std::string content_bytes;
//    unsigned char *ressource_content;
//    time_t file_date;

    //content_bytes = read_file(path_to_res);
    int code_read = read_file(path_to_res);
    if (code_read == 403 || code_read == 404)
        return (createResponse(getErrorPage()));
  //  ressource_content = reinterpret_cast<unsigned char *>(&content_bytes[0]);
    _headers["Content-Type"] = getMIME(path_to_res);
    _headers["Last-Modified"] = getLastModif(path_to_res);
    _code = 200;
    createResponse(content_bytes);

}

int Response::read_file(std::string path_to_res)
{
	std::ifstream		file;
	std::stringstream	buffer;

    file.open(path_to_res.c_str(), std::ifstream::in);
    if (isFile(path_to_res))
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

bool	Response::isFile(std::string path_to_res)
{
	struct stat s;
	if (stat(path_to_res.c_str(), &s) == 0 )
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

std::string Response::getMIME(std::string filename)
{
	std::map<std::string, std::string> m;
	std::string ext;
	size_t i;

	i = filename.size() - 1;
	while (i > 0 && filename[i] != '.')
		--i;
	if (i == 0)
		return ("text/plain");
	ext = std::string(filename, i + 1, filename.size() - i);
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


std::string Response::createResponse(std::string body)
{
    _headers["Content-Length"] = 100;
}