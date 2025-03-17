/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 16:44:18 by mohaben-          #+#    #+#             */
/*   Updated: 2025/03/17 11:14:32 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	ft_is_flag(char *arg)
{
	if (*arg != '-')
		return (0);
	arg++;
	if (*arg != 'n')
		return (0);
	while (*arg && *arg != ' ')
	{
		if (*arg != 'n')
			return (0);
		arg++;
	}
	return (1);
}

void	ft_echo(char **args, t_exec *exec)
{
	char	*err_msg;
	int		flag;
	int		i;
	
	err_msg = "minishell: echo: write error: Bad file descriptor\n";
	if (write(1, NULL, 0) == -1)
		return (exec->exit_status = 1, ft_putstr_fd(err_msg, 2));
	flag = 1;
	i = 1;
	while (args[i] && ft_is_flag(args[i]))
	{
		flag = 0;
		i++;
	}
	while (args[i])
	{
		ft_putstr_fd(args[i], 1);
		if (args[i + 1])
			ft_putchar_fd(' ', 1);
		i++;
	}
	if (flag)
		ft_putchar_fd('\n', 1);
}
