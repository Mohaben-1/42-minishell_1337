/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_ve_helper_bonus.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 12:16:11 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/23 17:43:53 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell_bonus.h"

int	ft_env_size(t_env *env)
{
	int	size;

	size = 0;
	while (env)
	{
		size++;
		env = env->next;
	}
	return (size);
}

char	*ft_strjoin_env(char *s1, char *s2)
{
	char	*join;
	size_t	i;
	size_t	j;

	if (!s1 && !s2)
		return (NULL);
	if (!s2)
		s2 = "";
	join = malloc((ft_strlen(s1) + ft_strlen(s2) + 2) * sizeof(char));
	if (!join)
		return (NULL);
	i = 0;
	j = 0;
	while (s1[j])
		join[i++] = s1[j++];
	join[i++] = '=';
	j = 0;
	while (s2[j])
		join[i++] = s2[j++];
	join[i] = '\0';
	return (join);
}

char	**ft_set_envp(t_env *env)
{
	char	**envp;
	int		size;
	int		i;

	if (!env)
		return (NULL);
	size = ft_env_size(env);
	envp = malloc((size + 1) * sizeof(char *));
	if (!envp)
		return (NULL);
	i = 0;
	while (i < size)
	{
		envp[i] = ft_strjoin_env(env->var, env->value);
		env = env->next;
		i++;
	}
	envp[i] = NULL;
	return (envp);
}

void	exec_path_err(t_ast_node *ast, char **envp)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(ast->args[0], 2);
	ft_putstr_fd(": No such file or directory\n", 2);
	free_double_ptr(envp);
	free_ast_node(ast);
	exit(127);
}
