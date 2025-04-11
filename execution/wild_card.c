/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wild_card.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 19:54:03 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/11 21:48:10 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "../minishell.h"
// #include <dirent.h>
// #include <fnmatch.h>

// int	is_wildcard(char *s)
// {
// 	while (*s)
// 	{
// 		if (*s == '*')
// 			return (1);
// 		s++;
// 	}
// 	return (0);
// }

// char **expand_glob(const char *pattern)
// {
// 	DIR *dir = opendir(".");
// 	struct dirent *entry;
// 	char **matches = NULL;
// 	int count = 0;

// 	while ((entry = readdir(dir)))
// 	{
// 		if (entry->d_name[0] == '.')
// 			continue;

// 		if (fnmatch(pattern, entry->d_name, 0) == 0)
// 		{
// 			matches = realloc(matches, sizeof(char *) * (count + 2));
// 			printf("%s\n", matches);
// 			matches[count++] = strdup(entry->d_name);
// 			matches[count] = NULL;
// 		}
// 	}
// 	closedir(dir);
// 	return matches;
// }

// void	ft_wild_card(t_ast_node *ast, t_exec *exec)
// {

// }
