/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_redir_helper_bonus.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 17:41:08 by mohaben-          #+#    #+#             */
/*   Updated: 2025/05/08 16:40:53 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell_bonus.h"

t_redirect	*create_redirect_type(t_token_node *token, int redir_type)
{
	t_redirect	*redir;

	redir = malloc(sizeof(t_redirect));
	if (!redir)
		return (NULL);
	redir->file = ft_strdup(token->data);
	if (!redir->file)
		return (free(redir), NULL);
	redir->type = redir_type;
	redir->heredoc_fd = -1;
	redir->quoted = get_quote_type(token->type);
	redir->is_spaced = 0;
	redir->next = NULL;
	return (redir);
}

void	add_redirect_to_list(t_redirect **head, t_redirect **current, 
	t_redirect *redir)
{
	if (!*head)
		*head = redir;
	else
		(*current)->next = redir;
	*current = redir;
}

void	mark_redirection_tokens(t_token_node *redir, t_token_node **last)
{
	*last = redir->next;
	redir->to_skip = 1;
	(*last)->to_skip = 1;
}
