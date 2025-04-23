/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_redirection.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 14:54:14 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/22 15:01:45 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	is_redirection(t_token_type type)
{
	return (type == token_in || type == token_out || 
		type == token_hrdc || type == token_appnd);
}

static void	set_quoted_type(t_redirect *redir, t_token_node *file_token)
{
	if (file_token->type == token_dquote)
		redir->quoted = token_dquote;
	else if (file_token->type == token_squote)
		redir->quoted = token_squote;
	else
		redir->quoted = 0;
}

t_redirect	*create_redirect(t_token_node *redir_token, 
	t_token_node *file_token)
{
	t_redirect	*redir;

	redir = malloc(sizeof(t_redirect));
	if (!redir)
		return (NULL);
	redir->type = redir_token->type;
	if (redir->type == token_hrdc)
		redir->heredoc_fd = -1;
	else
		redir->heredoc_fd = 0;
	set_quoted_type(redir, file_token);
	redir->file = strdup(file_token->data);
	if (!redir->file)
	{
		free(redir);
		return (NULL);
	}
	redir->is_spaced = 1;
	redir->next = NULL;
	return (redir);
}

void	process_joined_tokens(t_token_node **last, int redir_type, 
	t_redirect **current, t_redirect **head)
{
	t_token_node	*next;
	t_redirect		*new;

	(void)head;
	next = (*last)->next;
	while (next && !next->spaced && (next->type == token_cmd
			|| next->type == token_dquote || next->type == token_squote))
	{
		new = create_redirect_type(next, redir_type);
		if (!new)
			return ;
		(*current)->next = new;
		*current = new;
		next->to_skip = 1;
		*last = next;
		next = next->next;
	}
}

t_redirect	*parse_redirections(t_token_node **tokens)
{
	t_token_node	*tmp;
	t_token_node	*last;
	t_redirect		*head;
	t_redirect		*redir;
	t_redirect		*current;

	current = NULL;
	tmp = *tokens;
	head = NULL;
	while (tmp)
	{
		if (is_redirection(tmp->type) && tmp->next)
		{
			redir = create_redirect(tmp, tmp->next);
			if (!redir)
				return (head);
			add_redirect_to_list(&head, &current, redir);
			mark_redirection_tokens(tmp, &last);
			process_joined_tokens(&last, tmp->type, &current, &head);
			tmp = last->next;
		}
		else
			tmp = tmp->next;
	}
	return (head);
}
