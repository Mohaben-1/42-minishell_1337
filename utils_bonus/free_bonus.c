/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 17:37:13 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/23 17:37:14 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell_bonus.h"

void	free_double_ptr(char **s)
{
	int	i;

	if (!s)
		return ;
	i = 0;
	while (s[i])
	{
		free(s[i]);
		i++;
	}
	free(s);
}

void	free_env(t_env *env)
{
	t_env	*next;

	while (env)
	{
		next = env->next;
		free(env->var);
		free(env->value);
		free(env);
		env = next;
	}
}

void	free_token_list(t_token_node *tokens)
{
	t_token_node	*current;
	t_token_node	*next;

	if (!tokens)
		return ;
	current = tokens;
	while (current)
	{
		next = current->next;
		free(current->data);
		free(current);
		current = next;
	}
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

void	free_redirects(t_redirect *redirect)
{
	t_redirect	*tmp;

	while (redirect)
	{
		tmp = redirect;
		redirect = redirect->next;
		free(tmp->file);
		free(tmp);
	}
}
