#include "minishell.h"

void	ft_handle_sigint(int sig)
{
 	(void)sig;
	ft_putstr_fd("\n\033[1;32mminishell> \033[0m", 1);
}

void	ft_clear_screen()
{
	ft_putstr_fd("\033[H\033[2J", 1);
	ft_putstr_fd("\n\033[1;32mminishell> \033[0m", 1);
}


void print_token_type(t_token_type type)
{
    switch (type)
    {
    case token_cmd:
        printf("Command: ");
        break;
    case token_pipe:
        printf("Pipe: ");
        break;
    case token_in:
        printf("Redirect In: ");
        break;
    case token_out:
        printf("Redirect Out: ");
        break;
    case token_hrdc:
        printf("Here Document: ");
        break;
    case token_appnd:
        printf("Append: ");
        break;
	case token_or:
        printf("OR: ");
        break;
	case token_paren_open:
        printf("Open: ");
        break;
	case token_paren_close:
        printf("Close: ");
        break;
	case token_dquote:
        printf("double quotes: ");
        break;
	case token_squote:
        printf("single quotes: ");
        break;
	case token_wildcard:
        printf("wildcard: ");
        break;
    default:
        printf("Unknown token type: ");
        break;
    }
}

void print_tokens(t_token_node *tokens)
{
    t_token_node *current = tokens;

    while (current != NULL)
    {
        print_token_type(current->type);
        printf("%s\n", current->data);
        current = current->next;
    }
}

void	ft_init_exec(t_exec	*exec, t_ast_node *ast, t_env **env, char **envp)
{
	exec->env = env;
	exec->ast = ast;
	exec->envp = envp;
	exec->std_fd[0] = -1;
	exec->std_fd[1] = -1;
}

int main(int ac, char **av, char **envp)
{
	t_token_node	*tokens;
	t_ast_node		*ast;
	t_env			*env;
	t_exec			exec;
	char			*input;
	// int				pid;
	// int				status;

	(void)ac;
	(void)av;
	signal(SIGINT, ft_handle_sigint);
	signal(SIGQUIT, SIG_IGN);
	env = ft_init_env(envp);
	exec.exit_status = 0;
	rl_catch_signals = 0;
	while (1)
	{
		input = readline("\033[1;32mminishell> \033[0m");
		if (input && *input)
			add_history(input);
		if (!input)
		{
			ft_putstr_fd("exit\n", 1);
			clear_history();
			exit(0);
		}
		if (input[0] == '\f' && input[1] == '\0')
		{
			ft_clear_screen();
			free(input);
			continue;
		}
		if (input[0] == '\0')
		{
			free(input);
			continue;
		}
		tokens = ft_tokenize(input);
		ast = build_ast(tokens);
		ft_init_exec(&exec, ast, &env, envp);
		execute_ast(ast, &exec);
		// if (tokens)
		// 	print_tokens(tokens);
        // if (ast)
        // {
        //     printf("AST Structure:\n");
        //     print_ast(ast, 0);
		// }
		// if (!ft_strncmp(input, "exit", 4))
		// 	ft_exit(input, &exec);
		// else if (!ft_strncmp(input, "echo $?", 7))
		// 	printf("%d\n", exec.exit_status);
		// else if (!ft_strncmp(input, "echo", 4))
		// 	ft_echo(input, &exec);
		// else if (!ft_strncmp(input, "cd", 2))
		// 	ft_cd(input, &exec);
		// else if (!ft_strncmp(input, "export", 6))
		// 	ft_export(input, &exec);
		// else if (!ft_strncmp(input, "unset", 5))
		// 	ft_unset(input, &exec);
		// else if (!ft_strncmp(input, "env", 3))
		// 	ft_env(*(exec.env));
		// else if (!ft_strncmp(input, "pwd", 3))
		// 	ft_pwd(&exec);
		// else
		// {
		// 	pid = fork();
		// 	if (pid == 0)
		// 	{
		// 		ft_exec_cmd(input, *(exec.env));
		// 	}
		// 	else if (pid > 0)
		// 	{
		// 		waitpid(pid, &status, 0);
		// 		if (WIFEXITED(status))
		// 			exec.exit_status = WEXITSTATUS(status);
		// 		else
		// 			exec.exit_status = 1;
		// 	}
		// 	else
		// 	{
		// 		write(2, "Fork error\n", 11);
		// 		exec.exit_status = 1;
		// 	}
		// }
		free(input);
	}
	return (0);
}
