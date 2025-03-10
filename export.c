/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 14:40:55 by mohaben-          #+#    #+#             */
/*   Updated: 2025/03/10 21:43:28 by mohaben-         ###   ########.fr       */
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
	int	i;

	if (!env)
		return ;
	i = 0;
	while (env)
	{
		ft_putstr_fd("declare -x ", 1);
		ft_putstr_fd(env->var, 1);
		ft_putstr_fd(env->value, 1);
		ft_putchar_fd('\n', 1);
		env = env->next;
	}
}

static void	ft_update_env(char **var_splited, t_env **env)
{
	t_env	*current;

	current = *env;
	while (current->next)
	{
		if (current->var == var_splited[0])
		{
			current->value = var_splited[1];
			return ;
		}
		current = current->next;
	}
	ft_env_add_back(env, ft_env_new(ft_strdup(var_splited[0]), var_splited[1]));
}

void	ft_export(char *cmd, t_env **env)
{
	char	**cmd_splited;
	char	**var_splited;
	int		i;

	cmd_splited = ft_split(cmd, ' ');
	if (!cmd_splited[1])
		ft_print_export(env);
	else
	{
		i = 1;
		while (cmd_splited[i])
		{
			if (ft_strchr(cmd_splited[i], '='))
			{
				var_splited = ft_split(cmd_splited[i], '=');
				if (!ft_check_var_name(var_splited[0]))
					return (free_split(cmd_splited), ft_err_exprt(var_splited[0], var_splited));
				ft_update_env(var_splited, env);
				free_split(var_splited);
			}
			else
			{
				if (!ft_check_var_name(cmd_splited[i]))
					return (ft_err_exprt(cmd_splited[i], cmd_splited));
			}
			i++;
		}
	}
	free_split(cmd_splited);
}
