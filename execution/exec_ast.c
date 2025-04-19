/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_ast.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 11:46:54 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/19 11:46:56 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	execute_subshell(t_ast_node *ast, t_exec *exec)
{
	int	pid;
	int	status;

	if (!ast || !ast->child)
		return ;
	if (!ft_apply_redirect(ast, exec))
		return ;
	pid = fork();
	if (pid == -1)
	{
		ft_putstr_fd(FORK_ERROR, 2);
		return ;
	}
	if (pid == 0)
	{
		execute_ast(ast->child, exec);
		exit(exec->exit_status);
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		exec->exit_status = WEXITSTATUS(status);
	else
		exec->exit_status = 1;
}

void	ft_execute_or_and(t_ast_node *ast, t_exec *exec)
{
	if (ast->e_type == AST_AND_AND)
	{
		execute_ast(ast->left, exec);
		if (exec->exit_status == 0)
			execute_ast(ast->right, exec);
	}
	else if (ast->e_type == AST_OR_OR)
	{
		execute_ast(ast->left, exec);
		if (exec->exit_status != 0 && exec->exit_status != 130)
			execute_ast(ast->right, exec);
	}
}

void	expand_ast_args(t_ast_node *ast, t_exec *exec)
{
	t_redirect *redirect;
	t_redirect *processed_head = NULL;
	t_redirect *processed_tail = NULL;
	char *tmp;
	int i;

	i = 0;
	while (i < ast->arg_count)
	{
		if (ft_strchr(ast->args[i], '$') && ast->arg_quote_types[i] != AST_SQUOTES)
		{
			tmp = ast->args[i];
			ast->args[i] = ft_expand(tmp, exec);
			free(tmp);
		}
		i++;
	}
	redirect = ast->redirects;
	while (redirect)
	{
		t_redirect *new_redir = malloc(sizeof(t_redirect));
		if (!new_redir)
			return ;
		new_redir->type = redirect->type;
		new_redir->heredoc_fd = redirect->heredoc_fd;
		new_redir->quoted = redirect->quoted;
		new_redir->is_spaced = redirect->is_spaced;
		if (ft_strchr(redirect->file, '$') && redirect->type != token_hrdc && redirect->quoted != token_squote)
			new_redir->file = ft_expand(redirect->file, exec);
		else
			new_redir->file = ft_strdup(redirect->file);
		t_redirect *current = redirect->next;
		while (current && current->type == redirect->type && current->is_spaced == 0)
		{
			char *expanded;
			if (ft_strchr(current->file, '$') && redirect->type != token_hrdc && 
				current->quoted != token_squote)
			{
				expanded = ft_expand(current->file, exec);
			}
			else
			{
				expanded = ft_strdup(current->file);
			}
			tmp = new_redir->file;
			new_redir->file = ft_strjoin(tmp, expanded);
			free(tmp);
			free(expanded);
			current = current->next;
		}
		new_redir->next = NULL;
		if (!processed_head)
		{
			processed_head = new_redir;
			processed_tail = new_redir;
		}
		else
		{
			processed_tail->next = new_redir;
			processed_tail = new_redir;
		}
		if (current && current->type != redirect->type)
			redirect = current;
		else if (current)
			redirect = current;
		else
			redirect = NULL;
	}
	t_redirect *temp;
	redirect = ast->redirects;
	while (redirect)
	{
		temp = redirect;
		redirect = redirect->next;
		free(temp->file);
		free(temp);
	}
	ast->redirects = processed_head;
}

void	filter_ast_args(t_ast_node *ast, t_exec *exec)
{
	char	**new_args;
	int		*new_quote_types;
	int		i;
	int		j;

	if (!ast || !ast->args || !ast->arg_quote_types)
		return ;
	expand_ast_args(ast, exec);
	i = 0;
	j = 0;
	while (i < ast->arg_count)
	{
		if ((ast->args[i] && ast->args[i][0]) || ast->arg_quote_types[i])
			j++;
		i++;
	}
	if (j == 0)
	{
		new_args = malloc(2 * sizeof(char *));
		new_quote_types = malloc(sizeof(int));
		if (!new_args || !new_quote_types)
			return ;
		new_args[0] = ft_strdup("");
		new_quote_types[0] = 0;
		j = 1;
	}
	else
	{
		new_args = malloc((j + 1) * sizeof(char *));
		new_quote_types = malloc(j * sizeof(int));
		if (!new_args || !new_quote_types)
			return ;
		i = 0;
		j = 0;
		while (i < ast->arg_count)
		{
			if ((ast->args[i] && ast->args[i][0]) || ast->arg_quote_types[i])
			{
				new_args[j] = ft_strdup(ast->args[i]);
				new_quote_types[j] = ast->arg_quote_types[i];
				j++;
			}
			i++;
		}
	}
	new_args[j] = NULL;
	free_double_ptr(ast->args);
	free(ast->arg_quote_types);
	ast->args = new_args;
	ast->arg_quote_types = new_quote_types;
	ast->arg_count = j;
}

void	prepare_ast_args(t_ast_node *ast, t_exec *exec)
{
	int		*new_arg_is_spaced;
	char	**new_args;
	char	*tmp;
	int		i;
	int		j;

	filter_ast_args(ast, exec);
	if (!ast || !ast->args)
		return ;
	j = 1;
	i = 0;
	while (i < ast->arg_count)
	{
		if (ast->arg_is_spaced[i])
			j++;
		i++;
	}
	new_args = malloc((j + 1) * sizeof(char *));
	new_arg_is_spaced = malloc(j * sizeof(int));
	if (!new_args || !new_arg_is_spaced)
		return ;
	if (ast->arg_count > 0)
	{
		new_args[0] = ft_strdup(ast->args[0]);
		new_arg_is_spaced[0] = ast->arg_is_spaced[0];
		i = 1;
		j = 1;
		while (i < ast->arg_count)
		{
			if (ast->arg_is_spaced[i])
			{
				new_args[j] = ft_strdup(ast->args[i]);
				new_arg_is_spaced[j] = ast->arg_is_spaced[i];
				j++;
			}
			else
			{
				tmp = new_args[j - 1];
				new_args[j - 1] = ft_strjoin(tmp, ast->args[i]);
				free(tmp);
			}
			i++;
		}
	}
	new_args[j] = NULL;
	free_double_ptr(ast->args);
	free(ast->arg_is_spaced);
	ast->args = new_args;
	ast->arg_is_spaced = new_arg_is_spaced;
	ast->arg_count = j;
}

void	execute_ast(t_ast_node *ast, t_exec *exec)
{
	int	cmd_count;

	if (!ast)
		return ;
	prepare_ast_args(ast, exec);
	ft_expand_wildcard(ast);
	if (!ft_expand_redr_wild(ast, exec))
		return ;
	if (ast->e_type == AST_COMMAND)
		ft_execute_command(ast, exec);
	else if (ast->e_type == AST_PIPE)
	{
		cmd_count = count_pipe_cmd(ast);
		prepare_ast_args(ast->left, exec);
		prepare_ast_args(ast->right, exec);
		ft_execute_pipe(ast, exec, cmd_count);
	}
	else if (ast->e_type == AST_AND_AND || ast->e_type == AST_OR_OR)
		ft_execute_or_and(ast, exec);
	else if (ast->e_type == AST_SUBSHELL)
		execute_subshell(ast, exec);
	ft_restore_std_fd(exec);
}
