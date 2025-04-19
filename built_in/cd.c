/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 13:18:11 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/19 10:54:54 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	ft_error_cd(char *old_path, char **args, t_exec *exec)
{
	exec->exit_status = 1;
	ft_putstr_fd("minishell: cd: ", 2);
	perror(args[1]);
	free(old_path);
}

static void	ft_set_path(t_env *env, char **args, char **new_path)
{
	char	*path;
	char	*tmp;

	if (args[1] && args[1][0] == '~')
	{
		path = ft_substr(args[1], 1, ft_strlen(args[1]));
		tmp = path;
		path = ft_strjoin(ft_get_env(env, "HOME"), path);
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

static void	ft_cd_home(char **args, t_exec *exec, char *old_pwd)
{
	char	*home;

	home = ft_get_env(*(exec->env), "HOME");
	if (!home)
	{
		exec->exit_status = 1;
		ft_putstr_fd("minishell: cd: HOME not set\n", 2);
		ft_free_cd(args, old_pwd, NULL);
		return ;
	}
	if (chdir(home) != 0)
		return (ft_error_cd(old_pwd, args, exec));
	ft_set_env(*(exec->env), "OLDPWD", old_pwd);
	home = getcwd(NULL, 0);
	if (home)
	{
		ft_set_env(*(exec->env), "PWD", home);
		free(home);
	}
	else
		ft_putstr_fd("minishell: cd: failed to update PWD\n", 2);
	exec->exit_status = 0;
	free(old_pwd);
}

void	ft_cd(char **args, t_exec *exec)
{
	char	*new_pwd;
	char	*old_pwd;
	char	*pwd;

	old_pwd = getcwd(NULL, 0);
	if (!old_pwd)
		return ;
	if (!args[1] || !ft_strcmp(args[1], "~"))
		return (ft_cd_home(args, exec, old_pwd));
	new_pwd = NULL;
	ft_set_path(*(exec->env), args, &new_pwd);
	if (!new_pwd || chdir(new_pwd) != 0)
		return (ft_error_cd(old_pwd, args, exec));
	ft_set_env(*(exec->env), "OLDPWD", old_pwd);
	pwd = getcwd(NULL, 0);
	if (pwd)
	{
		ft_set_env(*(exec->env), "PWD", pwd);
		free(pwd);
	}
	else
		ft_putstr_fd("minishell: cd: failed to update PWD\n", 2);
	exec->exit_status = 0;
	ft_free_cd(args, old_pwd, new_pwd);
}
