# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: efumiko <efumiko@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/01/07 23:20:33 by ddraco            #+#    #+#              #
#    Updated: 2021/06/21 17:56:25 by efumiko          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SRCS			= main.cpp server/server.cpp server/AllServers.cpp\
					response/Response.cpp request/Request.cpp Config/Parcer.cpp response/utils.cpp
OBJS			= $(SRCS:.cpp=.o)

CXX				= clang++
RM				= rm -f
#CXXFLAGS		= -Wall -Wextra -Werror -std=c++98

NAME			= webserv

all:			$(NAME)

$(NAME):		$(OBJS)
				$(CXX) -o $(NAME) $(OBJS)

clean:
				$(RM) $(OBJS)

fclean:			clean
				$(RM) $(NAME)

re:				fclean $(NAME)

.PHONY:			all clean fclean re test