/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_tree.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 14:42:21 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/22 14:44:58 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	free_ast_args(t_ast_node *node)
{
	int	i;

	if (!node->args)
		return ;
	i = 0;
	while (node->args[i])
	{
		free(node->args[i]);
		i++;
	}
	free(node->args);
}

void	free_ast_node(t_ast_node *node)
{
	if (!node)
		return ;
	free_ast_node(node->left);
	free_ast_node(node->right);
	free_ast_node(node->child);
	free_ast_args(node);
	free(node->arg_quote_types);
	free(node->arg_is_spaced);
	free_redirects(node->redirects);
	free(node);
}

void	free_collected_args(char **args, int *quote_types, int *is_spaced)
{
	int	i;

	i = 0;
	while (args[i])
		free(args[i++]);
	free(args);
	free(quote_types);
	free(is_spaced);
}

char	**free_collected_args_and_return(char **args, int *qt, int *spaced)
{
	free_collected_args(args, qt, spaced);
	return (NULL);
}

t_token_node	*free_token_list_and_return_null(t_token_node *list)
{
	free_token_list(list);
	return (NULL);
}
