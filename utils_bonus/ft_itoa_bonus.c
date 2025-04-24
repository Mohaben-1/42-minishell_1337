/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 17:37:23 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/23 17:37:24 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell_bonus.h"

int	nb_len(int n)
{
	int	len;

	len = 0;
	if (n <= 0)
		len = 1;
	while (n != 0)
	{
		n /= 10;
		len++;
	}
	return (len);
}

char	*ft_itoa(int n)
{
	char	*res;
	int		len;
	long	num;

	len = nb_len(n);
	num = n;
	res = (char *)malloc((len + 1) * sizeof(char));
	if (res == NULL)
		return (NULL);
	res[len] = '\0';
	if (n < 0)
	{
		res[0] = '-';
		num = -num;
	}
	while (len > 0)
	{
		len--;
		if (n < 0 && len == 0)
			return (res);
		res[len] = (num % 10) + '0';
		num /= 10;
	}
	return (res);
}
