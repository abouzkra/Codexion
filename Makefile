NAME = codexion
CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread
SRCS = coders/ft_atoi.c	\
	   coders/queue.c	\
	   coders/parser.c	\
	   coders/monitor.c	\
	   coders/dongle.c	\
	   coders/coder.c	\
	   coders/init.c	\
	   coders/utils.c	\
	   coders/sim.c		\
	   coders/main.c
OBJS = $(SRCS:.c=.o)
RM = rm -rf

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c coders/codexion.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
