/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: didris <didris@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/13 14:42:27 by didris            #+#    #+#             */
/*   Updated: 2026/01/14 16:57:06 by didris           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../cub3d.h"

void	render_game(t_game_data *data)
{
	t_image	img;

	img.img = mlx_new_image(data->mlx, SCREEN_WIDTH, SCREEN_HEIGHT);
	img.addr = mlx_get_data_addr(img.img,
			&img.bits_per_pixel,
			&img.line_length,
			&img.endian);
	raycaster(data, img);
	draw_minimap(&img, data->map, &data->player);
	mlx_put_image_to_window(data->mlx, data->win, img.img, 0, 0);
	mlx_destroy_image(data->mlx, img.img);
}

void	set_player_fov(t_game_data *data)
{
	if (data->player.direction == 'N')
	{
		data->player.plane_x = 0.66;
		data->player.plane_y = 0.0;
	}
	else if (data->player.direction == 'S')
	{
		data->player.plane_x = -0.66;
		data->player.plane_y = 0.0;
	}
	else if (data->player.direction == 'E')
	{
		data->player.plane_x = 0.0;
		data->player.plane_y = 0.66;
	}
	else if (data->player.direction == 'W')
	{
		data->player.plane_x = 0.0;
		data->player.plane_y = -0.66;
	}
}

void	set_player_dir(t_game_data *data)
{
	if (data->player.direction == 'N')
	{
		data->player.dir_x = 0.0;
		data->player.dir_y = -1.0;
	}
	else if (data->player.direction == 'S')
	{
		data->player.dir_x = 0.0;
		data->player.dir_y = 1.0;
	}
	else if (data->player.direction == 'E')
	{
		data->player.dir_x = 1.0;
		data->player.dir_y = 0.0;
	}
	else if (data->player.direction == 'W')
	{
		data->player.dir_x = -1.0;
		data->player.dir_y = 0.0;
	}
}

void	start_game(t_game_data *data)
{
	data->mlx = mlx_init();
	if (!data->mlx)
	{
		printf("[ERROR] Failed to initialize MLX\n");
		return ;
	}
	data->win = mlx_new_window(data->mlx, SCREEN_WIDTH,
			SCREEN_HEIGHT, "AMAZING CubXD");
	if (!data->win)
	{
		printf("[ERROR] Failed to create window\n");
		return ;
	}
	load_texture(data, &data->north, data->texture.north_texture);
	load_texture(data, &data->south, data->texture.south_texture);
	load_texture(data, &data->east, data->texture.east_texture);
	load_texture(data, &data->west, data->texture.west_texture);
	set_player_dir(data);
	set_player_fov(data);
	render_game(data);
	mlx_hook(data->win, 2, 1L << 0, key_press, data);
	mlx_hook(data->win, 17, 0, close_window, data);
	mlx_hook(data->win, 6, 64, mouse_rotate, data);
	mlx_loop(data->mlx);
}
