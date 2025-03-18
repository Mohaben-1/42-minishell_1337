/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 12:33:01 by mohaben-          #+#    #+#             */
/*   Updated: 2025/03/18 16:42:52 by mohaben-         ###   ########.fr       */
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
	token_pipe,
	token_cmd,
	token_in,
	token_out,
	token_or,
	token_hrdc,
	token_appnd,
	token_dquote,
	token_squote,
	token_and_and,
	token_wildcard,
	token_paren_open,
	token_paren_close,
}	t_token_type;

typedef struct s_token_node
{
	t_token_type	type;
	char			*data;
	struct s_token_node	*next;
}	t_token_node;

typedef struct s_env
{
	char			*var;
	char			*value;
	struct s_env	*next;
}	t_env;

typedef struct s_redirect
{
	int					type;              // token_in, token_out, token_hrdc, token_appnd
	char				*file;            // Filename or delimiter
	struct s_redirect	*next;
} t_redirect;


typedef struct s_ast_node
{
    enum {
        AST_COMMAND,       // Simple command with args
        AST_PIPE,          // Pipe operator
        AST_AND_AND,       // && operator
        AST_OR_OR,         // || operator
        AST_SUBSHELL,	   // Commands in parentheses
		AST_DQUOTES,       // Double quoted string
        AST_SQUOTES        // Single quoted string
    } type;
    
    // For command nodes
    char **args;           // Command and its arguments
    int arg_count;         // Number of arguments
    
    // For redirection
    t_redirect *redirects; // List of redirections for this command
    
    // For binary operations (pipe, &&, ||)
    struct s_ast_node *left;
    struct s_ast_node *right;
    int *arg_quote_types;  // New field to track quote types for each argument

    // For subshell commands
    struct s_ast_node *child;
} t_ast_node;

typedef struct s_exec
{
	t_env		**env;
	t_ast_node	*ast;
	char		**envp;
	int			std_fd[2];
	int			exit_status;
}	t_exec;

char	*get_next_line(int fd);
size_t	ft_strlen(char *s);
int		ft_isdigit(char c);
int		ft_isalpha(int c);
int		ft_isalnum(int c);
long	ft_atoi(char *str);
char	*ft_strdup(char *s1);
char	*ft_substr(char *s, unsigned int start, size_t len);
char	*ft_strjoin(char *s1, char *s2);
int		ft_get_index(char *s, char c);
char	*ft_strchr(char *s, int c);
int		ft_strncmp(char *s1, char *s2, size_t n);
int		ft_strcmp(char *s1, char *s2);
char	*ft_strtrim(char *s1, char *set);
char	**ft_split(char *s, char c);
void	free_split(char **s);
int		ft_count_args(char **cmd_split);


void	ft_error(char *err, int exit_status);
void	ft_error_cmd(char *cmd, char **paths, int exit_status);
void	ft_error_file(char *file, t_exec *exec);


char	*ft_get_path(char **envp);
void	ft_exec_ve(t_ast_node *node, t_exec *exec);
void	ft_putchar_fd(char c, int fd);
void	ft_putstr_fd(char *s, int fd);
void	ft_putnbr_fd(int n, int fd);
void	ft_cd(char **args, t_exec *exec);
void	ft_export(t_ast_node *ast, t_exec *exec);
t_env	*ft_init_env(char **envp);
void	ft_env(t_env *env);
void	ft_env_add_back(t_env **lst, t_env *new);
t_env	*ft_env_new(char *var, char *value);
void	ft_unset(char **args, t_exec *exec);
int		ft_check_var_name(char *var);
void	ft_exit(char **args, t_exec *exec);
char	*ft_get_val_env(t_env *env, char *var);
void	ft_set_val_env(t_env *env, char *var, char *new_val);
void	ft_pwd(t_exec *exec);
void	ft_echo(char **args, t_exec *exec);


t_token_node	*ft_tokenize(char *input);
t_ast_node *build_ast(t_token_node *tokens);
t_ast_node *parse_logical_ops(t_token_node *tokens);
t_ast_node *parse_pipes(t_token_node *tokens);
t_ast_node *parse_command(t_token_node *tokens);
t_ast_node *parse_subshell(t_token_node *tokens);
t_token_node *find_op_at_level(t_token_node *tokens, t_token_type type1, t_token_type type2);
t_token_node *find_token_at_level(t_token_node *tokens, t_token_type type);
t_token_node *extract_tokens(t_token_node *start, t_token_node *end);
t_ast_node *create_ast_node(int type);
t_redirect *parse_redirections(t_token_node **tokens);
void free_ast(t_ast_node *ast);
int is_redirection(t_token_type type);
int count_args(t_token_node *tokens);
int token_list_len(t_token_node *tokens);
char **collect_args(t_token_node *tokens, int count, int **quote_types);
void print_ast(t_ast_node *ast, int indent_level);



void	execute_ast(t_ast_node *ast, t_exec *exec);
void	execute_command(t_ast_node *node, t_exec *exec);
void	ft_execute_pipe(t_ast_node *node, t_exec *exec);

#endif