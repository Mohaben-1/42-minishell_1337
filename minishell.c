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
	char	*cmd;
	int		pid;
	int		last_exit_status;
	int		status;

	(void)ac;
	(void)av;
	last_exit_status = 0;
	signal(SIGINT, ft_handle_sigint);
	signal(SIGQUIT, SIG_IGN);
	rl_catch_signals = 0;
	while (1)
	{
		cmd = readline("minishell> ");
		if (cmd && *cmd)
			add_history(cmd);
		if (!cmd)
		{
			write(1, "exit\n", 5);
			clear_history();
			exit(0);
		}
		if (!ft_strncmp(cmd, "exit", 4))
		{
			char	**splited_cmd = ft_split(cmd, ' ');
			
			if (splited_cmd[2])
			{
				free(cmd);
				write(1, "exit\n", 5);
				write(1, "minishell: exit: too many arguments\n", 36);
				last_exit_status = 1;
				continue ;
			}
			else
			{
				free(cmd);
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
		if (!ft_strncmp(cmd, "echo $", 6))
		{
			if (*(cmd + 6) == '?')
            {
                printf("%d\n", last_exit_status);
                free(cmd);
                continue;
            }
			if (getenv(cmd + 6))
				printf("%s\n", getenv(cmd +6));
			else
				write(1, "\n", 1);
			continue ;
		}
		if (cmd[0] == '\f' && cmd[1] == '\0')
		{
			ft_clear_screen();
			free(cmd);
			continue;
		}
		if (cmd[0] == '\0')
		{
			free(cmd);
			continue;
		}
		pid = fork();
		if (pid == 0)
		{
			ft_exec_cmd(cmd, envp);
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
		free(cmd);
	}
	return (0);
}
