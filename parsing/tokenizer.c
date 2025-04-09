/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 12:40:09 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/09 18:36:43 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_add_token(t_token_node **head, t_token_node **current, t_token_type type, char *data, int spaced)
{
	t_token_node	*new_token;

	// Don't add empty strings for token_cmd type
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
}

int is_operator(char c)
{
	return (c == '|' || c == '<' || c == '>' || c == '&');
}

int is_node_operator(t_token_node *node)
{
	if (node->type != token_squote && node->type != token_dquote)
		return (*node->data == '|' || *node->data == '<' || *node->data == '>' || *node->data == '&');
	return 0;
}

int is_whitespace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n');
}

int	is_quotes(char c)
{
	return (c == '"' || c == '\'');
}

int	is_parentesis(char c)
{
	return (c == '(' || c == ')');
}

static void ft_handle_quotes(char *input, int *i, t_token_node **head, t_token_node **current, int *error)
{
	int start;
	char *str;
	char quote_type = input[*i];
	int	is_spaced = 0;

	if (*i > 0)
	{
		if (is_whitespace(input[*i-1]))
			is_spaced = 1;
		else
			is_spaced = 0;
	}
	(*i)++;
	start = *i;
	
	while (input[*i] && input[*i] != quote_type)
		(*i)++;
	
	if (input[*i] == quote_type)
	{
		str = ft_substr(input, start, *i - start);
		if (quote_type == '"')
			ft_add_token(head, current, token_dquote, str, is_spaced);
		else
			ft_add_token(head, current, token_squote, str, is_spaced);
		free(str);
	}
	else
	{
		ft_putstr_fd("minishell: unexpected EOF while looking for matching `", 2);
		ft_putchar_fd(quote_type, 2);
		ft_putstr_fd("'\n", 2);
		ft_putstr_fd("minishell: syntax error: unexpected end of file\n", 2);
		*error = 1;
	}
}

static void	ft_handle_str(char *input, int *i, t_token_node **head, t_token_node **current)
{
	int		start;
	char	*str;

	start = *i;
	// Check if this token is preceded by whitespace
	int is_spaced = (*i > 0 && is_whitespace(input[*i - 1]));
	while (input[*i] && !is_operator(input[*i]) && !is_whitespace(input[*i]) && !is_quotes(input[*i]) && !is_parentesis(input[*i]))
		(*i)++;
	str = ft_substr(input, start, *i - start);
	if (is_operator(input[*i]) || is_quotes(input[*i]) || is_parentesis(input[*i]))
		(*i)--;
	ft_add_token(head, current, token_cmd, str, is_spaced);
	free(str);
}

t_token_node	*ft_token_last(t_token_node *list)
{
	if (!list)
		return (NULL);
	while(list->next)
	{
		list = list->next;
	}
	return (list);
}

t_token_node	*ft_before_this_token(t_token_node *list, t_token_node *token)
{
	if (!list || !list->next)
		return (NULL);
	while(list->next)
	{
		if (list->next == token)
			return (list);
		list = list->next;
	}
	return (NULL);
}

