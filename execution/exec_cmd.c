/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/16 13:12:17 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/12 18:27:45 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	ft_is_builtin(char *cmd)
{
	if (!ft_strcmp(cmd, "unset") || !ft_strcmp(cmd, "exit") || 
		!ft_strcmp(cmd, "pwd") || !ft_strcmp(cmd, "export") || 
		!ft_strcmp(cmd, "cd") || !ft_strcmp(cmd, "echo") || 
		!ft_strcmp(cmd, "env"))
		return (1);
	return (0);
}

void	execute_builtin(t_ast_node *ast, t_exec *exec)
{
	if (!ft_strcmp(ast->args[0], "unset"))
		ft_unset(ast->args, exec);
	else if (!ft_strcmp(ast->args[0], "exit"))
		ft_exit(ast, exec);
	else if (!ft_strcmp(ast->args[0], "pwd"))
		ft_pwd(exec);
	else if (!ft_strcmp(ast->args[0], "export"))
		ft_export(ast, exec);
	else if (!ft_strcmp(ast->args[0], "cd"))
		ft_cd(ast->args, exec);
	else if (!ft_strcmp(ast->args[0], "echo"))
		ft_echo(ast->args, exec);
	else if (!ft_strcmp(ast->args[0], "env"))
		ft_env(exec);
}

void	handle_cmd_exit(t_exec *exec, int status)
{
	if (WIFEXITED(status))
		exec->exit_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		exec->exit_status = 128 + WTERMSIG(status);
	else
		exec->exit_status = 1;
}

void	ft_execute_command(t_ast_node *ast, t_exec *exec)
{
	int			pid;
	int			status;

	if (!ast)
		return ;
	if (!ft_apply_redirect(ast, exec))
		return ;
	if (ast->args && ft_is_builtin(ast->args[0]))
			execute_builtin(ast, exec);
	else
	{
		pid = fork();
		if (pid == -1)
			return (ft_putstr_fd(FORK_ERROR, 2));
		if (pid == 0)
		{
			ft_exec_ve(ast, exec);
			exit(exec->exit_status);
		}
		waitpid(pid, &status, 0);
		handle_cmd_exit(exec, status);
	}
}
