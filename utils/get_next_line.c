/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 13:42:51 by mohaben-          #+#    #+#             */
/*   Updated: 2025/03/15 11:06:50 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*ft_get_data(int fd, char *saved_chars, char *buffer)
{
	ssize_t	bytes_read;
	char	*tmp;

	bytes_read = 1;
	while (bytes_read > 0)
	{
		bytes_read = read(fd, buffer, BUFFER_SIZE);
		if (bytes_read == -1)
		{
			free(saved_chars);
			saved_chars = NULL;
			return (NULL);
		}
		if (bytes_read == 0)
			break ;
		buffer[bytes_read] = '\0';
		tmp = saved_chars;
		saved_chars = ft_strjoin(tmp, buffer);
		free(tmp);
		if (!saved_chars)
			return (NULL);
		if (ft_strchr(saved_chars, '\n'))
			break ;
	}
	return (saved_chars);
}

char	*ft_get_line(char *data)
{
	size_t	i;

	if (!data)
		return (NULL);
	i = 0;
	while (data[i] && data[i] != '\n')
		i++;
	if (data[i] == '\n')
		i++;
	return (ft_substr(data, 0, i));
}

char	*ft_get_saved(char *data)
{
	size_t	i;

	if (!data)
		return (NULL);
	i = 0;
	while (data[i] && data[i] != '\n')
		i++;
	if (data[i] == '\n')
		i++;
	if (!data[i])
		return (NULL);
	return (ft_substr(data, i, ft_strlen(data) - i));
}

char	*get_next_line(int fd)
{
	static char	*saved_chars;
	char		*buffer;
	char		*line;
	char		*data;

	if (fd < 0 || BUFFER_SIZE <= 0 || read(fd, NULL, 0) < 0)
	{
		free(saved_chars);
		saved_chars = NULL;
		return (NULL);
	}
	buffer = malloc(BUFFER_SIZE + 1);
	if (!buffer)
		return (NULL);
	data = ft_get_data(fd, saved_chars, buffer);
	free(buffer);
	line = ft_get_line(data);
	saved_chars = ft_get_saved(data);
	free(data);
	return (line);
}
