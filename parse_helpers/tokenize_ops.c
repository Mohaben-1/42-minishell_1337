/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize_ops.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 11:37:28 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/21 11:38:53 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_handle_special_tokens(char *input, int *i, t_token_node **head)
{
	if (input[*i] == '|' && input[*i + 1] == '|')
	{
		ft_add_token(head, token_or, "||", 0);
		*i += 2;
	}
	else if (input[*i] == '&' && ((!input[*i + 1]
				|| is_whitespace(input[*i + 1])) || input[*i + 1] != '&'))
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `&'\n", 2);
		ft_token_node_free(head);
		*head = NULL;
	}
	else if (input[*i] == '&' && input[*i + 1] == '&')
	{
		ft_add_token(head, token_and_and, "&&", 0);
		*i += 2;
	}
}

void	ft_handle_redirections(char *input, int *i, t_token_node **head)
{
	if (input[*i] == '<' && input[*i + 1] == '<')
	{
		ft_add_token(head, token_hrdc, "<<", 0);
		*i += 2;
	}
	else if (input[*i] == '>' && input[*i + 1] == '>')
	{
		ft_add_token(head, token_appnd, ">>", 0);
		*i += 2;
	}
	else if (input[*i] == '<')
	{
		ft_add_token(head, token_in, "<", 0);
		(*i)++;
	}
	else if (input[*i] == '>')
	{
		ft_add_token(head, token_out, ">", 0);
		(*i)++;
	}
}

void	ft_handle_parenthesis(char *input, int *i, t_token_node **head)
{
	if (input[*i] == '(')
	{
		ft_add_token(head, token_paren_open, "(", 0);
		(*i)++;
	}
	else if (input[*i] == ')')
	{
		ft_add_token(head, token_paren_close, ")", 0);
		(*i)++;
	}
}

void	ft_handle_pipe(char *input, int *i, t_token_node **head)
{
	if (input[*i] == '|')
	{
		ft_add_token(head, token_pipe, "|", 0);
		(*i)++;
	}
}

void	ft_handle_str(char *input, int *i, t_token_node **head)
{
	int		start;
	char	*str;
	int		is_spaced;

	start = *i;
	is_spaced = (*i > 0 && is_whitespace(input[*i - 1]));
	while (input[*i] && !is_operator(input[*i]) && !is_whitespace(input[*i]) 
		&& !is_quotes(input[*i]) && !is_parentesis(input[*i]))
		(*i)++;
	str = ft_substr(input, start, *i - start);
	if (is_operator(input[*i]) || is_quotes(input[*i]) 
		|| is_parentesis(input[*i]))
		(*i)--;
	ft_add_token(head, token_cmd, str, is_spaced);
	free(str);
}
