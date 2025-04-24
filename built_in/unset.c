/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 11:59:56 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/24 13:29:57 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	free_env_entry(t_env *to_free)
{
	free(to_free->var);
	free(to_free->value);
	free(to_free);
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

void	ft_del_var(t_env **env, char *var)
{
	t_env	*current;
	t_env	*prev;
	t_env	*to_free;

	current = *env;
	prev = NULL;
	while (current)
	{
		if (ft_strcmp(current->var, var) == 0)
		{
			to_free = current;
			if (prev)
				prev->next = current->next;
			else
				*env = current->next;
			current = current->next;
			free_env_entry(to_free);
		}
		else
		{
			prev = current;
			current = current->next;
		}
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
			ft_del_var(exec->env, args[i]);
		i++;
	}
	if (!err_flag)
		exec->exit_status = 0;
	else
		exec->exit_status = 1;
}
