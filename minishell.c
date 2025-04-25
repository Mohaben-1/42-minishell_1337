/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 15:08:07 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/25 18:29:18 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_init_exec(t_exec *exec, t_env **env, char **envp)
{
	exec->env = env;
	exec->envp = envp;
	exec->std_fd[0] = -1;
	exec->std_fd[1] = -1;
}

void	process_input(char *input, t_exec *exec, t_env **env, char **envp)
{
	t_token_node	*tokens;
	t_ast_node		*ast;

	tokens = ft_tokenize(input, exec);
	free(input);
	ft_init_exec(exec, env, envp);
	ast = build_ast(tokens, exec);
	if (tokens)
		free_token_list(tokens);
	if (ast)
		execute_ast(ast, exec);
	free_ast_node(ast);
}

int	check_empty_input(char *input)
{
	if (!input)
	{
		rl_clear_history();
		ft_putstr_fd("exit\n", 1);
		return (1);
	}
	if (!input[0])
	{
		free(input);
		return (2);
	}
	return (0);
}

int	main_loop(t_env **env, t_exec *exec, char **envp)
{
	char	*input;
	int		check;

	while (1)
	{
		input = readline("minishell> ");
		if (input && *input)
			add_history(input);
		check = check_empty_input(input);
		if (check == 1)
			return (exec->exit_status);
		if (check == 2)
			continue ;
		process_input(input, exec, env, envp);
	}
	return (exec->exit_status);
}

int	main(int ac, char **av, char **envp)
{
	t_env	*env;
	t_exec	exec;
	int		exit_status;

	handle_main_sigs(ac, av);
	env = ft_init_env(envp);
	exec.exit_status = 0;
	exit_status = main_loop(&env, &exec, envp);
	free_env(env);
	return (exit_status);
}
