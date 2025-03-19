/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 14:40:55 by mohaben-          #+#    #+#             */
/*   Updated: 2025/03/19 13:33:23 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	ft_err_exprt(char *cmd)
{
	ft_putstr_fd("minishell: export: `", 2);
	ft_putstr_fd(cmd, 2);
	ft_putstr_fd("': not a valid identifier\n", 2);
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
	ft_env_add_back(env, ft_env_new(var, value));
	free(new_var);
}

static void	ft_update_env(char *var, char *value, t_env **env)
{
	t_env	*current;

	if (!*env)
	{
		*env = ft_env_new(var, value);
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
	ft_env_add_back(env, ft_env_new(var, value));
}

static void	remove_next_arg(t_ast_node *ast, int i)
{
	int	j;

	free(ast->args[i + 1]);
	j = i + 1;
	while (ast->args[j])
	{
		ast->args[j] = ast->args[j + 1];
		j++;
	}
}

static void	merge_args(t_ast_node *ast, int i)
{
	char	*merged;

	merged = ft_strjoin(ast->args[i], ast->args[i + 1]);
	free(ast->args[i]);
	ast->args[i] = merged;
	remove_next_arg(ast, i);
}

static void	merge_split_assignments(t_ast_node *ast)
{
	int	i;

	i = 0;
	while (ast->args[i] && ast->args[i + 1])
	{
		if (!strchr(ast->args[i], '=') && ast->args[i + 1][0] == '=')
			merge_args(ast, i);
		else
			i++;
	}
}

static void	merge_incomplete_assignments(t_ast_node *ast)
{
	int		i;
	char	*equals_pos;

	i = 0;
	while (ast->args[i] && ast->args[i + 1])
	{
		equals_pos = strchr(ast->args[i], '=');
		if (equals_pos && equals_pos == ast->args[i] + strlen(ast->args[i]) - 1)
			merge_args(ast, i);
		else
			i++;
	}
}

void	ft_merge_quoted_args(t_ast_node *ast)
{
	merge_split_assignments(ast);
	merge_incomplete_assignments(ast);
}

static void	ft_set_var_val(char *arg, char **var, char **value)
{
	int	j;

	j = ft_get_index(arg, '=');
	*var = ft_substr(arg, 0, j - 1);
	*value = ft_substr(arg, j, ft_strlen(arg) - j);
}

static void	ft_free_export(char *var, char *value)
{
	free(var);
	free(value);
}

void	ft_export(t_ast_node *ast, t_exec *exec)
{
	char	*var;
	char	*value;
	int		i;

	ft_merge_quoted_args(ast);
	if (!ast->args[1] || !*ast->args[1])
		ft_print_export(*(exec->env));
	else
	{
		i = 0;
		while (ast->args[++i])
		{
			if (!ft_check_var_name(ast->args[i]))
			{
				ft_err_exprt(ast->args[i]);
				continue ;
			}
			if (ft_strchr(ast->args[i], '='))
			{
				ft_set_var_val(ast->args[i], &var, &value);
				ft_update_env(var, value, exec->env);
				ft_free_export(var, value);
			}
		}
	}
}
