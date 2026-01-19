/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: didris <didris@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 22:10:21 by didris            #+#    #+#             */
/*   Updated: 2026/01/19 22:10:21 by didris           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB3D_H
# define CUB3D_H

# include "minilibx-linux/mlx.h"
# include <stdlib.h>
# include <fcntl.h>
# include <stdio.h>
# include "get_next_line/get_next_line.h"
# include <string.h>
# include <math.h>

# define SUCCESS 1
# define FAILURE 0
# define CONTINUE -1
# define SCREEN_WIDTH 1920
# define SCREEN_HEIGHT 1080
# define SPEED 0.10    
# define ROTATE_SPEED 0.10
# define MOUSE_ROTATE_SPEED 0.008
# define MINIMAP_SCALE 10 //1 map cell = 10x10 pixels
# define MINIMAP_X 20
# define MINIMAP_Y 20
# define MINIMAP_P_SIZE 4
# define MINIMAP_DIR_LENGTH 12

typedef struct s_image
{
	void	*img;
	char	*addr; //Pointer tp start of pixel data (our byte array)
	int		bits_per_pixel; //Bits per pixel (typ 32)
	int		line_length; //Bytes per row/line
	//how color is stored in memory (typically 0 in linux/mac = BGRA)
	//least sig byte first (rightmost)
	int		endian;
}	t_image;

typedef struct s_tex
{
	t_image	image;
	int		width;
	int		height;
}	t_tex;

typedef struct s_map
{
	char	**map;
	char	*line;
	int		width;
	int		height;
	int		tile_size;

}	t_map;

typedef struct s_texture_table
{
	char	*key;
	char	**target;
	int		key_len;
}	t_texture_table;

typedef struct s_rgb
{
	unsigned char	r;
	unsigned char	g;
	unsigned char	b;

}	t_rgb;

typedef struct s_texture
{
	char	*north_texture;
	char	*west_texture;
	char	*south_texture;
	char	*east_texture;

}	t_texture;

typedef struct s_player
{
	double	x;
	double	y;
	int		player_count;
	char	direction;
	double	dir_x;
	double	dir_y;
	double	plane_x;//FIELD OF VIEW WIDTH
	double	plane_y;//FILED OF VIEW HEIGHT
}	t_player;

typedef struct s_wall_tex
{
	t_tex	*tex;
	double	wall_x;
	int		tex_x;
}	t_wall_tex;

typedef struct s_wall_line
{
	int	line_h;
	int	draw_start;
	int	draw_end;

}	t_wall_line;

typedef struct s_raycast
{
	double		dir_x;//RAY DIRECTION
	double		dir_y;
	double		camera_x;//REPRESENTS HOW LEFT OR RIGHT RAY IS SHOOTING AT
	int			map_x;//CURRENT SQUARE RAY IS IN (COORDINATES OF SQUARE)
	int			map_y;
	//INITIAL DISTANCE RAY HAS TO TRAVEL TO GO TO FIRST X SIDE
	double		side_dis_x;
	//INITIAL DISTANCE RAY HAS TO TRAVEL TO GO TO FIRST Y SIDE
	double		side_dis_y;
	double		delta_dis_x;//DISTANCE RAY HAS TO TRAVEL TO GO TO NEXT X SIDE
	double		delta_dis_y;//DISTANCE RAY HAS TO TRAVEL TO GO TO NEXT Y SIDE
	int			hit;//DETERMINES IF LOOP ENDS (WAS WALL HIT?)
	int			side;//WAS THE WALL HIT FROM X SIDE (EW = 0) or Y SIDE (NS = 1)
	int			step_x;//DIRECTION TO STEP IN X DIRECTION
	int			step_y;//DIRECTION TO STEP IN Y DIRECTION
	double		perp_w_dist;
	t_wall_line	ver_line;

}	t_raycast;

typedef struct s_game_data
{
	void		*mlx;
	void		*win;
	t_map		map;
	t_texture	texture;
	t_rgb		floor;
	t_rgb		ceiling;
	t_player	player;
	t_raycast	raycast;
	int			first_phase_done;
	int			ceiling_set;
	int			floor_set;
	t_tex		north;
	t_tex		south;
	t_tex		east;
	t_tex		west;

}	t_game_data;

void	*parse_game(t_game_data *data, char *map);
char	**parse_map(t_game_data *data, char *map);
char	*find_texture_path(char *map_line);
int		validate_map_info(t_game_data *data, char *map_line);

//free.c
void	free_textures(t_game_data *data);
void	free_2d_array(char **string);

//libft
int		ss_counter(char const *s, char c);
char	*get_c(char const *s, int *i, char c);
char	**ft_split(char const *s, char c);
void	free_split_result(char **result);
size_t	ft_strlcpy(char *dst, const char *src, size_t dstsize);

//map.c
char	**validate_map(t_game_data *data, int fd);
int		char_patrol(char c);

//parsing.c
char	*delete_newlines(char *map_line);

//dfs.c
int		locate_player(t_game_data *data);
int		is_player(char c);
int		is_walkable(char c);
int		is_out_of_bounds(t_game_data *data, int x, int y);
int		check_neighbor(t_game_data *data, int x, int y);
int		dfs(t_game_data *data, char **visited, int x, int y);
char	**create_visited_map(t_game_data *data);
int		validate_map_with_dfs(t_game_data *data);

//execution.c
void	start_game(t_game_data *data);
void	load_texture(t_game_data *data, t_tex *tex, char *path);
void	put_pixel(t_image *img, int x, int y, int color);
t_tex	*set_texture(t_game_data *data, t_raycast *raycast);
void	raycaster(t_game_data *data, t_image img);
int		key_press(int keycode, t_game_data *dat);
int		close_window(t_game_data *data);
int		mouse_rotate(int x, int y, t_game_data *data);
void	render_game(t_game_data *data);
void	set_ver_line_height(t_raycast *raycast, t_wall_line *ver_line);
void	calc_wall_tex(t_game_data *data, t_raycast *raycast, t_wall_tex *wall);
void	draw_wall_tex(t_wall_tex *wall, t_wall_line *line, t_image *img, int x);
void	draw_ceiling_floor(t_game_data *data, t_image *img, int x);
void	draw_minimap(t_image *img, t_map map, t_player *player);
int		rgb_to_int(t_rgb color);

#endif