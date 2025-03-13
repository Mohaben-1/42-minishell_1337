/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 12:33:01 by mohaben-          #+#    #+#             */
/*   Updated: 2025/03/13 13:23:00 by mohaben-         ###   ########.fr       */
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

typedef enum e_token_type
{
	token_cmd,
	token_pipe,
	token_in,
	token_out,
	token_hrdc,
	token_appnd
}	t_token_type;

typedef struct s_token_node
{
	t_token_type	type;
	char			*data;
	struct s_token_node	*next;
}	t_token_node;

typedef struct s_ast
{
	t_token_type	token;
	char			*data;
	struct s_ast	*left;
	struct s_ast	*right;
}	t_ast;

typedef struct s_env
{
	char			*var;
	char			*value;
	struct s_env	*next;
}	t_env;

char	*get_next_line(int fd);
size_t	ft_strlen(char *s);
int		ft_isdigit(char c);
int		ft_isalpha(int c);
int		ft_isalnum(int c);
long	ft_atoi(char *str);
char	*ft_strdup(char *s1);
char	*ft_substr(char *s, unsigned int start, size_t len);
char	*ft_strjoin(char *s1, char *s2);
char	*ft_strchr(char *s, int c);
int		ft_strncmp(char *s1, char *s2, size_t n);
int		ft_strcmp(char *s1, char *s2);
char	*ft_strtrim(char *s1, char *set);
char	**ft_split(char *s, char c);
void	free_split(char **s);
int		ft_count_split(char **cmd_split);
void	p_error_cmd(char **cmd, char **paths, int exit_status);
void	p_error(char *err, int exit_status);
char	*ft_get_path(char **envp);
void	ft_exec_cmd(char *cmd, t_env *env);
void	ft_putchar_fd(char c, int fd);
void	ft_putstr_fd(char *s, int fd);
void	ft_putnbr_fd(int n, int fd);
void	ft_cd(char *cmd, t_env *env, int *exit_status);
void	ft_export(char *cmd, t_env **env);
t_env	*ft_init_env(char **envp);
void	ft_print_env(t_env *env);
void	ft_env_add_back(t_env **lst, t_env *new);
t_env	*ft_env_new(char *var, char *value);
void	ft_unset(char *cmd, t_env **env);
int		ft_check_var_name(char *var);
void	ft_exit(char *input, int *exit_status);
char	*ft_get_val_env(t_env *env, char *var);
void	ft_set_val_env(t_env *env, char *var, char *new_val);
void	ft_pwd(t_env *env);
void	ft_echo(char *input);

t_token_node	*ft_tokenize(char *input);

#endif