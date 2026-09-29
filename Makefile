NAME		= codexion

CC			= cc
CFLAGS		= -Wall -Wextra -Werror -pthread -Isrc/include

SRCS		= src/main.c \
			  src/parsing/parse_args.c \
			  src/parsing/init_simulation.c \
			  src/scheduler/queue_utils.c \
			  src/scheduler/heap.c \
			  src/scheduler/heap_internal.c \
			  src/simulation/dongle_manager.c \
			  src/simulation/coder_routine.c \
			  src/simulation/monitor.c \
			  src/utils/time.c \
			  src/utils/logger.c \
			  src/utils/cleanup.c

OBJS		= $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

# Regra com ThreadSanitizer para validar data races e deadlocks
tsan: CFLAGS += -fsanitize=thread -g
tsan: re

.PHONY: all clean fclean re tsan