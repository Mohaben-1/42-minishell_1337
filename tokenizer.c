/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 12:40:09 by mohaben-          #+#    #+#             */
/*   Updated: 2025/03/10 16:34:17 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_add_token(t_token_node **head, t_token_node **current, t_token_type type, char *data)
{
	t_token_node	*new_token;

	new_token = malloc(sizeof(t_token_node));
	if (!new_token)
		return ;
	new_token->type = type;
	new_token->data = ft_strdup(data);
	new_token->next = NULL;
	if (!*head)
		*head = new_token;
	else
		(*current)->next = new_token;
	*current = new_token;
}

void	ft_token_node_free(t_token_node **head)
{
	t_token_node	*next;

	if (!head || !*head)
		return ;
	next = NULL;
	while (*head)
	{
		next = (*head)->next;
		free((*head)->data);
		free(*head);
		*head = next;
	}
	free(head);
}

static void	ft_handle_str(char *input, int *i, t_token_node **head, t_token_node **current)
{
	int		start;
	char	*str;

	start = *i;
	while (input[*i] && input[*i] != '|' && input[*i] != '<' && input[*i] != '>')
		(*i)++;
	str = ft_substr(input, start, *i - start);
	if (input[*i] == '|' || input[*i] == '<' || input[*i] == '>')
		(*i)--;
	ft_add_token(head, current, token_cmd, str);
	free(str);
}

t_token_node	*ft_tokenize(char *input)
{
	t_token_node	*head;
	t_token_node	*current;
	int				i;

	head = NULL;
	current = NULL;
	i = 0;
	while (input && input[i])
	{
		while (input[i] && (input[i] == ' ' || input[i] == '\t'))
			i++;
		if (input[i] == '|')
			ft_add_token(&head, &current, token_pipe, "|");
		else if (input[i] == '<' && input[i + 1] == '<')
			ft_add_token(&head, &current, token_hrdc, "<<");
		else if (input[i] == '>' && input[i + 1] == '>')
			ft_add_token(&head, &current, token_appnd, ">>");
		else if (input[i] == '<')
			ft_add_token(&head, &current, token_in, "<");
		else if (input[i] == '>')
			ft_add_token(&head, &current, token_out, ">");
		else
			ft_handle_str(input, &i, &head, &current);
		i++;
	}
	return (head);
}
