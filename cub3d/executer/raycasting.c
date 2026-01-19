/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycasting.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: didris <didris@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 16:47:16 by didris            #+#    #+#             */
/*   Updated: 2026/01/14 16:47:17 by didris           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../cub3d.h"

void	dda_loop(t_raycast *ray, t_map *map)
{
	ray->hit = 0;
	while (ray->hit == 0)
	{
		if (ray->side_dis_x < ray->side_dis_y)
		{
			ray->side_dis_x += ray->delta_dis_x;
			ray->map_x += ray->step_x;
			ray->side = 0;
		}
		else
		{
			ray->side_dis_y += ray->delta_dis_y;
			ray->map_y += ray->step_y;
			ray->side = 1;
		}
		if (map->map[ray->map_y][ray->map_x] == '1')
			ray->hit = 1;
	}
}

void	init_ray(t_raycast *ray, t_player *player, int x)
{
	ray->camera_x = 2 * (x / (double)SCREEN_WIDTH) - 1;
	ray->dir_x = player->dir_x + (player->plane_x * ray->camera_x);
	ray->dir_y = player->dir_y + (player->plane_y * ray->camera_x);
	ray->map_x = (int)player->x;
	ray->map_y = (int)player->y;
	ray->delta_dis_x = 1e30;
	if (ray->dir_x != 0)
		ray->delta_dis_x = fabs(1.0 / ray->dir_x);
	ray->delta_dis_y = 1e30;
	if (ray->dir_y != 0)
		ray->delta_dis_y = fabs(1.0 / ray->dir_y);
	ray->step_x = 1;
	ray->side_dis_x = (ray->map_x + 1.0 - player->x) * ray->delta_dis_x;
	if (ray->dir_x < 0)
	{
		ray->step_x = -1;
		ray->side_dis_x = (player->x - ray->map_x) * ray->delta_dis_x;
	}
	ray->step_y = 1;
	ray->side_dis_y = (ray->map_y + 1.0 - player->y) * ray->delta_dis_y;
	if (ray->dir_y < 0)
	{
		ray->step_y = -1;
		ray->side_dis_y = (player->y - ray->map_y) * ray->delta_dis_y;
	}
}

void	raycaster(t_game_data *data, t_image img)
{
	int			x;
	t_wall_tex	wall;

	x = 0;
	while (x < SCREEN_WIDTH)
	{
		init_ray(&data->raycast, &data->player, x);
		dda_loop(&data->raycast, &data->map);
		set_ver_line_height(&data->raycast, &data->raycast.ver_line);
		calc_wall_tex(data, &data->raycast, &wall);
		draw_wall_tex(&wall, &data->raycast.ver_line, &img, x);
		draw_ceiling_floor(data, &img, x);
		x++;
	}
}
