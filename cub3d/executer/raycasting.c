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

void dda_loop(t_raycast *raycast, t_map *map)
{
    //SET HIT TO 0 INITIALLY MEANING NOTHING HIT
	raycast->hit = 0;

	//DDA LOOP
	while (raycast->hit == 0)
	{
		//CHECK WHICH GRID LINE IS CLOSER
		//JUMP TO NEXT SQUARE IN MAP IN X DIRECTION
		if (raycast->side_dis_x < raycast->side_dis_y)
		{
			raycast->side_dis_x += raycast->delta_dis_x; //increment side dist x
			raycast->map_x  += raycast->step_x;
			raycast->side = 0; //HIT EAST/WEST WALL
		}
		//JUMP TO NEXT SQUARE IN MAP IN Y DIRECTION
		else
		{
			raycast->side_dis_y += raycast->delta_dis_y; //increment side distance y
			raycast->map_y  += raycast->step_y;
			raycast->side = 1; //HIT NORTH/SOUTH WALL
		}
		//Check if ray has hit a wall
		if (map->map[raycast->map_y][raycast->map_x] == '1') 
			raycast->hit = 1;
	}
}

void init_ray(t_raycast *raycast, t_player *player, int x)
{
    //GET RAY POSITION AND DIRECTION
	//Camera x represents how left or right the ray direction is
	raycast->camera_x = 2 * (x / (double)SCREEN_WIDTH) - 1;
	//CALCULATE ACTUAL RAY DIRECTION X & Y
	raycast->dir_x = player->dir_x + (player->plane_x * raycast->camera_x);
	raycast->dir_y = player->dir_y + (player->plane_y * raycast->camera_x);
	//PERFORMING RAYCASTING
	//SET WHICH BOX OF MAP WE ARE IN
	raycast->map_x = (int)player->x;
	raycast->map_y = (int)player->y;
	//SETTING HOW FAR RAY TRAVELS TO CROSS ONE GRID SQUARE
	raycast->delta_dis_x = 1e30;
	if (raycast->dir_x != 0)
		raycast->delta_dis_x = fabs(1.0 / raycast->dir_x);
	raycast->delta_dis_y = 1e30;
	if (raycast->dir_y != 0)
		raycast->delta_dis_y = fabs(1.0 / raycast->dir_y);
	//INITIALIZE STEP X, STEP Y, SIDE DIST X, SIDE DIST Y
    // RAY GOING RIGHT
	raycast->step_x = 1;
	raycast->side_dis_x = (raycast->map_x + 1.0 - player->x) * raycast->delta_dis_x;
	if (raycast->dir_x < 0) // RAY GOING LEFT
	{
		raycast->step_x = -1;
		//REPRESENTS RAY DISTANCE CURR POSITION TO NEXT X GRID LINE
		raycast->side_dis_x = (player->x - raycast->map_x) * raycast->delta_dis_x;
	}
	// RAY GOING DOWN
	raycast->step_y = 1;
	raycast->side_dis_y = (raycast->map_y + 1.0 - player->y) * raycast->delta_dis_y;
	if (raycast->dir_y < 0) //RAY GOING UP 
	{
		raycast->step_y = -1;
		//REPRESENTS RAY DISTANCE CURR POSITION TO NEXT X GRID LINE
		raycast->side_dis_y = (player->y - raycast->map_y) * raycast->delta_dis_y;
	}
}

void raycaster(t_game_data *data, t_image img)
{
    int x;
    t_wall_tex	wall;

    x = 0;
    while (x < SCREEN_WIDTH)
	{
        init_ray(&data->raycast, &data->player, x);
        dda_loop(&data->raycast, &data->map);

        set_ver_line_height(&data->raycast, &data->raycast.ver_line);
		//DRAWING WALL TEXTURES
		calc_wall_texture(data, &data->raycast, &wall);
        draw_wall_texture(&wall, &data->raycast.ver_line, &img, x);
        draw_ceiling_floor(data, &img, x);
		x++; //INCREMENT WHILE LOOP
	}
}
