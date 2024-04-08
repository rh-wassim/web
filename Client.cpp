
#include "Client.hpp"


size_t Client::getcontentLength()
{
    return contentLength;
}

std::string Client::getExtensionByContentType(const std::string& contentType) 
{
    std::map<std::string, std::string>  contentTypeToExtension;

   contentTypeToExtension["text/html"] = ".html";
    contentTypeToExtension["text/css"] = ".css";
    contentTypeToExtension["application/javascript"] = ".js";
    contentTypeToExtension["image/jpeg"] = ".jpeg";
    contentTypeToExtension["image/png"] = ".png";
    contentTypeToExtension["image/gif"] = ".gif";
    contentTypeToExtension["video/mp4"] = ".mp4";
    contentTypeToExtension["application/pdf"] = ".pdf";
    contentTypeToExtension["text/plain"] = ".txt";
    contentTypeToExtension["application/xml"] = ".xml";
    contentTypeToExtension["application/json"] = ".json";
    contentTypeToExtension["text/csv"] = ".csv";
    contentTypeToExtension["application/msword"] = ".doc";
    contentTypeToExtension["application/vnd.openxmlformats-officedocument.wordprocessingml.document"] = ".docx";
    contentTypeToExtension["application/vnd.ms-excel"] = ".xls";
    contentTypeToExtension["application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"] = ".xlsx";
    contentTypeToExtension["application/vnd.ms-powerpoint"] = ".ppt";
    contentTypeToExtension["application/vnd.openxmlformats-officedocument.presentationml.presentation"] = ".pptx";
    contentTypeToExtension["application/zip"] = ".zip";
    contentTypeToExtension["application/x-tar"] = ".tar";
    contentTypeToExtension["application/gzip"] = ".gz";
    contentTypeToExtension["audio/mpeg"] = ".mp3";
    contentTypeToExtension["audio/ogg"] = ".ogg";
    contentTypeToExtension["font/woff"] = ".woff";
    contentTypeToExtension["font/woff2"] = ".woff2";
    contentTypeToExtension["text/x-c"] = ".cpp";
    contentTypeToExtension["text/x-java-source"] = ".java";
    contentTypeToExtension["application/x-yaml"] = ".yml";
    contentTypeToExtension["application/sql"] = ".sql";
    contentTypeToExtension["text/calendar"] = ".ics";
    contentTypeToExtension["text/php"] = ".php";
    contentTypeToExtension["image/x-icon"] = ".ico";
    contentTypeToExtension["image/svg+xml"] = ".svg";
    contentTypeToExtension["video/x-msvideo"] = ".avi";
    contentTypeToExtension["video/quicktime"] = ".mov";
    contentTypeToExtension["image/bmp"] = ".bmp";
    contentTypeToExtension["image/webp"] = ".webp";
    contentTypeToExtension["image/tiff"] = ".tiff";
    contentTypeToExtension["application/vnd.ms-excel.sheet.macroEnabled.12"] = ".ejs";
    contentTypeToExtension["video/mpeg"] = ".mpg";
    contentTypeToExtension["video/webm"] = ".webm";
    contentTypeToExtension["text/x-java-source"] = ".java";
    contentTypeToExtension["text/x-c++src"] = ".cpp";
    contentTypeToExtension["text/php"] = ".php";
    contentTypeToExtension["application/x-yaml"] = ".yml";
    contentTypeToExtension["application/sql"] = ".sql";
    contentTypeToExtension["text/calendar"] = ".ics";
    contentTypeToExtension["application/bat"] = ".bat";
    contentTypeToExtension["application/octet-stream"] = ".py";
    contentTypeToExtension["application/x-msdownload"] = ".dll";
    contentTypeToExtension["application/java-vm"] = ".class";
    contentTypeToExtension["image/vnd.adobe.photoshop"] = ".psd";
    contentTypeToExtension["audio/wav"] = ".wav";
    contentTypeToExtension["audio/flac"] = ".flac";
    contentTypeToExtension["application/x-httpd-php"] = ".php";
    contentTypeToExtension["image/jpeg"] = ".jpg";
        
    std::string lowercaseContentType = toLowerCase(contentType);

    std::map<std::string, std::string>::const_iterator it = contentTypeToExtension.find(lowercaseContentType);
    if (it != contentTypeToExtension.end()) {
        return it->second;
    } else {
        return "";
    }
}

Client::Client(int Socket, confi &cf):clientSocket(Socket), isGetFullReq(0), isSentFullRes(0), cfg(cf),isSetRequest(0),isFileCreated(0),isGetFullBody(0),bytes(0),isChunked(0)
{
    tmpFileName = "";
}

int    isReachedBody(char *buffer)
{
    int i = 0;

    while (buffer && buffer[i])
    {
        if (buffer[i] == '\r' && buffer[i + 1] == '\n' && buffer[i + 2] == '\r' && buffer[i + 3] == '\n')
            return (i + 4);
        ++i;
    }
    return 0;
}

void    Client::getRequest()
{ 
    if (!isGetFullReq)
    {
        int bodyBegin;        
        memset(buffer,0, 1024);
        bytes = recv(clientSocket, buffer, 1023, 0);
        if (bytes == -1) 
        {
            setResponse();
            isSentFullRes = 1;
            isGetFullReq = 1;
            return;
        }
        else if (bytes > 0)
        {
            buffer[bytes] = '\0';
            requestBuffer += buffer;
        }
        bodyBegin = isReachedBody(buffer);
        if (bytes != 1023 || bodyBegin)
        {
            memset(bodyBuffer,0, sizeof(bodyBuffer));
            isGetFullReq = 1;
            int i = 0;
            while (bodyBegin < bytes)
            {
                bodyBuffer[i] = buffer[bodyBegin];
                ++bodyBegin;
                ++i;
            }
            bytes = i;
            bodyBuffer[i] = 0;
            
        }
        else
        if (bytes == 0 && bodyBegin)
        {
            memset(bodyBuffer,0, sizeof(bodyBuffer));
            isGetFullReq = 1;
            int i = 0;
            while (bodyBegin < bytes)
            {
                bodyBuffer[i] = buffer[bodyBegin];
                ++bodyBegin;
                ++i;
            }
            bytes = i;
            bodyBuffer[i] = 0;
            
        }
    }
    else if (!isGetFullBody)
    {
        memset(bodyBuffer,0, sizeof(bodyBuffer));
        bytes = recv(clientSocket, bodyBuffer, 1023, 0);
        if (bytes == -1) 
        {
            isSentFullRes = 1;
            isGetFullReq = 1;
            if (isFileCreated && inputfile.is_open())
                inputfile.close();

        }
        else if (bytes >= 0)
        {
            bodyBuffer[bytes] = '\0';
        }
    }
}

bool    Client::isRequestFinished()
{
    return isGetFullReq;
}

bool    Client::isResponseFinished()
{
    return isSentFullRes;
}
int    Client::getClientSocket()const{
    return clientSocket;
}

