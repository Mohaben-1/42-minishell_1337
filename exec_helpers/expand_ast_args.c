/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_ast_args.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 11:19:13 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/23 13:01:55 by mohaben-         ###   ########.fr       */
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

void expand_args(t_ast_node *ast, t_exec *exec)
{
	t_new_ast_args	new;
	char			**splited;
	char			*expanded;
	char			*tmp;
	int				i;

	i = 0;
	while (i < ast->arg_count)
	{
		if (ft_strchr(ast->args[i], '$') && ast->arg_quote_types[i] != AST_SQUOTES)
		{
			tmp = ast->args[i];
			expanded = ft_expand(tmp, exec);
			if (ast->arg_quote_types[i] == AST_DQUOTES || !*expanded)
			{
				ast->args[i] = expanded;
				free(tmp);
				continue ;
			}
			splited = ft_split_whitespaces(expanded);
			free(expanded);

			int split_count = arg_count(splited);
			int new_arg_count = ast->arg_count - 1 + split_count;
			new.new_args = malloc((new_arg_count + 1) * sizeof(char *));
			new.new_arg_is_spaced = malloc(new_arg_count * sizeof(int));
			new.new_quote_types = malloc(new_arg_count * sizeof(int));
			for (int j = 0; j < i; j++) 
			{
				new.new_args[j] = ft_strdup(ast->args[j]);
				new.new_arg_is_spaced[j] = ast->arg_is_spaced[j];
				new.new_quote_types[j] = ast->arg_quote_types[j];
			}
			for (int j = 0; j < split_count; j++)
			{
				new.new_args[i + j] = ft_strdup(splited[j]);
				new.new_arg_is_spaced[i + j] = (j == 0) ? ast->arg_is_spaced[i] : 1;
				new.new_quote_types[i + j] = ast->arg_quote_types[i];
			}
			for (int j = i + 1; j < ast->arg_count; j++)
			{
				int new_pos = i + split_count + (j - (i + 1));
				new.new_args[new_pos] = ft_strdup(ast->args[j]);
				new.new_arg_is_spaced[new_pos] = ast->arg_is_spaced[j];
				new.new_quote_types[new_pos] = ast->arg_quote_types[j];
			}
			new.new_args[new_arg_count] = NULL;
			change_ast_args(ast, &new, new_arg_count);
			free_double_ptr(splited);
			i = 0;
		}
		else
			i++;
	}
}
