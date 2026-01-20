/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: didris <didris@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 16:47:45 by didris            #+#    #+#             */
/*   Updated: 2026/01/14 16:56:41 by didris           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../cub3d.h"

int	close_window(t_game_data *data)
{
	free_textures(data);
	if (data->win)
		mlx_destroy_window(data->mlx, data->win);
	if (data->mlx)
	{
		mlx_destroy_display(data->mlx);
		free(data->mlx);
	}
	free_2d_array(data->map.map);
	get_next_line(42);
	exit(0);
}

void	rotate_player(t_player *player, double angle)
{
	double	old_dir_x;
	double	old_plane_x;

	old_dir_x = player->dir_x;
	player->dir_x = player->dir_x * cos(angle) - player->dir_y * sin(angle);
	player->dir_y = old_dir_x * sin(angle) + player->dir_y * cos(angle);
	old_plane_x = player->plane_x;
	player->plane_x = player->plane_x * cos(angle)
		- player->plane_y * sin(angle);
	player->plane_y = old_plane_x * sin(angle) + player->plane_y * cos(angle);
}

int	mouse_rotate(int x, int y, t_game_data *data)
{
	int			delta_x;
	static int	last_x;

	(void)y;
	if (last_x == 0)
	{
		last_x = x;
		return (0);
	}
	delta_x = x - last_x;
	last_x = x;
	rotate_player(&data->player, delta_x * MOUSE_ROTATE_SPEED);
	render_game(data);
	return (0);
}

void	move_player(t_game_data *data, double move_x, double move_y)
{
	double	new_x;
	double	new_y;

	new_x = data->player.x + move_x;
	new_y = data->player.y + move_y;
	if (data->map.map[(int)(new_y)][(int)(new_x)] != '1')
	{
		data->player.x = new_x;
		data->player.y = new_y;
	}
}

int	key_press(int keycode, t_game_data *data)
{
	t_player	*player;

	player = &data->player;
	if (keycode == 65307)
		close_window(data);
	if (keycode == 119)
		move_player(data, player->dir_x * SPEED, player->dir_y * SPEED);
	else if (keycode == 97)
		move_player(data, -player->plane_x * SPEED, -player->plane_y * SPEED);
	else if (keycode == 115)
		move_player(data, -player->dir_x * SPEED, -player->dir_y * SPEED);
	else if (keycode == 100)
		move_player(data, player->plane_x * SPEED, player->plane_y * SPEED);
	else if (keycode == 65361)
		rotate_player(player, ROTATE_SPEED * -1);
	else if (keycode == 65363)
		rotate_player(player, ROTATE_SPEED * 1);
	render_game(data);
	return (0);
}
