/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 13:21:07 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/13 15:13:28 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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
