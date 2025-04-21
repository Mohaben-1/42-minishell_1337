/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wild_card.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 19:54:03 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/21 14:50:04 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	**merge_args(char **old_args, int i, char **expand)
{
	char	**new_args;
	int		idx;
	int		j;
	int		k;

	new_args = malloc(sizeof(char *) * (arg_count(old_args)
				+ arg_count(expand) + 1));
	if (!new_args)
		return (NULL);
	j = 0;
	k = 0;
	idx = 0;
	while (j < i)
		new_args[idx++] = ft_strdup(old_args[j++]);
	while (expand && expand[k])
		new_args[idx++] = ft_strdup(expand[k++]);
	j++;
	while (old_args[j])
		new_args[idx++] = ft_strdup(old_args[j++]);
	new_args[idx] = NULL;
	return (new_args);
}

static void	copy_quote_types(int *new_types, t_ast_node *ast, int pos
			, int exp_count)
{
	int	idx;
	int	j;

	idx = 0;
	j = 0;
	while (j < pos)
		new_types[idx++] = ast->arg_quote_types[j++];
	j = 0;
	while (j++ < exp_count)
		new_types[idx++] = 0;
	j = pos + 1;
	while (ast->args[j])
		new_types[idx++] = ast->arg_quote_types[j++];
}

char	**handle_wildcard_expansion(t_ast_node *ast, int i)
{
	char	**expanded;
	char	**new_args;
	int		*new_quote_types;
	int		expanded_count;
	int		args_count;

	expanded = wildcard_expand(ast->args[i]);
	if (!expanded)
		return (NULL);
	args_count = arg_count(ast->args);
	expanded_count = arg_count(expanded);
	new_quote_types = malloc(sizeof(int) * (args_count + expanded_count));
	if (!new_quote_types)
		return (free_double_ptr(expanded), NULL);
	copy_quote_types(new_quote_types, ast, i, expanded_count);
	new_args = merge_args(ast->args, i, expanded);
	if (!new_args)
	{
		free(new_quote_types);
		return (free_double_ptr(expanded), NULL);
	}
	free(ast->arg_quote_types);
	ast->arg_quote_types = new_quote_types;
	free_double_ptr(expanded);
	return (new_args);
}

void	ft_expand_wildcard(t_ast_node *ast)
{
	char	**new_args;
	int		i;

	if (!ast || ast->e_type != AST_COMMAND || !ast->args[0])
		return ;
	i = 0;
	while (ast->args[i])
	{
		if (ast->arg_quote_types[i] == 0 && is_wild_card(ast->args[i]))
		{
			new_args = handle_wildcard_expansion(ast, i);
			if (new_args)
			{
				free_double_ptr(ast->args);
				ast->args = new_args;
				ast->arg_count = arg_count(new_args);
				i = -1;
			}
		}
		i++;
	}
}
