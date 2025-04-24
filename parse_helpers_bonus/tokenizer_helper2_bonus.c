/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_helper2_bonus.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 17:41:56 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/23 17:41:58 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell_bonus.h"

int	is_node_operator(t_token_node *node)
{
	if (node->type != token_squote && node->type != token_dquote)
		return (*node->data == '|' || *node->data == '<' || 
			*node->data == '>' || *node->data == '&');
	return (0);
}

void	ft_is_quotes_spaced(char *input, int *i, int *is_spaced)
{
	if (*i > 0)
	{
		if (is_whitespace(input[*i - 1]))
			*is_spaced = 1;
		else
			*is_spaced = 0;
	}
}

void	ft_add_token_quotes(char quote_type, t_token_node **head, char *str, 
	int is_spaced)
{
	if (quote_type == '"')
		ft_add_token(head, token_dquote, str, is_spaced);
	else
		ft_add_token(head, token_squote, str, is_spaced);
}

t_token_node	*ft_before_this_token(t_token_node *list, t_token_node *token)
{
	if (!list || !list->next)
		return (NULL);
	while (list->next)
	{
		if (list->next == token)
			return (list);
		list = list->next;
	}
	return (NULL);
}

void	ft_check_open_paren(t_token_node *head, t_token_node *list,
	int *error)
{
	if (!list->next)
		ft_print_paren_error(head, list, error);
	else if (list->next && is_node_operator(list->next)
		&& *list->next->data != '<' && *list->next->data != '>')
	{
		ft_put_token_error(list->next);
		*error = 1;
	}
	else if (list->next && ft_before_this_token(head, list)
		&& !is_node_operator(ft_before_this_token(head, list))
		&& !is_parentesis(*ft_before_this_token(head, list)->data))
	{
		if (ft_before_this_token(head, ft_before_this_token(head, list)))
		{
			ft_putstr_fd("minishell: syntax error near ", 2);
			ft_putstr_fd("unexpected token `('\n", 2);
			*error = 1;
		}
		else
		{
			ft_put_token_error(list->next);
			*error = 1;
		}
	}
}
