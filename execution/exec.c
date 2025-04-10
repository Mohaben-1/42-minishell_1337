/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 11:10:22 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/09 21:15:11 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	execute_subshell(t_ast_node *ast, t_exec *exec)
{
	int	pid;
	int	status;

	if (!ast || !ast->child)
		return ;
	ft_apply_redirect(ast->child, exec);
	pid = fork();
	if (pid == -1)
	{
		ft_putstr_fd("minishell: fork: Resource temporarily unavailable\n", 2);
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
	if (ast->type == AST_AND_AND)
	{
		execute_ast(ast->left, exec);
		if (exec->exit_status == 0)
			execute_ast(ast->right, exec);
	}
	else if (ast->type == AST_OR_OR)
	{
		execute_ast(ast->left, exec);
		if (exec->exit_status != 0)
			execute_ast(ast->right, exec);
	}
}

void	expand_ast_args(t_ast_node *ast, t_exec *exec)
{
	t_redirect	*redirect;
	char		*tmp;
	int			i;

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
		if (ft_strchr(redirect->file, '$') && redirect->type != token_hrdc && redirect->quoted != token_squote)
		{
			tmp = redirect->file;
			redirect->file = ft_expand(tmp, exec);
			free(tmp);
		}
		redirect = redirect->next;
	}
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
	i = 1;
	j = 1;
	// while (i < ast->arg_count)
	// {
	// 	printf("space: %d\n", ast->arg_is_spaced[i]);
	// 	i++;
	// }
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
	new_args[0] = ft_strdup(ast->args[0]);
	new_arg_is_spaced[0] = ast->arg_is_spaced[0];
	i = 1;
	j = 1;
	if (i < ast->arg_count)
	{
		new_args[j] = ft_strdup(ast->args[i]);
		new_arg_is_spaced[i] = ast->arg_is_spaced[i];
		i++;
		while (i < ast->arg_count)
		{
			if (ast->arg_is_spaced[i])
			{
				j++;
				new_args[j] = ft_strdup(ast->args[i]);
				new_arg_is_spaced[i] = ast->arg_is_spaced[i];
			}
			else
			{
				tmp = new_args[j];
				new_args[j] = ft_strjoin(tmp, ast->args[i]);
				free(tmp);
			}
			i++;
		}
		j++;
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
	if (ast->type == AST_COMMAND)
		execute_command(ast, exec);
	else if (ast->type == AST_PIPE)
	{
		cmd_count = count_pipe_cmd(ast);
		ft_execute_pipe(ast, exec, cmd_count);
	}
	else if (ast->type == AST_AND_AND || ast->type == AST_OR_OR)
		ft_execute_or_and(ast, exec);
	else if (ast->type == AST_SUBSHELL)
		execute_subshell(ast, exec);
	ft_restore_std_fd(exec);
}
