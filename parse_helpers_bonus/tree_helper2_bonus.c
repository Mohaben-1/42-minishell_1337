/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tree_helper2_bonus.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 15:00:10 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/23 17:42:15 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell_bonus.h"

int	get_quote_type(int token_type)
{
	if (token_type == token_dquote)
		return (token_dquote);
	if (token_type == token_squote)
		return (token_squote);
	return (0);
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
