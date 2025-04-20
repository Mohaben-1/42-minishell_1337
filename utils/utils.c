/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 13:49:10 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/10 15:05:50 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*ft_get_env(t_env *env, char *var)
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

void	ft_set_env(t_env *env, char *var, char *new_val)
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

long	ft_atoi(char *str)
{
	int		sign;
	long	res;

	sign = 1;
	res = 0;
	while (*str == 32 || (*str >= 9 && *str <= 13))
		str++;
	if (*str == '+' || *str == '-')
	{
		if (*str == '-')
			sign = -1;
		str++;
	}
	while (*str >= '0' && *str <= '9')
		res = res * 10 + (*str++ - '0');
	return (res * sign);
}

int	ft_get_index(char *s, char c)
{
	int	i;

	i = 0;
	while (s && s[i] && s[i] != c)
		i++;
	if (*s && s[i] == c)
		i++;
	return (i);
}

int	arg_count(char **args)
{
	int	i;

	i = 0;
	while (args && args[i])
		i++;
	return (i);
}
