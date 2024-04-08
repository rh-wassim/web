#include "HttpRequest.hpp"

// Constructor to parse and initialize the request
HttpRequest::HttpRequest(const char* request):isGetAllHeaders(0), isGetFirstLine(0), isGetFullBody(0), body("")
{
    parse(request);
    // int index = 
    setQuery(this->resource.substr(this->resource.find('?') + 1));
    this->resource = this->resource.substr(0, this->resource.find('?'));
}





HttpRequest::HttpRequest(const HttpRequest& other)
{
    isGetAllHeaders = other.isGetAllHeaders;
    isGetFirstLine = other.isGetFirstLine;
    isGetFullBody = other.isGetFullBody;
    body = other.body;
    method = other.method;
    resource = other.resource;
    headers = other.headers;
    httpVersion = other.httpVersion;
    querystr = other.querystr;
}

HttpRequest& HttpRequest::operator=(const HttpRequest& other)
{
    if (this != &other) 
    {
        isGetAllHeaders = other.isGetAllHeaders;
        isGetFirstLine = other.isGetFirstLine;
        isGetFullBody = other.isGetFullBody;
        body = other.body;
        method = other.method;
        resource = other.resource;
        headers = other.headers;
        httpVersion = other.httpVersion;
        querystr = other.querystr;
    }
    return *this;
}

// Destructor
HttpRequest::~HttpRequest(){}

// Getters for request attributes
const std::string& HttpRequest::getMethod() const 
{
    return method;
}

const std::string& HttpRequest::getQuery() const
{
    return querystr;
}

const std::string& HttpRequest::getResource() const 
{
    return resource;
}

const std::map<std::string, std::string>& HttpRequest::getHeaders() const 
{
    return headers;
}

const std::string&  HttpRequest::getBody() const
{
    return body;
}

const std::string&  HttpRequest::getHttpVersion() const
{
    return httpVersion;
}

void    HttpRequest::setQuery( std::string qstr)
{
    querystr = qstr;
}

// Parse the HTTP request
static  std::string strtrim(const std::string& str) {
    size_t start = 0;
    size_t end = str.length();

    // Find the first non-whitespace character from the beginning
    while (start < end && std::isspace(str[start])) {
        ++start;
    }

    // Find the first non-whitespace character from the end
    while (end > start && std::isspace(str[end - 1])) {
        --end;
    }

    // Return the trimmed substring
    return str.substr(start, end - start);
}
void HttpRequest::parse(const char* request) 
{
    std::istringstream requestStream(request);
    std::string         line;

    // Parse the first line to get the method and resource
    if (!isGetFirstLine && std::getline(requestStream, line))
    {
        std::istringstream lineStream(line);
        lineStream >> method >> resource >> httpVersion;
        isGetFirstLine = 1;
    }


    // Parse headers
    int isR = 0;
    while (!isGetAllHeaders && !requestStream.eof() && std::getline(requestStream, line)) 
    {
        if (line == "\r" && line.size() == 1)
        {
            isGetAllHeaders = 1;
            break;
        }
        size_t separatorPos = line.find(':');
        if (isR != 2 && separatorPos != std::string::npos)
        {
            std::string key = line.substr(0, separatorPos);
            std::string value = line.substr(separatorPos + 1);
            headers[strtrim(key)] = strtrim(value);
        }
    }

    bool isfline = 0;
    while (!requestStream.eof() && std::getline(requestStream, line))
    {
        if (!isfline || requestStream.eof())
        {
            body += line;
            isfline = 1;
        }
        else body += "\n" + line;
    }
    
}


void    HttpRequest::setIsGetFullBody()
{
    isGetFullBody = 1;
}