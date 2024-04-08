#include "Config.hpp"

std::vector<std::string> ft_split(const std::string &str, char del) {
    std::vector<std::string> out;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, del)) {
        out.push_back(token);
    }
    
    return out;
}


void    confi::split_methods(std::string &str, location &loc) 
{
    char *word = strtok(const_cast<char *>(str.c_str()), " "); 

    word = strtok(NULL, " ");
    while (word)
    {
        if (!strcmp(word, "GET") || !strcmp(word, "POST") || !strcmp(word, "DELETE"))
            loc.allow_methods.push_back(word);
        else
        {
            std::cerr << "You must Follow Our Rules in Methods\n";
            exit(1);
        }
        word = strtok(NULL, " ");
    }
}

int      ft_isdigit(char *n)
{
    int i = 0;
    int num;
    while (n[i])
    {
        if (!isdigit(n[i]))
        {
            std::cout << "Not a number\n";
            exit(1);
        }
        i++;
    }
    num = atoi(n);
    return (num);
}

void     parse_host(std::string host)
{
    char *n;
    int i = 0;
    int num;

    n = strtok(const_cast<char *>(host.c_str()), ".");
    while (n != NULL)
    {
        num = ft_isdigit(n);
        if (num < 0 || num > 255)
        {
            std::cout << "Out of Range\n";
            exit(1);
        }
        n = strtok(NULL, ".");
        i++;
    }
    if (i != 4)
    {
        std::cout << "Error: Not IPv4\n";
        exit(1);
    }
}


