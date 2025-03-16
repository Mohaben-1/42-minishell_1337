/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fork.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/16 12:29:27 by mohaben-          #+#    #+#             */
/*   Updated: 2025/03/16 14:07:05 by mohaben-         ###   ########.fr       */
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

int	ft_env_size(t_env *env)
{
	int	size;

	size = 0;
	while (env)
	{
		size++;
		env = env->next;
	}
	return (size);
}

static char	*ft_strjoin_env(char *s1, char *s2)
{
	char	*join;
	size_t	i;
	size_t	j;

	if (!s1 && !s2)
		return (NULL);
	if (!s2)
		s2 = "";
	join = malloc((ft_strlen(s1) + ft_strlen(s2) + 1) * sizeof(char));
	if (!join)
		return (NULL);
	i = 0;
	j = 0;
	while (s1[j])
		join[i++] = s1[j++];
	join[i++] = '=';
	j = 0;
	while (s2[j])
		join[i++] = s2[j++];
	join[i] = '\0';
	return (join);
}

char	**ft_set_envp(t_env *env)
{
	char	**envp;
	int		size;
	int		i;

	if (!env)
		return (NULL);
	size = ft_env_size(env);
	envp = malloc((size + 1) * sizeof(char *));
	if (!envp)
		return (NULL);
	i = 0;
	while (i < size)
	{
		envp[i] = ft_strjoin_env(env->var, env->value);
		env = env->next;
		i++;
	}
	envp[i] = NULL;
	return (envp);
}

void	ft_exec_cmd(char *cmd, t_env *env)
{
	char	**cmd_splited;
	char	**paths;
	char	*path;
	char	*cmd_path;
	char	**envp;
	int		i;

	signal(SIGQUIT, SIG_DFL);
	envp = ft_set_envp(env);
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
	ft_error_cmd(cmd_splited, paths, 127);
}
