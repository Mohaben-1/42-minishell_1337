/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 11:19:20 by ahouass           #+#    #+#             */
/*   Updated: 2025/04/25 18:44:06 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_check_close_paren(t_token_node *list, int *paren_count,
	int *error)
{
	(*paren_count)--;
	if (list->next && (list->next->type == token_cmd
			|| list->next->type == token_dquote
			|| list->next->type == token_squote))
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
		ft_putstr_fd(list->next->data, 2);
		ft_putstr_fd("'\n", 2);
		*error = 1;
	}
	else if (*paren_count < 0)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
		ft_putstr_fd(list->data, 2);
		ft_putstr_fd("'\n", 2);
		*error = 1;
	}
}

static int	ft_check_next_token(t_token_node *list)
{
	return (list->next && (is_parentesis(*(list->next->data))
			|| *(list->next->data) == '<' || *(list->next->data) == '>'));
}

void	ft_valid_redirections(t_token_node *list, int *error)
{
	t_token_node	*head;
	int				is_redir;

	head = list;
	while (list && !*error)
	{
		is_redir = (*(list->data) == '>' || *(list->data) == '<') 
			&& is_node_operator(list);
		if (is_redir && ft_check_next_token(list))
		{
			ft_put_token_error(list->next);
			*error = 1;
		}
		else if (is_redir && ft_check_complex_case(list, head))
		{
			ft_put_token_error(list->next->next);
			*error = 1;
		}
		list = list->next;
	}
}

static int	ft_process_token(char *input, int *i, t_token_node **head)
{
	int	error;

	error = 0;
	if (input[*i] == '|' ||  input[*i] == '&')
	{
		if (input[*i + 1] == ')' || ((input[*i + 1] == '|' ||  input[*i + 1] == '&') && input[*i + 2] == ')'))
		{
			ft_putstr_fd("minishell: syntax error near unexpected token `)'\n", 2);
			return (1);
		}
	}
	if (input[*i] == '"' || input[*i] == '\'')
		ft_handle_quotes(input, i, head, &error);
	else if (input[*i] == '|' && input[*i + 1] == '|')
		ft_handle_special_tokens(input, i, head);
	else if (input[*i] == '&')
		ft_handle_special_tokens(input, i, head);
	else if ((input[*i] == '<' && input[*i + 1] == '<')
		|| (input[*i] == '>' && input[*i + 1] == '>'))
		ft_handle_redirections(input, i, head);
	else if (input[*i] == '(' || input[*i] == ')')
		ft_handle_parenthesis(input, i, head);
	else if (input[*i] == '|')
		ft_handle_pipe(input, i, head);
	else if (input[*i] == '<' || input[*i] == '>')
		ft_handle_redirections(input, i, head);
	else if (input[*i])
	{
		ft_handle_str(input, i, head);
		(*i)++;
	}
	return (error);
}

t_token_node	*ft_tokenize(char *input, t_exec *exec)
{
	t_token_node	*head;
	int				i;
	int				error;

	i = 0;
	error = 0;
	head = NULL;
	while (i < (int)ft_strlen(input) && !error)
	{
		while (input[i] && is_whitespace(input[i]))
			i++;
		if (!input[i])
			break ;
		error = ft_process_token(input, &i, &head);
		if (!error && !head)
			i++;
	}
	ft_token_syntax_error(head, &error);
	return (ft_handle_tokenize_error(&head, exec, error));
}
