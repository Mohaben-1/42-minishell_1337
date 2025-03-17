/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 13:18:11 by mohaben-          #+#    #+#             */
/*   Updated: 2025/03/17 11:10:55 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	ft_error_cd(char *old_path, char **args)
{
	ft_putstr_fd("minishell: cd: ", 2);
	perror(args[1]);
	free(old_path);
}

static void	ft_set_path(t_env *env,char **args, char **new_path)
{
	char	*path;
	char	*tmp;

	if (args[1] && args[1][0] == '~')
	{
		path = ft_substr(args[1], 1, ft_strlen(args[1]));
		tmp = path;
		path = ft_strjoin(ft_get_val_env(env, "HOME"), path);
		free(tmp);
		*new_path = path;
	}
	else
		*new_path = args[1];
}

static void	ft_free_cd(char **args, char *old_path, char *new_path)
{
	free(old_path);
	if (args[1] && args[1][0] == '~' && args[1][1])
		free(new_path);
}

void	ft_cd(char **args, t_exec *exec)
{
	char	*new_path;
	char	*old_path;
	char	*pwd;

	if (!exec->env)
		return (exec->exit_status = 1, ft_putstr_fd("minishell: cd: HOME not set\n", 2));
	old_path = getcwd(NULL, 0);
	new_path = NULL;
	if (!args[1] || !ft_strcmp(args[1], "~"))
	{
		new_path = ft_get_val_env(*(exec->env), "HOME");
		if (!new_path)
			return (ft_free_cd(args, old_path, new_path));
	}
	else
		ft_set_path(*(exec->env), args, &new_path);
	if (!new_path || chdir(new_path) != 0)
		return (ft_error_cd(old_path, args));
	ft_set_val_env(*(exec->env), "OLDPWD", old_path);
	pwd = getcwd(NULL, 0);
	if (pwd)
	{
		ft_set_val_env(*(exec->env), "PWD", pwd);
		free(pwd);
	}
	else
		ft_putstr_fd("minishell: cd: failed to update PWD\n", 2);
	ft_free_cd(args, old_path, new_path);
}
