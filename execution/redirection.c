/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 12:31:49 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/12 18:10:36 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_restore_std_fd(t_exec *exec)
{
	if (exec->std_fd[0] != -1)
	{
		dup2(exec->std_fd[0], STDIN_FILENO);
		close(exec->std_fd[0]);
		exec->std_fd[0] = -1;
	}
	if (exec->std_fd[1] != -1)
	{
		dup2(exec->std_fd[1], STDOUT_FILENO);
		close(exec->std_fd[1]);
		exec->std_fd[1] = -1;
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
	redr = ast->redirects;
	while (redr)
	{
		if (redr->type != token_hrdc && !redr->file)
		{
			ft_putstr_fd("minishell: $: ambiguous redirec\n", 2);
			return (0);
		}
		if (redr->type == token_hrdc)
		{
			if (dup2(redr->heredoc_fd, 0) == -1)
				return (0);
			close(redr->heredoc_fd);
			redr->heredoc_fd = -1;
		}
		else if (redr->type == token_in)
		{
			fd = open(redr->file, O_RDONLY);
			if (fd == -1)
				return (ft_error_file(redr->file, exec), 0);
			if (!check_hrdc_priority(redr))
				dup2(fd, 0);
			close(fd);
		}
		else if (redr->type == token_out)
		{
			if (!redr->file || !*redr->file)
				return (ft_error_file_expand("$", exec), 0);
			fd = open(redr->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (fd == -1)
				return (ft_error_file(redr->file, exec), 0);
			dup2(fd, 1);
			close(fd);
		}
		else if (redr->type == token_appnd)
		{
			fd = open(redr->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
			if (fd == -1)
				return (ft_error_file(redr->file, exec), 0);
			dup2(fd, 1);
			close(fd);
		}
		redr = redr->next;
	}
	return 1;
}
