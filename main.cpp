
#include "Client.hpp"
#include <list>

int checkClients(std::list<Client> &clients, fd_set  *readfds ,fd_set  *writefds)
{
    int max = 0;
    std::list<Client>::iterator it = clients.begin();
    while (it != clients.end())  
    {
        if (it->isResponseFinished())
        {
            it->closeClientSocket();
            FD_CLR(it->getClientSocket() , readfds);   
            FD_CLR(it ->getClientSocket() , writefds);
            it = clients.erase(it);
        }
        else
        {
            if (it->getClientSocket() > max)
                max = it->getClientSocket();
            ++it;
        }
    }
    return(max);
}

void startServer(std::deque<server>& serv, std::vector<int> &serverSockets) 
{
    for (size_t i = 0; i < serv.size(); i++)
    {
        try 
        {
            int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
            if (serverSocket == -1)
            {
                throw std::runtime_error("Error creating socket\n");
            }
            int enable = 1;            //set option level of socket
            if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1) //SO_REUSEPORT
            {
                throw std::runtime_error("Error seting socket\n");
            }
            sockaddr_in serverAddr; // is a structure representing an IPv4 socket address
            serverAddr.sin_family = AF_INET; //The address family, set to AF_INET for IPv4.
            serverAddr.sin_addr.s_addr = INADDR_ANY; //The IP address to bind to. INADDR_ANY means the server will accept connections on any available network interface
            serverAddr.sin_port = htons(serv[i].port); //The port number to bind to, converted to network byte order using htons().
            if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1) 
            {
                close(serverSocket);
                std::string err = "Error binding socket on port " + std::to_string(serv[i].port) + "\n";
                throw std::runtime_error(err);
            }
            if (listen(serverSocket, 1024) == -1) 
            {
                close(serverSocket);
                throw std::runtime_error("Error listening on socket\n");
            }
            std::cout << "Server listening on port " << serv[i].port << "...\n";
            serverSockets.push_back(serverSocket);
        }
        catch (std::exception &e) {
            std::cerr << "ERROR : " << e.what() << std::endl;
        }
    }
    if (serverSockets.size() == 0)
        throw std::runtime_error("Can not establish connections on this servers\n");
}

int main(int ac , char **av)
{
    try
    {
    if (ac != 2){
        std::cout << "----->  ./webserv [configuration file]\n";
        return (1);
    }
        confi cfg;
        std::srand(std::time(0));
        cfg.parse_configuration(av[1]);
        std::vector<int> serverSockets;
        startServer(cfg.servers, serverSockets);
        std::list<Client> clientSockets;
        fd_set  writefds;
        fd_set  readfds;
        int maxSocket = serverSockets.back();
        struct timeval timeout;
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        for (std::vector<int>::iterator it = serverSockets.begin(); it != serverSockets.end(); ++it) 
        {      
            const int& socket = *it;
            FD_SET(socket, &readfds);
            FD_SET(socket, &writefds);
        }
        signal(SIGPIPE, SIG_IGN);  
        while (true) 
        {
        for (std::vector<int>::iterator iit = serverSockets.begin(); iit != serverSockets.end(); ++iit) 
            {
                fd_set  tmpfdsread = readfds;
                fd_set  tmpfdswrite = writefds;
                timeout.tv_sec = 0;
                timeout.tv_usec = 0;
                if (select(maxSocket + 1, &tmpfdsread,&tmpfdswrite, 0, &timeout) == -1)
                {
                    std::cerr << "Error in select\n";
                }
                if (FD_ISSET(*iit, &tmpfdsread))
                {        
                    sockaddr_in clientAddr;
                    socklen_t clientAddrSize = sizeof(clientAddr);
                    int clientSocket = accept(*iit, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrSize);
                    if (clientSocket != -1)
                    {
                        fcntl(clientSocket, F_SETFL, O_NONBLOCK);
                        Client client(clientSocket, cfg);
                        FD_SET(clientSocket, &readfds);
                        FD_SET(clientSocket, &writefds);
                        clientSockets.push_back(client);
                    }
                    else
                        std::cerr << "Error accepting connection\n";
                }
                if (clientSockets.size())
                for (std::list<Client>::iterator it = clientSockets.begin(); it != clientSockets.end();++it)
                {
                    if (FD_ISSET((*it).getClientSocket(), &tmpfdsread)) 
                    {
                        (*it).handelread();
                    }
                    if (FD_ISSET((*it).getClientSocket(), &tmpfdswrite) && (*it).isRequestFinished() && !(*it).isResponseFinished())
                    {
                        (*it).handleClient();
                    }
                }
                
            if (clientSockets.size())
            {
                maxSocket = checkClients(clientSockets ,&readfds, &writefds);
                if (maxSocket == 0)
                    maxSocket = serverSockets.back();
            }
            else
                maxSocket = serverSockets.back();
            }
        }
        for (std::vector<int>::iterator it = serverSockets.begin(); it != serverSockets.end(); ++it) 
        {
            const int& socket = *it;
            close(socket);
        }

    }
    catch(std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Error" << std::endl;
    }
    return 0;
}
