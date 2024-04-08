#ifndef CONFIG_HPP
# define CONFIG_HPP

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <deque>
#include <map>
#include <string>
#include <cstring>
#include <vector>

struct location
{
    std::map<std::string, bool> encountered_keys;
    std::string path_location, root, auto_upload, auto_index, redirection, cgi, index;
    std::deque<std::string> allow_methods;
};

struct server
{
    std::map<std::string, bool> encountered_keys;
    std::string host, server_name;
    std::string root;
    int port;
    size_t mx_cl_bd_size;
    std::map<std::string, std::string> err_pages;
    std::deque<location> locations;
};

class confi
{
    public:
        std::deque<server> servers;
        void    split_methods(std::string &, location &lc);
        int     parse_configuration(std::string);
};

#endif