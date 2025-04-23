/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_ast.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 11:46:54 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/23 15:19:48 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	execute_subshell(t_ast_node *ast, t_exec *exec)
{
	int	pid;
	int	status;

	if (!ast || !ast->child)
		return ;
	if (!ft_apply_redirect(ast, exec))
		return ;
	pid = fork();
	if (pid == -1)
	{
		ft_putstr_fd(FORK_ERROR, 2);
		return ;
	}
	if (pid == 0)
	{
		execute_ast(ast->child, exec);
		exit(exec->exit_status);
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		exec->exit_status = WEXITSTATUS(status);
	else
		exec->exit_status = 1;
}

void	ft_execute_or_and(t_ast_node *ast, t_exec *exec)
{
	if (ast->e_type == AST_AND_AND)
	{
		execute_ast(ast->left, exec);
		if (exec->exit_status == 0)
			execute_ast(ast->right, exec);
	}
	else if (ast->e_type == AST_OR_OR)
	{
		execute_ast(ast->left, exec);
		if (exec->exit_status != 0 && exec->exit_status != 130)
			execute_ast(ast->right, exec);
	}
}

void	execute_ast(t_ast_node *ast, t_exec *exec)
{
	int	cmd_count;

	if (!ast)
		return ;
	prepare_ast_args(ast, exec);
	ft_expand_wildcard(ast);
	if (!ft_expand_redr_wild(ast, exec))
		return ;
	if (ast->e_type == AST_COMMAND)
		ft_execute_command(ast, exec);
	else if (ast->e_type == AST_PIPE)
	{
		cmd_count = count_pipe_cmd(ast);
		prepare_ast_args(ast->left, exec);
		prepare_ast_args(ast->right, exec);
		ft_execute_pipe(ast, exec, cmd_count);
	}
	else if (ast->e_type == AST_AND_AND || ast->e_type == AST_OR_OR)
		ft_execute_or_and(ast, exec);
	else if (ast->e_type == AST_SUBSHELL)
		execute_subshell(ast, exec);
	ft_restore_std_fd(exec);
}
