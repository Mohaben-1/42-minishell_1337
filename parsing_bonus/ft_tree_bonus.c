/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_tree_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 20:58:14 by ahouass           #+#    #+#             */
/*   Updated: 2025/04/23 17:46:40 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell_bonus.h"

t_ast_node	*build_ast(t_token_node *tokens, t_exec *exec)
{
	t_ast_node	*ast;

	if (!tokens)
		return (NULL);
	ast = parse_logical_ops(tokens, exec);
	return (ast);
}

t_token_node	*create_token_node(t_token_node *src)
{
	t_token_node	*new_node;

	new_node = malloc(sizeof(t_token_node));
	if (!new_node)
		return (NULL);
	new_node->type = src->type;
	new_node->to_skip = 0;
	new_node->data = ft_strdup(src->data);
	new_node->spaced = src->spaced;
	new_node->next = NULL;
	return (new_node);
}

t_ast_node	*create_ast_node(int type)
{
	t_ast_node	*node;

	node = malloc(sizeof(t_ast_node));
	if (!node)
		return (NULL);
	node->e_type = type;
	node->args = NULL;
	node->arg_count = 0;
	node->arg_quote_types = NULL;
	node->arg_is_spaced = NULL;
	node->redirects = NULL;
	node->left = NULL;
	node->right = NULL;
	node->child = NULL;
	return (node);
}

t_ast_node	*parse_pipes(t_token_node *tokens, t_exec *exec)
{
	t_token_node	*pipe;
	t_ast_node		*node;
	t_token_node	*left_tokens;
	t_token_node	*right_tokens;

	pipe = find_token_at_level(tokens, token_pipe);
	if (pipe)
	{
		node = create_ast_node(AST_PIPE);
		left_tokens = extract_tokens(tokens, pipe);
		right_tokens = pipe->next;
		node->left = parse_pipes(left_tokens, exec);
		node->right = parse_pipes(right_tokens, exec);
		free_token_list(left_tokens);
		return (node);
	}
	if (tokens->type == token_paren_open)
		return (parse_subshell(tokens, exec));
	return (parse_command(tokens));
}
