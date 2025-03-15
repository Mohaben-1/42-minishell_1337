/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 13:21:07 by mohaben-          #+#    #+#             */
/*   Updated: 2025/03/15 11:06:38 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	p_error(char *err, int exit_status)
{
	perror(err);
	exit(exit_status);
}

void	p_error_cmd(char **cmd, char **paths, int exit_status)
{
	write(2, "minishell: ", 11);
	if (cmd[0])
		write(2, cmd[0], ft_strlen(cmd[0]));
	else
		write(2, " ", 1);
	write(2, ": command not found\n", 20);
	free_split(cmd);
	free_split(paths);
	exit(exit_status);
}
