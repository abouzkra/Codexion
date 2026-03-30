NAME = codexion
CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread -fsanitize=thread
SRCS = coders/utils.c	\
	   coders/parser.c	\
	   coders/init.c 	\
	   coders/monitor.c \
	   coders/queue.c   \
	   coders/logger.c  \
	   coders/coder.c   \
	   coders/sim.c     \
	   coders/dongle.c  \
	   coders/cleanup.c	\
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

coders	   = 5
t_burnout  = 800
t_compile  = 200
t_debug	   = 200
t_refactor = 200
n_compiles = 7
cooldown   = 100
scheduler  = fifo
test: $(NAME)
	./$(NAME) $(coders) $(t_burnout) $(t_compile) $(t_debug) $(t_refactor) $(n_compiles) $(cooldown) $(scheduler)

val: $(NAME)
	valgrind ./$(NAME) $(coders) $(t_burnout) $(t_compile) $(t_debug) $(t_refactor) $(n_compiles) $(cooldown) $(scheduler)

.PHONY: all clean fclean re
