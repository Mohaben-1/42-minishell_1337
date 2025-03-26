/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 12:31:49 by mohaben-          #+#    #+#             */
/*   Updated: 2025/03/26 16:36:50 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// int	ft_apply_redirect(t_redirect *redirect, t_exec *exec)
// {
// 	t_redirect	*redr;
// 	int			fd;

// 	exec->std_fd[0] = dup(STDIN_FILENO);
// 	exec->std_fd[1] = dup(STDOUT_FILENO);
// 	redr = redirect;
// 	while (redr)
// 	{
// 		if (redr->type == token_hrdc)
// 			handle_heredoc(redr, exec);
// 		redr = redr->next;
// 	}
// 	redr = redirect;
// 	while (redr)
// 	{
// 		if (redr->type == token_in)
// 		{
// 			fd = open(redr->file, O_RDONLY);
// 			if (fd == -1)
// 				return (ft_error_file(redr->file, exec), 0);
// 			dup2(fd, STDIN_FILENO);
// 			close(fd);
// 		}
// 		else if (redr->type == token_out)
// 		{
// 			if (!redr->file || !*redr->file)
// 				return (ft_error_file_expand("$", exec), 0);
// 			fd = open(redr->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
// 			if (fd == -1)
// 				return (ft_error_file(redr->file, exec)), 0;
// 			dup2(fd, STDOUT_FILENO);
// 			close(fd);
// 		}
// 		else if (redr->type == token_appnd)
// 		{
// 			fd = open(redr->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
// 			if (fd == -1)
// 				return (ft_error_file(redr->file, exec), 0);
// 			dup2(fd, STDOUT_FILENO);
// 			close(fd);
// 		}
// 		redr = redr->next;
// 	}
// 	return (1);
// }


// void	handle_heredoc(t_redirect *redr, t_exec *exec)
// {
// 	char	*line;
// 	char	*expand_line;
// 	int		pipe_fd[2];

// 	if (pipe(pipe_fd) == -1)
// 	{
// 		exec->exit_status = 1;
// 		ft_putstr_fd("minishell: pipe: Resource temporarily unavailable", 2);
// 		return ;
// 	}
// 	while (1)
// 	{
// 		line = readline("> ");
// 		if (!line)
// 			break ;
// 		if (line && !ft_strcmp(line, redr->file))
// 		{
// 			free(line);
// 			break ;
// 		}
// 		if (!redr->quoted)
// 			expand_line = ft_expand(line, exec);
// 		else
// 			expand_line = ft_strdup(line);
// 		ft_putstr_fd(expand_line, pipe_fd[1]);
// 		ft_putchar_fd('\n', pipe_fd[1]);
// 		free(line);
// 		free(expand_line);
// 	}
// 	close(pipe_fd[1]);
// 	dup2(pipe_fd[0], 0);
// 	close(pipe_fd[0]);
// }




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














int	ft_apply_redirect(t_redirect *redirect, t_exec *exec)
{
	t_redirect	*redr;
	int			fd;

	exec->std_fd[0] = dup(0);
	exec->std_fd[1] = dup(1);
	redr = redirect;
	while (redr)
    {
		if (redr->type == token_hrdc && redr->heredoc_fd != -1)
		{
			if (dup2(redr->heredoc_fd, 0) == -1)
			{
				ft_putstr_fd("Error redirecting heredoc\n", 2);
				return (0);
			}
			close(redr->heredoc_fd);
			redr->heredoc_fd = -1;
		}
		redr = redr->next;
	}
	redr = redirect;
	while (redr)
	{
		if (redr->type == token_in)
		{
			fd = open(redr->file, O_RDONLY);
			if (fd == -1)
				return (ft_error_file(redr->file, exec), 0);
			dup2(fd, 0);
			close(fd);
		}
		else if (redr->type == token_out)
		{
			if (!redr->file || !*redr->file)
				return (ft_error_file_expand("$", exec), 0);
			fd = open(redr->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (fd == -1)
				return (ft_error_file(redr->file, exec)), 0;
			dup2(fd, 1);
			close(fd);
		}
		else if (redr->type == token_appnd)
		{
			fd = open(redr->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
			if (fd == -1)
				return (ft_error_file(redr->file, exec), 0);
			dup2(fd, 1);
			close(fd);
		}
		redr = redr->next;
	}
	return (1);
}



int	ft_handle_heredoc(t_redirect *redr, t_exec *exec)
{
	char	*expand_line;
	char	*line;
	int		pipe_fd[2];

	if (pipe(pipe_fd) == -1)
	{
		exec->exit_status = 1;
		ft_putstr_fd("minishell: pipe: Resource temporarily unavailable", 2);
		return (-1);
	}
	while (1)
	{
		line = readline("> ");
		if (!line)
			break ;
		if (!ft_strcmp(line, redr->file))
		{
			free(line);
			break ;
		}
		if (!redr->quoted)
			expand_line = ft_expand(line, exec);
		else
			expand_line = ft_strdup(line);
		ft_putstr_fd(expand_line, pipe_fd[1]);
		ft_putchar_fd('\n', pipe_fd[1]);
		free(line);
		free(expand_line);
	}
	close(pipe_fd[1]);
	return (pipe_fd[0]);
}

// void	ft_handle_all_heredoc(t_ast_node *ast, t_exec *exec)
// {
// 	t_redirect *redirect;

// 	if (!ast)
// 		return ;
// 	redirect = ast->redirects;
// 	while (redirect)
// 	{
// 		if (redirect->type == token_hrdc)
// 			redirect->heredoc_fd = ft_handle_heredoc(redirect, exec);
// 		redirect = redirect->next;
// 	}
// 	if (ast->type == AST_PIPE)
// 	{
// 		ft_handle_all_heredoc(ast->left, exec);
// 		ft_handle_all_heredoc(ast->right, exec);
// 	}
// }

