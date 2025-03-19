/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 11:10:22 by mohaben-          #+#    #+#             */
/*   Updated: 2025/03/19 15:49:35 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	execute_subshell(t_ast_node *ast, t_exec *exec)
{
	int	pid;
	int	status;

	if (!ast || !ast->child)
		return ;
	pid = fork();
	if (pid == -1)
	{
		ft_putstr_fd("minishell: fork: Resource temporarily unavailable\n", 2);
		return ;
	}
	if (pid == 0)
	{
		ft_apply_redirect(ast->redirects, exec);
		execute_ast(ast->child, exec);
		ft_restore_std_fd(exec);
		exit(exec->exit_status);
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		exec->exit_status = WEXITSTATUS(status);
	else
		exec->exit_status = 1;
}

void	execute_ast(t_ast_node *ast, t_exec *exec)
{
	if (!ast)
		return ;
	ft_apply_redirect(ast->redirects, exec);
	if (ast->type == AST_COMMAND)
		execute_command(ast, exec);
	else if (ast->type == AST_PIPE)
		ft_execute_pipe(ast, exec);
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
	ft_restore_std_fd(exec);
}
