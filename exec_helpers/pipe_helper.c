/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_helper.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 13:40:49 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/21 16:43:59 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	count_pipe_cmd(t_ast_node *ast)
{
	int	count;

	if (!ast)
		return (0);
	if (ast->e_type != AST_PIPE)
		return (1);
	count = 0;
	if (ast->left)
	{
		if (ast->left->e_type == AST_PIPE)
			count += count_pipe_cmd(ast->left);
		else
			count += 1;
	}
	if (ast->right)
	{
		if (ast->right->e_type == AST_PIPE)
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
	if (ast->e_type != AST_PIPE)
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

void	init_pipe_data(t_pipe_data *pipe_data, int cmd_count,
	int (*pipes_fd)[2], int *pids)
{
	pipe_data->cmd_count = cmd_count;
	pipe_data->pipes_fd = pipes_fd;
	pipe_data->pids = pids;
}

void	exec_pipe_cmd(t_ast_node *ast, t_exec *exec)
{
	if (!ast)
		exit(1);
	ft_expand_wildcard(ast);
	if (ast->e_type == AST_SUBSHELL && ast->child)
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
