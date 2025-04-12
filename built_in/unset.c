/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 11:59:56 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/12 18:46:01 by mohaben-         ###   ########.fr       */
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

int	ft_check_var_name_unset(char *var)
{
	if (!var || !*var)
		return (0);
	if (!ft_isalpha(var[0]) && var[0] != '_')
		return (0);
	while (*var)
	{
		if (!ft_isalnum(*var) && *var != '_')
			return (0);
		var++;
	}
	return (1);
}

void	ft_del_var(t_env *env, char *var)
{
	t_env	*current;
	t_env	*previous;

	current = env;
	previous = NULL;
	while (current)
	{
		if (!ft_strcmp(current->var, var))
		{
			if (!previous)
				env = current->next;
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

void	ft_unset(char **args, t_exec *exec)
{
	int		i;
	int		err_flag;

	if (!exec->env)
		return ;
	i = 1;
	err_flag = 0;
	while (args[i])
	{
		if (!ft_check_var_name_unset(args[i]))
		{
			ft_err_unset(args[i]);
			err_flag = 1;
		}
		else if (ft_check_var_exist(*(exec->env), args[i]))
			ft_del_var(*(exec->env), args[i]);
		i++;
	}
	if (!err_flag)
		exec->exit_status = 0;
	else
		exec->exit_status = 1;
}
