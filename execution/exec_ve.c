/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_ve.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 16:29:44 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/13 12:21:20 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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

void	handle_sig_exec_ve(int sig)
{
	if (sig == SIGINT)
	{
		ft_putchar_fd('\n', 1);
		exit(130);
	}
}

void ft_exec_ve(t_ast_node *ast, t_exec *exec)
{
	struct stat st;
	char **paths;
	char *path;
	char *cmd;
	char **envp;
	int i;

	signal(SIGINT, handle_sig_exec_ve);
	signal(SIGQUIT, SIG_DFL); 
	if ((!ast->args[0] || !ast->args[0][0]) && !ast->arg_quote_types[0])
		exit(0);
	envp = ft_set_envp(*(exec->env));
	cmd = ast->args[0];
	if (cmd && ft_strchr(cmd, '/'))
	{
		if (stat(cmd, &st) == 0)
		{
			if (S_ISDIR(st.st_mode))
			{
				ft_putstr_fd("minishell: ", 2);
				ft_putstr_fd(cmd, 2);
				ft_putstr_fd(": is a directory\n", 2);
				exit(126);
			}
			else if (access(cmd, X_OK) != 0)
			{
				ft_putstr_fd("minishell: ", 2);
				ft_putstr_fd(cmd, 2);
				ft_putstr_fd(": Permission denied\n", 2);
				exit(126);
			}
			execve(cmd, ast->args, envp);
		}
	}
	path = ft_get_path(envp);
	if (!path)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(ast->args[0], 2);
		ft_putstr_fd(": No such file or directory\n", 2);
		exit(127);
	}
	paths = ft_split(path, ':');
	i = -1;
	while (paths[++i] && ast->args[0][0] != '.')
	{
		path = ft_strjoin(paths[i], "/");
		cmd = ft_strjoin(path, ast->args[0]);
		free(path);
		if (stat(cmd, &st) == 0)
		{
			if (S_ISDIR(st.st_mode))
			{
				free(cmd);
				continue;
			}
			else if (access(cmd, X_OK) == 0)
				execve(cmd, ast->args, envp);
		}
		free(cmd);
	}
	ft_error_cmd(*(exec->env), ast->args[0], paths, 127);
}
