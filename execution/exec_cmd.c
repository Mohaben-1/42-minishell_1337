/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/16 13:12:17 by mohaben-          #+#    #+#             */
/*   Updated: 2025/03/19 16:50:43 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	ft_is_builtin(char *cmd)
{
	if (!ft_strcmp(cmd, "unset") || !ft_strcmp(cmd, "exit") || 
		!ft_strcmp(cmd, "pwd") || !ft_strcmp(cmd, "export") || 
		!ft_strcmp(cmd, "cd") || !ft_strcmp(cmd, "echo") || 
		!ft_strcmp(cmd, "env"))
		return (1);
	return (0);
}

void	handle_heredoc(char *delimiter)
{
	char	*line;
	int		pipe_fd[2];

	if (pipe(pipe_fd) == -1)
    {
		ft_putstr_fd("minishell: pipe: Resource temporarily unavailable", 2);
		return ;
    }
	// delimiter = ft_strjoin(delimiter, "\n");
	while (1)
	{
		line = readline("> ");
		if (!line && !ft_strcmp(line, delimiter))
		{
			free(line);
			break ;
		}
		ft_putstr_fd(line, pipe_fd[1]);
		ft_putchar_fd('\n', pipe_fd[1]);
		free(line);
	}
	close(pipe_fd[1]);
	dup2(pipe_fd[0], 0);
	close(pipe_fd[0]);
}

void	ft_apply_redirect(t_redirect *redirect, t_exec *exec)
{
	t_redirect	*redr;
	int			fd;

	exec->std_fd[0] = dup(STDIN_FILENO);
	exec->std_fd[1] = dup(STDOUT_FILENO);
	redr = redirect;
	while (redr)
	{
		if (redr->type == token_in)
		{
			fd = open(redr->file, O_RDONLY);
			if (fd == -1)
				return (ft_error_file(redr->file, exec));
			dup2(fd, STDIN_FILENO);
			close(fd);
		}
		else if (redr->type == token_out)
		{
			fd = open(redr->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (fd == -1)
				return (ft_error_file(redr->file, exec));
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		else if (redr->type == token_appnd)
		{
			fd = open(redr->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
			if (fd == -1)
				return (ft_error_file(redr->file, exec));
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		else if (redr->type == token_hrdc)
			handle_heredoc(redr->file);
		redr = redr->next;
	}
}

void	ft_restore_std_fd(t_exec *exec)
{
	if (exec->std_fd[0] != -1)
	{
		dup2(exec->std_fd[0], STDIN_FILENO);
		close(exec->std_fd[0]);
		exec->std_fd[0] = -1;
	}
	if (exec->std_fd[1] != -1)
	{
		dup2(exec->std_fd[1], STDOUT_FILENO);
		close(exec->std_fd[1]);
		exec->std_fd[1] = -1;
	}
}

void	execute_builtin(t_ast_node *node, t_exec *exec)
{
	// ft_apply_redirect(node->redirects, exec);
	if (!ft_strcmp(node->args[0], "unset"))
		ft_unset(node->args, exec);
	else if (!ft_strcmp(node->args[0], "exit"))
		ft_exit(node->args, exec);
	else if (!ft_strcmp(node->args[0], "pwd"))
		ft_pwd(exec);
	else if (!ft_strcmp(node->args[0], "export"))
		ft_export(node, exec);
	else if (!ft_strcmp(node->args[0], "cd"))
		ft_cd(node->args, exec);
	else if (!ft_strcmp(node->args[0], "echo"))
		ft_echo(node->args, exec);
	else if (!ft_strcmp(node->args[0], "env"))
		ft_env(*(exec->env));
	// ft_restore_std_fd(exec);
}

void	execute_command(t_ast_node *node, t_exec *exec)
{
	int	pid;
	int	status;

	if (ft_is_builtin(node->args[0]))
		execute_builtin(node, exec);
	else
	{
		pid = fork();
		if (pid == -1)
		{
			ft_putstr_fd("minishell: fork: Resource temporarily unavailable", 2);
			return ;
		}
		if (pid == 0)
		{
			ft_exec_ve(node, exec);
			exit(exec->exit_status);
		}
		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
			exec->exit_status = WEXITSTATUS(status);
		else
			exec->exit_status = 1;
	}
}
