#ifndef CGI_HPP
#define CGI_HPP

#include <sys/wait.h>
#include <signal.h>

#include "Client.hpp"

typedef	std::string	string;
typedef	std::map<string, string>	mapstring;

class Client;

#define MAX_INPUT 1024
class Cgi {
	private:
		HttpRequest	_req;
		string		_file;
		string		_exten;
		string		_init_line;
    	mapstring   _env;
    	string		resp;
		string		execut;
    	char 		**env;
    	char 		**av;
		int			clientSocket;
		std::string toDeleteFile;
	public:
		Cgi(string file, string extention, Client& res);
		~Cgi( void );
		void	init( void );
		char 	**getEnvir( void );
		char 	**getArgv( void );
		void 	fill_env( string , HttpRequest &, mapstring &);
		void    free_all();
		char**	convertMapToCharArray(mapstring& );
		int     handle_cgi(std::string file,std::string exten, Client& );
		bool		_flag;
};
#endif