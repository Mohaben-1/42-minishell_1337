/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 14:40:55 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/25 17:31:33 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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

static void	ft_set_var_val(char *arg, char **var, char **value)
{
	int	j;

	j = ft_get_index(arg, '=');
	*var = ft_substr(arg, 0, j - 1);
	*value = ft_substr(arg, j, ft_strlen(arg) - j);
}

void	ft_export_helper(char	*arg, char **var, char **value, t_exec *exec)
{
	int	printed;

	if (ft_strchr(arg, '='))
	{
		printed = 1;
		ft_set_var_val(arg, var, value);
	}
	else
	{
		*var = ft_strdup(arg);
		*value = ft_strdup("");
		printed = 0;
	}
	ft_update_env(*var, *value, exec->env, printed);
	ft_free_export(*var, *value);
}

void	ft_export(t_ast_node *ast, t_exec *exec)
{
	char	*var;
	char	*value;
	int		err_flag;
	int		i;

	err_flag = 0;
	if (ast->arg_count == 1)
		ft_print_export(*(exec->env));
	else
	{
		i = 0;
		while (ast->args[++i])
		{
			if (!ft_check_var_name(ast->args[i]))
			{
				ft_err_exprt(ast->args[i], &err_flag);
				continue ;
			}
			// if (ft_strchr(ast->args[i], '='))
			ft_export_helper(ast->args[i], &var, &value, exec);
		}
	}
	export_exit_status(exec, err_flag);
}
