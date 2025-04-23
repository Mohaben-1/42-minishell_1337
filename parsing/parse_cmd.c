/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_cmd.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 14:49:29 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/22 14:53:43 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	is_valid_arg_token(t_token_node *token)
{
	return ((token->type == token_cmd || token->type == token_dquote
			|| token->type == token_squote) && !token->to_skip);
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
