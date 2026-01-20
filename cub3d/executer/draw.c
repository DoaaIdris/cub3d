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

#include "../cub3d.h"

void	set_ver_line_height(t_raycast *ray, t_wall_line *ver_line)
{
	if (ray->side == 0)
		ray->perp_w_dist = (ray->side_dis_x - ray->delta_dis_x);
	else
		ray->perp_w_dist = (ray->side_dis_y - ray->delta_dis_y);
	ver_line->line_h = (int)(SCREEN_HEIGHT / ray->perp_w_dist);
	ver_line->draw_start = -ver_line->line_h / 2 + SCREEN_HEIGHT / 2;
	ver_line->draw_end = ver_line->line_h / 2 + SCREEN_HEIGHT / 2;
	if (ver_line->draw_start < 0)
		ver_line->draw_start = 0;
	if (ver_line->draw_end >= SCREEN_HEIGHT)
		ver_line->draw_end = SCREEN_HEIGHT - 1;
}

void	draw_wall_tex(t_wall_tex *wall, t_wall_line *line, t_image *img, int x)
{
	double	step;
	int		y;
	char	*src;
	int		color;

	step = 1.0 * wall->tex->height / line->line_h;
	wall->tex_pos = (line->draw_start - SCREEN_HEIGHT / 2 + line->line_h / 2) * step;
	y = line->draw_start;
	while (y <= line->draw_end)
	{
		wall->tex_y = (int)wall->tex_pos % wall->tex->height;
		wall->tex_pos += step;
		src = wall->tex->image.addr
			+ (wall->tex_y * wall->tex->image.line_length
				+ wall->tex_x * (wall->tex->image.bits_per_pixel / 8));
		color = *(unsigned int *)src;
		put_pixel(img, x, y, color);
		y++;
	}
}

void	draw_ceiling_floor(t_game_data *data, t_image *img, int x)
{
	int	y;
	int	ceiling_color;
	int	floor_color;

	ceiling_color = rgb_to_int(data->ceiling);
	floor_color = rgb_to_int(data->floor);
	y = 0;
	while (y < data->raycast.ver_line.draw_start)
		put_pixel(img, x, y++, ceiling_color);
	y = data->raycast.ver_line.draw_end + 1;
	while (y < SCREEN_HEIGHT)
		put_pixel(img, x, y++, floor_color);
}

void	calc_wall_tex(t_game_data *data, t_raycast *ray, t_wall_tex *wall)
{
	if (ray->side == 0)
		wall->wall_x = data->player.y + ray->perp_w_dist * ray->dir_y;
	else
		wall->wall_x = data->player.x + ray->perp_w_dist * ray->dir_x;
	wall->wall_x -= floor(wall->wall_x);
	wall->tex = set_texture(data, &data->raycast);
	wall->tex_x = (int)(wall->wall_x * wall->tex->width);
	if (ray->side == 0 && ray->dir_x > 0)
		wall->tex_x = wall->tex->width - wall->tex_x - 1;
	if (ray->side == 1 && ray->dir_y < 0)
		wall->tex_x = wall->tex->width - wall->tex_x - 1;
}
