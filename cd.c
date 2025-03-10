/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 13:18:11 by mohaben-          #+#    #+#             */
/*   Updated: 2025/03/10 14:27:22 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	ft_error_cd(char *old_path, char **cmd_splited)
{
	ft_putstr_fd("minishell: cd: ", 2);
	perror(cmd_splited[1]);
	free(old_path);
	free_split(cmd_splited);
}

static void	ft_set_path(char **cmd_splited, char **new_path)
{
	char	*path;
	char	*tmp;

	if (cmd_splited[1] && cmd_splited[1][0] == '~')
	{
		path = ft_substr(cmd_splited[1], 1, ft_strlen(cmd_splited[1]));
		tmp = path;
		path = ft_strjoin(getenv("HOME"), path);
		free(tmp);
		*new_path = path;
	}
	else
		*new_path = cmd_splited[1];
}

static void	ft_free_cd(char **cmd_splited, char *old_path, char *new_path)
{
	free(old_path);
	if (cmd_splited[1] && cmd_splited[1][0] == '~' && cmd_splited[1][1])
		free(new_path);
	free_split(cmd_splited);
}

void	ft_cd(char *cmd)
{
	char	*new_path;
	char	*old_path;
	char	**cmd_splited;

	old_path = getcwd(NULL, 0);
	cmd_splited = ft_split(cmd, ' ');
	if (!cmd_splited)
		return ;
	if (!cmd_splited[1] || !ft_strcmp(cmd_splited[1], "~"))
		new_path = getenv("HOME");
	else if (!ft_strcmp(cmd_splited[1], "-"))
	{
		ft_putstr_fd(getenv("OLDPWD"), 1);
		write(1, "\n", 1);
		new_path = getenv("OLDPWD");
	}
	else
		ft_set_path(cmd_splited, &new_path);
	if (!new_path || chdir(new_path) != 0)
		return (ft_error_cd(old_path, cmd_splited));
	setenv("OLDPWD", old_path, 1);
	setenv("PWD", getcwd(NULL, 0), 1);
	ft_free_cd(cmd_splited, old_path, new_path);
}
