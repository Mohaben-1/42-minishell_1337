/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_errors.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 11:31:03 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/21 13:32:06 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_quotes_error(char quote_type)
{
	ft_putstr_fd("minishell: unexpected EOF while looking for matching `", 2);
	ft_putchar_fd(quote_type, 2);
	ft_putstr_fd("'\n", 2);
}

void	ft_print_paren_error(t_token_node *head, t_token_node *list,
	int *error)
{
	if (ft_before_this_token(head, list) 
		&& ft_before_this_token(head, list)->type
		== token_cmd 
		&& ft_before_this_token(head, ft_before_this_token(head, list))
		&& ft_before_this_token(head, ft_before_this_token(head, list))->type
		!= token_cmd)
	{
		ft_putstr_fd("minishell: syntax error near ", 2);
		ft_putstr_fd("unexpected token `newline'\n", 2);
		*error = 1;
	}
	else if (ft_before_this_token(head, list)
		&& ft_before_this_token(head, list)->type == token_cmd)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `('\n", 2);
		*error = 1;
	}
	else
	{
		ft_putstr_fd("minishell: syntax error: unexpected end of file\n", 2);
		*error = 1;
	}
}

void	ft_put_token_error(t_token_node *list)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
	ft_putstr_fd(list->data, 2);
	ft_putstr_fd("'\n", 2);
}

void	ft_token_syntax_error(t_token_node *list, int *error)
{
	if (!list)
		return ;
	if (is_node_operator(list) && *(list->data) != '>' && *(list->data) != '<')
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
		ft_putstr_fd(list->data, 2);
		ft_putstr_fd("'\n", 2);
		*error = 1;
		return ;
	}
	else
	{
		ft_consecutive_operators(list, error);
		if (*error)
			return ;
	}
	ft_valid_parentesis(list, error);
	if (*error)
		return ;
	ft_valid_redirections(list, error);
	if (*error)
		return ;
	ft_check_last_token(list, error);
	if (*error)
		return ;
}

t_token_node	*ft_handle_tokenize_error(t_token_node **head, t_exec *exec,
int error)
{
	if (error)
	{
		ft_token_node_free(head);
		exec->exit_status = 258;
		return (NULL);
	}
	return (*head);
}
