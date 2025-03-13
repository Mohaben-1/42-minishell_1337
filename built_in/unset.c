/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 11:59:56 by mohaben-          #+#    #+#             */
/*   Updated: 2025/03/13 10:51:01 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	ft_err_unset(char *var)
{
	ft_putstr_fd("minishell: unset: `", 2);
	ft_putstr_fd(var, 2);
	ft_putstr_fd("': not a valid identifier\n", 2);
}

int	ft_check_var_exist(t_env *env, char *var)
{
	while (env)
	{
		if (!ft_strcmp(env->var, var))
			return (1);
		env = env->next;
	}
	return (0);
}

void	ft_del_var(t_env **env, char *var)
{
	t_env	*current;
	t_env	*previous;

	current = *env;
	previous = NULL;
	while (current)
	{
		if (!ft_strcmp(current->var, var))
		{
			if (!previous)
				*env = current->next;
			else
				previous->next = current->next;
			free(current->var);
			free(current->value);
			free(current);
		}
		previous = current;
		current = current->next;
	}
}

void	ft_unset(char *cmd, t_env **env)
{
	char	**cmd_split;
	int		i;

	if (!env || !*env)
		return ;
	cmd_split = ft_split(cmd, ' ');
	if (!cmd_split[1])
		return (free_split(cmd_split));
	i = 1;
	while (cmd_split[i])
	{
		if (!ft_check_var_name(cmd_split[i]))
			ft_err_unset(cmd_split[i]);
		else if (ft_check_var_exist(*env, cmd_split[i]))
			ft_del_var(env, cmd_split[i]);
		i++;
	}
	free_split(cmd_split);
}
