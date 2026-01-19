#include "../cub3d.h"

int char_patrol(char c)
{
    if (ft_strchr("10NSWE ",c) == NULL)
        return (FAILURE);
    return (SUCCESS);
}

char	**update_map(char **map, char *line, int size)
{
	char	**new_map;
	int		i;

	i = 0;
	new_map = malloc(sizeof(char *) * (size + 1));
	if (!new_map)
		return (NULL);
	if (map)
	{
		while (i < size - 1)
		{
			new_map[i] = map[i];
			i++;
		}
	}
	new_map[i++] = line;
	new_map[i] = NULL;
	if (map)
		free(map);
	
	return (new_map);
}

char **validate_map(t_game_data *data, int fd)
{
    int i;
    int max_i = 0;
    int line_count = 0;
    char *original_line;
    char *cleaned_line;

    while (data->map.line)
    {   
        i = 0;
        original_line = data->map.line; 
        cleaned_line = delete_newlines(original_line);
        if (cleaned_line[0] == '\0')
        {
            printf("[ERROR] hole spotted in the map!\n");
            free(original_line);
            if (cleaned_line)
                free(cleaned_line);
            return (NULL);
        }
        while (cleaned_line[i])
        {     
            if (char_patrol(cleaned_line[i]) == FAILURE)
            {
                printf("incorrect character spotted\n");
                free(original_line);
                free(cleaned_line);
                return (NULL);
            }
            i++;
        }
        if (i > max_i)
            max_i = i;
        line_count++;
        data->map.map = update_map(data->map.map, strdup(cleaned_line), line_count);
        if (!data->map.map)
        {
            free(original_line);
            free(cleaned_line);
            return (NULL);
        }
        free(original_line);
        free(cleaned_line);
        data->map.line = get_next_line(fd);
    }
    data->map.height = line_count;
    data->map.width = max_i;
    //printf("the maximum length is........[%d]\n",data->map.width);
    return (data->map.map);
}
