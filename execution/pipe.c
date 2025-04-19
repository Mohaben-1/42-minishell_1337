/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 12:38:54 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/19 13:45:43 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	process_all_heredocs(t_ast_node *ast, t_exec *exec)
{
	t_redirect *redirect;

	if (!ast)
		return ;
	redirect = ast->redirects;
	while (redirect)
	{
		if (redirect->type == token_hrdc && redirect->heredoc_fd == -1)
			redirect->heredoc_fd = ft_handle_heredoc(redirect, exec);
		if (exec->exit_status == 1)
			return ;
		redirect = redirect->next;
	}
	if (ast->e_type == AST_PIPE || ast->e_type == AST_AND_AND || ast->e_type == AST_OR_OR)
	{
		process_all_heredocs(ast->left, exec);
		process_all_heredocs(ast->right, exec);
	}
	else if (ast->e_type == AST_SUBSHELL && ast->child)
		process_all_heredocs(ast->child, exec);
}

static void	handle_wait_status(t_exec *exec, int *pids, int count)
{
	int	status;
	int	i;

	i = 0;
	while (i < count)
	{
		if (i == count - 1)
			waitpid(pids[i], &status, 0);
		else
			waitpid(pids[i], NULL, 0);
		i++;
	}
	 if (WIFEXITED(status))
		exec->exit_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		exec->exit_status = 128 + WTERMSIG(status);
	else
		exec->exit_status = 1;
}

void	ft_handle_pipes(t_pipe_data *pipe_data, t_ast_node **ast_pipes, t_exec *exec)
{
	int	i;

	i = 0;
	while (i < pipe_data->cmd_count)
	{
		pipe_data->pids[i] = fork();
		if (pipe_data->pids[i] == -1)
		{
			ft_putstr_fd(FORK_ERROR, 2);
			exec->exit_status = 1;
			return ;
		}
		if (pipe_data->pids[i] == 0)
		{
			if (i > 0)
				dup2(pipe_data->pipes_fd[i - 1][0], 0);
			if (i < pipe_data->cmd_count - 1)
				dup2(pipe_data->pipes_fd[i][1], 1);
			close_pipes_fd(pipe_data->pipes_fd, pipe_data->cmd_count);
			exec_pipe_cmd(ast_pipes[i], exec);
			exit(exec->exit_status);
		}
		i++;
	}
	close_pipes_fd(pipe_data->pipes_fd, pipe_data->cmd_count);
}



void	ft_execute_pipe(t_ast_node *ast, t_exec *exec, int cmd_count)
{
	t_ast_node	*ast_pipes[cmd_count];
	int			pipes_fd[cmd_count - 1][2];
	int			pids[cmd_count];
	t_pipe_data	pipe_data;
	int			index;

	if (!ast)
		return ;
	index = 0;
	collect_pipe_cmd(ast, ast_pipes, &index);
	process_hrdc_pipes(ast_pipes, cmd_count, exec);
	process_pipes(pipes_fd, cmd_count, exec);
	if (exec->exit_status == 1)
		return ;
	init_pipe_data(&pipe_data, cmd_count, pipes_fd, pids);
	ft_handle_pipes(&pipe_data, ast_pipes, exec);
	handle_wait_status(exec, pids, cmd_count);
}
