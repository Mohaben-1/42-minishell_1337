/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 16:44:18 by mohaben-          #+#    #+#             */
/*   Updated: 2025/03/13 12:17:03 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	ft_is_flag(char *cmd)
{
	if (*cmd != '-')
		return (0);
	cmd++;
	if (*cmd != 'n')
		return (0);
	while (*cmd && *cmd != ' ')
	{
		if (*cmd != 'n')
			return (0);
		cmd++;
	}
	return (1);
}

void	ft_echo(char *input)
{
	char	**cmd_split;
	int		flag;
	int		i;
	
	if (write(1, NULL, 0) == -1)
		return (ft_putstr_fd("minishell: echo: write error: Bad file descriptor\n", 2));
	cmd_split = ft_split(input, ' ');
	if (!cmd_split)
		return ;
	flag = 1;
	i = 1;
	while (cmd_split[i] && ft_is_flag(cmd_split[i]))
	{
		flag = 0;
		i++;
	}
	while (cmd_split[i])
	{
		ft_putstr_fd(cmd_split[i], 1);
		if (cmd_split[i + 1])
			ft_putchar_fd(' ', 1);
		i++;
	}
	if (flag)
		ft_putchar_fd('\n', 1);
	free_split(cmd_split);
}
