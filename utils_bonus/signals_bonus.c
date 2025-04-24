/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 17:38:00 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/23 17:38:02 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell_bonus.h"

void	ft_handle_sigint(int sig)
{
	(void)sig;
	ft_putchar_fd('\n', 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

void	handle_sig_exec_ve(int sig)
{
	if (sig == SIGINT)
	{
		ft_putchar_fd('\n', 1);
		exit(130);
	}
}

void	heredoc_child_signal(int sig)
{
	(void)sig;
	ft_putchar_fd('\n', 1);
	exit(1);
}

void	handle_main_sigs(int ac, char **av)
{
	(void)ac;
	(void)av;
	rl_catch_signals = 0;
	signal(SIGINT, ft_handle_sigint);
	signal(SIGQUIT, SIG_IGN);
}
