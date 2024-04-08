NAME = webserv

SRC = main.cpp ConfigFile.cpp HttpRequest.cpp Client.cpp Cgi.cpp

OBJ = $(SRC:.cpp=.o)

CXX = c++

CXXFLAGS = -std=c++98 -Wall -Wextra -Werror -fsanitize=address -g 

RM = rm -rf

all : $(NAME)

$(NAME) : $(OBJ) Cgi.hpp Client.hpp Config.hpp HttpRequest.hpp
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

clean :
	$(RM) $(OBJ)

fclean : clean
	$(RM) $(NAME)

re : fclean all 