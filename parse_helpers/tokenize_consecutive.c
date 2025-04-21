/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize_consecutive.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 11:40:50 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/21 11:42:23 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	ft_check_complex_case(t_token_node *list, t_token_node *head)
{
	return (list->next && list->next->next
		&& (list->next->next->type == token_cmd
			|| list->next->next->type == token_dquote
			|| list->next->next->type == token_squote)
		&& ft_before_this_token(head, list)
		&& ft_before_this_token(head, list)->type == token_paren_close);
}

void	ft_check_last_token(t_token_node *list, int *error)
{
	if (*ft_token_last(list)->data == '(' 
		&& is_node_operator(ft_token_last(list)))
	{
		ft_putstr_fd("minishell: syntax error near ", 2);
		ft_putstr_fd("unexpected token `newline'\n", 2);
		*error = 1;
	}
	else if (is_node_operator(ft_token_last(list)))
	{
		if (*(ft_token_last(list)->data) == '|' && list != ft_token_last(list))
			ft_putstr_fd("minishell: syntax error: unexpected end of file\n", 2);
		else if (*(ft_token_last(list)->data) == '<' 
			|| *(ft_token_last(list)->data) == '>')
		{
			ft_putstr_fd("minishell: syntax error near ", 2);
			ft_putstr_fd("unexpected token `newline'\n", 2);
		}
		else
		{
			ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
			ft_putstr_fd(ft_token_last(list)->data, 2);
			ft_putstr_fd("'\n", 2);
		}
		*error = 1;
	}
}

void	ft_check_consecutive_paren(t_token_node *list, t_token_node *head,
	int *error)
{
	if (*(list->data) == '(' && *(list->next->data) == ')'
		&& !list->next->next && ft_before_this_token(head, list))
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `)'\n", 2);
		*error = 1;
	}
	else if (*(list->data) == '(' && is_node_operator(list->next)
		&& *list->next->data != '<' && *list->next->data != '>')
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `(\n`", 2);
		*error = 1;
	}
}

void	ft_check_consecutive_operators(t_token_node *list, int *error)
{
	if (is_node_operator(list) && is_node_operator(list->next)
		&& (*(list->data) == '<' || *(list->data) == '>')
		&& (*(list->next->data) == '<' || *(list->next->data) == '>'))
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
		ft_putstr_fd(list->next->data, 2);
		ft_putstr_fd("'\n", 2);
		*error = 1;
	}
	else if ((is_node_operator(list) && is_node_operator(list->next)
			&& *(list->next->data) != '<' && *(list->next->data) != '>')
		|| (*(list->data) == '(' && *(list->next->data) == ')'))
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
		ft_putstr_fd(list->next->data, 2);
		ft_putstr_fd("'\n", 2);
		*error = 1;
	}
}

void	ft_consecutive_operators(t_token_node *list, int *error)
{
	t_token_node	*head;

	head = list;
	while (list->next && !*error)
	{
		ft_check_consecutive_paren(list, head, error);
		if (!*error)
			ft_check_consecutive_operators(list, error);
		if (*error)
			return ;
		list = list->next;
	}
}
