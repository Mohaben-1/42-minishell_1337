#include "minishell.h"

void	ft_handle_sigint(int sig)
{
 	(void)sig;
	ft_putchar_fd('\n', 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
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

void	ft_init_exec(t_exec	*exec, t_env **env, char **envp)
{
	exec->env = env;
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

	(void)ac;
	(void)av;
	signal(SIGINT, ft_handle_sigint);
	signal(SIGQUIT, SIG_IGN);
	env = ft_init_env(envp);
	exec.exit_status = 0;
	rl_catch_signals = 0;
	while (1)
	{
		input = readline("minishell> ");
		if (input && *input)
			add_history(input);
		if (!input)
		{
			ft_putstr_fd("exit\n", 1);
			exit(0);
		}
		if (!input[0])
		{
			free(input);
			continue;
		}
		tokens = ft_tokenize(input, &exec);
		ft_init_exec(&exec, &env, envp);
		ast = build_ast(tokens, &exec);
		execute_ast(ast, &exec);
		// print_ast(ast, 0);
		free(input);
	}
	return (0);
}