int     confi::parse_configuration(std::string file)
{
    std::fstream config_file;
    std::string line;
    struct server serv;
    struct location loc;
    serv.port = -1;
    int i = -1;
    int j = -1;
    serv.err_pages["201"] = "";
	serv.err_pages["204"] = "";
	serv.err_pages["301"] = "";
	serv.err_pages["400"] = "";
	serv.err_pages["403"] = "";
	serv.err_pages["404"] = "";
	serv.err_pages["405"] = "";
	serv.err_pages["409"] = "";
	serv.err_pages["413"] = "";
	serv.err_pages["414"] = "";
	serv.err_pages["500"] = "";
	serv.err_pages["501"] = "";

    
    config_file.open(file.c_str());
    if (config_file.is_open())
    {
        config_file.seekg(0, std::ios::end);
        if (config_file.tellg() == 0)
        {
            std::cout << "Error : Config File is Empty\n";
            exit(1);
        }
        config_file.seekg(0, std::ios::beg);
        while (getline(config_file, line))
        {
            i = -1;
            if (!line.compare("server"))
            {
                i = 0;
                j = -1;
                serv.mx_cl_bd_size = 1000000;
                serv.encountered_keys.clear();
                serv.root.clear();
                while (getline(config_file, line))
                {
                    std::string key, value;
                    std::stringstream strm(line);
                    strm >> key;
                    strm >> value;
                    // Check for duplicated keys
                    if (key != "location" && key != "error_page" && serv.encountered_keys.find(key) != serv.encountered_keys.end())
                    {
                        std::cerr << "Error: Duplicated key '" << key << "' inside the 'server' block.\n";
                        exit(1);
                    }
                    serv.encountered_keys[key] = true;
                    if (!line.compare("{"))
                        i++;
                    else if (!line.compare("}"))
                    {
                        if ((serv.host.empty() || serv.server_name.empty()) || serv.locations.empty() || serv.port == -1)
                        {
                            std::cerr << "You must Follow Our Rules" << std::endl;
                            exit(1);
                        }
                        servers.push_back(serv); 
                        serv.err_pages.clear();
                        serv.locations.clear();
                        i--;
                        break;
                    }
                    else if (!key.compare("host") && strm.eof() && !value.empty())
                    {
                        parse_host(value);
                        serv.host = value;
                    }
                    else if (!key.compare("port") && strm.eof() && !value.empty())
                    {
                        int port_num;
                        port_num = ft_isdigit(const_cast<char *>(value.c_str()));
                        if (port_num < 0 || port_num > 65535)
                        {
                            std::cout << "Error in Port\n";
                            exit(1);
                        }
                        serv.port = port_num;  // storing port as integer.
                    }
                    else if (!key.compare("max_client_body_size") && strm.eof() && !value.empty()) // why !key.compare() and not key.compare()? because we are checking if the key is not empty.
                    {
                        if (value.find_first_not_of("0123456789") != std::string::npos)
                        {
                            std::cerr << "Error: max_client_body_size must be a positive integer\n";
                            exit(1);
                        }
                        serv.mx_cl_bd_size = std::atoi(value.c_str());
                    }
                    else if (!key.compare("server_name") && strm.eof() && !value.empty())
                        serv.server_name = value;
                    else if (!key.compare("error_page") && !value.empty())
					{
						std::vector<std::string>err_r;
						err_r = ft_split(line, ' ');
						if(err_r.size() != 3 || err_r[1].find_first_not_of("0123456789") != std::string::npos)
                        {
							std::cerr << "error page format not valid\n";
                            exit(1);
                        }
						else
							serv.err_pages[err_r[1]] = err_r[2];
					}
                    else if (!key.compare("root") && strm.eof() && !value.empty()) 
                        serv.root = value;
                    else if (!key.compare("location"))
                    {
                        j = 0;
                        if (!strm.eof())
                        {
                            std::cerr << "Error : location Path\n";
                            exit(1);
                        }
                        if (!value.empty()){
                            loc.path_location = value;
                        }
                        else
							{
                            std::cerr << "Location path is empty" << std::endl;
                            exit(1);
                            }
                        loc.auto_index = "off";
                        loc.auto_upload = "off";
                        loc.cgi = "off";
                        loc.encountered_keys.clear();
                        while (getline(config_file, line))
                        {
                            std::string key_local, value_local;
                            std::stringstream strm_local(line);
                            strm_local >> key_local;
                            strm_local >> value_local;
                            if (loc.encountered_keys.find(key_local) != loc.encountered_keys.end())
                            {
                                std::cerr << "Error: Duplicated local_key '" << key_local << "' inside the 'location' block.\n";
                                exit(1);
                            }
                            loc.encountered_keys[key_local] = true;
                            if (!line.compare("{"))
                            {
                                j++;
                                continue;
                            }
                            else if (!line.compare("}"))
                            {
                                j--;
                                serv.locations.push_back(loc);
                                loc.allow_methods.clear();
                                loc.index.clear();
                                loc.path_location.clear();
                                loc.root.clear();
                                loc.auto_upload.clear();
                                loc.auto_index.clear();
                                loc.redirection.clear();
                                loc.cgi.clear();
                                break;
                            }
                            if (!key_local.compare("allow_methods") && !value_local.empty())
                                split_methods(line, loc);
                            else if (!key_local.compare("index") && strm_local.eof() && !value_local.empty())
                                loc.index = value_local;
                            else if (!key_local.compare("root") && strm_local.eof() && !value_local.empty())
                                loc.root = value_local;
                            else if (!key_local.compare("auto_upload") && strm_local.eof() && !value_local.empty())
                            {
                                if (value_local == "on" || value_local == "off")
                                    loc.auto_upload = value_local;
                                else
                                {
                                    std::cerr << "Error: Invalid value for auto_upload. Use 'on' or 'off'\n";
                                    exit(1);
                                }
                            }
                            else if (!key_local.compare("auto_index") && strm_local.eof() && !value_local.empty())
                            {
                                if (value_local == "on" || value_local == "off")
                                    loc.auto_index = value_local;
                                else
                                {
                                    std::cerr << "Error: Invalid value for auto_index. Use 'on' or 'off'\n";
                                    exit(1);
                                }
                            }
                            else if (!key_local.compare("return") && strm_local.eof() && !value_local.empty())
                                loc.redirection = value_local;
                            else if (!key_local.compare("cgi") && strm_local.eof() && !value_local.empty())
                            {
                                if (value_local == "on" || value_local == "off")
                                    loc.cgi = value_local;
                                else
                                {
                                    std::cerr << "Error: Invalid value for cgi. Use 'on' or 'off'\n";
                                    exit(1);
                                }
                            }
                            else
                            {
                                std::cerr << "You must Follow Our Rules in Location" << std::endl;
                                exit(1);
                            }

                        }
                        if (j != 0)
                        {
                            std::cerr << "You must Follow Our Rules" << std::endl;
                            exit(1);
                        }
                    }
                    else
                    {
                        std::cerr << "You must Follow Our Rules" << std::endl;
                        exit(1);
                    }
                }
                if(serv.root.empty()){
                    std::cerr << "server root empty" << std::endl;
                    exit(1);
                }
            }
            if (i != 0)
            {
                std::cerr << "You must Follow Our Rules" << std::endl;
                exit(1);
            }
        }

    }
    else
    {
        std::cerr << "Error: can't open config file\n";
        exit(1);
    }
    return (0);
}
