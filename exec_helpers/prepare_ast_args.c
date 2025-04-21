/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prepare_ast_args.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 15:51:34 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/21 16:29:46 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	expand_args(t_ast_node *ast, t_exec *exec)
{
	char	*tmp;
	int		i;

	i = 0;
	while (i < ast->arg_count)
	{
		if (ft_strchr(ast->args[i], '$')
			&& ast->arg_quote_types[i] != AST_SQUOTES)
		{
			tmp = ast->args[i];
			ast->args[i] = ft_expand(tmp, exec);
			free(tmp);
		}
		i++;
	}
}

int	count_spaced_args(t_ast_node *ast)
{
	int	count;
	int	i;

	count = 1;
	i = 0;
	while (i < ast->arg_count)
	{
		if (ast->arg_is_spaced[i])
			count++;
		i++;
	}
	return (count);
}

static void	change_ast_args(t_ast_node *ast, char **new_args,
	int *new_arg_is_spaced, int arg_count)
{
	free_double_ptr(ast->args);
	free(ast->arg_is_spaced);
	ast->args = new_args;
	ast->arg_is_spaced = new_arg_is_spaced;
	ast->arg_count = arg_count;
}

void	fill_new_args_spaced(t_ast_node *ast, char **new_args,
	int *new_arg_is_spaced, int *j)
{
	char	*tmp;
	int		i;

	new_args[0] = ft_strdup(ast->args[0]);
	new_arg_is_spaced[0] = ast->arg_is_spaced[0];
	i = 1;
	*j = 1;
	while (i < ast->arg_count)
	{
		if (ast->arg_is_spaced[i])
		{
			new_args[*j] = ft_strdup(ast->args[i]);
			new_arg_is_spaced[*j] = ast->arg_is_spaced[i];
			(*j)++;
		}
		else
		{
			tmp = new_args[*j - 1];
			new_args[*j - 1] = ft_strjoin(tmp, ast->args[i]);
			free(tmp);
		}
		i++;
	}
}

void	prepare_ast_args(t_ast_node *ast, t_exec *exec)
{
	int		*new_arg_is_spaced;
	char	**new_args;
	int		j;

	filter_unquoted_empty_args(ast, exec);
	if (!ast || !ast->args)
		return ;
	j = count_spaced_args(ast);
	new_args = malloc((j + 1) * sizeof(char *));
	new_arg_is_spaced = malloc(j * sizeof(int));
	if (!new_args || !new_arg_is_spaced)
		return ;
	if (ast->arg_count > 0)
		fill_new_args_spaced(ast, new_args, new_arg_is_spaced, &j);
	new_args[j] = NULL;
	change_ast_args(ast, new_args, new_arg_is_spaced, j);
}
