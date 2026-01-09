#include "cub3d.h"

void patch_map(t_game_data *data)
{
    char **padded_map;
    int i;
    int j;
    int current_len;

    i = 0;
    padded_map = malloc(sizeof(char *) * (data->map.height + 1));
    if (!padded_map)
        return;
    while (i < data->map.height)
    {
        current_len = strlen(data->map.map[i]);
        padded_map[i] = malloc(sizeof(char) * (data->map.width + 1));
        if (!padded_map[i])
        {
            free_2d_array(padded_map);
            return;
        }
        j = 0;
        while (j < current_len)
        {   
            if (data->map.map[i][j] == ' ')
                padded_map[i][j] = 'X';
            else
                padded_map[i][j] = data->map.map[i][j];
            j++;
        }
        while (j < data->map.width)
        {
            padded_map[i][j] = 'X';
            j++;
        }
        padded_map[i][j] = '\0';
        i++;
    }
    padded_map[i] = NULL;
    free_2d_array(data->map.map);
    data->map.map = padded_map;
}


int main(int argc, char *argv[])
{
    t_game_data data;

    data = (t_game_data){0};
    if (argc == 2)
    {
        parse_map(&data, argv[1]);
        patch_map(&data);
        
        if (validate_map_with_dfs(&data) == FAILURE)
        {
            printf("[ERROR] Map validation failed\n");
            free_2d_array(data.map.map);
            free_textures(&data);
            get_next_line(42);
            return (1);
        }
        
        printf("✓ Map validation successful! Starting game...\n");
        start_game(&data);
        
        free_2d_array(data.map.map);
        free_textures(&data);
        get_next_line(42);
    }
    else
    {
        printf("Usage: %s <map_file.cub>\n", argv[0]);
        return (1);
    }
    
    return (0);
}
