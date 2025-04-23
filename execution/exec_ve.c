/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_ve.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 16:29:44 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/22 20:53:57 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	exec_ve_sigs(void)
{
	signal(SIGINT, handle_sig_exec_ve);
	signal(SIGQUIT, SIG_DFL); 
}

void	exec_stat_absolute(t_ast_node *ast, char *cmd, char **envp)
{
	struct stat	st;

	if (stat(cmd, &st) == 0)
	{
		if (S_ISDIR(st.st_mode))
		{
			ft_putstr_fd("minishell: ", 2);
			ft_putstr_fd(cmd, 2);
			ft_putstr_fd(": is a directory\n", 2);
			free_double_ptr(envp);
			free_ast_node(ast);
			exit(126);
		}
		else if (access(cmd, X_OK) != 0)
		{
			ft_putstr_fd("minishell: ", 2);
			ft_putstr_fd(cmd, 2);
			ft_putstr_fd(": Permission denied\n", 2);
			free_double_ptr(envp);
			free_ast_node(ast);
			exit(126);
		}
		execve(cmd, ast->args, envp);
	}
}

static void	check_ast_args(t_ast_node *ast)
{
	exec_ve_sigs();
	if ((!ast->args || !ast->args[0] || !ast->args[0][0])
		&& (!ast->arg_quote_types || !ast->arg_quote_types[0]))
		exit(0);
}

void	exec_stat_relative(t_ast_node *ast, char **paths, char **envp)
{
	struct stat	st;
	char		*cmd;
	char		*path;
	int			i;

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
				continue ;
			}
			else if (access(cmd, X_OK) == 0)
				execve(cmd, ast->args, envp);
		}
		free(cmd);
	}
}

void	ft_exec_ve(t_ast_node *ast, t_exec *exec)
{
	char	**paths;
	char	*path;
	char	*cmd;
	char	**envp;

	check_ast_args(ast);
	envp = ft_set_envp(*(exec->env));
	cmd = ast->args[0];
	if (cmd && ft_strchr(cmd, '/'))
		exec_stat_absolute(ast, cmd, envp);
	path = ft_get_env(*(exec->env), "PATH");
	if (!path)
		exec_path_err(ast, envp);
	paths = ft_split(path, ':');
	exec_stat_relative(ast, paths, envp);
	free_double_ptr(envp);
	ft_error_cmd(*(exec->env), ast, paths, 127);
}
