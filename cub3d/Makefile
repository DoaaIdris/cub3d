NAME = cub3D

CC = cc
CFLAGS = -Wall -Wextra -Werror -Iminilibx-linux -O3

SRCS =  main.c	   \
		parser/parsing.c	\
		free.c		\
		ft_split.c	\
		parser/map.c		\
		parser/dfs.c		\
		executer/draw.c	\
		executer/execute.c	\
		executer/execute_utils.c	\
		executer/game.c	\
		executer/raycasting.c\
		minimap.c	\
		
GET_NEXT_LINE_SRC = get_next_line/get_next_line.c get_next_line/get_next_line_utils.c

OBJS = $(SRCS:.c=.o)
GET_NEXT_LINE_OBJ = $(GET_NEXT_LINE_SRC:.c=.o)

MLX_DIR = minilibx-linux
MLX = $(MLX_DIR)/libmlx_Linux.a
LDFLAGS = $(MLX) -L/usr/lib -lXext -lX11 -lm -lz

all: $(NAME)

$(NAME): $(OBJS) $(GET_NEXT_LINE_OBJ)
	$(CC) $(OBJS) $(GET_NEXT_LINE_OBJ) $(LDFLAGS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(GET_NEXT_LINE_OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
