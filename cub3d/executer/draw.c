/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: didris <didris@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 16:47:12 by didris            #+#    #+#             */
/*   Updated: 2026/01/14 16:57:45 by didris           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void set_ver_line_height(t_raycast *raycast, t_wall_line *ver_line)
{
    //CALCULATE DISTANCE FROM RAY TO WALL (FROM CAMERA PLANE)
	if (raycast->side == 0) 
		raycast->perp_wall_dist = (raycast->side_dis_x - raycast->delta_dis_x);
	else          
		raycast->perp_wall_dist = (raycast->side_dis_y - raycast->delta_dis_y);

	//CALCULATE LINE HEIGHT & DRAW POSITIONS
	//HOW TALL IS WALL?
	ver_line->line_h = (int)(SCREEN_HEIGHT / raycast->perp_wall_dist);

	//CENTER WALL VERTICALLY ON SCREEN
	ver_line->drawStart = -ver_line->line_h / 2 + SCREEN_HEIGHT / 2;
	ver_line->drawEnd   = ver_line->line_h / 2 + SCREEN_HEIGHT / 2;

	//CLAMP TO SCREEN
	if (ver_line->drawStart < 0) 
		ver_line->drawStart = 0;
	if (ver_line->drawEnd >= SCREEN_HEIGHT) 
		ver_line->drawEnd = SCREEN_HEIGHT - 1;

}

void draw_wall_texture(t_wall_tex *wall, t_wall_line *line, t_image *img, int x)
{
    //DRAW TEXTURED WALL COLUMN
		double step = 1.0 * wall->tex->height / line->line_h; //How muh texture height corresponds to one pixel
		//align text top with wall top
		//Which vertical pixel of the texture corresponds to the first wall pixel on screen
		//we want tex_y = 0 at draw start
		//double tex_pos = (drawStart - SCREEN_HEIGHT / 2 + line_h / 2) * step;
		double tex_pos = 0;
		int y;
		y = line->drawStart;
			
		while (y <= line->drawEnd)
		{
			int tex_y = (int)tex_pos % wall->tex->height;
			tex_pos += step;
			//get color at specific texture point
			char *src = wall->tex->image.addr +
				(tex_y * wall->tex->image.line_length +
				wall->tex_x * (wall->tex->image.bits_per_pixel / 8));

			int color = *(unsigned int *)src;
			put_pixel(img, x, y, color);
			y++;
		}
}

void draw_celing_floor(t_game_data *data, t_image *img, int x)
{
    // ceiling
        int y;
		int color;
		color = (data->ceiling.r << 16) | (data->ceiling.g << 8) | data->ceiling.b;
		y = 0;
		while (y < data->raycast.vertical_line.drawStart)
		{
			put_pixel(&img, x, y, color);
			y++;
		}
		// floor
		color = (data->floor.r << 16) | (data->floor.g << 8) | data->floor.b;
		y = data->raycast.vertical_line.drawEnd + 1;
		while (y < SCREEN_HEIGHT)
		{
			put_pixel(&img, x, y, color);
			y++;
		}
}

void calc_wall_texture(t_game_data *data, t_raycast *raycast, t_wall_tex *wall)
{
    //WHICH PART OF THE WALL DID THE RAY HIT?
		//WHICH VERTICAL SLICE OF TEXTURE TO USE
		//BETWEEN 0.0 AND 1.0 (wall x)
		if (raycast->side == 0)
			wall->wall_x = data->player.y + raycast->perp_wall_dist * raycast->dir_y;
		else    
			wall->wall_x = data->player.x + raycast->perp_wall_dist * raycast->dir_x;
		//TO MAKE IT START FROM 0
		wall->wall_x -= floor(wall->wall_x);

		//DETERMINE WHICH TEXTURE IT IS:
        wall->tex = set_texture(data, &data->raycast);

        //TEXTURE X COORDINATE
		//EACH RAY PICKS DIFF TEXTURE COLUMN
		//x coordinate of texture based on wall hit
		wall->tex_x = (int)(wall->wall_x *  wall->tex->width);
		//FLIP TEXTURE COLUMN WHEN NEEDED (MIRRORING)
		if (raycast->side == 0 && raycast->dir_x > 0)
			 wall->tex_x =  wall->tex->width -  wall->tex_x - 1;
		if (raycast->side == 1 && raycast->dir_y < 0)
			 wall->tex_x =  wall->tex->width -  wall->tex_x - 1;
}
