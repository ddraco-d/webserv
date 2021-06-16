# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ddraco <marvin@42.fr>                      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/01/07 23:20:33 by ddraco            #+#    #+#              #
#    Updated: 2021/01/07 23:21:32 by ddraco           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SRCS			= main.cpp server.cpp AllServers.cpp
OBJS			= $(SRCS:.cpp=.o)

CXX				= clang++
RM				= rm -f
#CXXFLAGS		= -Wall -Wextra -Werror

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