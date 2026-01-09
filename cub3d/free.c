#include "cub3d.h"

void free_textures(t_game_data *data)
{
    if (data->texture.north_texture)
        // mlx_destroy_image(data->mlx, data->texture.north_texture);
        free(data->texture.north_texture);
    if (data->texture.south_texture)
        free(data->texture.south_texture);
    if (data->texture.west_texture)
        free(data->texture.west_texture);
    if (data->texture.east_texture)
        free(data->texture.east_texture);
}

void free_2d_array(char **string)
{
    int i;

    i = 0;
    if (!string)
        return;
    while(string[i])
    {   
        free(string[i]);
        i++;
    }
    free (string);
}