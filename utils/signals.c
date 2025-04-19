/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 12:18:05 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/19 13:11:16 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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
