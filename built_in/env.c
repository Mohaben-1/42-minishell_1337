/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 16:42:55 by mohaben-          #+#    #+#             */
/*   Updated: 2025/03/16 13:50:35 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_env	*ft_env_new(char *var, char *value)
{
	t_env	*new;
	char	quote;

	new = malloc(sizeof(t_env));
	if (!new)
		return (NULL);
	new->var = ft_strdup(var);
	if (value)
	{
		if (value[0] == '\'' || value[0] == '"')
		{
			quote = value[0];
			value = ft_strtrim(value, &quote);
		}
		else
			value = ft_strdup(value);
		new->value = value;
	}
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
	char	*pwd;
	char	**envp_splited;
	char	*last_exec;

	if (!envp)
		return (NULL);
	if (!*envp)
	{
		pwd = getcwd(NULL, 0);
		last_exec = ft_strjoin(pwd, "./minishell");
		head = ft_env_new("PWD", pwd);
		ft_env_add_back(&head, ft_env_new("SHLVL", "1"));
		ft_env_add_back(&head, ft_env_new("_", last_exec));
		free(pwd);
		free(last_exec);
		return (head);
	}
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

void	ft_env(t_env *env)
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
