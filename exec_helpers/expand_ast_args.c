/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_ast_args.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 11:19:13 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/23 13:59:32 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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

void	init_new_ast_args(t_new_ast_args *new, int new_arg_count)
{
	new->new_args = malloc((new_arg_count + 1) * sizeof(char *));
	new->new_arg_is_spaced = malloc(new_arg_count * sizeof(int));
	new->new_quote_types = malloc(new_arg_count * sizeof(int));
}

void	expand_without_split(t_ast_node *ast, char *expanded, int *i, char *tmp)
{
	ast->args[*i] = expanded;
	free(tmp);
	(*i)++;
}

static void	handle_expansion(t_ast_node *ast, t_exec *exec, int *i, 
	int new_arg_count)
{
	t_new_ast_args	new;
	char			**splited;
	char			*expanded;
	char			*tmp;
	int				split_count;

	tmp = ast->args[*i];
	expanded = ft_expand(tmp, exec);
	if (ast->arg_quote_types[*i] == AST_DQUOTES || !*expanded)
		return (expand_without_split(ast, expanded, i, tmp));
	splited = ft_split_whitespaces(expanded);
	free(expanded);
	split_count = arg_count(splited);
	new_arg_count = ast->arg_count - 1 + split_count;
	init_new_ast_args(&new, new_arg_count);
	copy_first_args(&new, ast, *i);
	copy_split_args(&new, splited, *i);
	copy_remaining_args(&new, ast, *i, split_count);
	new.new_args[new_arg_count] = NULL;
	change_ast_args(ast, &new, new_arg_count);
	*i = 0;
	free_double_ptr(splited);
}

void	expand_args(t_ast_node *ast, t_exec *exec)
{
	int	new_arg_count;
	int	i;

	i = 0;
	new_arg_count = 0;
	while (i < ast->arg_count)
	{
		if (ft_strchr(ast->args[i], '$')
			&& ast->arg_quote_types[i] != AST_SQUOTES)
			handle_expansion(ast, exec, &i, new_arg_count);
		else
			i++;
	}
}
