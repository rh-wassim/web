#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "HttpRequest.hpp"
#include <sstream>
#include "Cgi.hpp"
#include <cstdlib>
#include <ctime>
#include <sys/time.h>

class Cgi;

class Client
{
    private:
    int             clientSocket;
    std::string     requestBuffer;
    bool            isGetFullReq;
    bool            isSentFullRes;
    confi           cfg;
    int             flag_error;
    std::string     response;
    location*       bestMatchLocation;
    std::string     bestmatch;
    size_t          bestMatchLength;
    HttpRequest     request;
    std::string     filePath;
    bool            methodAllowed;
    bool            isSentHreders;
    bool            isSetRequest;
    std::ifstream   fileStream;
    struct location loc;
    std::ofstream   inputfile;
    std::ofstream   tmpInputfile;
    std::ifstream   tmpInputfile2;
    int             fileDescriptor;
    char            bodyBuffer[1024];
    char            buffer[1024];
    bool            isFileCreated;
    bool            isGetFullBody;
    int             bytes;
    bool            isChunked;
    std::ifstream   fileerror;
    std::string     tmpFileName;
    size_t          totalReaded;
    size_t          contentLength;
    std::string     fileNamePost;
    int             f;
    bool            isParseChunkedFiniched;//********
    bool            isInitChunkedPara;//********
    int             chunkSize;//********
    bool            isFirstLine;//********
    int             total;//********
    std::map<std::string, std::string> err_page;
public:
    Client(){}
    Client(int Socket, confi &cf);
    std::string     getExtensionByContentType(const std::string& contentType);
    void            Delete_method();
    int             clear_dir(const std::string& dir_name);
    void            handleDirectoryAutoIndex();
    bool            sendHredes2(std::string  fullPath);
    void            methodGetFile();
    void            methodGet();
    void            notfound();
    bool            handleErrorResponse(const std::string& errorCode,std::string title) ;
    void            methodnotallow();
    void            getRequest();
    void            handelread();
    bool            isInvalidURI(const std::string& uri);
    bool            isRequestFinished();
    bool            isResponseFinished();
    int             getClientSocket()const;
    void            closeClientSocket();
    void            setResponse();
    void            setParsedRequest();
    HttpRequest&    getParsedRequest();
    ~Client();
    void            methodpost();
    void            handleClient();
    ///****************
    bool            sendHredes();
    std::string     getfilePath();
    std::string     &getbestmatch();
    const location* getbestMatchLocation();
    void            setClientResponce();
    void            setresponse(const std::string& status,const std::string& contentType,const std::string& content);
    const char      *getresponse();
    void            matchinglocation();
    void            split(const std::string& input, char delimiter, std::string& part1, std::string& part2);
    std::string     getFileExtension(const std::string& filePath);
    bool            deleteResource(std::string fileName);
    std::string     concatenatePaths(const std::string& path1, const std::string& path2);
    std::string     toLowerCase(const std::string& str);
    std::string     getContentTypeByExtension(const std::string& extension);
    bool            createFile();
    void            parseChunked();
    size_t          getcontentLength();
    std::string     getFileNamePost();
    void            isRequestWellFormed();
    void            handleRequest();
    static void*    handleClientThread(void* arg);

    Client(const Client& other);
    Client& operator=(const Client& other);
};
int         handle_cgi(std::string file,std::string exten, Client& );
void        printParsedRequest(HttpRequest &request);
std::string generateNewFileName();
std::string generateUniqueId();
#endif
