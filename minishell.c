#include "minishell.h"

void	ft_handle_sigint(int sig)
{
 	(void)sig;
	write(1, "\nminishell> ", 12);
}

void	ft_clear_screen()
{
	write(1, "\033[H\033[2J", 7);
	write(1, "\nminishell> ", 12);
}

int main(int ac, char **av, char **envp)
{
	char	*input;
	int		pid;
	int		last_exit_status;
	int		status;
	t_token	token;

	(void)ac;
	(void)av;
	last_exit_status = 0;
	signal(SIGINT, ft_handle_sigint);
	signal(SIGQUIT, SIG_IGN);
	rl_catch_signals = 0;
	while (1)
	{
		input = readline("minishell> ");
		token = ft_tokenizer(input);
		if (token.token_cmd)
			printf("cmd\n");
		else if (token.token_append)
			printf("append\n");
		else if (token.token_here_doc)
			printf("heredoc\n");
		else if (token.token_pipe)
			printf("pipe\n");
		else if (token.token_redir_in)
			printf("in\n");
		else if (token.token_redir_out)
			printf("out\n");
		
		if (input && *input)
			add_history(input);
		if (!input)
		{
			write(1, "exit\n", 5);
			clear_history();
			exit(0);
		}
		if (!ft_strncmp(input, "exit", 4))
		{
			char	**splited_cmd = ft_split(input, ' ');
			
			if (splited_cmd[2])
			{
				free(input);
				write(1, "exit\n", 5);
				write(1, "minishell: exit: too many arguments\n", 36);
				last_exit_status = 1;
				continue ;
			}
			else
			{
				free(input);
				write(1, "exit\n", 5);
				clear_history();
				if (!splited_cmd[1])
					exit(0);
				else if (splited_cmd[1] && *splited_cmd[1] == '-' && ft_isdigit(*(splited_cmd[1] + 1)))
					exit(ft_atoi(splited_cmd[1]));
				else
				{
					write(1, "minishell: exit: xx: numeric argument required", 46);
					exit(255);
				}
			}
		}
		if (!ft_strncmp(input, "echo $", 6))
		{
			if (*(input + 6) == '?')
			{
				printf("%d\n", last_exit_status);
				free(input);
				continue;
			}
			if (getenv(input + 6))
				printf("%s\n", getenv(input +6));
			else
				write(1, "\n", 1);
			continue ;
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
		pid = fork();
		if (pid == 0)
		{
			ft_exec_cmd(input, envp);
		}
		else if (pid > 0)
		{
			waitpid(pid, &status, 0);
            if (WIFEXITED(status))
                last_exit_status = WEXITSTATUS(status);
			else
                last_exit_status = 1;
		}
		else
		{
			write(2, "Fork error\n", 11);
			last_exit_status = 1;
		}
		free(input);
	}
	return (0);
}
