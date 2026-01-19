#include "../cub3d.h"

int locate_player(t_game_data *data)
{
    int x;
    int y = 0;

    while (data->map.map[y])
    {
        x = 0;
        while (data->map.map[y][x])
        {   
            if (data->map.map[y][x] == 'N' || data->map.map[y][x] == 'E' || 
                data->map.map[y][x] == 'W' || data->map.map[y][x] == 'S')
            {   
                data->player.x = x + 0.5;
                data->player.y = y + 0.5;
                data->player.direction = data->map.map[y][x];
                data->player.player_count++;
            }
            x++;
        }
        y++;
    }
    if (data->player.player_count != 1)
        return (printf("[ERROR] INVALID PLAYER COUNT: %d\n", data->player.player_count), FAILURE);
    return (SUCCESS);
}

int is_player(char c)
{
    return (c == 'N' || c == 'S' || c == 'E' || c == 'W');
}

int is_walkable(char c)
{
    return (c == '0' || is_player(c));
}

int is_out_of_bounds(t_game_data *data, int x, int y)
{
    if (y < 0 || y >= data->map.height)
        return (1);
    if (x < 0 || x >= data->map.width)
        return (1);
    return (0);
}

int check_neighbor(t_game_data *data, int x, int y)
{
    char c;

    if (is_out_of_bounds(data, x, y))
        return (FAILURE);
    c = data->map.map[y][x];
    if (c == '1')
        return (SUCCESS);
    return (CONTINUE);
}

int dfs(t_game_data *data, char **visited, int x, int y)
{
    int directions[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    int i;
    int new_x;
    int new_y;
    int check;

    visited[y][x] = '1';
    i = 0;
    while (i < 4)
    {
        new_x = x + directions[i][0];
        new_y = y + directions[i][1];
        check = check_neighbor(data, new_x, new_y);
        if (check == FAILURE)
        {
            printf("[ERROR] Map not enclosed at position (%d, %d)\n", new_x, new_y);
            return (FAILURE);
        }
        if (check == CONTINUE && visited[new_y][new_x] == '0')
        {
            if (dfs(data, visited, new_x, new_y) == FAILURE)
                return (FAILURE);
        }
        i++;
    }
    return (SUCCESS);
}

char **create_visited_map(t_game_data *data)
{
    char **visited;
    int i = 0;

    visited = malloc(sizeof(char *) * (data->map.height + 1));
    if (!visited)
        return (NULL);
    while (i < data->map.height)
    {
        visited[i] = calloc(data->map.width + 1, sizeof(char));
        if (!visited[i])
        {
            free_2d_array(visited);
            return (NULL);
        }
        i++;
    }
    visited[i] = NULL;
    return (visited);
}

int validate_map_with_dfs(t_game_data *data)
{
    char **visited;
    int result;

    if (locate_player(data) == FAILURE)
        return (FAILURE);
    visited = create_visited_map(data);
    if (!visited)
    {
        printf("[ERROR] Failed to allocate visited map\n");
        return (FAILURE);
    }
    //printf("Starting DFS validation from player position (%f, %f)...\n", 
    //       data->player.x, data->player.y);
    result = dfs(data, visited, data->player.x, data->player.y);
    free_2d_array(visited);
    // if (result == SUCCESS)
    //     printf("✓ Map validation successful - all walkable areas are enclosed!\n");
    // else
    //     printf("✗ Map validation failed - map is not properly enclosed\n");
    return (result);
}