void	ft_valid_parentesis(t_token_node *list, int *error)
{
	int				paren_count;
	t_token_node	*head;
	paren_count = 0;
	head = list;

	while (list)
	{
		if (list->type == token_paren_open && !list->next)
		{
			if (ft_before_this_token(head, list) && ft_before_this_token(head, list)->type == token_cmd && ft_before_this_token(head, ft_before_this_token(head, list)) && ft_before_this_token(head, ft_before_this_token(head, list))->type != token_cmd)
			{
				// write(2, "parentesis yes\n", 15);
				ft_putstr_fd("minishell: syntax error near unexpected token `newline'\n", 2);
				*error = 1;
				return ;
			}
			else if (ft_before_this_token(head, list) && ft_before_this_token(head, list)->type == token_cmd)
			{
				// write(2, "parentesis yes\n", 15);
				ft_putstr_fd("minishell: syntax error near unexpected token `('\n", 2);
				*error = 1;
				return ;
			}
			else
			{
				// write(2, "parentesis yes\n", 15);
				ft_putstr_fd("minishell: syntax error: unexpected end of file\n", 2);
				*error = 1;
				return ;
			}
			
		}
		else if (list->type == token_paren_open && list->next && is_node_operator(list->next) && *list->next->data != '<' && *list->next->data != '>')
		{
			// write(2, "parentesis yes\n", 15);
			ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
			ft_putstr_fd(list->next->data, 2);
			ft_putstr_fd("'\n", 2);
			*error = 1;
			return ;
		}
		else if (list->type == token_paren_open && list->next && ft_before_this_token(head, list) && !is_node_operator(ft_before_this_token(head, list)) && !is_parentesis(*ft_before_this_token(head, list)->data))
		{
			if (ft_before_this_token(head, ft_before_this_token(head, list)))
			{
				// write(2, "parentesis yes\n", 15);
				ft_putstr_fd("minishell: syntax error near unexpected token `('\n", 2);
				*error = 1;
				return ;
			}
			else
			{
				// write(2, "parentesis yes\n", 15);
				ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
				ft_putstr_fd(list->next->data, 2);
				ft_putstr_fd("'\n", 2);
				*error = 1;
				return ;
			}
		}
		else if (list->type == token_paren_open)
			paren_count++;
		else if (list->type == token_paren_close && list->next && (list->next->type == token_cmd || list->next->type == token_dquote || list->next->type == token_squote))
		{
			// write(2, "parentesis yes\n", 15);
			ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
			ft_putstr_fd(list->next->data, 2);
			ft_putstr_fd("'\n", 2);
			*error = 1;
			return;
		}
		else if (list->type == token_paren_close)
		{
			paren_count--;
			if (paren_count < 0)
			{
				// write(2, "parentesis yes\n", 15);
				ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
				ft_putstr_fd(list->data, 2);
				ft_putstr_fd("'\n", 2);
				*error = 1;
				return;
			}
		}
		list = list->next;
	}
	if (paren_count > 0)
	{
		// write(2, "parentesis yes\n", 15);
		ft_putstr_fd("minishell: syntax error: unexpected end of file\n", 2);
		*error = 1;
	}
}

void	ft_valid_redirections(t_token_node *list, int *error)
{
	t_token_node *head = list;
	while (list)
		{
			if ((*(list->data) == '>' ||  *(list->data) == '<') && is_node_operator(list) && list->next && (is_parentesis(*(list->next->data)) || *(list->next->data) == '<' || *(list->next->data) == '>'))
			{
				ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
				ft_putstr_fd(list->next->data, 2);
				ft_putstr_fd("'\n", 2);
				*error = 1;
				return ;
			}
			if ((*(list->data) == '>' ||  *(list->data) == '<') && is_node_operator(list) && list->next && list->next->next && (list->next->next->type == token_cmd  ||  list->next->next->type == token_dquote || list->next->next->type == token_squote) && ft_before_this_token(head, list) && ft_before_this_token(head, list)->type == token_paren_close)
			{
				ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
				ft_putstr_fd(list->next->next->data, 2);
				ft_putstr_fd("'\n", 2);
				*error = 1;
				return ;
			}
			list = list->next;
		}
}

