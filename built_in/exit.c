/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 15:02:10 by mohaben-          #+#    #+#             */
/*   Updated: 2025/03/15 16:41:29 by mohaben-         ###   ########.fr       */
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

int	ft_is_numeric_argument(char *str)
{
	int i;

	if (!str || !*str)
		return (0);
	if (*str == '-' || *str == '+')
		str++;
	i = 0;
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}

void	ft_exit(char *input, t_exec *exec)
{
	char	**cmd;

	cmd = ft_split(input, ' ');
	if (ft_count_split(cmd) > 2)
	{
		ft_putstr_fd("minishell: exit: too many arguments\n", 1);
		exec->exit_status = 1;
		ft_exit_free(input, cmd, 0, 0);
	}
	else
	{
		ft_putstr_fd("exit\n", 1);
		clear_history();
		if (!cmd[1])
			ft_exit_free(input, cmd, 0, 1);
		else if (ft_is_numeric_argument(cmd[1]) || !ft_strcmp(cmd[1], "9223372036854775807"))
			ft_exit_free(input, cmd, ft_atoi(cmd[1]), 1);
		else
			ft_err_exit(input, cmd);
	}
}
