/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prepare_ast_args.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 15:51:34 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/23 13:58:08 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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

static void	change_ast_args(t_ast_node *ast, t_new_ast_args *new, int arg_count)
{
	free_double_ptr(ast->args);
	free(ast->arg_is_spaced);
	free(ast->arg_quote_types);
	ast->arg_quote_types = new->new_quote_types;
	ast->args = new->new_args;
	ast->arg_is_spaced = new->new_arg_is_spaced;
	ast->arg_count = arg_count;
}

void	fill_new_args_spaced(t_ast_node *ast, t_new_ast_args *new, int *j)
{
	char	*tmp;
	int		i;

	new->new_args[0] = ft_strdup(ast->args[0]);
	new->new_arg_is_spaced[0] = ast->arg_is_spaced[0];
	new->new_quote_types[0] = ast->arg_quote_types[0];
	i = 0;
	*j = 1;
	while (++i < ast->arg_count)
	{
		if (ast->arg_is_spaced[i])
		{
			new->new_args[*j] = ft_strdup(ast->args[i]);
			new->new_arg_is_spaced[*j] = ast->arg_is_spaced[i];
			new->new_quote_types[*j] = ast->arg_quote_types[i];
			(*j)++;
		}
		else
		{
			tmp = new->new_args[*j - 1];
			new->new_args[*j - 1] = ft_strjoin(tmp, ast->args[i]);
			new->new_quote_types[*j - 1] = ast->arg_quote_types[i];
			free(tmp);
		}
	}
}

void	prepare_ast_args(t_ast_node *ast, t_exec *exec)
{
	t_new_ast_args	new;
	int				j;

	filter_unquoted_empty_args(ast, exec);
	if (!ast || !ast->args)
		return ;
	j = count_spaced_args(ast);
	new.new_args = malloc((j + 1) * sizeof(char *));
	new.new_arg_is_spaced = malloc(j * sizeof(int));
	new.new_quote_types = malloc(j * sizeof(int));
	if (!new.new_args || !new.new_arg_is_spaced || !new.new_quote_types)
	{
		free(new.new_args);
		free(new.new_arg_is_spaced);
		free(new.new_quote_types);
		return ;
	}
	if (ast->arg_count > 0)
		fill_new_args_spaced(ast, &new, &j);
	new.new_args[j] = NULL;
	change_ast_args(ast, &new, j);
}
