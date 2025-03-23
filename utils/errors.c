/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 13:21:07 by mohaben-          #+#    #+#             */
/*   Updated: 2025/03/23 13:53:52 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_error(char *err, int exit_status)
{
	perror(err);
	exit(exit_status);
}

void	ft_error_cmd(char *cmd, char **paths, int exit_status)
{
	ft_putstr_fd("minishell: ", 2);
	if (cmd)
		ft_putstr_fd(cmd, 2);
	ft_putstr_fd(": command not found\n", 2);
	free_split(paths);
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
