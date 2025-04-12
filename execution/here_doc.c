/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 12:24:51 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/12 20:18:40 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	heredoc_signal_handler(int sig)
{
	if (sig == SIGINT)
	{
		g_heredoc_signal = 1;
		rl_replace_line("", 0);
	}
}

static int	heredoc_error(t_exec *exec, void (*old_handler)(int))
{
	ft_putstr_fd(PIPE_ERROR, 2);
	exec->exit_status = 1;
	signal(SIGINT, old_handler);
	return (-1);
}

static void	process_heredoc_line(char *line, t_redirect *redr, 
								int pipe_fd, t_exec *exec)
{
	char	*line_expanded;

	if (!redr->quoted)
		line_expanded = ft_expand(line, exec);
	else
		line_expanded = ft_strdup(line);
	ft_putstr_fd(line_expanded, pipe_fd);
	ft_putchar_fd('\n', pipe_fd);
	free(line_expanded);
}

int	ft_handle_heredoc(t_redirect *redr, t_exec *exec)
{
	char			*line;
	int				pipe_fd[2];
	void			(*old_handler)(int);

	old_handler = signal(SIGINT, heredoc_signal_handler);
	g_heredoc_signal = 0;
	if (pipe(pipe_fd) == -1)
		return (heredoc_error(exec, old_handler));
	while (1)
	{
		line = readline("> ");
		if (!line || g_heredoc_signal || !ft_strcmp(line, redr->file))
			break ;
		process_heredoc_line(line, redr, pipe_fd[1], exec);
		free(line);
	}
	free(line);
	close(pipe_fd[1]);
	signal(SIGINT, old_handler);
	if (g_heredoc_signal)
	{
		exec->exit_status = 1;
		return (close(pipe_fd[0]), -1);
	}
	return (pipe_fd[0]);
}
