/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 12:40:09 by mohaben-          #+#    #+#             */
/*   Updated: 2025/03/06 14:14:51 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_init_token(t_token *token)
{
	token->token_cmd = 0;
	token->token_pipe = 0;
	token->token_redir_in = 0;
	token->token_redir_out = 0;
	token->token_here_doc = 0;
	token->token_append = 0;
}

t_token	ft_tokenizer(char *input)
{
	t_token	token;
	int		i;

	ft_init_token(&token);
	i = 0;
	while (input && input[i])
	{
		if (input[i] == ' ' || input[i] == '\t')
			i++;
		else if (input[i] == '|')
			token.token_pipe = 1;
		else if (input[i] == '<' && input[i + 1] == '<')
			token.token_here_doc = 1;
		else if (input[i] == '>' && input[i + 1] == '>')
			token.token_append = 1;
		else if (input[i] == '<' && input[i + 1] != '<')
			token.token_redir_in = 1;
		else if (input[i] == '>' && input[i + 1] != '>')
			token.token_redir_out = 1;
		else
			token.token_cmd = 1;
		i++;
	}
	return (token);
}