void	ft_check_last_token(t_token_node *list, int *error)
{
	if (*ft_token_last(list)->data == '(')
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `newline'\n", 2);
		*error = 1;
	}
	else if (is_node_operator(ft_token_last(list)))
	{
		if (*(ft_token_last(list)->data) == '|' && list != ft_token_last(list))
			ft_putstr_fd("minishell: syntax error: unexpected end of file\n", 2);
		else if (*(ft_token_last(list)->data) == '<' || *(ft_token_last(list)->data) == '>')
			ft_putstr_fd("minishell: syntax error near unexpected token `newline'\n", 2);
		else
		{
			ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
			ft_putstr_fd(ft_token_last(list)->data, 2);
			ft_putstr_fd("'\n", 2);
		}
		*error = 1;
	}
}
void	ft_consecutive_operators(t_token_node *list, int *error)
{
	t_token_node	*head;
	head = list;
	while (list->next)
	{
		if (*(list->data) == '(' && *(list->next->data) == ')' && !list->next->next && ft_before_this_token(head, list))
		{
			ft_putstr_fd("minishell: syntax error near unexpected token `)'\n", 2);
			*error = 1;
			return ;
		}
		if (*(list->data) == '(' && is_node_operator(list->next) && *list->next->data != '<' && *list->next->data != '>')
		{
			ft_putstr_fd("minishell: syntax error near unexpected token `(\n`", 2);
			*error = 1;
			return ;
		}
		if (is_node_operator(list) && is_node_operator(list->next) && (*(list->data) == '<' || *(list->data) == '>') && (*(list->next->data) == '<' || *(list->next->data) == '>'))
		{
			ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
			ft_putstr_fd(list->next->data, 2);
			ft_putstr_fd("'\n", 2);
			*error = 1;
			return ;
		}
		if ((is_node_operator(list) && is_node_operator(list->next) && *(list->next->data) != '<' && *(list->next->data) != '>') || (*(list->data) == '(' && *(list->next->data) == ')'))
		{
			ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
			ft_putstr_fd(list->next->data, 2);
			ft_putstr_fd("'\n", 2);
			*error = 1;
			return ;
		}
		list = list->next;
	}
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

t_token_node	*ft_tokenize(char *input, t_exec *exec)
{
	t_token_node	*head;
	t_token_node	*current;
	int				i;
	int				error;

	i = 0;
	error = 0;
	head = NULL;
	current = NULL;
	while (input && input[i])
	{
		while (input[i] && is_whitespace(input[i]))
			i++;
		if (!input[i])
			break;
		if (input[i] == '"' || input[i] == '\'')
		{
			ft_handle_quotes(input, &i, &head, &current, &error);
			if (error)
				break;
			// Don't increment i again, ft_handle_quotes already advances it
		}
		else if (input[i] == '|' && input[i + 1] == '|')
		{
			ft_add_token(&head, &current, token_or, "||", 0);
			i += 2; // Skip both characters explicitly
			continue; // Skip the increment at the end
		}
		else if (input[i] == '&' && (!input[i + 1] || is_whitespace(input[i + 1])))
		{
			ft_putstr_fd("syntax error near unexpected token `&'\n", 2);
			ft_token_node_free(&head);
			return (NULL);
		}
		else if (input[i] == '&' && input[i + 1] == '&')
		{
			ft_add_token(&head, &current, token_and_and, "&&", 0);
			i += 2; // Skip both characters explicitly
			continue; // Skip the increment at the end
		}
		else if (input[i] == '<' && input[i + 1] == '<')
		{
			ft_add_token(&head, &current, token_hrdc, "<<", 0);
			i += 2; // Skip both characters explicitly
			continue; // Skip the increment at the end
		}
		else if (input[i] == '>' && input[i + 1] == '>')
		{
			ft_add_token(&head, &current, token_appnd, ">>", 0);
			i += 2; // Skip both characters explicitly
			continue; // Skip the increment at the end
		}
		else if (input[i] == '(')
		{
			ft_add_token(&head, &current, token_paren_open, "(", 0);
			i++; // Skip one character explicitly
			continue; // Skip the increment at the end
		}
		else if (input[i] == ')')
		{
			ft_add_token(&head, &current, token_paren_close, ")", 0);
			i++; // Skip one character explicitly
			continue; // Skip the increment at the end
		}
		else if (input[i] == '|')
		{
			ft_add_token(&head, &current, token_pipe, "|", 0);
			i++; // Skip one character explicitly
			continue; // Skip the increment at the end
		}
		else if (input[i] == '<')
		{
			ft_add_token(&head, &current, token_in, "<", 0);
			i++; // Skip one character explicitly
			continue; // Skip the increment at the end
		}
		else if (input[i] == '>')
		{
			ft_add_token(&head, &current, token_out, ">", 0);
			i++; // Skip one character explicitly
			continue; // Skip the increment at the end
		}
		else if (input[i])
		{
			ft_handle_str(input, &i, &head, &current);
			// Don't increment i again, ft_handle_str already advances it
			// But we need to continue to skip the increment at the end
			i++;
			continue ;
		}
		
		// We only reach here if none of the continue statements were hit
		i++;
	}
	ft_token_syntax_error(head, &error);
	if (error)
	{
		ft_token_node_free(&head);
		exec->exit_status = 258;
		return (NULL);
	}
	return (head);
}
