/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   filter_ast.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 13:51:29 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/23 11:17:13 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	count_quoted_args(t_ast_node *ast)
{
	int	count;
	int	i;

	count = 0;
	i = 0;
	while (i < ast->arg_count)
	{
		if ((ast->args[i] && ast->args[i][0]) || ast->arg_quote_types[i])
			count++;
		i++;
	}
	return (count);
}

static void	change_ast_args(t_ast_node *ast, char **new_args, 
		int *new_quote_types, int arg_count)
{
	free_double_ptr(ast->args);
	free(ast->arg_quote_types);
	ast->args = new_args;
	ast->arg_quote_types = new_quote_types;
	if (!arg_count)
		ast->arg_count = 1;
	else
		ast->arg_count = arg_count;
}

void	fill_new_args_quote_type(t_ast_node *ast, char **new_args,
	int *new_quote_types)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (i < ast->arg_count)
	{
		if ((ast->args[i] && ast->args[i][0]) || ast->arg_quote_types[i])
		{
			new_args[j] = ft_strdup(ast->args[i]);
			new_quote_types[j] = ast->arg_quote_types[i];
			j++;
		}
		i++;
	}
	new_args[j] = NULL;
}

void	fill_single_new_args_quote_type(char **new_args, int *new_quote_types)
{
	new_args[0] = ft_strdup("");
	new_quote_types[0] = 0;
	new_args[1] = NULL;
}

void	filter_unquoted_empty_args(t_ast_node *ast, t_exec *exec)
{
	char	**new_args;
	int		*new_quote_types;
	int		j;

	if (!ast || !ast->args || !ast->arg_quote_types)
		return ;
	expand_ast_args(ast, exec);
	j = count_quoted_args(ast);
	if (j == 0)
	{
		new_args = malloc(2 * sizeof(char *));
		new_quote_types = malloc(sizeof(int));
		if (!new_args || !new_quote_types)
			return ;
		fill_single_new_args_quote_type(new_args, new_quote_types);
	}
	else
	{
		new_args = malloc((j + 1) * sizeof(char *));
		new_quote_types = malloc(j * sizeof(int));
		if (!new_args || !new_quote_types)
			return ;
		fill_new_args_quote_type(ast, new_args, new_quote_types);
	}
	change_ast_args(ast, new_args, new_quote_types, j);
}
