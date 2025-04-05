/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 12:38:54 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/05 19:52:19 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	ft_exec_left_pipe(t_ast_node *ast, t_exec *exec, int *pipe_fd)
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
		close(pipe_fd[0]);
		dup2(pipe_fd[1], 1);
		close(pipe_fd[1]);
		if (ast->type == AST_PIPE)
			ft_execute_pipe(ast, exec);
		else if (ast->type == AST_COMMAND)
			execute_command(ast, exec);
		else if (ast->type == AST_AND_AND)
		{
			execute_ast(ast->left, exec);
			if (exec->exit_status == 0)
				execute_ast(ast->right, exec);
		}
		else if (ast->type == AST_OR_OR)
		{
			execute_ast(ast->left, exec);
			if (exec->exit_status != 0)
				execute_ast(ast->right, exec);
		}
		else if (ast->type == AST_SUBSHELL)
			execute_subshell(ast, exec);
		exit(exec->exit_status);
	}
	return (pid);
}

int	ft_exec_right_pipe(t_ast_node *ast, t_exec *exec, int *pipe_fd)
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
		close(pipe_fd[1]);
		dup2(pipe_fd[0], 0);
		close(pipe_fd[0]);
		if (ast->type == AST_PIPE)
			ft_execute_pipe(ast, exec);
		else if (ast->type == AST_COMMAND)
			execute_command(ast, exec);
		else if (ast->type == AST_AND_AND)
		{
			execute_ast(ast->left, exec);
			if (exec->exit_status == 0)
				execute_ast(ast->right, exec);
		}
		else if (ast->type == AST_OR_OR)
		{
			execute_ast(ast->left, exec);
			if (exec->exit_status != 0)
				execute_ast(ast->right, exec);
		}
		else if (ast->type == AST_SUBSHELL)
			execute_subshell(ast, exec);
		exit(exec->exit_status);
	}
	return (pid);
}

void	ft_execute_pipe(t_ast_node *node, t_exec *exec)
{
	int	pipe_fd[2];
	int	status;
	int	pid1;
	int	pid2;

	if (pipe(pipe_fd) < 0)
	{
		ft_putstr_fd("minishell: pipe: Resource unavailable\n", 2);
		exec->exit_status = 1;
		return ;
	}
	ft_handle_heredoc_pipe(node, exec);
	pid1 = ft_exec_left_pipe(node->left, exec, pipe_fd);
	pid2 = ft_exec_right_pipe(node->right, exec, pipe_fd);
	// ft_close_heredoc_fds(node);
	close(pipe_fd[0]);
	close(pipe_fd[1]);
	waitpid(pid1, NULL, 0);
	waitpid(pid2, &status, 0);
	if (WIFEXITED(status))
		exec->exit_status = WEXITSTATUS(status);
	else
		exec->exit_status = 1;
	ft_restore_std_fd(exec);
}
