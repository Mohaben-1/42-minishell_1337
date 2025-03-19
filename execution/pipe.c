/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 12:38:54 by mohaben-          #+#    #+#             */
/*   Updated: 2025/03/19 15:34:56 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	ft_exec_left_pipe(t_ast_node *node, t_exec *exec, int *pipe_fd)
{
	int	pid;

	pid = fork();
	if (pid == -1)
	{
		ft_putstr_fd("minishell: fork: Resource temporarily unavailable\n", 2);
		return (-1);
	}
	if (pid == 0)
	{
		dup2(pipe_fd[1], 1);
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		execute_ast(node, exec);
		exit(exec->exit_status);
	}
	return (pid);
}

int	ft_exec_right_pipe(t_ast_node *node, t_exec *exec, int *pipe_fd)
{
	int	pid;

	pid = fork();
	if (pid == -1)
	{
		ft_putstr_fd("minishell: fork: Resource temporarily unavailable\n", 2);
		return (-1);
	}
	if (pid == 0)
	{
		dup2(pipe_fd[0], 0);
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		execute_ast(node, exec);
		exit(exec->exit_status);
	}
	return (pid);
}

void	ft_execute_pipe(t_ast_node *node, t_exec *exec)
{
	int	pipe_fd[2];
	int	pid1;
	int	pid2;
	int	status;

	if (pipe(pipe_fd) < 0)
	{
		ft_putstr_fd("minishell: pipe: Resource temporarily unavailable\n", 2);
		return ;
	}
	pid1 = ft_exec_left_pipe(node->left, exec, pipe_fd);
	pid2 = ft_exec_right_pipe(node->right, exec, pipe_fd);
	close(pipe_fd[0]);
	close(pipe_fd[1]);
	waitpid(pid1, NULL, 0);
	waitpid(pid2, &status, 0);
	if (WIFEXITED(status))
		exec->exit_status = WEXITSTATUS(status);
	else
		exec->exit_status = 1;
}
