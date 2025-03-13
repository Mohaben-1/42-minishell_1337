/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 12:14:55 by mohaben-          #+#    #+#             */
/*   Updated: 2025/03/13 15:12:10 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	count_words(char *s, char c)
{
	char	quote;
	int		count;
	int		i;

	count = 0;
	i = 0;
	while (s[i])
	{
		while (s[i] == c)
			i++;
		if (!s[i])
			break ;
		count++;
		if (s[i] == '\'' || s[i] == '"')
		{
			quote = s[i++];
			while (s[i] && s[i] != quote)
				i++;
			if (s[i] == quote)
				i++;
		}
		while (s[i] && s[i] != c && s[i] != '\'' && s[i] != '"')
			i++;
	}
	return (count);
}

static void	free_all(char **res, int i)
{
	while (i >= 0)
	{
		free(res[i]);
		res[i] = NULL;
		i--;
	}
	free(res);
}

void	ft_set_start_end(char *s, char c, int *start, int *end)
{
	char	quote_char;

	while (s[*end] == c)
		(*end)++;
	if (s[*end] == '\'' || s[*end] == '"')
	{
		quote_char = s[*end];
		(*start) = ++(*end);
		while (s[*end] && s[*end] != quote_char)
				(*end)++;
		if (s[*end])
			(*end)++;
	}
	else
	{
		*start = *end;
		while (s[*end] && s[*end] != c && s[*end] != '\'' && s[*end] != '"')
			(*end)++;
	}
}

static char	**ft_fill(char **arr, char *s, char c)
{
	int		start;
	int		end;
	int		i;

	start = 0;
	end = 0;
	i = 0;
	while (s[end] && i < count_words(s, c))
	{
		ft_set_start_end(s, c, &start, &end);
		arr[i] = ft_substr(s, start, end - start);
		if (arr[i] == NULL)
		{
			free_all(arr, i - 1);
			arr = NULL;
			return (arr);
		}
		i++;
	}
	arr[i] = NULL;
	return (arr);
}

char	**ft_split(char *s, char c)
{
	char	**arr;

	if (s == NULL)
		p_error("Malloc error", 1);
	arr = malloc((count_words(s, c) + 1) * sizeof(char *));
	if (arr == NULL)
		p_error("Malloc error", 1);
	arr = ft_fill(arr, s, c);
	if (arr == NULL)
		p_error("Malloc error", 1);
	return (arr);
}
