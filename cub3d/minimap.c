/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minimap.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: didris <didris@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 19:06:45 by didris            #+#    #+#             */
/*   Updated: 2026/01/19 19:06:45 by didris           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	draw_square(t_image *img, int x, int y, int flag)
{
	int	i;
	int	j;
	int	size;
	int	color;

	i = 0;
	size = MINIMAP_SCALE;
	color = 0x00FFFFFF;
	if (flag == 0)
		color = 0x00000000;
	if (flag == 2)
	{
		size = MINIMAP_P_SIZE;
		color = 0x00FF0000;
	}
	while (i < size)
	{
		j = 0;
		while (j < size)
		{
			put_pixel(img, x + j, y + i, color);
			j++;
		}
		i++;
	}
}

void	draw_player_dir(t_image *img, t_player *player)
{
	int		i;
	double	x;
	double	y;

	i = 0;
	while (i < MINIMAP_DIR_LENGTH)
	{
		x = player->x + player->dir_x * (double)i / 4;
		y = player->y + player->dir_y * (double)i / 4;
		put_pixel(img,
			MINIMAP_X + (int)(x * MINIMAP_SCALE),
			MINIMAP_Y + (int)(y * MINIMAP_SCALE),
			0x0000FF00);
		i++;
	}
}

void	draw_player(t_image *img, t_player *player)
{
	int	mm_px;
	int	mm_py;

	mm_px = MINIMAP_X + (int)(player->x * MINIMAP_SCALE);
	mm_py = MINIMAP_Y + (int)(player->y * MINIMAP_SCALE);
	draw_square(img,
		mm_px - MINIMAP_P_SIZE / 2,
		mm_py - MINIMAP_P_SIZE / 2,
		2);
}

void	draw_map(t_image *img, char **map)
{
	int	x;
	int	y;

	y = 0;
	while (map[y])
	{
		x = 0;
		while (map[y][x])
		{
			if (map[y][x] == '1')
				draw_square(img,
					MINIMAP_X + x * MINIMAP_SCALE,
					MINIMAP_Y + y * MINIMAP_SCALE,
					1);
			else
				draw_square(img,
					MINIMAP_X + x * MINIMAP_SCALE,
					MINIMAP_Y + y * MINIMAP_SCALE,
					0);
			x++;
		}
		y++;
	}
}

void	draw_minimap(t_image *img, t_map map, t_player *player)
{
	draw_map(img, map.map);
	draw_player_dir(img, player);
	draw_player(img, player);
}
