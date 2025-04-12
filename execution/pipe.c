/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 12:38:54 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/12 15:22:14 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	count_pipe_cmd(t_ast_node *ast)
{
	int	count;

	if (!ast)
		return (0);
	if (ast->type != AST_PIPE)
		return (1);
	count = 0;
	if (ast->left)
	{
		if (ast->left->type == AST_PIPE)
			count += count_pipe_cmd(ast->left);
		else
			count += 1;
	}
	if (ast->right)
	{
		if (ast->right->type == AST_PIPE)
			count += count_pipe_cmd(ast->right);
		else
			count += 1;
	}
	return (count);
}

void	collect_pipe_cmd(t_ast_node *ast, t_ast_node **ast_pipes, int *index)
{
	if (!ast)
		return ;
	if (ast->type != AST_PIPE)
	{
		ast_pipes[*index] = ast;
		(*index)++;
		return ;
	}
	if (ast->left)
		collect_pipe_cmd(ast->left, ast_pipes, index);
	if (ast->right)
		collect_pipe_cmd(ast->right, ast_pipes, index);
}

void	close_pipes_fd(int pipes_fd[][2], int count)
{
	int	i;

	i = 0;
	while (i < count - 1)
	{
		close(pipes_fd[i][0]);
		close(pipes_fd[i][1]);
		i++;
	}
}

void	process_all_heredocs(t_ast_node *ast, t_exec *exec)
{
	t_redirect *redirect;

	if (!ast)
		return ;
	redirect = ast->redirects;
	while (redirect)
	{
		if (redirect->type == token_hrdc && redirect->heredoc_fd == -1)
			redirect->heredoc_fd = ft_handle_heredoc(redirect, exec);
		redirect = redirect->next;
	}
	if (ast->type == AST_PIPE || ast->type == AST_AND_AND || ast->type == AST_OR_OR)
	{
		process_all_heredocs(ast->left, exec);
		process_all_heredocs(ast->right, exec);
	}
	else if (ast->type == AST_SUBSHELL && ast->child)
		process_all_heredocs(ast->child, exec);
}

void	exec_pipe_cmd(t_ast_node *ast, t_exec *exec)
{
	if (!ast)
		exit(1);
	ft_expand_wildcard(ast);
	if (ast->type == AST_SUBSHELL && ast->child)
	{
		if (ft_apply_redirect(ast, exec))
			execute_ast(ast->child, exec);
		exit(exec->exit_status);
	}
	if (!ast->args)
		exit(1);
	if (ft_is_builtin(ast->args[0]))
	{
		if (ft_apply_redirect(ast, exec))
			execute_builtin(ast, exec);
		exit(exec->exit_status);
	}
	else
	{
		if (ft_apply_redirect(ast, exec))
			ft_exec_ve(ast, exec);
		exit(exec->exit_status);
	}
}

void	handle_wait_status(t_exec *exec, int *pids, int count)
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
	else
		exec->exit_status = 1;
}

void	ft_execute_pipe(t_ast_node *ast, t_exec *exec, int cmd_count)
{
	t_ast_node	*ast_pipes[cmd_count];
	int			pipes_fd[cmd_count - 1][2];
	int			pids[cmd_count];
	int			index;
	int			i;

	if (!ast)
		return ;
	index = 0;
	collect_pipe_cmd(ast, ast_pipes, &index);
	i = 0;
	while (i < cmd_count)
		process_all_heredocs(ast_pipes[i++], exec);
	i = 0;
	while (i < cmd_count - 1)
	{
		if (pipe(pipes_fd[i]) < 0)
		{
			ft_putstr_fd("minishell: pipe: Resource unavailable\n", 2);
			exec->exit_status = 1;
			return ;
		}
		i++;
	}
	i = 0;
	while (i < cmd_count)
	{
		pids[i] = fork();
		if (pids[i] == -1)
		{
			ft_putstr_fd("minishell: fork: Resource temporarily unavailable\n", 2);
			exec->exit_status = 1;
			return ;
		}
		if (pids[i] == 0)
		{
			if (i > 0)
				dup2(pipes_fd[i - 1][0], 0);
			if (i < cmd_count - 1)
				dup2(pipes_fd[i][1], 1);
			close_pipes_fd(pipes_fd, cmd_count);
			exec_pipe_cmd(ast_pipes[i], exec);
			exit(exec->exit_status);
		}
		i++;
	}
	close_pipes_fd(pipes_fd, cmd_count);
	handle_wait_status(exec, pids, cmd_count);
}
