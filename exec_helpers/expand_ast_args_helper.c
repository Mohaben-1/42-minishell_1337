/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_ast_args_helper.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 13:55:18 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/24 19:29:13 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	copy_first_args(t_new_ast_args *new, t_ast_node *ast, int i)
{
	int	j;

	j = 0;
	while (j < i)
	{
		new->new_args[j] = ft_strdup(ast->args[j]);
		new->new_arg_is_spaced[j] = ast->arg_is_spaced[j];
		new->new_quote_types[j] = ast->arg_quote_types[j];
		j++;
	}
}

void	copy_split_args(t_new_ast_args *new, t_ast_node *ast,
	char **splited, int i)
{
	int	j;
	int	split_count;

	split_count = arg_count(splited);
	j = 0;
	while (j < split_count)
	{
		new->new_args[i + j] = ft_strdup(splited[j]);
		if (j == 0)
			new->new_arg_is_spaced[i + j] = ast->arg_is_spaced[i];
		else
			new->new_arg_is_spaced[i + j] = 1;
		new->new_quote_types[i + j] = AST_SQUOTES;
		j++;
	}
}

void	copy_remaining_args(t_new_ast_args *new, t_ast_node *ast, 
	int i, int split_count)
{
	int	j;
	int	new_pos;

	j = i + 1;
	while (j < ast->arg_count)
	{
		new_pos = i + split_count + (j - (i + 1));
		new->new_args[new_pos] = ft_strdup(ast->args[j]);
		new->new_arg_is_spaced[new_pos] = ast->arg_is_spaced[j];
		new->new_quote_types[new_pos] = ast->arg_quote_types[j];
		j++;
	}
}
