#include "minishell.h"

char	*ft_get_path(char **envp)
{
	int		i;
	
	if (!envp)
		return (NULL);
	i = 0;
	while (envp[i])
	{
		if (!ft_strncmp(envp[i], "PATH=", 5))
			return (envp[i] + 5);
		i++;
	}
	return (NULL);
}

void	p_error_cmd(char **cmd, char **paths, int exit_status)
{
	write(2, "minishell: ", 11);
	if (cmd[0])
		write(2, cmd[0], ft_strlen(cmd[0]));
	else
		write(2, " ", 1);
	write(2, ": command not found\n", 20);
	free_split(cmd);
	free_split(paths);
	exit(exit_status);
}

void	ft_exec_cmd(char *cmd, char **envp)
{
	char	**cmd_splited;
	char	**paths;
	char	*path;
	char	*cmd_path;
	int		i;

	cmd_splited = ft_split(cmd, ' ');
	if (cmd_splited[0] && !access(cmd_splited[0], X_OK))
		execve(cmd_splited[0], cmd_splited, envp);
	path = ft_get_path(envp);
	paths = ft_split(path, ':');
	i = -1;
	while (paths[++i])
	{
		path = ft_strjoin(paths[i], "/");
		cmd_path = ft_strjoin(path, cmd_splited[0]);
		free(path);
		if (cmd_path && !access(cmd_path, X_OK))
			execve(cmd_path, cmd_splited, envp);
		free(cmd_path);
	}
	p_error_cmd(cmd_splited, paths, 127);
}

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

	(void)ac;
	(void)av;
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
		if (!ft_strncmp(cmd, "exit", 5))
		{
			free(cmd);
			clear_history();
			write(1, "exit\n", 5);
			exit(0);
		}
		if (!ft_strncmp(cmd, "echo $", 6))
		{
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
			waitpid(pid, NULL, 0);
		else
			write(2, "Fork error\n", 11);
		free(cmd);
	}
	return (0);
}
