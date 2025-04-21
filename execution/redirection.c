/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 12:31:49 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/21 13:49:36 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	proccess_all_heredocs(t_ast_node *ast, t_exec *exec)
{
	t_redirect	*redr;

	redr = ast->redirects;
	while (redr)
	{
		if (redr->type == token_hrdc)
		{
			if (redr->heredoc_fd == -1)
				redr->heredoc_fd = ft_handle_heredoc(redr, exec);
			if (redr->heredoc_fd == -1)
				return (0);
		}
		redr = redr->next;
	}
	return (1);
}

int	check_hrdc_priority(t_redirect *redirect)
{
	while (redirect)
	{
		if (redirect->type == token_hrdc)
			return (1);
		redirect = redirect->next;
	}
	return (0);
}

static int	proccess_redirect_helper(t_redirect *redr, t_exec *exec, int fd)
{
	if (redr->type != token_hrdc && (!redr->file || !*(redr->file)))
		return (ft_putstr_fd("minishell: $: ambiguous redirec\n", 2), 0);
	else if (redr->type == token_hrdc)
	{
		if (!proccess_heredoc(redr))
			return (0);
	}
	else if (redr->type == token_in)
	{
		if (!proccess_redr_in(redr, fd, exec))
			return (0);
	}
	else if (redr->type == token_out)
	{
		if (!proccess_redr_out(redr, fd, exec))
			return (0);
	}
	else if (redr->type == token_appnd)
	{
		if (!proccess_redr_append(redr, fd, exec))
			return (0);
	}
	return (1);
}

static int	proccess_redirect(t_redirect *redr, t_exec *exec, int fd)
{
	while (redr)
	{
		if (!proccess_redirect_helper(redr, exec, fd))
			return (0);
		redr = redr->next;
	}
	return (1);
}

int	ft_apply_redirect(t_ast_node *ast, t_exec *exec)
{
	t_redirect	*redr;
	int			fd;

	exec->std_fd[0] = dup(0);
	exec->std_fd[1] = dup(1);
	if (!proccess_all_heredocs(ast, exec))
		return (0);
	redr = ast->redirects;
	fd = 0;
	if (!proccess_redirect(redr, exec, fd))
		return (0);
	return (1);
}
