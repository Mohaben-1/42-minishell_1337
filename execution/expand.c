/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 16:53:44 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/15 20:10:14 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*ft_strjoin_free(char *s1, char *s2)
{
	char	*join;
	size_t	i;
	size_t	j;

	if (s1 == NULL && s2 == NULL)
		return (NULL);
	if (s1 == NULL && s2 != NULL)
		return (ft_strdup(s2));
	if (s1 != NULL && s2 == NULL)
		return (ft_strdup(s1));
	join = malloc((ft_strlen(s1) + ft_strlen(s2) + 1) * sizeof(char));
	if (join == NULL)
		return (NULL);
	i = 0;
	j = 0;
	while (s1[j])
		join[i++] = s1[j++];
	j = 0;
	while (s2[j])
		join[i++] = s2[j++];
	join[i] = '\0';
	free(s1);
	free(s2);
	return (join);
}

void	ft_expand_helper(char **expand, char *arg, t_exec *exec, int *i)
{
	char	*var;
	char	*value;
	int		start;

	(*i)++;
	if (arg[*i] == '?')
	{
		*expand = ft_strjoin_free(*expand, ft_itoa(exec->exit_status));
		(*i)++;
		return ;
	}
	start = *i;
	while (arg[*i] && (ft_isalnum(arg[*i]) || arg[*i] == '_'))
		(*i)++;
	var = ft_substr(arg, start, *i - start);
	value = ft_get_env(*(exec->env), var);
	if (value)
		*expand = ft_strjoin_free(*expand, ft_strdup(value));
	else
		*expand = ft_strjoin_free(*expand, ft_strdup(""));
	if (var)
		free(var);
}

char	*ft_expand(char *arg, t_exec *exec)
{
	char	*expand;
	char	*tmp;
	int		i;

	i = 0;
	expand = ft_strdup("");
	while (arg[i])
	{
		if (arg[i] == '$' && arg[i + 1] && (ft_isalnum(arg[i + 1]) || arg[i + 1] == '_' || arg[i + 1] == '?'))
			ft_expand_helper(&expand, arg, exec, &i);
		else
		{
			tmp = malloc(2);
			tmp[0] = arg[i];
			tmp[1] = '\0';
			expand = ft_strjoin_free(expand, tmp);
			i++;
		}
	}
	return (expand);
}
