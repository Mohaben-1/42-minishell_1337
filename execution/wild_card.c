/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wild_card.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 19:54:03 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/12 12:55:10 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include <dirent.h>

static int	is_wild_card(char *s)
{
	while (*s)
	{
		if (*s == '*')
			return (1);
		s++;
	}
	return (0);
}

static int	match(char *pattern, char *str)
{
	if (*pattern == '\0' && *str == '\0')
		return (1);
	if (*pattern == '*')
	{
		if (match(pattern + 1, str))
			return (1);
		if (*str && match(pattern, str + 1))
			return (1);
	}
	else if (*pattern == *str)
		return (match(pattern + 1, str + 1));
	return (0);
}

static int	count_matches(char *pattern)
{
	DIR				*d;
	struct dirent	*dir;
	int				count;

	d = opendir(".");
	if (!d)
		return (0);
	count = 0;
	while ((dir = readdir(d)))
	{
		if (dir->d_name[0] == '.' && pattern[0] != '.')
			continue ;
		if (match(pattern, dir->d_name))
			count++;
	}
	closedir(d);
	return (count);
}

static char	**wildcard_expand(char *pattern)
{
	DIR				*d;
	struct dirent	*dir;
	char			**result;
	int				count;
	int				i;

	count = count_matches(pattern);
	result = malloc(sizeof(char *) * (count + 1));
	if (!result)
		return (NULL);
	d = opendir(".");
	if (!d)
		return (NULL);
	i = 0;
	while ((dir = readdir(d)) && i < count)
	{
		if (dir->d_name[0] == '.' && pattern[0] != '.')
			continue ;
		if (match(pattern, dir->d_name))
			result[i++] = ft_strdup(dir->d_name);
	}
	result[i] = NULL;
	closedir(d);
	return (result);
}

static int	arg_count(char **args)
{
	int	i = 0;

	while (args && args[i])
		i++;
	return (i);
}

static char	**merge_args(char **old, int i, char **exp)
{
	int		old_len;
	int		exp_len;
	char	**new_args;
	int		idx;
	int		j;
	int		k;

	old_len = arg_count(old);
	exp_len = arg_count(exp);
	new_args = malloc(sizeof(char *) * (old_len + exp_len));
	if (!new_args)
		return (NULL);
	j = 0;
	k = 0;
	idx = 0;
	while (j < i)
		new_args[idx++] = ft_strdup(old[j++]);
	while (exp && exp[k])
		new_args[idx++] = ft_strdup(exp[k++]);
	j++; // skip wildcard
	while (old[j])
		new_args[idx++] = ft_strdup(old[j++]);
	new_args[idx] = NULL;
	return (new_args);
}

void	expand_ast_wildcards(t_ast_node *ast)
{
	char	**expanded;
	char	**new_args;
	int		i;

	if (!ast || ast->type != AST_COMMAND || !ast->args)
		return ;
	i = 0;
	while (ast->args[i])
	{
		if (ast->arg_quote_types[i] == 0 && is_wild_card(ast->args[i]))
		{
			expanded = wildcard_expand(ast->args[i]);
			if (expanded)
			{
				new_args = merge_args(ast->args, i, expanded);
				free_double_ptr(ast->args);
				ast->args = new_args;
				free_double_ptr(expanded);
				i = -1;
			}
		}
		i++;
	}
}
