/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 13:43:48 by mohaben-          #+#    #+#             */
/*   Updated: 2025/03/13 10:50:57 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_pwd(t_env *env)
{
	char	*pwd;

	pwd = getcwd(NULL, 0);
	if (pwd)
	{
		ft_putstr_fd(pwd, 1);
		ft_putchar_fd('\n', 1);
		free(pwd);
	}
	else
	{
		pwd = ft_get_val_env(env, "PWD");
		if (pwd)
		{
			ft_putstr_fd(pwd, 1);
			ft_putchar_fd('\n', 1);
		}
		else
		{
			ft_putstr_fd("minishell: pwd:  No such file or directory", 2);
		}
	}
}
