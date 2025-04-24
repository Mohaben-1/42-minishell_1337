/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 17:38:10 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/23 17:38:12 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell_bonus.h"

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

int	is_operator(char c)
{
	return (c == '|' || c == '<' || c == '>' || c == '&');
}
