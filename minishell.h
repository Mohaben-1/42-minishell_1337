/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 12:33:01 by mohaben-          #+#    #+#             */
/*   Updated: 2025/03/06 16:44:14 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <unistd.h>
# include <fcntl.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/wait.h>
# include <readline/readline.h>
# include <readline/history.h>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1
# endif

typedef struct s_token
{
	int	token_cmd;
	int	token_pipe;
	int	token_redir_in;
	int	token_redir_out;
	int	token_here_doc;
	int	token_append;
}	t_token;

typedef struct s_node
{
	t_token			token;
	char			*data;
	struct s_node	*left;
	struct s_node	*right;
}	t_node;

char	*get_next_line(int fd);
size_t	ft_strlen(char *s);
int		ft_isdigit(char c);
int		ft_atoi(const char *str);
char	*ft_strdup(char *s1);
char	*ft_substr(char *s, unsigned int start, size_t len);
char	*ft_strjoin(char *s1, char *s2);
char	*ft_strchr(char *s, int c);
int		ft_strncmp(char *s1, char *s2, size_t n);
char	**ft_split(char *s, char c);
void	free_split(char **s);
void	p_error_cmd(char **cmd, char **paths, int exit_status);
void	p_error(char *err, int exit_status);
char	*ft_get_path(char **envp);
void	ft_exec_cmd(char *cmd, char **envp);

t_token	ft_tokenizer(char *input);

#endif