NAME = codexion
CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread
SRCS = coders/queue.c	\
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

val:
	valgrind --tool=helgrind --max-threads=3000 ./codexion 199 2000 10 10 10 2 10 fifo 2> out

.PHONY: all clean fclean re
