/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_helper.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 14:02:03 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/19 14:03:41 by mohaben-         ###   ########.fr       */
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

int	proccess_redr_in(t_redirect *redr, int fd, t_exec *exec)
{
	fd = open(redr->file, O_RDONLY);
	if (fd == -1)
		return (ft_error_file(redr->file, exec), 0);
	if (!check_hrdc_priority(redr))
		dup2(fd, 0);
	close(fd);
	return (1);
}

int	proccess_redr_out(t_redirect *redr, int fd, t_exec *exec)
{
	if (!redr->file || !*redr->file)
		return (ft_error_file_expand("$", exec), 0);
	fd = open(redr->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
		return (ft_error_file(redr->file, exec), 0);
	dup2(fd, 1);
	close(fd);
	return (1);
}

int	proccess_redr_append(t_redirect *redr, int fd, t_exec *exec)
{
	fd = open(redr->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd == -1)
		return (ft_error_file(redr->file, exec), 0);
	dup2(fd, 1);
	close(fd);
	return (1);
}

int	proccess_heredoc(t_redirect *redr)
{
	if (dup2(redr->heredoc_fd, 0) == -1)
		return (0);
	close(redr->heredoc_fd);
	redr->heredoc_fd = -1;
	return (1);
}
