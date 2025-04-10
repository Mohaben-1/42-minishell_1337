/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 15:02:10 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/10 15:27:52 by mohaben-         ###   ########.fr       */
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

static int	check_numeric_overflow(char *str)
{
	long long	nb;
	int			sign;
	int			i;

	nb = 0;
	sign = 1;
	i = 0;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}
	while (str[i] && str[i] >= '0' && str[i] <= '9')
	{
		if ((sign == 1 && (nb > LONG_MAX / 10 || 
			(nb == LONG_MAX / 10 && str[i] - '0' > LONG_MAX % 10))) ||
			(sign == -1 && (nb > -(LONG_MIN / 10) || 
			(nb == -(LONG_MIN / 10) && str[i] - '0' > -(LONG_MIN % 10)))))
			return (0);
		nb = nb * 10 + (str[i] - '0');
		i++;
	}
	return (1);
}

void	ft_exit(t_ast_node *ast, t_exec *exec)
{
	if (ast->arg_count > 2)
	{
		ft_putstr_fd("exit\n", 2);
		if (!ft_is_numeric_argument(ast->args[1]))
			ft_err_exit(ast->args);
		ft_putstr_fd("minishell: exit: too many arguments\n", 2);
		exec->exit_status = 1;
		return ;
	}
	ft_putstr_fd("exit\n", 1);
	if (!ast->args[1])
		exit(0);
	if (!ft_is_numeric_argument(ast->args[1]) || !check_numeric_overflow(ast->args[1]))
		ft_err_exit(ast->args);
	exit((unsigned char)ft_atoi(ast->args[1]));
}
