/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tree_helper_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 14:39:16 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/23 17:42:06 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell_bonus.h"

t_ast_node	*parse_logical_ops(t_token_node *tokens, t_exec *exec)
{
	t_token_node	*op;
	t_ast_node		*node;
	t_token_node	*left_tokens;
	t_token_node	*right_tokens;
	int				node_type;

	op = find_op_at_level(tokens, token_and_and, token_or);
	if (op)
	{
		if (op->type == token_and_and)
			node_type = AST_AND_AND;
		else
			node_type = AST_OR_OR;
		node = create_ast_node(node_type);
		left_tokens = extract_tokens(tokens, op);
		right_tokens = op->next;
		node->left = parse_logical_ops(left_tokens, exec);
		node->right = parse_logical_ops(right_tokens, exec);
		free_token_list(left_tokens);
		return (node);
	}
	return (parse_pipes(tokens, exec));
}

t_ast_node	*handle_logical_operators(t_ast_node *node, t_token_node *tokens, 
	t_exec *exec)
{
	t_token_node	*op;
	t_ast_node		*logical_node;
	int				node_type;

	if (!tokens)
		return (node);
	op = find_op_at_level(tokens, token_and_and, token_or);
	if (op)
	{
		if (op->type == token_and_and)
			node_type = AST_AND_AND;
		else
			node_type = AST_OR_OR;
		logical_node = create_ast_node(node_type);
		logical_node->left = node;
		logical_node->right = parse_logical_ops(op->next, exec);
		return (logical_node);
	}
	return (node);
}

t_token_node	*find_op_at_level(t_token_node *tokens, t_token_type type1, 
	t_token_type type2)
{
	int				paren_level;
	t_token_node	*last_op;
	t_token_node	*tmp;

	tmp = tokens;
	last_op = NULL;
	paren_level = 0;
	while (tmp)
	{
		if (tmp->type == token_paren_open)
			paren_level++;
		else if (tmp->type == token_paren_close)
			paren_level--;
		if (paren_level == 0 && (tmp->type == type1 || tmp->type == type2))
			last_op = tmp;
		tmp = tmp->next;
	}
	return (last_op);
}

t_token_node	*find_token_at_level(t_token_node *tokens, t_token_type type)
{
	int				paren_level;
	t_token_node	*tmp;
	t_token_node	*last_token;

	tmp = tokens;
	paren_level = 0;
	last_token = NULL;
	while (tmp)
	{
		if (tmp->type == token_paren_open)
			paren_level++;
		else if (tmp->type == token_paren_close)
			paren_level--;
		if (paren_level == 0 && tmp->type == type)
			last_token = tmp;
		tmp = tmp->next;
	}
	return (last_token);
}

t_token_node	*extract_tokens(t_token_node *start, t_token_node *end)
{
	t_token_node	*new_node;
	t_token_node	*current;
	t_token_node	*result;
	t_token_node	*tmp;

	tmp = start;
	result = NULL;
	current = NULL;
	if (!start || start == end)
		return (NULL);
	while (tmp && tmp != end)
	{
		new_node = create_token_node(tmp);
		if (!new_node)
			return (free_token_list_and_return_null(result));
		if (!result)
			result = new_node;
		else
			current->next = new_node;
		current = new_node;
		tmp = tmp->next;
	}
	return (result);
}
