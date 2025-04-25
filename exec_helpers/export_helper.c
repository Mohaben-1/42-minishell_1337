/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_helper.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 11:37:18 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/25 17:39:10 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_free_export(char *var, char *value)
{
	free(var);
	free(value);
}

void	export_exit_status(t_exec *exec, int err_flag)
{
	if (!err_flag)
		exec->exit_status = 0;
	else
		exec->exit_status = 1;
}

void	ft_print_export(t_env *env)
{
	if (!env)
		return ;
	while (env)
	{
		ft_putstr_fd("declare -x ", 1);
		ft_putstr_fd(env->var, 1);
		if (env->printed)
		{
			ft_putstr_fd("=\"", 1);
			ft_putstr_fd(env->value, 1);
			ft_putstr_fd("\"\n", 1);
		}
		else
		{
			ft_putstr_fd("\n", 1);
		}
		env = env->next;
	}
}

void	ft_append_env(char *var, char *value, t_env **env)
{
	t_env	*current;
	char	*new_var;
	char	*new_value;

	current = *env;
	new_var = ft_strtrim(var, "+");
	while (current)
	{
		if (!ft_strcmp(current->var, new_var))
		{
			new_value = ft_strjoin(current->value, value);
			free(current->value);
			free(new_var);
			current->value = new_value;
			return ;
		}
		if (!current->next)
			break ;
		current = current->next;
	}
	ft_env_add_back(env, ft_env_new(var, value, 1));
	free(new_var);
}

void	ft_update_env(char *var, char *value, t_env **env, int printed)
{
	t_env	*current;

	if (!*env)
	{
		*env = ft_env_new(var, value, printed);
		return ;
	}
	if (ft_strchr(var, '+'))
		return (ft_append_env(var, value, env));
	current = *env;
	while (current)
	{
		if (!ft_strcmp(current->var, var))
		{
			free(current->value);
			current->value = ft_strdup(value);
			return ;
		}
		if (!current->next)
			break ;
		current = current->next;
	}
	ft_env_add_back(env, ft_env_new(var, value, printed));
}
