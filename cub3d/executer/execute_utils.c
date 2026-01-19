/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: didris <didris@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 16:54:04 by didris            #+#    #+#             */
/*   Updated: 2026/01/14 16:57:36 by didris           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../cub3d.h"

void	load_texture(t_game_data *data, t_tex *tex, char *path)
{
	tex->image.img = mlx_xpm_file_to_image(data->mlx, path,
			&tex->width,
			&tex->height);
	if (!tex->image.img)
	{
		printf("[ERROR] FAILED TO LOAD TEXTURE: %s\n", path);
		free_2d_array(data->map.map);
		free_textures(data);
		get_next_line(42);
		exit(1);
	}
	tex->image.addr = mlx_get_data_addr(
			tex->image.img,
			&tex->image.bits_per_pixel,
			&tex->image.line_length,
			&tex->image.endian);
}

void	put_pixel(t_image *img, int x, int y, int color)
{
	char	*dst;

	if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT)
		return ;
	dst = img->addr + (y * img->line_length + x * (img->bits_per_pixel / 8));
	*(unsigned int *)dst = color;
}

t_tex	*set_texture(t_game_data *data, t_raycast *ray)
{
	t_tex	*tex;

	if (ray->side == 0)
	{
		if (ray->dir_x > 0)
			tex = &data->west;
		else
			tex = &data->east;
	}
	else
	{
		if (ray->dir_y > 0)
			tex = &data->north;
		else
			tex = &data->south;
	}
	return (tex);
}

int	rgb_to_int(t_rgb color)
{
	return ((color.r << 16) | (color.g << 8) | color.b);
}
