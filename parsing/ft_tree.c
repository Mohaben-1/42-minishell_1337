/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_tree.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 20:58:14 by ahouass           #+#    #+#             */
/*   Updated: 2025/04/21 13:24:36 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_ast_node	*build_ast(t_token_node *tokens, t_exec *exec)
{
	t_ast_node	*ast;

	if (!tokens)
		return (NULL);
	ast = parse_logical_ops(tokens, exec);
	return (ast);
}

t_ast_node	*parse_logical_ops(t_token_node *tokens, t_exec *exec)
{
	t_token_node	*op;
	t_ast_node		*node;
	t_token_node	*left_tokens;
	t_token_node	*right_tokens;
	int				node_type;

	op = find_op_at_level(tokens, token_and_and, token_or);
	if (op)
	{
		if (op->type == token_and_and)
			node_type = AST_AND_AND;
		else
			node_type = AST_OR_OR;
		node = create_ast_node(node_type);
		left_tokens = extract_tokens(tokens, op);
		right_tokens = op->next;
		node->left = parse_logical_ops(left_tokens, exec);
		node->right = parse_logical_ops(right_tokens, exec);
		free_token_list(left_tokens);
		return (node);
	}
	return (parse_pipes(tokens, exec));
}

t_ast_node	*parse_pipes(t_token_node *tokens, t_exec *exec)
{
	t_token_node	*pipe;
	t_ast_node		*node;
	t_token_node	*left_tokens;
	t_token_node	*right_tokens;

	pipe = find_token_at_level(tokens, token_pipe);
	if (pipe)
	{
		node = create_ast_node(AST_PIPE);
		left_tokens = extract_tokens(tokens, pipe);
		right_tokens = pipe->next;
		node->left = parse_pipes(left_tokens, exec);
		node->right = parse_pipes(right_tokens, exec);
		free_token_list(left_tokens);
		return (node);
	}
	if (tokens->type == token_paren_open)
		return (parse_subshell(tokens, exec));
	return (parse_command(tokens));
}

t_ast_node	*handle_logical_operators(t_ast_node *node, t_token_node *tokens, 
	t_exec *exec)
{
	t_token_node	*op;
	t_ast_node		*logical_node;
	int				node_type;

	if (!tokens)
		return (node);
	op = find_op_at_level(tokens, token_and_and, token_or);
	if (op)
	{
		if (op->type == token_and_and)
			node_type = AST_AND_AND;
		else
			node_type = AST_OR_OR;
		logical_node = create_ast_node(node_type);
		logical_node->left = node;
		logical_node->right = parse_logical_ops(op->next, exec);
		return (logical_node);
	}
	return (node);
}

t_token_node	*find_matching_paren(t_token_node *open_paren)
{
	int				paren_count;
	t_token_node	*tmp;

	paren_count = 1;
	tmp = open_paren->next;
	while (tmp && paren_count > 0)
	{
		if (tmp->type == token_paren_open)
			paren_count++;
		else if (tmp->type == token_paren_close && --paren_count == 0)
			return (tmp);
		tmp = tmp->next;
	}
	return (NULL);
}

t_ast_node	*parse_subshell(t_token_node *tokens, t_exec *exec)
{
	t_token_node	*closing;
	t_ast_node		*node;
	t_token_node	*inner_tokens;
	t_token_node	*remaining;

	(void)exec;
	if (tokens->type != token_paren_open)
		return (NULL);
	closing = find_matching_paren(tokens);
	if (!closing)
		return (NULL);
	node = create_ast_node(AST_SUBSHELL);
	inner_tokens = extract_tokens(tokens->next, closing);
	node->child = parse_logical_ops(inner_tokens, exec);
	free_token_list(inner_tokens);
	remaining = closing->next;
	if (remaining && is_redirection(remaining->type))
		node->redirects = parse_redirections(&remaining);
	return (handle_logical_operators(node, remaining, exec));
}

t_ast_node	*parse_command(t_token_node *tokens)
{
	t_ast_node		*node;
	t_token_node	*cmd_tokens;
	int				arg_count;

	if (!tokens)
		return (NULL);
	node = create_ast_node(AST_COMMAND);
	cmd_tokens = tokens;
	node->redirects = parse_redirections(&cmd_tokens);
	arg_count = count_args(cmd_tokens);
	if (arg_count > 0)
	{
		node->args = collect_args(cmd_tokens, arg_count, 
				&node->arg_quote_types, &node->arg_is_spaced);
		node->arg_count = arg_count;
	}
	else
	{
		node->args = NULL;
		node->arg_count = 0;
		node->arg_quote_types = NULL;
	}
	return (node);
}

t_token_node	*find_op_at_level(t_token_node *tokens, t_token_type type1, 
	t_token_type type2)
{
	int				paren_level;
	t_token_node	*last_op;
	t_token_node	*tmp;

	tmp = tokens;
	last_op = NULL;
	paren_level = 0;
	while (tmp)
	{
		if (tmp->type == token_paren_open)
			paren_level++;
		else if (tmp->type == token_paren_close)
			paren_level--;
		if (paren_level == 0 && (tmp->type == type1 || tmp->type == type2))
			last_op = tmp;
		tmp = tmp->next;
	}
	return (last_op);
}

t_token_node	*find_token_at_level(t_token_node *tokens, t_token_type type)
{
	int				paren_level;
	t_token_node	*tmp;
	t_token_node	*last_token;

	tmp = tokens;
	paren_level = 0;
	last_token = NULL;
	while (tmp)
	{
		if (tmp->type == token_paren_open)
			paren_level++;
		else if (tmp->type == token_paren_close)
			paren_level--;
		if (paren_level == 0 && tmp->type == type)
			last_token = tmp;
		tmp = tmp->next;
	}
	return (last_token);
}

t_token_node	*free_token_list_and_return_null(t_token_node *list)
{
	free_token_list(list);
	return (NULL);
}

t_token_node	*create_token_node(t_token_node *src)
{
	t_token_node	*new_node;

	new_node = malloc(sizeof(t_token_node));
	if (!new_node)
		return (NULL);
	new_node->type = src->type;
	new_node->to_skip = 0;
	new_node->data = ft_strdup(src->data);
	new_node->spaced = src->spaced;
	new_node->next = NULL;
	return (new_node);
}

t_token_node	*extract_tokens(t_token_node *start, t_token_node *end)
{
	t_token_node	*new_node;
	t_token_node	*current;
	t_token_node	*result;
	t_token_node	*tmp;

	tmp = start;
	result = NULL;
	current = NULL;
	if (!start || start == end)
		return (NULL);
	while (tmp && tmp != end)
	{
		new_node = create_token_node(tmp);
		if (!new_node)
			return (free_token_list_and_return_null(result));
		if (!result)
			result = new_node;
		else
			current->next = new_node;
		current = new_node;
		tmp = tmp->next;
	}
	return (result);
}

t_ast_node	*create_ast_node(int type)
{
	t_ast_node	*node;

	node = malloc(sizeof(t_ast_node));
	if (!node)
		return (NULL);
	node->e_type = type;
	node->args = NULL;
	node->arg_count = 0;
	node->arg_quote_types = NULL;
	node->arg_is_spaced = NULL;
	node->redirects = NULL;
	node->left = NULL;
	node->right = NULL;
	node->child = NULL;
	return (node);
}

void	free_token_node(t_token_node *token)
{
	if (!token)
		return ;
	free(token->data);
	token->data = NULL;
	free(token);
	token = NULL;
}

int	get_quote_type(int token_type)
{
	if (token_type == token_dquote)
		return (token_dquote);
	if (token_type == token_squote)
		return (token_squote);
	return (0);
}

t_redirect	*create_redirect_type(t_token_node *token, int redir_type)
{
	t_redirect	*redir;

	redir = malloc(sizeof(t_redirect));
	if (!redir)
		return (NULL);
	redir->file = strdup(token->data);
	if (!redir->file)
		return (free(redir), NULL);
	redir->type = redir_type;
	redir->heredoc_fd = -1;
	redir->quoted = get_quote_type(token->type);
	redir->is_spaced = 0;
	redir->next = NULL;
	return (redir);
}

void	process_joined_tokens(t_token_node **last, int redir_type, 
	t_redirect **current, t_redirect **head)
{
	t_token_node	*next;
	t_redirect		*new;

	(void)head;
	next = (*last)->next;
	while (next && !next->spaced && (next->type == token_cmd
			|| next->type == token_dquote || next->type == token_squote))
	{
		new = create_redirect_type(next, redir_type);
		if (!new)
			return ;
		(*current)->next = new;
		*current = new;
		next->to_skip = 1;
		*last = next;
		next = next->next;
	}
}

void	add_redirect_to_list(t_redirect **head, t_redirect **current, 
	t_redirect *redir)
{
	if (!*head)
		*head = redir;
	else
		(*current)->next = redir;
	*current = redir;
}

static void	set_quoted_type(t_redirect *redir, t_token_node *file_token)
{
	if (file_token->type == token_dquote)
		redir->quoted = token_dquote;
	else if (file_token->type == token_squote)
		redir->quoted = token_squote;
	else
		redir->quoted = 0;
}

t_redirect	*create_redirect(t_token_node *redir_token, 
	t_token_node *file_token)
{
	t_redirect	*redir;

	redir = malloc(sizeof(t_redirect));
	if (!redir)
		return (NULL);
	redir->type = redir_token->type;
	if (redir->type == token_hrdc)
		redir->heredoc_fd = -1;
	else
		redir->heredoc_fd = 0;
	set_quoted_type(redir, file_token);
	redir->file = strdup(file_token->data);
	if (!redir->file)
	{
		free(redir);
		return (NULL);
	}
	redir->is_spaced = 1;
	redir->next = NULL;
	return (redir);
}

void	mark_redirection_tokens(t_token_node *redir, t_token_node **last)
{
	*last = redir->next;
	redir->to_skip = 1;
	(*last)->to_skip = 1;
}

t_redirect	*parse_redirections(t_token_node **tokens)
{
	t_token_node	*tmp;
	t_token_node	*last;
	t_redirect		*head;
	t_redirect		*redir;
	t_redirect		*current;

	current = NULL;
	tmp = *tokens;
	head = NULL;
	while (tmp)
	{
		if (is_redirection(tmp->type) && tmp->next)
		{
			redir = create_redirect(tmp, tmp->next);
			if (!redir)
				return (head);
			add_redirect_to_list(&head, &current, redir);
			mark_redirection_tokens(tmp, &last);
			process_joined_tokens(&last, tmp->type, &current, &head);
			tmp = last->next;
		}
		else
			tmp = tmp->next;
	}
	return (head);
}

int	is_redirection(t_token_type type)
{
	return (type == token_in || type == token_out || 
		type == token_hrdc || type == token_appnd);
}

int	count_args(t_token_node *tokens)
{
	int				count;
	t_token_node	*tmp;

	count = 0;
	tmp = tokens;
	while (tmp)
	{
		if ((tmp->type == token_cmd || tmp->type == token_dquote || 
				tmp->type == token_squote) && !tmp->to_skip)
			count++;
		tmp = tmp->next;
	}
	return (count);
}

void	free_collected_args(char **args, int *quote_types, int *is_spaced)
{
	int	i;

	i = 0;
	while (args[i])
		free(args[i++]);
	free(args);
	free(quote_types);
	free(is_spaced);
}

int	process_token(t_token_node *token, char **args, t_arg_meta *meta, int index)
{
	args[index] = ft_strdup(token->data);
	if (!args[index])
		return (0);
	if (token->type == token_dquote)
		meta->quote_types[index] = AST_DQUOTES;
	else if (token->type == token_squote)
		meta->quote_types[index] = AST_SQUOTES;
	else
		meta->quote_types[index] = 0;
	meta->is_spaced[index] = token->spaced;
	return (1);
}

char	**allocate_args_memory(int count, int **quote_types, int **is_spaced)
{
	char	**args;

	args = malloc(sizeof(char *) * (count + 1));
	if (!args)
		return (NULL);
	*quote_types = malloc(sizeof(int) * count);
	if (!(*quote_types))
	{
		free(args);
		return (NULL);
	}
	*is_spaced = malloc(sizeof(int) * count);
	if (!(*is_spaced))
	{
		free(*quote_types);
		free(args);
		return (NULL);
	}
	return (args);
}

char	**free_collected_args_and_return(char **args, int *qt, int *spaced)
{
	free_collected_args(args, qt, spaced);
	return (NULL);
}

int	is_valid_arg_token(t_token_node *token)
{
	return ((token->type == token_cmd || token->type == token_dquote
			|| token->type == token_squote) && !token->to_skip);
}

char	**collect_args(t_token_node *tokens, int count, int **quote_types, 
	int **is_spaced)
{
	char			**args;
	t_token_node	*tmp;
	t_arg_meta		meta;
	int				i;

	args = allocate_args_memory(count, quote_types, is_spaced);
	if (!args)
		return (NULL);
	meta.quote_types = *quote_types;
	meta.is_spaced = *is_spaced;
	tmp = tokens;
	i = 0;
	while (tmp && i < count)
	{
		if (is_valid_arg_token(tmp))
		{
			if (!process_token(tmp, args, &meta, i))
				return (free_collected_args_and_return(args, 
						*quote_types, *is_spaced));
			i++;
		}
		tmp = tmp->next;
	}
	args[i] = NULL;
	return (args);
}


void	free_ast_args(t_ast_node *node)
{
	int	i;

	if (!node->args)
		return ;
	i = 0;
	while (node->args[i])
	{
		free(node->args[i]);
		i++;
	}
	free(node->args);
}

void	free_ast_node(t_ast_node *node)
{
	if (!node)
		return ;
	free_ast_node(node->left);
	free_ast_node(node->right);
	free_ast_node(node->child);
	free_ast_args(node);
	free(node->arg_quote_types);
	free(node->arg_is_spaced);
	free_redirects(node->redirects);
	free(node);
}
