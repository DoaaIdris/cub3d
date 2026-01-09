#include "cub3d.h"
// the goal of this file is to fetch data from the map and initialize it's info 
// accordingly to it's rightful struct to prep it for execution


// the goal of this function is to read from the map
// validate errors
// and create a functioning struct with a properly initialized variable


// line by line check with the use of getnextline
// closer inspection for tracking NO, SO, WE, EEA and the colors

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*substr;
	size_t	i;
	size_t	s_tlen;

	if (!s)
		return (NULL);
	s_tlen = ft_strlen(s);
	if (start >= s_tlen)
		return (ft_strdup(""));
	if (len > s_tlen - start)
		len = s_tlen - start;
	substr = (char *)malloc(sizeof(char) * (len + 1));
	if (!substr)
		return (NULL);
	i = 0;
	while (i < len)
	{
		substr[i] = s[start + i];
		i++;
	}
	substr[i] = '\0';
	return (substr);
}

char *find_texture_path(char *map_line)
{
    int i = 0;

    while(map_line[i] && map_line[i] != ' ' && map_line[i] != '\t')
    {
        i++;
    }
    while(map_line[i] && (map_line[i] == ' ' || map_line[i] == '\t'))
    {   
        i++;
    }
    return (&map_line[i]);
}

char **rgb_init(char *map_line, char c)
{
    char *skip_the_character;
    char **rgb_contents;

    int i = 0;

    while(map_line[i] && map_line[i] == c)
    {   
        i++;
    }
    // printf("total_count[%d]\n",i);
    skip_the_character = ft_substr(map_line, i, ft_strlen(map_line) - i);
    //printf("have i successfully skipped the character?[%s]\n",skip_the_character);
   rgb_contents = ft_split(skip_the_character,',');
   free(skip_the_character);

   return (rgb_contents);
}

// check for invalid characters
// check for 
int validate_rgb(char *str,unsigned char *result)
{
    int i = 0;
    int num = 0;
    if (strlen(str) > 4)
        return (printf("bigger than 4\n"),FAILURE);
    while(str[i])
    {
        if (str[i] < '0' || str[i] > '9')
            return (printf("[ERROR] invalid character in rgb SPOTTED!!\n"),(FAILURE));
        i++;
    }
    i = 0;
    while (str[i] >= '0' && str[i] <= '9')
	{
		num = num * 10 + (str[i] - '0');
		if (num > 255)
			return (printf("rgb range overflow/underflow\n"),FAILURE);
		i++;
	}
    *result = (unsigned char)num;
    return (SUCCESS);
}
int  first_phase_done(t_game_data *data)
{   
    if (data->texture.north_texture && data->texture.west_texture && data->texture.south_texture
        && data->texture.east_texture && data->ceiling_set && data->floor_set)
      {
         data->first_phase_done = 1;
         return (SUCCESS);
      }
    return (FAILURE); 
}

int validate_map_info(t_game_data *data, char *map_line)
{   
    t_texture_table textures[] = {
        {"NO", &data->texture.north_texture, 2},
        {"SO", &data->texture.south_texture, 2},
        {"WE", &data->texture.west_texture, 2},
        {"EA", &data->texture.east_texture, 2},
        {NULL, NULL, 0}
    };
    int i;

    i = 0;
    while (textures[i].key)
    {
        if (strncmp(map_line, textures[i].key, textures[i].key_len) == 0 
            && map_line[textures[i].key_len] == ' ')
        {
        if (*textures[i].target)
                return (printf("Error: duplicate %s texture\n", textures[i].key), 0);
            *textures[i].target = strdup(find_texture_path(map_line));
            // printf("Loaded %s texture: [%s]\n", textures[i].key, *textures[i].target);
            return (SUCCESS);
        }
        i++;
    }
    if (strncmp(map_line, "F", 1) == 0 && map_line[1] == ' ')
    {   
        char **init_f_rgb = rgb_init(map_line,'F');

        // ill have to create a function that converts ascii to unsigned char
        if (validate_rgb(init_f_rgb[0],&data->floor.r) == FAILURE 
            || validate_rgb(init_f_rgb[1],&data->floor.g) == FAILURE 
            || validate_rgb(init_f_rgb[2],&data->floor.b) == FAILURE)
        {
            free_2d_array(init_f_rgb);
            return (printf("invalid rgb input\n"),FAILURE);
        }
        // printf("data->floor.b contents[%d]\n",data->floor.b);
        free_2d_array(init_f_rgb);
        // free(temp);
        // split the comma and store the variables accordingly
        data->floor_set = 1;
        return (SUCCESS);
    }
    if (strncmp(map_line, "C", 1) == 0 && map_line[1] == ' ')
    {   
       char **init_c_rgb = rgb_init(map_line,'C');

        if (validate_rgb(init_c_rgb[0],&data->ceiling.r) == FAILURE 
            || validate_rgb(init_c_rgb[1],&data->ceiling.g) == FAILURE 
            || validate_rgb(init_c_rgb[2],&data->ceiling.b) == FAILURE)
            return (free_2d_array(init_c_rgb),FAILURE);
        free_2d_array(init_c_rgb);
        data->ceiling_set = 1;
        return (SUCCESS);
    }
    if (map_line[0] == '\0')
        return (SUCCESS);
    if (first_phase_done(data) == SUCCESS)
        return (printf("first phase done check\n"),CONTINUE);
    else
        {
            printf("not done..\n");
        }
    return (FAILURE);
}

char *delete_newlines(char *map_line)
{   
    int i = 0;
    char *line;

    while (map_line[i] && map_line[i] != '\n' && map_line[i] != '\r')
        i++;
    line = strndup(map_line,i);
    return (line);
}
int is_valid_first_wall(char *map_line)
{
    int i = 0;

    while (map_line[i] == '\0')
        i++;
    while(map_line[i])
    {
        if (map_line[i] == '1')
            return (SUCCESS);
        i++;
    }
    return (FAILURE);
}

char **parse_map(t_game_data *data, char *map)
{
    int fd;
    char *temp;

    printf("inside parse_map\n");
    if (!map)
        return (NULL);
    fd = open(map, O_RDONLY);
    if (fd < 0)
        return (NULL); // print appropriate error output
    // printf("[%s] what u might be\n",data->map.line); 
    // printf("[%d] should be zero\n",data->map.map_width);
    data->map.line = get_next_line(fd);
    while(data->map.line)
    {   
        temp = data->map.line;
        data->map.line = delete_newlines(data->map.line);
        free(temp);
        printf("inside data->map.line[%s]\n",data->map.line);
        int i = validate_map_info(data,data->map.line);
        if (i == FAILURE)
        {   
            close(fd);
            printf("you are missing sumn\n");
            free(data->map.line);
            return (NULL);
        }
        if (data->first_phase_done && i == CONTINUE)
        {   
            printf("enters here\n");
            if (is_valid_first_wall(data->map.line) == SUCCESS)
            {
                printf("moving on to the second phase....\n");
                 data->map.map = validate_map(data,fd);
                //  int j = 0;
                //  while(data->map.map[j])
                //  {
                //     printf("contents of data->map.map[j][%s]\n",data->map.map[j]);
                //     j++;
                //  }
                 return (data->map.map);
            }
            else
            {
                printf("[ERROR]ahem... where's your wall?\n");
                close(fd);
                free(data->map.line);
                return (NULL);
            }
        }
        // printf("%s\n",data->map.line);
        free(data->map.line);
        data->map.line = get_next_line(fd);
    }
    close(fd);
    return (NULL);
}

