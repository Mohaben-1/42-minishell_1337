/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 12:31:49 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/19 14:03:50 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	proccess_all_heredocs(t_ast_node *ast, t_exec *exec)
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

int	ft_apply_redirect(t_ast_node *ast, t_exec *exec)
{
	t_redirect	*redr;
	int			fd;

	exec->std_fd[0] = dup(0);
	exec->std_fd[1] = dup(1);
	proccess_all_heredocs(ast, exec);
	redr = ast->redirects;
	while (redr)
	{
		if (redr->type != token_hrdc && !redr->file)
			return (ft_putstr_fd("minishell: $: ambiguous redirec\n", 2), 0);
		if (redr->type == token_hrdc)
			proccess_heredoc(redr);
		else if (redr->type == token_in)
			proccess_redr_in(redr, fd, exec);
		else if (redr->type == token_out)
			proccess_redr_out(redr, fd, exec);
		else if (redr->type == token_appnd)
			proccess_redr_in(redr, fd, exec);
		redr = redr->next;
	}
	return 1;
}
