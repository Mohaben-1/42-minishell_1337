/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 12:38:54 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/23 17:43:13 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell_bonus.h"

static void	free_resources(t_ast_node **ast_pipes, int (*pipes_fd)[2],
	int *pids)
{
	free(ast_pipes);
	if (pipes_fd)
		free(pipes_fd);
	free(pids);
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

void	ft_handle_pipes(t_pipe_data *pipe_data, t_ast_node **ast_pipes,
	t_exec *exec)
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

int	alloce_for_pipe(t_ast_node ***ast_pipes, int (**pipes_fd)[2]
	, int **pids, int cmd_count)
{
	*ast_pipes = NULL;
	*pipes_fd = NULL;
	*pids = NULL;
	*ast_pipes = malloc(cmd_count * sizeof(t_ast_node *));
	if (!*ast_pipes)
		return (0);
	if (cmd_count > 1)
	{
		*pipes_fd = malloc((cmd_count - 1) * sizeof(int [2]));
		if (!*pipes_fd)
		{
			free(*ast_pipes);
			return (0);
		}
	}
	*pids = malloc(cmd_count * sizeof(int));
	if (!*pids)
	{
		free(*ast_pipes);
		if (*pipes_fd)
			free(*pipes_fd);
		return (0);
	}
	return (1);
}

void	ft_execute_pipe(t_ast_node *ast, t_exec *exec, int cmd_count)
{
	t_ast_node	**ast_pipes;
	int			(*pipes_fd)[2];
	int			*pids;
	t_pipe_data	pipe_data;
	int			index;

	if (!ast || cmd_count <= 0)
		return ;
	if (!alloce_for_pipe(&ast_pipes, &pipes_fd, &pids, cmd_count))
	{
		exec->exit_status = 1;
		return ;
	}
	index = 0;
	collect_pipe_cmd(ast, ast_pipes, &index);
	process_hrdc_pipes(ast_pipes, cmd_count, exec);
	process_pipes(pipes_fd, cmd_count, exec);
	if (exec->exit_status == 1)
		return (free_resources(ast_pipes, pipes_fd, pids));
	init_pipe_data(&pipe_data, cmd_count, pipes_fd, pids);
	ft_handle_pipes(&pipe_data, ast_pipes, exec);
	ft_restore_std_fd(exec);
	handle_wait_status(exec, pids, cmd_count);
	close_heredoc_fds(ast_pipes, cmd_count);
	free_resources(ast_pipes, pipes_fd, pids);
}
