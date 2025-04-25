/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 16:42:55 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/25 17:26:29 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_env	*ft_env_new(char *var, char *value, int printed)
{
	t_env	*new;

	new = malloc(sizeof(t_env));
	if (!new)
		return (NULL);
	new->var = ft_strdup(var);
	if (value)
	{
		value = ft_strdup(value);
		new->value = value;
	}
	else
		new->value = ft_strdup("");
	new->printed = printed;
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

t_env	*ft_init_null_envp(void)
{
	t_env	*head;
	char	*pwd;
	char	*last_exec;

	head = NULL;
	pwd = getcwd(NULL, 0);
	last_exec = ft_strjoin(pwd, "./minishell");
	head = ft_env_new("PWD", pwd, 1);
	ft_env_add_back(&head, ft_env_new("SHLVL", "1", 1));
	ft_env_add_back(&head, ft_env_new("_", last_exec, 1));
	free(pwd);
	free(last_exec);
	return (head);
}

t_env	*ft_init_env(char **envp)
{
	t_env	*head;

	char	**envp_splited;

	if (!envp)
		return (NULL);
	if (!*envp)
		return (ft_init_null_envp());
	head = NULL;
	while (*envp)
	{
		envp_splited = ft_split(*envp, '=');
		ft_env_add_back(&head, ft_env_new(envp_splited[0], envp_splited[1], 1));
		free_double_ptr(envp_splited);
		envp++;
	}
	return (head);
}

void	ft_env(t_exec *exec)
{
	t_env	*current;

	if (!*(exec->env))
		return ;
	current = *(exec->env);
	if (*(exec->envp) && !ft_get_env(current, "PATH"))
	{
		ft_putstr_fd("minishell: env: No such file or directory\n", 2);
		exec->exit_status = 127;
		return ;
	}
	while (current)
	{
		if (current->printed)
		{
			ft_putstr_fd(current->var, 1);
			ft_putchar_fd('=', 1);
			ft_putstr_fd(current->value, 1);
			ft_putchar_fd('\n', 1);
		}
		current = current->next;
	}
	exec->exit_status = 0;
}
