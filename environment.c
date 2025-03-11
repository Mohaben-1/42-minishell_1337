/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   environment.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 17:14:58 by mohaben-          #+#    #+#             */
/*   Updated: 2025/03/11 11:07:42 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_env	*ft_env_new(char *var, char *value)
{
	t_env	*new;

	new = malloc(sizeof(t_env));
	if (!new)
		return (NULL);
	new->var = ft_strdup(var);
	if (value)
		new->value = ft_strdup(value);
	else
		new->value = ft_strdup("");
	new->next = NULL;
	return (new);
}

void	ft_env_add_back(t_env **lst, t_env *new)
{
	t_env	*last;

	if (!lst || !new)
		return ;
	if (!*lst)
	{
		*lst = new;
		return ;
	}
	last = *lst;
	while (last->next)
		last = last->next;
	last->next = new;
}

t_env	*ft_init_env(char **envp)
{
	t_env	*head;
	char	**envp_splited;

	if (!envp || !*envp)
		return (NULL);
	head = NULL;
	while (*envp)
	{
		envp_splited = ft_split(*envp, '=');
		ft_env_add_back(&head, ft_env_new(envp_splited[0], envp_splited[1]));
		free_split(envp_splited);
		envp++;
	}
	return (head);
}

void	ft_print_env(t_env *env)
{
	if (!env)
		return ;
	while (env)
	{
		ft_putstr_fd(env->var, 1);
		ft_putchar_fd('=', 1);
		ft_putstr_fd(env->value, 1);
		ft_putchar_fd('\n', 1);
		env = env->next;
	}
}
