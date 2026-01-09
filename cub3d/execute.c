/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: didris <didris@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/13 14:42:27 by didris            #+#    #+#             */
/*   Updated: 2025/12/15 18:26:21 by didris           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void load_texture(t_game_data *data, t_tex *tex, char *path)
{
    tex->image.img = mlx_xpm_file_to_image(data->mlx, path, 
                    &tex->width, 
                    &tex->height);
    if (!tex->image.img)
    {
        printf("[ERROR] FAILED TO LOAD TEXTURE: %s\n", path);
        //FREE DATA?
        exit(1);
    }
    tex->image.addr = mlx_get_data_addr(
                    tex->image.img,
                    &tex->image.bits_per_pixel,
                    &tex->image.line_length,
                    &tex->image.endian);
}   

int close_window(t_game_data *data)
{
    //TO DO: FREE STUFF
    // get_next_line(42);s
    
     free_2d_array(data->map.map);
    free_textures(data);
    get_next_line(42);
    mlx_destroy_window(data->mlx, data->win);
    exit(0);
}

void    put_pixel(t_image *img, int x, int y, int color)
{
    char    *dst;

    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT)
        return;

    dst = img->addr + (y * img->line_length + x * (img->bits_per_pixel / 8));
    *(unsigned int *)dst = color;
}

void render_game(t_game_data *data)
{
    t_image img;
    
    //INITIALIZE NEW IMAGE TO WRITE PIXELS
    img.img = mlx_new_image(data->mlx, SCREEN_WIDTH, SCREEN_HEIGHT);
    img.addr = mlx_get_data_addr(img.img, &img.bits_per_pixel, &img.line_length, &img.endian);

    //START RAYCASTING
    //CAST A RAY FOR EVERY PIXEL OF SCREEN WIDTH
    int x = 0;
    while (x < SCREEN_WIDTH)
    {
        //GET RAY POSITION AND DIRECTION
        //Camera x represents how left or right the ray direction is
        data->raycast.camera_x = 2 * (x / (double)SCREEN_WIDTH) - 1;
        //CALCULATE ACTUAL RAY DIRECTION X & Y
        data->raycast.dir_x = data->player.dir_x + (data->player.plane_x * data->raycast.camera_x);
        data->raycast.dir_y = data->player.dir_y + (data->player.plane_y * data->raycast.camera_x);
        
        //PERFORMING RAYCASTING
        //SET WHICH BOX OF MAP WE ARE IN
        data->raycast.map_x = (int)data->player.x;
        data->raycast.map_y = (int)data->player.y;

        //SETTING HOW FAR RAY TRAVELS TO CROSS ONE GRID SQUARE
        data->raycast.delta_dis_x = 1e30;
        if (data->raycast.dir_x != 0)
            data->raycast.delta_dis_x = fabs(1.0 / data->raycast.dir_x);
        data->raycast.delta_dis_y = 1e30;
        if (data->raycast.dir_y != 0)
            data->raycast.delta_dis_y = fabs(1.0 / data->raycast.dir_y);

        //INITIALIZE STEP X, STEP Y, SIDE DIST X, SIDE DIST Y
        if (data->raycast.dir_x < 0) // RAY GOING LEFT
        {
            data->raycast.step_x = -1;
            //REPRESENTS RAY DISTANCE CURR POSITION TO NEXT X GRID LINE
            data->raycast.side_dis_x = (data->player.x - data->raycast.map_x) * data->raycast.delta_dis_x;
        }
        else // RAY GOING RIGHT
        {
            data->raycast.step_x = 1;
            data->raycast.side_dis_x = (data->raycast.map_x + 1.0 - data->player.x) * data->raycast.delta_dis_x;
        }   
        if (data->raycast.dir_y < 0) //RAY GOING UP 
        {
            data->raycast.step_y = -1;
            //REPRESENTS RAY DISTANCE CURR POSITION TO NEXT X GRID LINE
            data->raycast.side_dis_y = (data->player.y - data->raycast.map_y) * data->raycast.delta_dis_y;
        }
        else // RAY GOING DOWN
        {
            data->raycast.step_y = 1;
            data->raycast.side_dis_y = (data->raycast.map_y + 1.0 - data->player.y) * data->raycast.delta_dis_y;
        }

        //SET HIT TO 0 INITIALLY MEANING NOTHING HIT
        data->raycast.hit = 0;

        //DDA LOOP
        while (data->raycast.hit == 0)
        {
            //CHECK WHICH GRID LINE IS CLOSER
            //JUMP TO NEXT SQUARE IN MAP IN X DIRECTION
            if (data->raycast.side_dis_x < data->raycast.side_dis_y)
            {
                data->raycast.side_dis_x += data->raycast.delta_dis_x; //increment side dist x
                data->raycast.map_x  += data->raycast.step_x;
                data->raycast.side = 0; //HIT EAST/WEST WALL
            }
            //JUMP TO NEXT SQUARE IN MAP IN Y DIRECTION
            else
            {
                data->raycast.side_dis_y += data->raycast.delta_dis_y; //increment side distance y
                data->raycast.map_y  += data->raycast.step_y;
                data->raycast.side = 1; //HIT NORTH/SOUTH WALL
            }
            //Check if ray has hit a wall
            if (data->map.map[data->raycast.map_y][data->raycast.map_x] == '1') 
                data->raycast.hit = 1;
        }

        //CALCULATE DISTANCE FROM RAY TO WALL (FROM CAMERA PLANE)
        double perpWallDist;
        if (data->raycast.side == 0) 
            perpWallDist = (data->raycast.side_dis_x - data->raycast.delta_dis_x);
        else          
            perpWallDist = (data->raycast.side_dis_y - data->raycast.delta_dis_y);

        // printf("x=%d, player=(%.2f,%.2f), map=(%d,%d), perpWallDist=%.3f, side=%d\n",
        // x, data->player.x, data->player.y,
        // data->raycast.map_x, data->raycast.map_y,
        // perpWallDist, data->raycast.side);

        //CALCULATE LINE HEIGHT & DRAW POSITIONS
        int line_h;
        int drawStart;
        int drawEnd;

        //HOW TALL IS WALL?
        line_h = (int)(SCREEN_HEIGHT / perpWallDist);

        //CENTER WALL VERTICALLY ON SCREEN
        drawStart = -line_h / 2 + SCREEN_HEIGHT / 2;
        drawEnd   = line_h / 2 + SCREEN_HEIGHT / 2;

        //CLAMP TO SCREEN
        if (drawStart < 0) 
            drawStart = 0;
        if (drawEnd >= SCREEN_HEIGHT) 
            drawEnd = SCREEN_HEIGHT - 1;

        //DRAWING WALL TEXTURES
        //WHICH PART OF THE WALL DID THE RAY HIT?
        //WHICH VERTICAL SLICE OF TEXTURE TO USE
        double wall_x;      //BETWEEN 0.0 AND 1.0
        if (data->raycast.side == 0)
            wall_x = data->player.y + perpWallDist * data->raycast.dir_y;
        else    
            wall_x = data->player.x + perpWallDist * data->raycast.dir_x;
        //TO MAKE IT START FROM 0
        wall_x -= floor(wall_x);

        //DETERMINE WHICH TEXTURE IT IS:
        t_tex *tex;

        if (data->raycast.side == 0)
        {
            if (data->raycast.dir_x > 0)
                tex = &data->west;
            else
                tex = &data->east;
        }
        else
        {
            if (data->raycast.dir_y > 0)
                tex = &data->north;
            else
                tex = &data->south;
        }

        //TEXTURE X COORDINATE
        //EACH RAY PICKS DIFF TEXTURE COLUMN
        int tex_x; //x coordinate of texture based on wall hit
        tex_x = (int)(wall_x * tex->width);
        //FLIP TEXTURE COLUMN WHEN NEEDED (MIRRORING)
        if (data->raycast.side == 0 && data->raycast.dir_x > 0)
            tex_x = tex->width - tex_x - 1;
        if (data->raycast.side == 1 && data->raycast.dir_y < 0)
            tex_x = tex->width - tex_x - 1;

        //DRAW TEXTURED WALL COLUMN
        double step = 1.0 * tex->height / line_h; //How muh texture height corresponds to one pixel
        //align text top with wall top
        //Which vertical pixel of the texture corresponds to the first wall pixel on screen
        //we want tex_y = 0 at draw start
        //double tex_pos = (drawStart - SCREEN_HEIGHT / 2 + line_h / 2) * step;
        double tex_pos = 0;
        int y;
        y = drawStart;
            
        while (y <= drawEnd)
        {
            int tex_y = (int)tex_pos % tex->height;
            tex_pos += step;

            //get color at specific texture point
            char *src = tex->image.addr +
                (tex_y * tex->image.line_length +
                tex_x * (tex->image.bits_per_pixel / 8));

            int color = *(unsigned int *)src;
            put_pixel(&img, x, y, color);
            y++;
        }

        // ceiling
        int color_c;
        color_c = (data->ceiling.r << 16) | (data->ceiling.g << 8) | data->ceiling.b;
        y = 0;
        while (y < drawStart)
        {
            put_pixel(&img, x, y, color_c);
            y++;
        }

        // floor
        int color_f;
        color_f = (data->floor.r << 16) | (data->floor.g << 8) | data->floor.b;
        y = drawEnd + 1;
        while (y < SCREEN_HEIGHT)
        {
            put_pixel(&img, x, y, color_f);
            y++;
        }

        x++; //INCREMENT WHILE LOOP
    }
    
    // SEND COMPLETE IMAGE TO WINDOW AND DISPLAY IT
    mlx_put_image_to_window(data->mlx, data->win, img.img, 0, 0);

    //DESTROY OLD IMAGE TO PREVENT MEM LEEK :)
    mlx_destroy_image(data->mlx, img.img);
}

void move_player(t_game_data *data, double move_x, double move_y)
{
    double new_x;
    double new_y;
    
    // Calculate new position
    new_x = data->player.x + move_x;
    new_y = data->player.y + move_y;
    
    // Check if new position is valid (not a wall)
    if (data->map.map[(int)(new_y)][(int)(new_x)] != '1')
    {
        data->player.x = new_x;
        data->player.y = new_y;
    }
}

void rotate_player(t_game_data *data, int direction)
{
    double speed;
    double old_dir_x;
    double old_plane_x;

    speed = ROTATE_SPEED * direction;

    //ROTATE DIRECTION
    old_dir_x = data->player.dir_x;
    data->player.dir_x =  data->player.dir_x * cos(speed) - data->player.dir_y * sin(speed);
    data->player.dir_y =  old_dir_x * sin(speed) + data->player.dir_y * cos(speed);

    //ROTATE PLANE
    old_plane_x = data->player.plane_x;
    data->player.plane_x = data->player.plane_x * cos(speed) - data->player.plane_y * sin(speed);
    data->player.plane_y = old_plane_x * sin(speed) + data->player.plane_y * cos(speed);
}

int key_press(int keycode, t_game_data *data)
{
    if (keycode == 65307) //ESC
        close_window(data);
    if (keycode == 119) //W
        move_player(data, data->player.dir_x * SPEED, data->player.dir_y * SPEED);
    else if (keycode == 97) //A
        move_player(data, -data->player.plane_x * SPEED, -data->player.plane_y * SPEED);
    else if (keycode == 115) //S
        move_player(data, -data->player.dir_x * SPEED, -data->player.dir_y * SPEED);
    else if (keycode == 100) //D
        move_player(data, data->player.plane_x * SPEED, data->player.plane_y * SPEED);
    //ROTATION
    else if (keycode == 65361) //LEFT ARROW
        rotate_player(data, -1);
    else if (keycode == 65363) //RIGHT ARROW 
        rotate_player(data, 1);
    //RE RENDER AFTER MOVEMENT
    render_game(data);
    return (0);
}

void set_player_direction(t_game_data *data)
{
    if (data->player.direction == 'N')
    {
        data->player.dir_x = 0.0;
        data->player.dir_y = -1.0;
        data->player.plane_x = 0.66;
        data->player.plane_y = 0.0;
    }
    else if (data->player.direction == 'S')
    {
        data->player.dir_x = 0.0;
        data->player.dir_y = 1.0;
        data->player.plane_x = -0.66;
        data->player.plane_y = 0.0;
    }
    else if (data->player.direction == 'E')
    {
        data->player.dir_x = 1.0;
        data->player.dir_y = 0.0;
        data->player.plane_x = 0.0;
        data->player.plane_y = 0.66;
    }
    else if (data->player.direction == 'W')
    {
        data->player.dir_x = -1.0;
        data->player.dir_y = 0.0;
        data->player.plane_x = 0.0;
        data->player.plane_y = -0.66;
    }
}

void start_game(t_game_data *data)
{
    //INITIALIZE MLX
    data->mlx = mlx_init();
    if (!data->mlx)
    {
        printf("[ERROR] Failed to initialize MLX\n");
        return ;
    }

    //INITIALIZE MLX WINDOW
    data->win = mlx_new_window(data->mlx, SCREEN_WIDTH,  SCREEN_HEIGHT, "AMAZING CubXD");
    if (!data->win)
    {
        printf("[ERROR] Failed to create window\n");
        return ;
    }

    // LOAD WALL TEXTURES HERE ⬇️⬇️⬇️
    load_texture(data, &data->north, data->texture.north_texture);
    load_texture(data, &data->south, data->texture.south_texture);
    load_texture(data, &data->east,  data->texture.east_texture);
    load_texture(data, &data->west,  data->texture.west_texture);

    //INITIALIZE PLAYER DIRECTION AND FOV
    set_player_direction(data);

    //RENDER GAME
    render_game(data);

    //MLX HOOKS
    mlx_hook(data->win, 2, 1L<<0, key_press, data);
    mlx_hook(data->win, 17, 0, close_window, data);

    //MLX LOOP
    mlx_loop(data->mlx);
}
