/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_helper2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 13:45:00 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/19 13:48:09 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	process_all_heredocs(t_ast_node *ast, t_exec *exec)
{
	t_redirect *redirect;

	if (!ast)
		return ;
	redirect = ast->redirects;
	while (redirect)
	{
		if (redirect->type == token_hrdc && redirect->heredoc_fd == -1)
			redirect->heredoc_fd = ft_handle_heredoc(redirect, exec);
		if (exec->exit_status == 1)
			return ;
		redirect = redirect->next;
	}
	if (ast->e_type == AST_PIPE || ast->e_type == AST_AND_AND || ast->e_type == AST_OR_OR)
	{
		process_all_heredocs(ast->left, exec);
		process_all_heredocs(ast->right, exec);
	}
	else if (ast->e_type == AST_SUBSHELL && ast->child)
		process_all_heredocs(ast->child, exec);
}

void	process_hrdc_pipes(t_ast_node **ast_pipes, int cmd_count, t_exec *exec)
{
	int	i;

	i = 0;
	while (i < cmd_count)
	{
		process_all_heredocs(ast_pipes[i], exec);
		if (exec->exit_status == 1)
			return ;
		i++;
	}
}

void	process_pipes(int pipes_fd[][2], int cmd_count, t_exec *exec)
{
	int	i;

	i = 0;
	while (i < cmd_count - 1)
	{
		if (pipe(pipes_fd[i]) < 0)
		{
			ft_putstr_fd(PIPE_ERROR, 2);
			exec->exit_status = 1;
			return ;
		}
		i++;
	}
}
