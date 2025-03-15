/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 14:40:55 by mohaben-          #+#    #+#             */
/*   Updated: 2025/03/15 11:06:03 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	ft_err_exprt(char *cmd, char **cmd_splited)
{
	ft_putstr_fd("minishell: export: `", 2);
	ft_putstr_fd(cmd, 2);
	ft_putstr_fd("': not a valid identifier\n", 2);
	free_split(cmd_splited);
}

int	ft_check_append(char *cmd)
{
	int	i;

	i = 0;
	while (cmd[i])
	{
		if (cmd[i] == '+' && cmd[i + 1] == '=')
			return (1);
		i++;
	}
	return (0);
}

int	ft_check_var_name(char *var)
{
	if (!var || !*var)
		return (0);
	if (!ft_isalpha(var[0]) && var[0] != '_')
		return (0);
	while (*var)
	{
		if (*var == '+')
			return (*(var + 1) == '=');
		if (*var == '=')
			break ;
		if (!ft_isalnum(*var) && *var != '_')
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
		ft_putstr_fd("=\"", 1);
		ft_putstr_fd(env->value, 1);
		ft_putstr_fd("\"\n", 1);
		env = env->next;
	}
}

void	ft_append_env(char **var_splited, t_env **env)
{
	t_env	*current;
	char	*var;
	char	*value;

	current = *env;
	var = ft_strtrim(var_splited[0], "+");
	while (current)
	{

		if (!ft_strcmp(current->var, var))
		{
			value = ft_strjoin(current->value, var_splited[1]);
			free(current->value);
			free(var);
			current->value = value;
			return ;
		}
		if (!current->next)
			break ;
		current = current->next;
	}
	ft_env_add_back(env, ft_env_new(var, var_splited[1]));
	free(var);
}

static void	ft_update_env(char **var_splited, t_env **env)
{
	t_env	*current;

	if (!*env)
	{
		*env = ft_env_new(var_splited[0], var_splited[1]);
		return ;
	}
	if (ft_strchr(var_splited[0], '+'))
		return (ft_append_env(var_splited, env));
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
	char	**var_split;
	int		i;

	cmd_split = ft_split(cmd, ' ');
	if (!cmd_split[1])
		ft_print_export(*env);
	else
	{
		i = 0;
		while (cmd_split[++i])
		{
			if (!ft_check_var_name(cmd_split[i]))
				return (ft_err_exprt(cmd_split[i], cmd_split));
			if (ft_strchr(cmd_split[i], '='))
			{
				var_split = ft_split(cmd_split[i], '=');
				ft_update_env(var_split, env);
				free_split(var_split);
			}
		}
	}
	free_split(cmd_split);
}
