/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 15:02:10 by mohaben-          #+#    #+#             */
/*   Updated: 2025/03/13 12:23:24 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	ft_exit_free(char *input, char **splited, int status, int is_exit)
{
	free_split(splited);
	if (is_exit)
	{
		free(input);
		exit(status);
	}
}

static void	ft_err_exit(char *input, char **cmd_split)
{
	ft_putstr_fd("minishell: exit: ", 1);
	ft_putstr_fd(cmd_split[1], 1);
	ft_putstr_fd(": numeric argument required", 1);
	ft_exit_free(input, cmd_split, 255, 1);
}

void	ft_exit(char *input, int *exit_status)
{
	char	**cmd;

	cmd = ft_split(input, ' ');
	if (ft_count_split(cmd) > 2)
	{
		ft_putstr_fd("minishell: exit: too many arguments\n", 1);
		*exit_status = 1;
		ft_exit_free(input, cmd, 0, 0);
	}
	else
	{
		ft_putstr_fd("exit\n", 1);
		clear_history();
		if (!cmd[1])
			ft_exit_free(input, cmd, 0, 1);
		else if ((cmd[1] && (*cmd[1] == '-' || *cmd[1] == '+') && ft_isdigit(*(cmd[1] + 1)))
			|| !ft_strcmp(cmd[1], "9223372036854775807"))
			ft_exit_free(input, cmd, ft_atoi(cmd[1]), 1);
		else
			ft_err_exit(input, cmd);
	}
}
