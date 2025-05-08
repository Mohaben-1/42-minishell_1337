/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wild_card_helper.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 11:27:32 by mohaben-          #+#    #+#             */
/*   Updated: 2025/05/08 16:13:28 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	is_wild_card(char *s)
{
	while (*s)
	{
		if (*s == '*')
			return (1);
		s++;
	}
	return (0);
}

char	**allocate_wild_res(char *file_name, int *count)
{
	char	**result;

	*count = count_wild_matches(file_name);
	if (*count == 0)
		return (NULL);
	result = malloc(sizeof(char *) * (*count + 1));
	if (!result)
		return (NULL);
	return (result);
}

char	**wildcard_expand(char *file_name)
{
	DIR				*d;
	struct dirent	*dir;
	char			**result;
	int				count;
	int				i;

	result = allocate_wild_res(file_name, &count);
	if (!result)
		return (NULL);
	d = opendir(".");
	if (!d)
		return (free(result), NULL);
	i = 0;
	dir = readdir(d);
	while (dir && i < count)
	{
		if (dir->d_name[0] != '.' || file_name[0] == '.')
		{
			if (match_wild_card(file_name, dir->d_name))
				result[i++] = ft_strdup(dir->d_name);
		}
		dir = readdir(d);
	}
	result[i] = NULL;
	return (closedir(d), result);
}

int	ft_expand_redr_wild(t_ast_node *ast, t_exec *exec)
{
	char		**expanded;
	t_redirect	*redr;
	char		*tmp;

	redr = ast->redirects;
	while (redr)
	{
		if (redr->file && redr->type != token_hrdc && is_wild_card(redr->file))
		{
			expanded = wildcard_expand(redr->file);
			if (!expanded || expanded[1])
				return (rdr_wild_err(expanded, redr->file, exec), 0);
			else
			{
				tmp = redr->file;
				redr->file = expanded[0];
				free(tmp);
				free(expanded);
			}
		}
		redr = redr->next;
	}
	return (1);
}
