SRCS = $(wildcard srcs/*.cpp)

NAME = ClanTracker

OBJ = $(SRCS:.cpp=.o)

CC = c++

CFLAGS = -Wall -Wextra -Werror -std=c++17

CPPFLAGS = -Iincludes

LDFLAGS = -lcurl

%.o: %.cpp
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@


all : $(NAME)

$(NAME) : $(OBJ)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(OBJ) $(LDFLAGS) -o $(NAME)

clean :
		rm -f $(OBJ)

fclean : clean
		rm -f $(NAME)

re : fclean all

.PHONY : all clean fclean re