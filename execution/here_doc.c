/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 12:24:51 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/19 12:21:45 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	heredoc_child_process(t_redirect *redr, int pipe_fd[2], t_exec *exec)
{
	char	*line;

	signal(SIGINT, heredoc_child_signal);
	close(pipe_fd[0]);
	while (1)
	{
		line = readline("> ");
		if (!line || !ft_strcmp(line, redr->file))
		{
			free(line);
			break ;
		}
		if (!redr->quoted)
			line = ft_expand(line, exec);
		ft_putstr_fd(line, pipe_fd[1]);
		ft_putchar_fd('\n', pipe_fd[1]);
		free(line);
	}
	close(pipe_fd[1]);
	exit(0);
}

int	ft_handle_heredoc(t_redirect *redr, t_exec *exec)
{
	void			(*original_sigint)(int);
	struct termios	original_term;
	int				pipe_fd[2];
	pid_t			pid;
	int				status;

	tcgetattr(STDIN_FILENO, &original_term);
	if (pipe(pipe_fd) == -1)
		return (-1);
	original_sigint = signal(SIGINT, SIG_IGN);
	pid = fork();
	if (pid == 0)
		heredoc_child_process(redr, pipe_fd, exec);
	else
	{
		close(pipe_fd[1]);
		waitpid(pid, &status, 0);
		tcsetattr(STDIN_FILENO, TCSANOW, &original_term);
		signal(SIGINT, original_sigint);
		if (WEXITSTATUS(status) == 1)
			return (close(pipe_fd[0]), exec->exit_status = 1, -1);
	}
	return (pipe_fd[0]);
}