void    Client::closeClientSocket()
{
    close(clientSocket);
}

void    Client::setResponse()
{
    isSentFullRes = 1;
}

void    Client::setParsedRequest()
{
    HttpRequest req(requestBuffer.c_str());
    request = req;
    isSetRequest = 1;
    contentLength = 0;
    std::map<std::string, std::string>::const_iterator it = request.getHeaders().find("Content-Length");
    if (it != request.getHeaders().end() && (it->second).size())
    {
        contentLength = atoi((it->second).c_str());
    }//*****************************************************************
    if (request.getHttpVersion() != "HTTP/1.1")
    {
           setResponse();
           return;
    }
}

HttpRequest&    Client::getParsedRequest()
{
    return request;
}

Client::~Client(){}

void Client::isRequestWellFormed()
{ 
    int bytsend ;
    if (request.getResource().size() > 2048)
    {
        if(!isResponseFinished() &&!handleErrorResponse("414"," URI Too Long"))
        setresponse("HTTP/1.1 414 URI Too Long", "text/html",
        "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n"
        "<html><head>\r\n<title>414 URI Too Long</title>\r\n</head><body>\r\n"
        "<h1>URI Too Long</h1>\r\n<p>The requested URL is too long and cannot be processed by this server.</p>\r\n"
        "</body></html>\r\n");
        bytsend= send(getClientSocket(), getresponse(), strlen(getresponse()), 0);
        if(bytsend == 0 || bytsend == -1)
        {
            setResponse();
            return;
        }
        setResponse();
        return;
    }
    std::map<std::string, std::string>::const_iterator transferEncodingIt = request.getHeaders().find("Transfer-Encoding");
        if ( transferEncodingIt != request.getHeaders().end() && transferEncodingIt->second != "chunked")
    {
        if(!isResponseFinished() && !handleErrorResponse("501"," Not Implemented"))
        {
                setresponse("HTTP/1.1 501 Not Implemented", "text/html",
        "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n"
        "<html><head>\r\n<title>501 Not Implemented</title>\r\n</head><body>\r\n"
        "<h1>Not Implemented</h1>\r\n<p>The requested functionality is not implemented on this server.</p>\r\n"
        "</body></html>\r\n");
        bytsend= send(getClientSocket(), getresponse(), strlen(getresponse()), 0);
            if(bytsend == 0 || bytsend == -1)
        {
            setResponse();
            return ;
        }
            setResponse();
                return;
        }       
    }
    else if (request.getMethod() == "POST")
    {
        std::map<std::string, std::string>::const_iterator ContentLength = request.getHeaders().find("Content-Length");
        if (transferEncodingIt == request.getHeaders().end() && ContentLength == request.getHeaders().end())
        {
                if (!isResponseFinished() &&!handleErrorResponse("400"," Bad Request"))
            {
            setresponse("HTTP/1.1 400 Bad Request", "text/html",
                "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n"
            "<html><head>\r\n<title>400 Bad Request</title>\r\n</head><body>\r\n"
            "<h1>400 Bad Request</h1>\r\n<p>The request could not be understood or is missing required information.</p>\r\n"
            "</body></html>\r\n");
            bytsend =send(getClientSocket(), getresponse(), strlen(getresponse()), 0);
            if(bytsend == 0 || bytsend == -1)
                {
            setResponse();
            return;
            }
            setResponse();
                return;
            } 
        }
        
    }
    else if (isInvalidURI(request.getResource()))
    {
            
        if (!isResponseFinished() &&!handleErrorResponse("400"," Bad Request"))
        {
            setresponse("HTTP/1.1 400 Bad Request", "text/html",
                "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n"
            "<html><head>\r\n<title>400 Bad Request</title>\r\n</head><body>\r\n"
            "<h1>400 Bad Request</h1>\r\n<p>The request could not be understood or is missing required information.</p>\r\n"
            "</body></html>\r\n");
            bytsend= send(getClientSocket(), getresponse(), strlen(getresponse()), 0);
            if(bytsend == 0 || bytsend == -1)
            {
                setResponse();
                return;
            }
            setResponse();
            return;
    }
    }
}

bool Client::isInvalidURI(const std::string& uri) {
   
    for (std::size_t i = 0; i < uri.length(); ++i) {
        if (std::iscntrl(uri[i])) {
            return true;
        }
    }

    // Check for unsafe characters
    const char* unsafeChars = "<>#%{}|\\^~[]`";
    if (strpbrk(uri.c_str(), unsafeChars) != NULL) {
        return true;
    }
    const char* invalidPathChars = ":*?\"<>|";
    if (strpbrk(uri.c_str(), invalidPathChars) != NULL) {
        return true;
    }  
    return false;
}

void Client::handelread()
{
    getRequest();
    if (!isRequestFinished() || isResponseFinished()) 
            return;
}


void    Client::handleClient()
{

        if (!isSetRequest && isRequestFinished() && !isResponseFinished())
        {
            setParsedRequest(); 
            isSetRequest = 1;
            setClientResponce(); 
         }
        if (isResponseFinished())
            return;
        if (!isRequestFinished() && request.getMethod() != "POST")
            return;
        if(request.getMethod() == "POST")
            methodpost();
        else if(request.getMethod() == "DELETE" &&  isRequestFinished() == true)
        {
                Delete_method();
        }
        else if(request.getMethod() == "GET" && isRequestFinished() == true)
            methodGet();
       
}


void Client::setresponse(const std::string& status,const std::string& contentType,const std::string& content) 
 {
    response = status + "\r\n"
                   "Content-Type: " + contentType + "\r\n"
                   "Content-Length: " +std::to_string(content.size())+ "\r\n"
                   "\r\n" + content;
}

const char *Client::getresponse()
{
    return response.c_str();
}

void    Client::setClientResponce()
{
    isChunked = false;
    std::map<std::string, std::string>::const_iterator it = request.getHeaders().find("Transfer-Encoding");
    if (it != request.getHeaders().end() && it->second == "chunked") 
        isChunked = true;
    isSentHreders = 0;
    methodAllowed = false;
    bestmatch = "none";
    bestMatchLength = 0;
    bestMatchLocation = NULL;
    matchinglocation();
    isParseChunkedFiniched = 0;
    isInitChunkedPara = 0;
}
const location*  Client::getbestMatchLocation()
{
    return  bestMatchLocation;
}

std::string Client::getFileExtension(const std::string& filePath) 
{
    size_t dotPosition = filePath.find_last_of('.');
    if (dotPosition != std::string::npos && dotPosition < filePath.length() - 1) {
        return "." + filePath.substr(dotPosition + 1);
    }
    return ".txt";
}

