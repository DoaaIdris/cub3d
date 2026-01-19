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

void	draw_square(t_image *img, int x, int y, int size, int color)
{
	int	i;
	int	j;

	i = 0;
	while (i < size) //draw 10 x 10 pixel square
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

void draw_player_dir(t_image *img, t_player *player)
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
			MINIMAP_x + (int)(x * MINIMAP_SCALE),
			MINIMAP_y + (int)(y * MINIMAP_SCALE),
			0x0000FF00);
		i++;
	}
}

void	draw_player(t_image *img, t_player *player)
{
	int	mm_px;
	int	mm_py;

	mm_px = MINIMAP_x + (int)(player->x * MINIMAP_SCALE);
	mm_py = MINIMAP_y + (int)(player->y * MINIMAP_SCALE);
	draw_square(img,
		mm_px - MINIMAP_P_SIZE / 2,
		mm_py - MINIMAP_P_SIZE / 2,
		MINIMAP_P_SIZE,
		0x00FF0000);
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
					MINIMAP_x + x * MINIMAP_SCALE,
					MINIMAP_y + y * MINIMAP_SCALE,
					MINIMAP_SCALE,
					0x00FFFFFF); // wall
			else
				draw_square(img,
					MINIMAP_x + x * MINIMAP_SCALE,
					MINIMAP_y + y * MINIMAP_SCALE,
					MINIMAP_SCALE,
					0x00000000); // empty space
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
