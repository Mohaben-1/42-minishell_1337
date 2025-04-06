/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 12:24:51 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/06 20:28:11 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	handle_heredoc_node(t_ast_node *ast, t_exec *exec)
{
	t_redirect	*redirect;

	redirect = ast->redirects;
	while (redirect)
	{
		if (redirect->type == token_hrdc)
			redirect->heredoc_fd = ft_handle_heredoc(redirect, exec);
		redirect = redirect->next;
	}
}

void	ft_handle_heredoc_pipe(t_ast_node *ast, t_exec *exec)
{
	if (!ast)
		return ;
	handle_heredoc_node(ast, exec);
	if (ast->type == AST_PIPE)
	{
		ft_handle_heredoc_pipe(ast->left, exec);
		ft_handle_heredoc_pipe(ast->right, exec);
	}
}
