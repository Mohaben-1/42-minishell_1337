/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 14:40:55 by mohaben-          #+#    #+#             */
/*   Updated: 2025/03/11 11:07:24 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	ft_err_exprt(char *cmd, char **cmd_splited)
{
	ft_putstr_fd("minishell: export: `", 2);
	ft_putstr_fd(cmd, 2);
	ft_putstr_fd("': not a valid identifier\n", 2);
	free_split(cmd_splited);
}

static int	ft_check_var_name(char *var)
{
	if (!var || !*var)
		return (0);
	if (!ft_isalpha(var[0]) && var[0] != '_')
		return (0);
	while (*var)
	{
		if (!ft_isalpha(*var) && !ft_isdigit(*var) && *var != '_')
			return (0);
		var++;
	}
	return (1);
}

static	void	ft_print_export(t_env *env)
{
	if (!env)
		return ;
	while (env)
	{
		ft_putstr_fd("declare -x ", 1);
		ft_putstr_fd(env->var, 1);
		ft_putchar_fd('=', 1);
		ft_putstr_fd(env->value, 1);
		ft_putchar_fd('\n', 1);
		env = env->next;
	}
}

static void	ft_update_env(char **var_splited, t_env **env)
{
	t_env	*current;

	if (!*env)
	{
		*env = ft_env_new(var_splited[0], var_splited[1]);
		return ;
	}
	current = *env;
	while (current)
	{
		if (!ft_strcmp(current->var, var_splited[0]))
		{
			free(current->value);
			current->value = ft_strdup(var_splited[1]);
			return ;
		}
		if (!current->next)
			break ;
		current = current->next;
	}
	ft_env_add_back(env, ft_env_new(var_splited[0], var_splited[1]));
}

void	ft_export(char *cmd, t_env **env)
{
	char	**cmd_split;
	char	**var;
	int		i;

	cmd_split = ft_split(cmd, ' ');
	if (!cmd_split[1])
		ft_print_export(*env);
	else
	{
		i = 0;
		while (cmd_split[++i])
		{
			if (ft_strchr(cmd_split[i], '='))
			{
				var = ft_split(cmd_split[i], '=');
				if (!ft_check_var_name(var[0]))
					return (free_split(cmd_split), ft_err_exprt(var[0], var));
				ft_update_env(var, env);
				free_split(var);
			}
			else if (!ft_check_var_name(cmd_split[i]))
				return (ft_err_exprt(cmd_split[i], cmd_split));
		}
	}
	free_split(cmd_split);
}
