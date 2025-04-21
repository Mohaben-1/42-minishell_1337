/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_helper.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 11:27:42 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/21 11:40:03 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_token_node	*ft_token_last(t_token_node *list)
{
	if (!list)
		return (NULL);
	while (list->next)
	{
		list = list->next;
	}
	return (list);
}

void	ft_add_token(t_token_node **head, t_token_type type, char *data, 
	int spaced)
{
	t_token_node	*new_token;
	t_token_node	*current;

	if (type == token_cmd && (!data || !data[0]))
		return ;
	new_token = malloc(sizeof(t_token_node));
	if (!new_token)
		return ;
	if (spaced)
		new_token->spaced = 1;
	else
		new_token->spaced = 0;
	new_token->type = type;
	new_token->data = ft_strdup(data);
	new_token->to_skip = 0;
	new_token->next = NULL;
	if (!*head)
		*head = new_token;
	else
	{
		current = ft_token_last(*head);
		current->next = new_token;
	}
}

void	ft_handle_quotes(char *input, int *i, t_token_node **head, 
	int *error)
{
	int		start;
	char	*str;
	int		is_spaced;
	char	quote_type;

	is_spaced = 0;
	quote_type = input[*i];
	ft_is_quotes_spaced(input, i, &is_spaced);
	(*i)++;
	start = *i;
	while (input[*i] && input[*i] != quote_type)
		(*i)++;
	if (input[*i] == quote_type)
	{
		str = ft_substr(input, start, *i - start);
		ft_add_token_quotes(quote_type, head, str, is_spaced);
		free(str);
		(*i)++;
	}
	else
	{
		ft_quotes_error(quote_type);
		*error = 1;
	}
}

void	ft_valid_parentesis(t_token_node *list, int *error)
{
	int				paren_count;
	t_token_node	*head;

	paren_count = 0;
	head = list;
	while (list && !*error)
	{
		if (list->type == token_paren_open)
		{
			ft_check_open_paren(head, list, error);
			if (!*error)
				paren_count++;
		}
		else if (list->type == token_paren_close)
			ft_check_close_paren(list, &paren_count, error);
		list = list->next;
	}
	if (paren_count > 0 && !*error)
	{
		ft_putstr_fd("minishell: syntax error: unexpected end of file\n", 2);
		*error = 1;
	}
}
