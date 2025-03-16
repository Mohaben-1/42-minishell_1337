/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 11:10:22 by mohaben-          #+#    #+#             */
/*   Updated: 2025/03/16 15:30:04 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	execute_ast(t_ast_node *ast, t_exec *exec)
{
	if (!ast)
		return ;
	if (ast->type == AST_COMMAND)
		execute_command(ast, exec);
	// else if (ast->type == AST_PIPE)
	// 	execute_pipe(ast, exec);
	// else if (ast->type == AST_AND_AND)
	// 	execute_and_and(ast, exec);
	// else if (ast->type == AST_OR_OR)
	// 	execute_or_or(ast, exec);
	// else if (ast->type == AST_SUBSHELL)
	// 	execute_subshell(ast, exec);
}

// void execute_command(t_ast_node *node, t_exec *exec);
// void execute_pipe(t_ast_node *node, t_exec *exec);
// void execute_and_and(t_ast_node *node, t_exec *exec);
// void execute_or_or(t_ast_node *node, t_exec *exec);
// void execute_subshell(t_ast_node *node, t_exec *exec);