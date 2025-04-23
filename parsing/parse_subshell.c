/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_subshell.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 14:46:56 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/22 14:47:54 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_token_node	*find_matching_paren(t_token_node *open_paren)
{
	int				paren_count;
	t_token_node	*tmp;

	paren_count = 1;
	tmp = open_paren->next;
	while (tmp && paren_count > 0)
	{
		if (tmp->type == token_paren_open)
			paren_count++;
		else if (tmp->type == token_paren_close && --paren_count == 0)
			return (tmp);
		tmp = tmp->next;
	}
	return (NULL);
}

t_ast_node	*parse_subshell(t_token_node *tokens, t_exec *exec)
{
	t_token_node	*closing;
	t_ast_node		*node;
	t_token_node	*inner_tokens;
	t_token_node	*remaining;

	(void)exec;
	if (tokens->type != token_paren_open)
		return (NULL);
	closing = find_matching_paren(tokens);
	if (!closing)
		return (NULL);
	node = create_ast_node(AST_SUBSHELL);
	inner_tokens = extract_tokens(tokens->next, closing);
	node->child = parse_logical_ops(inner_tokens, exec);
	free_token_list(inner_tokens);
	remaining = closing->next;
	if (remaining && is_redirection(remaining->type))
		node->redirects = parse_redirections(&remaining);
	return (handle_logical_operators(node, remaining, exec));
}
