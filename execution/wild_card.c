/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wild_card.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 19:54:03 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/13 17:45:38 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	is_wild_card(char *s)
{
	while (*s)
	{
		if (*s == '*')
			return (1);
		s++;
	}
	return (0);
}

static int	match(char *file_name, char *d_name)
{
	if (*file_name == '\0' && *d_name == '\0')
		return (1);
	if (*file_name == '*')
	{
		if (match(file_name + 1, d_name))
			return (1);
		if (*d_name && match(file_name, d_name + 1))
			return (1);
	}
	else if (*file_name == *d_name)
		return (match(file_name + 1, d_name + 1));
	return (0);
}

static int	count_matches(char *file_name)
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
		if (match(file_name, dir->d_name))
			count++;
	}
	closedir(d);
	return (count);
}

static char	**wildcard_expand(char *file_name)
{
	DIR				*d;
	struct dirent	*dir;
	char			**result;
	int				count;
	int				i;

	count = count_matches(file_name);
	if (count == 0)
		return (NULL);
	result = malloc(sizeof(char *) * (count + 1));
	if (!result)
		return (NULL);
	d = opendir(".");
	if (!d)
		return (free(result), NULL);
	i = 0;
	while ((dir = readdir(d)) && i < count)
	{
		if (dir->d_name[0] == '.' && file_name[0] != '.')
			continue ;
		if (match(file_name, dir->d_name))
			result[i++] = strdup(dir->d_name);
	}
	result[i] = NULL;
	return (closedir(d), result);
}

static int	arg_count(char **args)
{
	int	i;

	i = 0;
	while (args && args[i])
		i++;
	return (i);
}

static char **merge_args(char **old_args, int i, char **expand)
{
	char	**new_args;
	int		idx;
	int		j;
	int		k;

	new_args = malloc(sizeof(char *) * (arg_count(old_args) + arg_count(expand) + 1));
	if (!new_args)
		return (NULL);
	j = 0;
	k = 0;
	idx = 0;
	while (j < i)
		new_args[idx++] = ft_strdup(old_args[j++]);
	while (expand && expand[k])
		new_args[idx++] = ft_strdup(expand[k++]);
	j++;
	while (old_args[j])
		new_args[idx++] = ft_strdup(old_args[j++]);
	new_args[idx] = NULL;
	return (new_args);
}

void	rdr_wild_err(char **expanded, char *file, t_exec *exec)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(file, 2);
	ft_putstr_fd(": ambiguous redirect\n", 2);
	exec->exit_status = 1;
	free_double_ptr(expanded);
}

int	ft_expand_redr_wild(t_ast_node *ast, t_exec *exec)
{
	char		**expanded;
	t_redirect	*redr;
	char		*tmp;

	redr = ast->redirects;
	while (redr)
	{
		if (redr->file && redr->type != token_hrdc && is_wild_card(redr->file))
		{
			expanded = wildcard_expand(redr->file);
			if (!expanded || expanded[1])
			{
				rdr_wild_err(expanded, redr->file, exec);
				return (0);
			}
			else
			{
				tmp = redr->file;
				redr->file = expanded[0];
				free(tmp);
			}
		}
		redr = redr->next;
	}
	return (1);
}

void ft_expand_wildcard(t_ast_node *ast)
{
    char **expanded;
    char **new_args;
    int  *new_quote_types; // NEW: Parallel array for quote types
    int   i;

    if (!ast || ast->e_type != AST_COMMAND || !ast->args)
        return ;
    
    i = 0;
    while (ast->args[i])
    {
        if (ast->arg_quote_types[i] == 0 && is_wild_card(ast->args[i]))
        {
            expanded = wildcard_expand(ast->args[i]);
            if (expanded)
            {
                // Create new parallel quote type array
                new_quote_types = malloc(sizeof(int) * (arg_count(ast->args) + arg_count(expanded) + 1));
                
                // Copy existing quote types
                int idx = 0;
                for (int j = 0; j < i; j++)
                    new_quote_types[idx++] = ast->arg_quote_types[j];
                
                // Add new entries (wildcard expansions are unquoted)
                for (int k = 0; expanded[k]; k++)
                    new_quote_types[idx++] = 0;
                
                // Copy remaining original entries
                for (int j = i + 1; ast->args[j]; j++)
                    new_quote_types[idx++] = ast->arg_quote_types[j];

                // Replace old arrays with new ones
                new_args = merge_args(ast->args, i, expanded);
                free(ast->arg_quote_types); // Free old array
                ast->arg_quote_types = new_quote_types; // Update pointer
                free_double_ptr(ast->args);
                ast->args = new_args;
                ast->arg_count = arg_count(new_args);
                free_double_ptr(expanded);
                i = -1; // Reset loop after modifying array
            }
        }
        i++;
    }
}