std::string Client::concatenatePaths(const std::string& path1, const std::string& path2) 
{
    std::string tmp = "." + request.getResource();
    if (tmp == filePath && tmp.size() == filePath.size() )
    {
        return path2;
    }
    else
    {
         if (path1.empty() || path1[path1.size() - 1] == '/')
        return path1 + path2;
    else
        return path1 + '/' + path2;
    }
       
}
std::string Client::toLowerCase(const std::string& str) 
{
    std::string result = str;
    for (size_t i = 0; i < result.size(); ++i) {
        result[i] = tolower(result[i]);
    }
    return result;
}

std::string Client::getContentTypeByExtension(const std::string& extension) 
{
    std::map<std::string, std::string> extensionToContentType;
    extensionToContentType[".html"] = "text/html";
        extensionToContentType[".css"] = "text/css";
        extensionToContentType[".js"] = "application/javascript";
        extensionToContentType[".jpeg"] = "image/jpeg";
        extensionToContentType[".png"] = "image/png";
        extensionToContentType[".gif"] = "image/gif";
        extensionToContentType[".mp4"] = "video/mp4";
        extensionToContentType[".pdf"] = "application/pdf";
        extensionToContentType[".txt"] = "text/plain";
        extensionToContentType[".xml"] = "application/xml";
        extensionToContentType[".json"] = "application/json";
        extensionToContentType[".csv"] = "text/csv";
        extensionToContentType[".doc"] = "application/msword";
        extensionToContentType[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
        extensionToContentType[".xls"] = "application/vnd.ms-excel";
        extensionToContentType[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
        extensionToContentType[".ppt"] = "application/vnd.ms-powerpoint";
        extensionToContentType[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
        extensionToContentType[".zip"] = "application/zip";
        extensionToContentType[".tar"] = "application/x-tar";
        extensionToContentType[".gz"] = "application/gzip";
        extensionToContentType[".mp3"] = "audio/mpeg";
        extensionToContentType[".ogg"] = "audio/ogg";
        extensionToContentType[".woff"] = "font/woff";
        extensionToContentType[".woff2"] = "font/woff2";
        extensionToContentType[".ico"] = "image/x-icon";
        extensionToContentType[".svg"] = "image/svg+xml";
        extensionToContentType[".avi"] = "video/x-msvideo";
        extensionToContentType[".mov"] = "video/quicktime";
        extensionToContentType[".bmp"] = "image/bmp";
        extensionToContentType[".webp"] = "image/webp";
        extensionToContentType[".tiff"] = "image/tiff";
        extensionToContentType[".ejs"] = "application/vnd.ms-excel.sheet.macroEnabled.12";
        extensionToContentType[".mpg"] = "video/mpeg";
        extensionToContentType[".webm"] = "video/webm";
        extensionToContentType[".ics"] = "text/calendar";
        extensionToContentType[".java"] = "text/x-java-source";
        extensionToContentType[".cpp"] = "text/x-c++src";
        extensionToContentType[".php"] = "text/php";
        extensionToContentType[".yml"] = "application/x-yaml";
        extensionToContentType[".sql"] = "application/sql";
        extensionToContentType[".log"] = "text/plain";
        extensionToContentType[".bat"] = "application/bat";
        extensionToContentType[".exe"] = "application/octet-stream";
        extensionToContentType[".dll"] = "application/x-msdownload";
        extensionToContentType[".class"] = "application/java-vm";
        extensionToContentType[".psd"] = "image/vnd.adobe.photoshop";
        extensionToContentType[".wav"] = "audio/wav";
        extensionToContentType[".flac"] = "audio/flac";
        extensionToContentType[".jpg"] = "image/jpeg";

    std::string lowercaseExtension = toLowerCase(extension);

    std::map<std::string, std::string>::const_iterator it = extensionToContentType.find(lowercaseExtension);
    if (it != extensionToContentType.end()) {
        return it->second;
    } else {
        return "text/plain";
    }
}

void Client::split(const std::string& input, char delimiter, std::string& part1, std::string& part2) 
{
    std::size_t pos = input.find(delimiter);
    if (pos != std::string::npos) {
        part1 = input.substr(0, pos);
        part2 = input.substr(pos + 1);
    } 
    else 
    {
        part1 = input;
        part2 = "";
    }
}

std::string &Client::getbestmatch()
{
    return bestmatch;
}

std::string Client::getfilePath()
{
    size_t pos = request.getResource().find(getbestmatch());
    if(pos !=std::string::npos)
    {
        return filePath = getbestMatchLocation()->root + request.getResource().substr(pos + getbestmatch().length());
    }
    return "error";
}

bool Client::sendHredes()
{
    if (isSentHreders)
        return 1;
    fileStream.open(getfilePath(), std::ios::binary);
   
    fileStream.seekg(0, std::ios::end);
    size_t             file_size = fileStream.tellg();
    fileStream.seekg(0, std::ios::beg);
    std::string     fileContent;
    if(fileStream.is_open())
    {
        std::string fileExtension = getFileExtension(filePath);
        std::string contentType =getContentTypeByExtension(fileExtension);
        fileContent = "HTTP/1.1 200 OK";
        fileContent += "\r\n"
        "Content-Type: " + contentType + "\r\n"
        "Content-Length: " + std::to_string(file_size) + "\r\n\r\n";
       int bytsend= send(getClientSocket(), fileContent.c_str(), fileContent.size(), 0);
        if(bytsend == 0 || bytsend == -1)
           {
                setResponse();
           }
        fileStream.close();
        fileDescriptor = open(getfilePath().c_str(), O_RDONLY | O_NONBLOCK);
    }
    else 
    {
        notfound();
    }
    isSentHreders = 1;
    return 0;

}

void Client::methodGetFile()
{
   int bufferSize = 1024;
    char buffer[bufferSize];
    memset(buffer, 0, bufferSize);
    if(sendHredes())
    {  
        int bytes = read(fileDescriptor,buffer, bufferSize - 1);
        if (bytes == -1  || bytes == 0)
        {
            close(fileDescriptor);
            setResponse();
            return;
        }
        buffer[bytes] = '\0';
        ssize_t sentBytes= send(getClientSocket(), buffer, bytes, 0);
        if (sentBytes == 0 || sentBytes == -1)
        {
            close(fileDescriptor);
            setResponse();
            return;
        }
    }                   
}

bool Client::sendHredes2(std::string  fullPath)
{
    
    if (isSentHreders)
        return 1;
    fileStream.open(fullPath, std::ios::binary);
    fileStream.seekg(0, std::ios::end);
    size_t             file_size = fileStream.tellg();
    fileStream.seekg(0, std::ios::beg);
    std::string     fileContent;
    if(fileStream.is_open())
    {
        std::string fileExtension = getFileExtension(fullPath);
        std::string contentType =getContentTypeByExtension(fileExtension);
        fileContent = "HTTP/1.1 200 OK";
        fileContent += "\r\n"
        "Content-Type: " + contentType + "\r\n"
        "Content-Length: " + std::to_string(file_size) + "\r\n\r\n";
        int bytsend = send(getClientSocket(), fileContent.c_str(), fileContent.size(), 0);
        if(bytsend == 0 || bytsend == -1)
           {
                fileStream.close();
                setResponse();
           }
        fileStream.close();
        fileDescriptor = open(fullPath.c_str(), O_RDONLY | O_NONBLOCK);
    }
    else 
    {
        notfound();
    }
    isSentHreders = 1;
    return 0;

}

void Client::handleDirectoryAutoIndex()
{
    int sendbytes;
    if(request.getResource().back() != '/')
    {
        std::string other =request.getResource() + "/";
        setresponse("HTTP/1.1 301 Moved Permanently", "text/html", "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n<html><head><title>301 Moved Permanently</title></head><body><h1>Moved Permanently</h1><p>The document has moved <a href='http://"+ 
        request.getHeaders().find("Host")->second + other + "'>here</a>.</p></body></html>");
        sendbytes= send(getClientSocket(),getresponse(),strlen(getresponse()),0);   
        if(sendbytes == 0 || sendbytes == -1)
        {
            setResponse();
            return;
        }   
        setResponse();
        return;         
    }
    else 
    {
        std::vector<std::string> filelist;
        DIR* directory = opendir(filePath.c_str());
        if (directory != NULL)
        {     
            if (!getbestMatchLocation()->index.empty())
            {  
                std::string fullPath = filePath  + getbestMatchLocation()->index;
                closedir(directory);
                if( access(fullPath.c_str(), F_OK) == -1)
                {
                     if (!isResponseFinished() && handleErrorResponse("403"," Forbidden") == false) 
                    {
                    setresponse("HTTP/1.1 403 Forbidden","text/html","<!DOCTYPE HTML PUBLIC -//IETF//DTD HTML 2.0//EN><html><head><title>403 Forbidden</title></head><body><h1>Forbidden</h1><p>You don t have permission to access /forbidden/index.html on this server.</p></body></html>");
                    sendbytes = send(getClientSocket(),getresponse(),strlen(getresponse()),0);
                    if(sendbytes == 0 || sendbytes == -1)
                    {
                        setResponse();
                        return;
                    }
                    setResponse();
                    } 
                }
                else if ((getFileExtension(fullPath) == ".py" || getFileExtension(fullPath) == ".php")
                    && getbestMatchLocation()->cgi =="on") {
                    handle_cgi(fullPath,getFileExtension(fullPath),*this);
                    setResponse();
                    return ;
                }
                else 
                {    
                    if(sendHredes2(fullPath))
                    {  
                        int bufferSize = 1024;
                        char buffer[bufferSize];
                        memset(buffer, 0, bufferSize);
                        int bytes = read(fileDescriptor,buffer, bufferSize - 1);
                        if (bytes == -1  || bytes == 0)
                        {
                            close(fileDescriptor);
                            setResponse();
                            return;
                        }
                        buffer[bytes] = '\0';
                        sendbytes = send(getClientSocket(), buffer, bytes, 0);
                        if(sendbytes == 0 || sendbytes == -1)
                        {
                            setResponse();
                            return;
                        } 
                    }
                }      
            }
            else
            {
                if(getbestMatchLocation()->auto_index == "on")
                {
                    struct dirent* entry;
                    while ((entry = readdir(directory)) != NULL)
                    {
                        filelist.push_back(entry->d_name);
                    }
                    closedir(directory);
                    std::string htmlContent = "<!DOCTYPE html>\n<html>\n<head><title>Index of " + filePath + "</title></head>\n<body>\n<h1>Index of "+ filePath + "</h1>\n<ul>\n";
                    std::vector <std::string>::const_iterator file;
                    for (file = filelist.begin(); file != filelist.end(); ++file) 
                    {
                        htmlContent += "<li><a href=\"" + concatenatePaths(request.getResource(), *file)  + "\">" + *file +"</a></li>\n";
                    }
                    setresponse("HTTP/1.1 200 OK", "text/html",htmlContent);
                    sendbytes=  send(getClientSocket(), getresponse(), strlen(getresponse()), 0);
                    if(sendbytes == 0 || sendbytes == -1)
                    {
                        setResponse();
                        return;
                    } 
                    setResponse(); 
                }
                else if(getbestMatchLocation()->auto_index == "off")
                {
                    closedir(directory);
                    if (!isResponseFinished() && handleErrorResponse("403"," Forbidden") == false) 
                    {
                    setresponse("HTTP/1.1 403 Forbidden","text/html","<!DOCTYPE HTML PUBLIC -//IETF//DTD HTML 2.0//EN><html><head><title>403 Forbidden</title></head><body><h1>Forbidden</h1><p>You don t have permission to access /forbidden/index.html on this server.</p></body></html>");
                    sendbytes = send(getClientSocket(),getresponse(),strlen(getresponse()),0);
                    if(sendbytes == 0 || sendbytes == -1)
                    {
                        setResponse();
                        return;
                    } 
                    setResponse();
                    }                
                }
            }
        }
        else {
            closedir(directory);
            if (!isResponseFinished() && !handleErrorResponse("500"," Internal Server Error") ) 
            {
            setresponse("HTTP/1.1 500 Internal Server Error", "text/html",
            "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n"
            "<html><head>\r\n<title>500 Internal Server Error</title>\r\n</head><body>\r\n"
            "<h1>Internal Server Error</h1>\r\n<p>The server encountered an internal error while processing the request.</p>\r\n"
            "</body></html>\r\n");    
            bytes =send(getClientSocket(),getresponse(), strlen(getresponse()), 0);
            if(bytes == 0 || bytes == -1)
            {
                setResponse();   
            }
            setResponse();
            }
        }
            
    }
}

void Client::methodGet()
{
    struct stat fileinfo;   
    if (stat(getfilePath().c_str(), &fileinfo) == 0 && access(getfilePath().c_str(), F_OK) != -1) 
    {
        if (S_ISREG(fileinfo.st_mode)) 
        {
            if (( getFileExtension( getfilePath()) == ".py" ||  getFileExtension( getfilePath()) == ".php")
                && getbestMatchLocation()->cgi == "on") {
                handle_cgi(getfilePath(), getFileExtension(getfilePath()), *this);
                this->setResponse();
                return ;
            }
            methodGetFile();
        } 
        else if (S_ISDIR(fileinfo.st_mode)) 
        {
            handleDirectoryAutoIndex();
        }      
    } 
    else
    {
          notfound();
    }
      
}

void Client::notfound()
{
   
    if (!isResponseFinished() && handleErrorResponse("404"," Not Found") == false) 
    {
        setresponse("HTTP/1.1 404 Not Found", "text/html", "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n<html><head>\r\n<title>404 Not Found</title>\r\n</head><body>\r\n<h1>Not Found</h1>\r\n<p>The requested URL was not found on this server.</p>\r\n</body></html>\r\n");
       int bytesend= send(getClientSocket(), getresponse(), strlen(getresponse()), 0);
       if(bytesend == 0 || bytesend == -1)
       {
        
        setResponse();
        return;
       }
       setResponse();
    }
}

bool Client::handleErrorResponse(const std::string& errorCode, std::string title)
{
    std::map<std::string, std::string>& err_pages = err_page;
    if (err_page.find(errorCode) != err_pages.end() ) 
    {
            if( access(err_page.find(errorCode)->second.c_str(), F_OK) == -1)
                return false;
            fileerror.open(err_page.find(errorCode)->second.c_str(), std::ios::binary);
            fileerror.seekg(0, std::ios::end);
            size_t             file_size = fileerror.tellg();
            fileerror.seekg(0, std::ios::beg);
            std::string     fileContent;
            if (fileerror.is_open() ) 
            {       
                if (file_size > 2000) 
                {
                    fileerror.close();
                    return false;
                }
                std::string fileExtension = getFileExtension(err_page.find(errorCode)->second.c_str());
                std::string contentType = getContentTypeByExtension(fileExtension);
                fileContent = "HTTP/1.1 " + errorCode + title;////************* message error 
                fileContent += "\r\n"
                "Content-Type: " + contentType + "\r\n"
                "Content-Length: " + std::to_string(file_size) + "\r\n\r\n";
                fileerror.close();
                f = open(err_page.find(errorCode)->second.c_str(), O_RDONLY | O_NONBLOCK); 
                if( f < 0)
                    return(false);  
            }
            else
                return false ;   
            char errorBuffer[file_size + 1];
            int b;
            b = read(f,errorBuffer, file_size);
            errorBuffer[b] = '\0';
            fileContent += errorBuffer;
            if (b == -1  || b == 0)
            {
                
                close(f);
                setResponse();
                return true;
            }
            ssize_t sentBytes= send(getClientSocket(), fileContent.c_str(),b, 0);
            if (sentBytes < 0 || sentBytes == 0)
            {
                close(f);
                setResponse();
                return false;
            }
            close(f);
            setResponse();
            return true;
       // }
    }
    return false;
}

void Client::methodnotallow()
{
    if(!isResponseFinished() && handleErrorResponse("405"," Method Not Allowed") == false )
    {
        setresponse("HTTP/1.1 405 Method Not Allowed", "text/html", "<html><head><title>405 Method Not Allowed</title></head><body><h1>Method Not Allowed</h1><p>The requested method is not allowed for the requested URL.</p></body></html>");
        int bytes= send(getClientSocket(), getresponse(), strlen(getresponse()), 0);
        if(bytes == 0 || bytes == -1)
        {
            setResponse();
            return;
        }
        setResponse();
    } 
}

void Client::matchinglocation()
{
     int bytesend ;
    int i = 0;
    if (request.getHeaders().find("Host") != request.getHeaders().end())
    {
        std::string servName, port;
        split(request.getHeaders().find("Host")->second, ':', servName, port);
        if(servName =="localhost")
            servName = "127.0.0.1";
        if(port.empty())
            port ="80";
        for (std::deque<server>::iterator iit = cfg.servers.begin(); iit != cfg.servers.end() ; iit++)
        {
            if (!(*iit).server_name.empty() && servName != (*iit).server_name && std::stoi(port) != (*iit).port)
                continue;
            i++;
           // std::cerr << "\nservName ***************** " << servName << std::endl;
            if(!port.empty() && !servName.empty() && !(*iit).server_name.empty() && (*iit).port == std::stoi(port) && (((*iit).host.compare(servName)) == 0 || (*iit).server_name == servName))
            {
                err_page = (*iit).err_pages;
                if(getcontentLength() >(*iit).mx_cl_bd_size)
                {
                    if (!isResponseFinished() && !handleErrorResponse("413"," Request Entity Too Large"))
                    {
                        setresponse("HTTP/1.1 413 Request Entity Too Large", "text/html", "<html><head><title>413 Request Entity Too Large</title></head><body><h1>Request Entity Too Large</h1><p>The server is refusing to process the request because the request entity is too large.</p></body></html>");
                        bytesend = send(getClientSocket(), getresponse(), strlen(getresponse()), 0);
                        if(bytesend == 0 || bytesend == -1)
                        {
                        setResponse();
                        return; 
                        }
                        setResponse();
                        return;
                    }
                }
            
                for (std::deque<location>::iterator it = (*iit).locations.begin(); it != (*iit).locations.end(); ++it) 
                {
                    if(request.getResource().find((*it).path_location) == 0)
                    {    
                        if ((*it).path_location.length() > bestMatchLength) 
                        {
                            bestmatch = (*it).path_location;
                            bestMatchLocation = &(*it);
                            bestMatchLength = (*it).path_location.length();
                        }
                    }
                }
                if(bestMatchLocation && bestMatchLocation->root.empty())
                {
                    bestMatchLocation->root = (*iit).root;
                }  
                isRequestWellFormed();
                if(!isResponseFinished() && (bestmatch == "" || bestmatch.size() == 0))
                {
                    if (request.getMethod() == "POST")
                    {
                        if (isChunked && contentLength >= totalReaded)
                            notfound();
                        else  if (contentLength <= totalReaded)
                            notfound();
                        totalReaded += bytes;
                    }
                    else
                        notfound();
                }
            }
        }
    }
    if(bestMatchLocation)
    {
        if( !bestMatchLocation->redirection.empty() )
        {
            response = "HTTP/1.1 301 Moved Permanently\r\n"
           "Content-Type: text/html\r\n"
           "Content-Length: " + std::to_string(0) + "\r\n"
           "Location: " + bestMatchLocation->redirection + "\r\n"
           "\r\n";
            int b=send(getClientSocket(), response.c_str(), response.size(), 0); 
            if(b == 0 || b == -1)
            {
                setResponse();
                return;
            }
            setResponse();
            return;
        }
        const std::string& requestMethod = request.getMethod();
        for (std::deque<std::string>::iterator methodIt = (*bestMatchLocation).allow_methods.begin(); methodIt != bestMatchLocation->allow_methods.end(); ++methodIt)
        {
            if (*methodIt == requestMethod)
            {
                methodAllowed = true;
                break;
            }
        }
        if (!isResponseFinished() && !methodAllowed)
            methodnotallow();
    }
    if (i == 0)
    {
        setResponse();
    }
}

bool    Client::createFile()
{
    if (isFileCreated)
        return true;
    if (!bytes)
    {
        setResponse();
        return false;
    }
    std::map<std::string, std::string>::const_iterator it = request.getHeaders().find("Transfer-Encoding");
    if (it != request.getHeaders().end() && it->second == "chunked")
    {
        isChunked = true;
        std::stringstream ss;
        ss << getbestMatchLocation()->root+ "/" << getClientSocket();
        tmpFileName = ss.str();
        tmpInputfile.open(tmpFileName, std::ios::binary);
        if(!tmpInputfile.is_open())
        {
            notfound();
            isGetFullBody = 1;
            return false;
        }
        tmpInputfile.write(bodyBuffer, bytes);
        totalReaded = bytes;
        std::string fileExtension = getExtensionByContentType(request.getHeaders().find("Content-Type")->second);
        fileNamePost = getbestMatchLocation()->root+ "/" + generateNewFileName() + fileExtension;
        inputfile.open(fileNamePost, std::ios::binary);
        if(!inputfile.is_open())
        {
            notfound();           
            return false;
        }
        isFileCreated = true;
        return false;
    }
    std::string fileExtension = getExtensionByContentType(request.getHeaders().find("Content-Type")->second);
    fileNamePost = getbestMatchLocation()->root+ "/" + generateNewFileName() + fileExtension;
    inputfile.open(fileNamePost, std::ios::binary);     
    if(!inputfile.is_open())
    {
       notfound();
        return false;
    }
    inputfile.write(bodyBuffer, bytes);
    totalReaded = bytes;
    isFileCreated = true;
    return false;
}

std::string Client::getFileNamePost()
{
    return fileNamePost;
}

bool    isHexadecimal(char c)
{
    char str[] = "0123456789abcdefABCDEF";
    int i = 0;
    while (str[i])
    {
        if (c == str[i])
        {
            return true;
        }
        ++i;
    }
    return false;
}

int hexToDecimal(const std::string& hexString) 
{
    int decimalValue = 0;

    if (hexString.empty()) {
        return 0;
    }
    size_t start = 0;
    if (hexString.size() >= 2 && hexString.substr(0, 2) == "0x") {
        start = 2;
    }
    for (size_t i = start; i < hexString.size(); ++i) {
        decimalValue *= 16;  
        if (hexString[i] >= '0' && hexString[i] <= '9') {
            decimalValue += hexString[i] - '0';
        } else if (hexString[i] >= 'A' && hexString[i] <= 'F') {
            decimalValue += hexString[i] - 'A' + 10;
        } else if (hexString[i] >= 'a' && hexString[i] <= 'f') {
            decimalValue += hexString[i] - 'a' + 10;
        } else {
            return 0;
        }
    }
    return decimalValue;
}

void Client::parseChunked()
{
    if (!isInitChunkedPara)
    {
        memset(buffer,0, sizeof(buffer));
        tmpInputfile.close();
        chunkSize = 0;
        isFirstLine = 0;
        total = 0;
        memset(buffer,0, sizeof(buffer));
        tmpInputfile2.open(tmpFileName, std::ios::binary);
        if(!tmpInputfile2.is_open())
        {
            notfound();
            isGetFullBody = 1;
            isInitChunkedPara = true;
            return;
        }
        isInitChunkedPara = true;
    }
    if (!chunkSize && !isFirstLine)
    {
        std::string chunkSizeLine;
        tmpInputfile2.read(buffer, 1);
        while (isHexadecimal(buffer[0]))
        {
            chunkSizeLine += buffer[0];
            tmpInputfile2.read(buffer, 1);
        }
        if (buffer[0] == '\r')
            tmpInputfile2.read(buffer, 1);
        chunkSize = hexToDecimal(chunkSizeLine);
        isFirstLine = 1;
        memset(buffer,0, sizeof(buffer));
        if (chunkSize == 0)
        {
        if (!isResponseFinished())
        {
            remove(tmpFileName.c_str());
            remove(fileNamePost.c_str());
            setresponse("HTTP/1.1 204 No Content", "text/html",
            "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n"
            "<html><head>\r\n<title>204 No Content</title>\r\n</head><body>\r\n"
            "<h1>No Content</h1>\r\n<p>The request was successful, but there is no content to send in the response.</p>\r\n"
            "</body></html>\r\n");
            int sendReturn = send(getClientSocket(), getresponse(), strlen(getresponse()), 0);
            if(sendReturn != 0 || sendReturn != -1)
                setResponse();
        }
        }
    }
    else if (!chunkSize && isFirstLine)
    {
        std::string chunkSizeLine;
        tmpInputfile2.read(buffer, 1);
        if (buffer[0] == '\r')
            tmpInputfile2.read(buffer, 1);
        if (buffer[0] == '\n')
            tmpInputfile2.read(buffer, 1);
        while (isHexadecimal(buffer[0]))
        {
            chunkSizeLine += buffer[0];
            tmpInputfile2.read(buffer, 1);
        }
        if (buffer[0] == '\r')
            tmpInputfile2.read(buffer, 1);
        chunkSize = hexToDecimal(chunkSizeLine);
        memset(buffer,0, sizeof(buffer));
    }
    if (chunkSize > 1024)
    {
        tmpInputfile2.read(buffer, 1024);
        std::streamsize bytesRead = tmpInputfile2.gcount();
        if (bytesRead > 0)
            inputfile.write(buffer, bytesRead);
        chunkSize = chunkSize - bytesRead;
        total += bytesRead;
    }
    else
    {
        tmpInputfile2.read(buffer, chunkSize);
        std::streamsize bytesRead = tmpInputfile2.gcount();
        if (bytesRead > 0)
            inputfile.write(buffer, bytesRead);
        total += bytesRead;
        chunkSize = 0;
    }
    if (tmpInputfile2.eof())
        isParseChunkedFiniched = true;
}

void Client::methodpost()
{
    if(getbestMatchLocation()->auto_upload =="on")
    {      
        if (createFile() && !isGetFullBody)
        {
            if (isChunked)
            {
                if (contentLength >= totalReaded)
                {
                    tmpInputfile.write(bodyBuffer, bytes);
                    totalReaded += bytes;
                    memset(bodyBuffer,0, 1024);
                }
                else if(contentLength <= totalReaded)
                {
                    if (!isParseChunkedFiniched)
                    {
                        parseChunked();
                        return;
                    }
                    if (getbestMatchLocation()->cgi == "on")
                    {
                        inputfile.close();
                        std::string fullPath = getfilePath() + getbestMatchLocation()->index;
                        std::string fileExtension = getFileExtension(fullPath);
                        if (fileExtension == ".py" || fileExtension == ".php") 
                        {
                            handle_cgi(getfilePath(), fileExtension, *this);
                            setResponse();
                            isGetFullBody = 1;
                            remove(tmpFileName.c_str());
                            return;
                        }
                    }
                    setresponse("HTTP/1.1 201 Created", "text/html", "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n<html><head>\r\n<title>201 Created</title>\r\n</head><body>\r\n<h1>Created</h1>\r\n<p>The resource has been successfully created.</p>\r\n</body></html>\r\n");
                    int byte = send(getClientSocket(),getresponse(),strlen(getresponse()),0);
                    if(byte == 0 || byte == -1)
                    {
                        setResponse();
                        isGetFullBody = 1;
                        inputfile.close();
                        if (remove(tmpFileName.c_str()) != 0)
                            setResponse();
                        return;
                    }
                    setResponse();
                    isGetFullBody = 1;
                    inputfile.close();
                    if (remove(tmpFileName.c_str()) != 0)
                        setResponse();
                }
                else memset(bodyBuffer,0, 1024); 
            }
            else
            {
                inputfile.write(bodyBuffer, bytes);
                totalReaded += bytes;
                if(contentLength <= totalReaded)
                {
                    if (getbestMatchLocation()->cgi == "on" ) 
                    {
                        inputfile.close();
                        std::string fullPath = getfilePath() + getbestMatchLocation()->index;
                        std::string fileExtension = getFileExtension(fullPath);
                        if (fileExtension == ".py" || fileExtension == ".php") 
                        {
                            handle_cgi(fullPath, fileExtension, *this);
                            this->setResponse();
                            isGetFullBody = 1;
                            return;
                        }
                    }
                    setresponse("HTTP/1.1 201 Created", "text/html", "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n<html><head>\r\n<title>201 Created</title>\r\n</head><body>\r\n<h1>Created</h1>\r\n<p>The resource has been successfully created.</p>\r\n</body></html>\r\n");
                   bytes = send(getClientSocket(),getresponse(),strlen(getresponse()),0);
                   if(bytes == 0 || bytes == -1)
                   {
                         setResponse();
                         inputfile.close(); 
                    }
                    setResponse();
                    isGetFullBody = 1;
                    inputfile.close();     
                }
                bytes = 0;
                memset(bodyBuffer,0, 1024);
            }
        } 
    }
    else 
    {
        if (isChunked && contentLength >= totalReaded)
            notfound();
        else  if (contentLength <= totalReaded)
            notfound();
        totalReaded += bytes;
    }
        
}

std::string generateUniqueId() {
    std::ostringstream oss;
    oss << std::time(0);
    return oss.str();
}

std::string     generateNewFileName() {
    return (generateUniqueId() + std::to_string(rand()));
}


int handle_cgi(std::string file, std::string exten, Client &client) {
    Cgi cgi(file, exten, client);
    int status;
    cgi.init();
    unlink("out");
    int bytes;
    int out = open("out", O_WRONLY | O_CREAT, 0755);
    int pid = fork();
    if (pid < 0) {
        cgi._flag = false;
        if (!client.isResponseFinished() && !client.handleErrorResponse("500", "Internal Server Error")) {
            client.setresponse("HTTP/1.1 500 Internal Server Error", "text/html",
                                "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n"
                                "<html><head>\r\n<title>500 Internal Server Error</title>\r\n</head><body>\r\n"
                                "<h1>Internal Server Error</h1>\r\n<p>The server encountered an internal error while processing the request.</p>\r\n"
                                "</body></html>\r\n");
            bytes = send(client.getClientSocket(), client.getresponse(), strlen(client.getresponse()), 0);
            if (bytes != 0 || bytes != -1) {
                client.setResponse();
            }
        }
    }
    if (pid == 0) {
        // Child process
        if (client.getParsedRequest().getMethod() == "POST") {
            int in = open(client.getFileNamePost().c_str(), O_RDWR);
            dup2(in, 0);
            close(in);
        }
        dup2(out, 1);
        close(out);
        execve(cgi.getArgv()[0], cgi.getArgv(), cgi.getEnvir());
    }
    close(out);

    //timeout for CGI process 2s
    const int CGI_TIMEOUT_SEC = 2;

    // Get current time
    struct timeval start_time, curr_time;
    gettimeofday(&start_time, NULL);

    // Non-blocking wait for child process
    while (true) {
        gettimeofday(&curr_time, NULL);
        int elapsed_time_sec = curr_time.tv_sec - start_time.tv_sec;

        if (elapsed_time_sec >= CGI_TIMEOUT_SEC) {
            // Timeout reached, kill the child process
            kill(pid, SIGKILL);
            cgi._flag = false;
            waitpid(pid, &status, 0); // Clean up zombie process
            client.setresponse("HTTP/1.1 500 Internal Server Error", "text/html",
                                "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n"
                                "<html><head>\r\n<title>500 Internal Server Error</title>\r\n</head><body>\r\n"
                                "<h1>Internal Server Error</h1>\r\n<p>The server encountered an internal error while processing the request.</p>\r\n"
                                "</body></html>\r\n");
            bytes = send(client.getClientSocket(), client.getresponse(), strlen(client.getresponse()), 0);
            if (bytes != 0 || bytes != -1) {
                client.setResponse();
            }
            break;
        }

        int wait_ret = waitpid(pid, &status, WNOHANG);
        if (wait_ret == pid) {
            //std::cout << "status ----->" << status << std::endl;
            if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                // Normal-good termination
                break;
            } else {
                // Bad-Abnormal termination
                cgi._flag = false;
                client.setresponse("HTTP/1.1 500 Internal Server Error", "text/html",
                                    "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n"
                                    "<html><head>\r\n<title>500 Internal Server Error</title>\r\n</head><body>\r\n"
                                    "<h1>Internal Server Error</h1>\r\n<p>The server encountered an internal error while processing the request.</p>\r\n"
                                    "</body></html>\r\n");
                bytes = send(client.getClientSocket(), client.getresponse(), strlen(client.getresponse()), 0);
                if (bytes != 0 || bytes != -1) {
                    client.setResponse();
                }
                break;
            }
        } else if (wait_ret == 0) {
            usleep(10000); 
        } else if (wait_ret < 0 && errno != EINTR) {
            perror("waitpid");
            break;
        }
    }
    return 0;
}


int Client::clear_dir(const std::string& dir_name) 
{
    DIR* dir = opendir(dir_name.c_str());
    int result;
    if (dir == NULL)
        return 403;
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string file_name = entry->d_name;

        if (file_name != "." && file_name != "..")
        {
            std::string full_path = dir_name + "/" + file_name;

            if (entry->d_type == DT_DIR) 
            {
                result = clear_dir(full_path);
                if (result != 204)
                    return result;
                result = rmdir(full_path.c_str());
                if (result)
                {
                    if(result == EACCES)
                        return 500;
                    return 403;
                }
            } 
            else
            {
                result = unlink(full_path.c_str());
                if (result)
                {
                    if(result == EACCES)
                        return 500;
                    return 403;
                }
            }
        }
    }
    closedir(dir);
    return 204;
}

void Client::Delete_method()
{
    int sendReturn ;
    std::string fullpath =getfilePath();
    DIR *dir = opendir(fullpath.c_str());
    int result;
    if (dir)
    {
        closedir(dir);
        if (fullpath.back() != '/')
        {
            fullpath += "/";
            if (!isResponseFinished() && !handleErrorResponse("409"," Conflict")) 
            {
            
                setresponse("HTTP/1.1 409 Conflict", "text/html",
                "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n"
                "<html><head>\r\n<title>409 Conflict</title>\r\n</head><body>\r\n"
                "<h1>Conflict</h1>\r\n<p>The request could not be completed due to a conflict with the current state of the target resource.</p>\r\n"
                "</body></html>\r\n");
                sendReturn = send(getClientSocket(), getresponse(), strlen(getresponse()), 0);
               if(sendReturn != 0 || sendReturn != -1)
                    setResponse();
            }
        }
        clear_dir(fullpath);
        result = clear_dir(fullpath);
        if (result != 204)
        {   
            if (result == 403 && !isResponseFinished() && !handleErrorResponse(std::to_string(result)," Forbidden")) 
            {
                setresponse("HTTP/1.1 403 Forbidden", "text/html",
                    "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n"
                    "<html><head>\r\n<title>403 Forbidden</title>\r\n</head><body>\r\n"
                    "<h1>Forbidden</h1>\r\n<p>You don't have permission to access this resource on the server.</p>\r\n"
                    "</body></html>\r\n");
                sendReturn = send(getClientSocket(), getresponse(), strlen(getresponse()), 0);
                if(sendReturn != 0 || sendReturn != -1)
                    setResponse();
            }
            else  if (result == 500 && !isResponseFinished() && !handleErrorResponse(std::to_string(result)," Internal Server Error")) 
            {
                if (!isResponseFinished() && !handleErrorResponse("500"," Internal Server Error") )
                {
                    setresponse("HTTP/1.1 500 Internal Server Error", "text/html",
                    "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n"
                    "<html><head>\r\n<title>500 Internal Server Error</title>\r\n</head><body>\r\n"
                    "<h1>Internal Server Error</h1>\r\n<p>The server encountered an internal error while processing the request.</p>\r\n"
                    "</body></html>\r\n");    
                    sendReturn = send(getClientSocket(), getresponse(), strlen(getresponse()), 0);
                    if(sendReturn != 0 || sendReturn != -1)
                        setResponse();
            }
            }
        }
        result = rmdir(fullpath.c_str());
        if (result)
        {
            if (result == EACCES)
            {
                if (!isResponseFinished() && !handleErrorResponse("500"," Internal Server Error") ){
                    setresponse("HTTP/1.1 500 Internal Server Error", "text/html",
                    "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n"
                    "<html><head>\r\n<title>500 Internal Server Error</title>\r\n</head><body>\r\n"
                    "<h1>Internal Server Error</h1>\r\n<p>The server encountered an internal error while processing the request.</p>\r\n"
                    "</body></html>\r\n");    
                    sendReturn= send(getClientSocket(), getresponse(), strlen(getresponse()), 0);
                    if(sendReturn != 0 || sendReturn != -1)
                        setResponse();
                }
            }
            else if (!isResponseFinished() && !handleErrorResponse("403"," Forbidden")) 
            {
                setresponse("HTTP/1.1 403 Forbidden", "text/html",
                    "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n"
                    "<html><head>\r\n<title>403 Forbidden</title>\r\n</head><body>\r\n"
                    "<h1>Forbidden</h1>\r\n<p>You don't have permission to access this resource on the server.</p>\r\n"
                    "</body></html>\r\n");
               sendReturn = send(getClientSocket(), getresponse(), strlen(getresponse()), 0);
                if(sendReturn != 0 || sendReturn != -1)
                    setResponse();
            }
        }
        else
        {
            setresponse("HTTP/1.1 204 No Content", "text/html",
                "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n"
                "<html><head>\r\n<title>204 No Content</title>\r\n</head><body>\r\n"
                "<h1>No Content</h1>\r\n<p>The request was successful, but there is no content to send in the response.</p>\r\n"
                "</body></html>\r\n");
            sendReturn = send(getClientSocket(), getresponse(), strlen(getresponse()), 0);
            if(sendReturn != 0 || sendReturn != -1)
                setResponse();
        }
    }
    result = unlink(fullpath.c_str());
    if (result)
    {
        if(result == EACCES)
        {
            if (!isResponseFinished() && !handleErrorResponse("500"," Internal Server Erro") ) 
            {
                setresponse("HTTP/1.1 500 Internal Server Error", "text/html",
                "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n"
                "<html><head>\r\n<title>500 Internal Server Error</title>\r\n</head><body>\r\n"
                "<h1>Internal Server Error</h1>\r\n<p>The server encountered an internal error while processing the request.</p>\r\n"
                "</body></html>\r\n");    
                sendReturn =  send(getClientSocket(), getresponse(), strlen(getresponse()), 0);
                if(sendReturn != 0 || sendReturn != -1)
                    setResponse();
            }
        }
        if (!isResponseFinished() && !handleErrorResponse("403"," Forbidden")) 
        {
            setresponse("HTTP/1.1 403 Forbidden", "text/html",
                "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n"
                "<html><head>\r\n<title>403 Forbidden</title>\r\n</head><body>\r\n"
                "<h1>Forbidden</h1>\r\n<p>You don't have permission to access this resource on the server.</p>\r\n"
                "</body></html>\r\n");
            sendReturn = send(getClientSocket(), getresponse(), strlen(getresponse()), 0);
            if(sendReturn != 0 || sendReturn != -1)
                setResponse();
        }
    }
    if (!isResponseFinished()) 
    {
        setresponse("HTTP/1.1 204 No Content", "text/html",
        "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n"
        "<html><head>\r\n<title>204 No Content</title>\r\n</head><body>\r\n"
        "<h1>No Content</h1>\r\n<p>The request was successful, but there is no content to send in the response.</p>\r\n"
        "</body></html>\r\n");
        sendReturn = send(getClientSocket(), getresponse(), strlen(getresponse()), 0);
        if(sendReturn != 0 || sendReturn != -1)
            setResponse();
    }
}

Client::Client(const Client& other)
{
        clientSocket = other.clientSocket;
        requestBuffer = other.requestBuffer;
        isGetFullReq = other.isGetFullReq;
        isSentFullRes = other.isSentFullRes;
        cfg = other.cfg;  // Assuming confi has an appropriate assignment operator
        response = other.response;
        bestMatchLocation = other.bestMatchLocation;
        bestmatch = other.bestmatch;
        bestMatchLength = other.bestMatchLength;
        request = other.request;
        filePath = other.filePath;
        methodAllowed = other.methodAllowed;
        isSentHreders = other.isSentHreders;
        isSetRequest = other.isSetRequest;
        fileNamePost = other.fileNamePost;
}
Client& Client::operator=(const Client& other)
{
    if (this != &other)
    {
        // Copy members here similar to the copy constructor
        clientSocket = other.clientSocket;
        requestBuffer = other.requestBuffer;
        isGetFullReq = other.isGetFullReq;
        isSentFullRes = other.isSentFullRes;
        cfg = other.cfg;  // Assuming confi has an appropriate assignment operator
        response = other.response;
        bestMatchLocation = other.bestMatchLocation;
        bestmatch = other.bestmatch;
        bestMatchLength = other.bestMatchLength;
        request = other.request;
        filePath = other.filePath;
        methodAllowed = other.methodAllowed;
        isSentHreders = other.isSentHreders;
        isSetRequest = other.isSetRequest;
        fileNamePost = other.fileNamePost;
        //fileStream = other.fileStream;  // Assuming std::ifstream is copyable
    }
    return *this;
}