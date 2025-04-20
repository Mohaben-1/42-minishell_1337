#include "../minishell.h"

void	rdr_wild_err(char **expanded, char *file, t_exec *exec)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(file, 2);
	ft_putstr_fd(": ambiguous redirect\n", 2);
	exec->exit_status = 1;
	free_double_ptr(expanded);
}

