/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_ast.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 11:59:43 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/22 13:46:52 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_redirect	*expand_redir_helper(t_redirect *current, t_redirect *redir, 
	t_exec *exec)
{
	char	*expanded;
	char	*tmp;

	while (current && current->type == redir->type && current->is_spaced == 0)
	{
		if (ft_strchr(current->file, '$') && current->quoted != token_squote
			&& redir->type != token_hrdc)
			expanded = ft_expand(current->file, exec);
		else
			expanded = ft_strdup(current->file);
		tmp = redir->file;
		redir->file = ft_strjoin(tmp, expanded);
		free(tmp);
		free(expanded);
		current = current->next;
	}
	return (current);
}

t_redirect	*create_new_redir(t_redirect *redir, t_exec *exec)
{
	t_redirect	*new;

	new = malloc(sizeof(t_redirect));
	if (!new)
		return (NULL);
	new->type = redir->type;
	new->heredoc_fd = redir->heredoc_fd;
	new->quoted = redir->quoted;
	new->is_spaced = redir->is_spaced;
	if (ft_strchr(redir->file, '$') && redir->type != token_hrdc
		&& redir->quoted != token_squote)
		new->file = ft_expand(redir->file, exec);
	else
		new->file = ft_strdup(redir->file);
	new->next = NULL;
	return (new);
}

void	add_to_processed(t_redirect **head, t_redirect **tail, t_redirect *new)
{
	if (!*head)
	{
		*head = new;
		*tail = new;
	}
	else
	{
		(*tail)->next = new;
		*tail = new;
	}
}

t_redirect	*expand_redir(t_redirect *redirect, t_exec *exec)
{
	t_redirect	*processed_head;
	t_redirect	*processed_tail;
	t_redirect	*new_redir;
	t_redirect	*next;

	processed_head = NULL;
	processed_tail = NULL;
	while (redirect)
	{
		new_redir = create_new_redir(redirect, exec);
		if (!new_redir)
			return (NULL);
		next = expand_redir_helper(redirect->next, new_redir, exec);
		add_to_processed(&processed_head, &processed_tail, new_redir);
		redirect = next;
	}
	return (processed_head);
}

void	expand_ast_args(t_ast_node *ast, t_exec *exec)
{
	t_redirect	*old;
	t_redirect	*tmp;

	expand_args(ast, exec);
	old = ast->redirects;
	ast->redirects = expand_redir(old, exec);
	while (old)
	{
		tmp = old;
		old = old->next;
		free(tmp->file);
		free(tmp);
	}
}
