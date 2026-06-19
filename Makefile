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
	$(info $(SRCS))
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c coders/codexion.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

coders	   = 2
t_burnout  = 1400
t_compile  = 100
t_debug	   = 100
t_refactor = 100
n_compiles = 2
cooldown   = 600
scheduler  = edf

test: $(NAME)
	./$(NAME) $(coders) $(t_burnout) $(t_compile) $(t_debug) $(t_refactor) $(n_compiles) $(cooldown) $(scheduler)

.PHONY: all clean fclean re
