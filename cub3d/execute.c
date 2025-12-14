/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: didris <didris@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/13 14:42:27 by didris            #+#    #+#             */
/*   Updated: 2025/12/13 14:42:27 by didris           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int key_press(int keycode, t_game_data *data)
{
    (void)data;
    (void)keycode;

    return (0);
}

int close_window(t_game_data *data)
{
    mlx_destroy_window(data->mlx, data->win);
    exit(0);
    return (0);
}

void set_player_direction(t_game_data *data)
{
    if (data->player.direction == 'N')
    {
        data->player.dir_x = 0;
        data->player.dir_y = -1;
        data->player.plane_x = 0.66;
        data->player.plane_y = 0;
    }
    else if (data->player.direction == 'S')
    {
        data->player.dir_x = 0;
        data->player.dir_y = 1;
        data->player.plane_x = -0.66;
        data->player.plane_y = 0;
    }
    else if (data->player.direction == 'E')
    {
        data->player.dir_x = 1;
        data->player.dir_y = 0;
        data->player.plane_x = 0;
        data->player.plane_y = 0.66;
    }
    else if (data->player.direction == 'W')
    {
        data->player.dir_x = -1;
        data->player.dir_y = 0;
        data->player.plane_x = 0;
        data->player.plane_y = -0.66;
    }
}

void    put_pixel(t_image *img, int s_w, int s_h, int x, int y, int color)
{
    char    *dst;

    if (x < 0 || x >= s_w || y < 0 || y >= s_h)
        return;

    dst = img->addr + (y * img->line_length + x * (img->bits_per_pixel / 8));
    *(unsigned int *)dst = color;
}


void start_game(t_game_data *data)
{
    int screen_w;
    int screen_h;
    t_image img;

     //INITIALIZE MLX
    data->mlx = mlx_init();
    if (!data->mlx)
    {
        printf("[ERROR] Failed to initialize MLX\n");
        return ;
    }
    //GET SCREEN WIDTH AND HEIGHT
    mlx_get_screen_size(data->mlx, &screen_w, &screen_h);

    //INITIALIZE MLX WINDOW
    data->win = mlx_new_window(data->mlx, screen_w,  screen_h, "AMAZING CubXD");
    if (!data->win)
    {
        printf("[ERROR] Failed to create window\n");
        return ;
    }

    //INITIALIZE IMAGE TO WRITE PIXELS
    img.img = mlx_new_image(data->mlx, screen_w, screen_h);
    img.addr = mlx_get_data_addr(img.img, &img.bits_per_pixel, &img.line_length, &img.endian);

    //INITIALIZE PLAYER DIRECTION AND FOV
    set_player_direction(data);

    //START RAYCASTING
    //CAST A RAY FOR EVERY PIXEL OF SCREEN WIDTH
    int x = 0;
    while (x < screen_w)
    {
        //GET RAY POSITION AND DIRECTION
        data->raycast.camera_x = 2 * x / (double)screen_w - 1;
        data->raycast.dir_x = data->player.dir_x + data->player.plane_x * data->raycast.camera_x;
        data->raycast.dir_y = data->player.dir_y + data->player.plane_y * data->raycast.camera_x;
        
        //PERFORMING RAYCASTING
        //SET WHICH BOX OF MAP WE ARE IN
        data->raycast.map_x = (int)data->player.x;
        data->raycast.map_y = (int)data->player.y;

        //SETTING LENGTH OF RAY FROM ONE X/Y SIDE TO NEXT
        data->raycast.delta_dis_x = 1e30;
        if (data->raycast.dir_x != 0)
            data->raycast.delta_dis_x = fabs(1.0 / data->raycast.dir_x);
        data->raycast.delta_dis_y = 1e30;
        if (data->raycast.dir_y != 0)
            data->raycast.delta_dis_y = fabs(1.0 / data->raycast.dir_y);

        //INITIALIZE STEP X, STEP Y, SIDE DIST X, SIDE DIST Y
        if (data->raycast.dir_x < 0)
        {
            data->raycast.step_x = -1;
            data->raycast.side_dis_x = (data->player.x - data->raycast.map_x) * data->raycast.delta_dis_x;
        }
        else
        {
            data->raycast.step_x = 1;
            data->raycast.side_dis_x = (data->raycast.map_x + 1.0 - data->player.x) * data->raycast.delta_dis_x;
        }   
        if (data->raycast.dir_y < 0)
        {
            data->raycast.step_y = -1;
            data->raycast.side_dis_y = (data->player.y - data->raycast.map_y) * data->raycast.delta_dis_y;
        }
        else
        {
            data->raycast.step_y = 1;
            data->raycast.side_dis_y = (data->raycast.map_y + 1.0 - data->player.y) * data->raycast.delta_dis_y;
        }

        //SET HIT TO 0 INITIALLY MEANING NOTHING HIT
        data->raycast.hit = 0;

        //DDA LOOP
        while (data->raycast.hit == 0)
        {
            //JUMP TO NEXT SQUARE IN MAP IN X DIRECTION
            if (data->raycast.side_dis_x < data->raycast.side_dis_y)
            {
                data->raycast.side_dis_x += data->raycast.delta_dis_x;
                data->raycast.map_x  += data->raycast.step_x;
                data->raycast.side = 0;
            }
            //JUMP TO NEXT SQUARE IN MAP IN Y DIRECTION
            else
            {
                data->raycast.side_dis_y += data->raycast.delta_dis_y;
                data->raycast.map_y  += data->raycast.step_y;
                data->raycast.side = 1;
            }
            //Check if ray has hit a wall
            if (data->map.map[data->raycast.map_y][data->raycast.map_x] > 0) 
                data->raycast.hit = 1;
        }

        //CALCULATE DISTANCE FROM RAY TO WALL (FROM CAMERA PLANE)
        double perpWallDist;
        if (data->raycast.side == 0) 
            perpWallDist = (data->raycast.side_dis_x - data->raycast.delta_dis_x);
        else          
            perpWallDist = (data->raycast.side_dis_y - data->raycast.delta_dis_y);

        printf("x=%d, player=(%.2f,%.2f), map=(%d,%d), perpWallDist=%.3f, side=%d\n",
        x, data->player.x, data->player.y,
        data->raycast.map_x, data->raycast.map_y,
        perpWallDist, data->raycast.side);


        //CALCULATE LINE HEIGHT & DRAW POSITIONS
        int line_h;
        int drawStart;
        int drawEnd;

        line_h = (int)(screen_h / perpWallDist);
        drawStart = -line_h / 2 + screen_h / 2;
        drawEnd   = line_h / 2 + screen_h / 2;

        //CLAMP TO SCREEN
        if (drawStart < 0) 
            drawStart = 0;
        if (drawEnd >= screen_h) 
            drawEnd = screen_h - 1;

        printf("x=%d, drawStart=%d, drawEnd=%d\n", x, drawStart, drawEnd);

        //DRAW VERTICAL LINE
        int y;
        y = drawStart;
        int color_line = (255 << 16) | (0 << 8) | 0;
        while (y <= drawEnd)
        {
            put_pixel(&img, screen_w, screen_h, x, y, color_line);
            y++;
        }

        // ceiling
        int color_c = (data->ceiling.r << 16) | (data->ceiling.g << 8) | data->ceiling.b;
        y = 0;
        while (y < drawStart)
        {
            put_pixel(&img, screen_w, screen_h, x, y, color_c);
            y++;
        }

        // floor
        int color_f = (data->floor.r << 16) | (data->floor.g << 8) | data->floor.b;
        y = drawEnd + 1;
        while (y < screen_h)
        {
            put_pixel(&img, screen_w, screen_h, x, y, color_f);
            y++;
        }

        x++; //INCREMENT WHILE LOOP
    }

    // SEND IMAGE TO WINDOW
    mlx_put_image_to_window(data->mlx, data->win, img.img, 0, 0);

    //MLX HOOKS
    mlx_hook(data->win, 2, 1L<<0, key_press, data);
    mlx_hook(data->win, 17, 0, close_window, data);

    //MLX LOOP
    mlx_loop(data->mlx);
}
