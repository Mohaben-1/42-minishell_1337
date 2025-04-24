/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 17:36:34 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/23 17:36:37 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell_bonus.h"

void	ft_error(char *err, int exit_status)
{
	perror(err);
	exit(exit_status);
}

void	ft_error_cmd(t_env *env, t_ast_node *ast, char **paths, int exit_status)
{
	ft_putstr_fd("minishell: ", 2);
	if (exit_status == 127 || ft_get_env(env, "PATH"))
	{
		if (ast->args[0])
			ft_putstr_fd(ast->args[0], 2);
		ft_putstr_fd(": command not found\n", 2);
	}
	else
		perror(ast->args[0]);
	free_double_ptr(paths);
	free_ast_node(ast);
	exit(exit_status);
}

void	ft_error_file(char *file, t_exec *exec)
{
	exec->exit_status = 1;
	ft_putstr_fd("minishell: ", 2);
	perror(file);
}

void	ft_error_file_expand(char *file, t_exec *exec)
{
	exec->exit_status = 1;
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(file, 2);
	ft_putstr_fd(": ambiguous redirect\n", 2);
}

void	ft_err_exprt(char *cmd, int *err_flag)
{
	*err_flag = 1;
	ft_putstr_fd("minishell: export: `", 2);
	ft_putstr_fd(cmd, 2);
	ft_putstr_fd("': not a valid identifier\n", 2);
}
