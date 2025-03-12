/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 13:49:10 by mohaben-          #+#    #+#             */
/*   Updated: 2025/03/12 14:54:08 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*ft_get_val_env(t_env *env, char *var)
{
	if (!env)
		return (NULL);
	while (env)
	{
		if (!ft_strcmp(env->var, var))
			return (env->value);
		env = env->next;
	}
	return (NULL);
}

void	ft_set_val_env(t_env *env, char *var, char *new_val)
{
	char	*value;

	if (!env)
		return ;
	if (new_val)
		value = ft_strdup(new_val);
	else
		value = ft_strdup("");
	while (env)
	{
		if (!ft_strcmp(env->var, var))
		{
			free(env->value);
			env->value = value;
			return ;
		}
		env = env->next;
	}
}
