/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 20:09:29 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/13 20:10:05 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

# define BUFFER_SIZE 1

char	*ft_fill(int fd, char *saved_chars, char *buffer)
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
		if (saved_chars == NULL)
			return (NULL);
		if (ft_strchr(saved_chars, '\n'))
			break ;
	}
	return (saved_chars);
}

char	*ft_extract_line(char *data)
{
	char	*line;
	size_t	i;

	if (data == NULL)
		return (NULL);
	i = 0;
	while (data[i] && data[i] != '\n')
		i++;
	if (data[i] == '\n')
		i++;
	line = ft_substr(data, 0, i);
	return (line);
}

char	*ft_save_chars_left(char *data)
{
	size_t	i;
	char	*chars_left;

	if (data == NULL)
		return (NULL);
	i = 0;
	while (data[i] && data[i] != '\n')
		i++;
	if (data[i] == '\n')
		i++;
	if (data[i] == '\0')
		return (NULL);
	chars_left = ft_substr(data, i, ft_strlen(data) - i);
	return (chars_left);
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
	buffer = malloc((BUFFER_SIZE + 1) * sizeof(char));
	if (buffer == NULL)
	{
		free(saved_chars);
		saved_chars = NULL;
		return (NULL);
	}
	data = ft_fill(fd, saved_chars, buffer);
	free(buffer);
	line = ft_extract_line(data);
	saved_chars = ft_save_chars_left(data);
	free(data);
	return (line);
}
