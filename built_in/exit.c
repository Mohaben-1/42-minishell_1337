/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 15:02:10 by mohaben-          #+#    #+#             */
/*   Updated: 2025/03/17 13:03:37 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	ft_err_exit(char **args)
{
	ft_putstr_fd("minishell: exit: ", 2);
	ft_putstr_fd(args[1], 1);
	ft_putstr_fd(": numeric argument required\n", 2);
	exit(255);
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

void	ft_exit(char **args, t_exec *exec)
{
	if (ft_count_args(args) > 2)
	{
		ft_putstr_fd("exit\n", 2);
		if (!ft_is_numeric_argument(args[1]))
			ft_err_exit(args);
		ft_putstr_fd("minishell: exit: too many arguments\n", 2);
		exec->exit_status = 1;
	}
	else
	{
		ft_putstr_fd("exit\n", 1);
		clear_history();
		if (!args[1])
			exit(0);
		else if (ft_is_numeric_argument(args[1]) || !ft_strcmp(args[1], "9223372036854775807"))
			exit(ft_atoi(args[1]));
		else
			ft_err_exit(args);
	}
}
