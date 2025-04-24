/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wild_card_helper2_bonus.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 11:27:29 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/23 17:45:09 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell_bonus.h"

int	match_wild_card(char *file_name, char *d_name)
{
	if (*file_name == '\0' && *d_name == '\0')
		return (1);
	if (*file_name == '*')
	{
		if (match_wild_card(file_name + 1, d_name))
			return (1);
		if (*d_name && match_wild_card(file_name, d_name + 1))
			return (1);
	}
	else if (*file_name == *d_name)
		return (match_wild_card(file_name + 1, d_name + 1));
	return (0);
}

int	count_wild_matches(char *file_name)
{
	DIR				*d;
	struct dirent	*dir;
	int				count;

	d = opendir(".");
	if (!d)
		return (0);
	count = 0;
	while (1)
	{
		dir = readdir(d);
		if (!dir)
			return (closedir(d), count);
		if (dir->d_name[0] == '.' && file_name[0] != '.')
			continue ;
		if (match_wild_card(file_name, dir->d_name))
			count++;
	}
	closedir(d);
	return (count);
}
