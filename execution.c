/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 13:18:42 by mohaben-          #+#    #+#             */
/*   Updated: 2025/03/06 16:46:10 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*ft_get_path(char **envp)
{
	int		i;

	if (!envp)
		return (NULL);
	i = 0;
	while (envp[i])
	{
		if (!ft_strncmp(envp[i], "PATH=", 5))
			return (envp[i] + 5);
		i++;
	}
	return (NULL);
}

void	ft_exec_cmd(char *cmd, char **envp)
{
	char	**cmd_splited;
	char	**paths;
	char	*path;
	char	*cmd_path;
	int		i;

	signal(SIGQUIT, SIG_DFL);
	cmd_splited = ft_split(cmd, ' ');
	if (cmd_splited[0] && !access(cmd_splited[0], X_OK))
		execve(cmd_splited[0], cmd_splited, envp);
	path = ft_get_path(envp);
	paths = ft_split(path, ':');
	i = -1;
	while (paths[++i] && cmd[0] != '.')
	{
		path = ft_strjoin(paths[i], "/");
		cmd_path = ft_strjoin(path, cmd_splited[0]);
		free(path);
		if (cmd_path && !access(cmd_path, X_OK))
			execve(cmd_path, cmd_splited, envp);
		free(cmd_path);
	}
	p_error_cmd(cmd_splited, paths, 127);
}
