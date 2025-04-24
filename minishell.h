/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 12:33:01 by mohaben-          #+#    #+#             */
/*   Updated: 2025/04/24 19:30:00 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <unistd.h>
# include <fcntl.h>
# include <stdio.h>
# include <stdlib.h>
# include <signal.h>
# include <limits.h>
# include <dirent.h>
# include <termios.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <readline/readline.h>
# include <readline/history.h>

# define FORK_ERROR "minishell: fork: Resource temporarily unavailable\n"
# define PIPE_ERROR "minishell: pipe: Resource unavailable\n"

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
	t_token_type		type;
	char				*data;
	int					spaced;
	int					to_skip;
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
	char				*file;
	int					type;
	int					heredoc_fd;
	int					quoted;
	int					is_spaced;
	struct s_redirect	*next;
}	t_redirect;

typedef struct s_ast_node
{
	enum
	{
		AST_COMMAND,
		AST_PIPE,
		AST_AND_AND,
		AST_OR_OR,
		AST_SUBSHELL,
		AST_DQUOTES,
		AST_SQUOTES
	}	e_type;
	char				**args;
	int					arg_count;
	t_redirect			*redirects;
	struct s_ast_node	*left;
	struct s_ast_node	*right;
	int					*arg_quote_types;
	int					*arg_is_spaced;
	struct s_ast_node	*child;
}	t_ast_node;

typedef struct s_arg_meta
{
	int	*quote_types;
	int	*is_spaced;
}	t_arg_meta;

typedef struct s_pipe_data
{
	int	cmd_count;
	int	(*pipes_fd)[2];
	int	*pids;
}	t_pipe_data;

typedef struct s_exec
{
	t_env		**env;
	char		**envp;
	int			std_fd[2];
	int			exit_status;
}	t_exec;

typedef struct s_new_ast_args
{
	int		*new_arg_is_spaced;
	int		*new_quote_types;
	char	**new_args;
}	t_new_ast_args;

//Utils
size_t			ft_strlen(char *s);
int				ft_isdigit(char c);
int				ft_isalpha(int c);
int				ft_isalnum(int c);
long			ft_atoi(char *str);
char			*ft_itoa(int n);
char			*ft_strdup(char *s1);
char			*ft_substr(char *s, unsigned int start, size_t len);
char			*ft_strjoin(char *s1, char *s2);
char			*ft_strjoin_free(char *s1, char *s2);
int				ft_get_index(char *s, char c);
char			*ft_strchr(char *s, int c);
int				ft_strncmp(char *s1, char *s2, size_t n);
int				ft_strcmp(char *s1, char *s2);
char			*ft_strtrim(char *s1, char *set);
char			**ft_split(char *s, char c);
char			**ft_split_whitespaces(char *s);
int				arg_count(char **args);
int				is_whitespace(char c);
int				is_quotes(char c);
int				is_parentesis(char c);
int				is_operator(char c);

//Errors
void			ft_error(char *err, int exit_status);
void			ft_error_cmd(t_env *env, t_ast_node *ast, char **paths,
					int exit_status);
void			ft_error_file(char *file, t_exec *exec);
void			ft_error_file_expand(char *file, t_exec *exec);
void			ft_err_exprt(char *cmd, int *err_flag);
void			rdr_wild_err(char **expanded, char *file, t_exec *exec);
void			ft_err_unset(char *var);

//Buil_in
void			ft_exec_ve(t_ast_node *node, t_exec *exec);
void			ft_putchar_fd(char c, int fd);
void			ft_putstr_fd(char *s, int fd);
void			ft_putnbr_fd(int n, int fd);
void			ft_cd(char **args, t_exec *exec);
void			ft_export(t_ast_node *ast, t_exec *exec);
t_env			*ft_init_env(char **envp);
void			ft_env(t_exec *exec);
void			ft_env_add_back(t_env **lst, t_env *new);
t_env			*ft_env_new(char *var, char *value);
void			ft_unset(char **args, t_exec *exec);
int				ft_check_var_name(char *var);
void			ft_exit(t_ast_node *ast, t_exec *exec);
char			*ft_get_env(t_env *env, char *var);
void			ft_set_env(t_env *env, char *var, char *new_val);
void			ft_pwd(t_exec *exec);
void			ft_echo(char **args, t_exec *exec);

//Parsing
t_token_node	*ft_tokenize(char *input, t_exec *exec);
t_ast_node		*build_ast(t_token_node *tokens, t_exec *exec);
t_ast_node		*parse_logical_ops(t_token_node *tokens, t_exec *exec);
t_ast_node		*parse_pipes(t_token_node *tokens, t_exec *exec);
t_ast_node		*parse_command(t_token_node *tokens);
t_ast_node		*parse_subshell(t_token_node *tokens, t_exec *exec);
t_token_node	*find_op_at_level(t_token_node *tokens, t_token_type type1, 
					t_token_type type2);
t_token_node	*find_token_at_level(t_token_node *tokens, t_token_type type);
t_token_node	*extract_tokens(t_token_node *start, t_token_node *end);
t_ast_node		*create_ast_node(int type);
t_redirect		*parse_redirections(t_token_node **tokens);
int				is_redirection(t_token_type type);
int				count_args(t_token_node *tokens);
char			**collect_args(t_token_node *tokens, int count, 
					int **quote_types, int **is_spaced);
void			process_joined_tokens(t_token_node **last, int redir_type, 
					t_redirect **current, t_redirect **head);
t_redirect		*create_redirect_type(t_token_node *token, int redir_type);
t_token_node	*create_token_node(t_token_node *src);

//Execution
void			execute_ast(t_ast_node *ast, t_exec *exec);
void			ft_execute_command(t_ast_node *node, t_exec *exec);
void			ft_execute_pipe(t_ast_node *node, t_exec *exec, int cmd_count);
void			execute_subshell(t_ast_node *ast, t_exec *exec);
void			ft_restore_std_fd(t_exec *exec);
int				ft_apply_redirect(t_ast_node *node, t_exec *exec);
char			*ft_expand(char *arg, t_exec *exec);
void			execute_builtin(t_ast_node *node, t_exec *exec);
int				ft_is_builtin(char *cmd);
int				ft_handle_heredoc(t_redirect *redr, t_exec *exec);
int				count_pipe_cmd(t_ast_node *ast);
void			ft_expand_wildcard(t_ast_node *ast);
int				ft_expand_redr_wild(t_ast_node *ast, t_exec *exec);

//Signals
void			ft_handle_sigint(int sig);
void			handle_sig_exec_ve(int sig);
void			heredoc_child_signal(int sig);
void			handle_main_sigs(int ac, char **av);

//Free
void			free_double_ptr(char **s);
void			free_ast_node(t_ast_node *ast);
void			free_env(t_env *env);
void			free_token_list(t_token_node *tokens);
void			ft_token_node_free(t_token_node **head);
void			free_redirects(t_redirect *redirect);

//exec helpers
void			exec_path_err(t_ast_node *ast, char **envp);
char			**ft_set_envp(t_env *env);
char			*ft_strjoin_env(char *s1, char *s2);
int				ft_env_size(t_env *env);
void			exec_pipe_cmd(t_ast_node *ast, t_exec *exec);
void			init_pipe_data(t_pipe_data *pipe_data, int cmd_count,
					int (*pipes_fd)[2], int *pids);
void			close_pipes_fd(int pipes_fd[][2], int count);
void			collect_pipe_cmd(t_ast_node *ast, t_ast_node **ast_pipes, 
					int *index);
int				count_pipe_cmd(t_ast_node *ast);
void			process_hrdc_pipes(t_ast_node **ast_pipes, int cmd_count, 
					t_exec *exec);
void			process_pipes(int pipes_fd[][2], int cmd_count, t_exec *exec);
void			ft_free_export(char *var, char *value);
void			export_exit_status(t_exec *exec, int err_flag);
void			ft_print_export(t_env *env);
void			ft_append_env(char *var, char *value, t_env **env);
void			ft_update_env(char *var, char *value, t_env **env);
void			ft_restore_std_fd(t_exec *exec);
int				proccess_redr_in(t_redirect *redr, int fd, t_exec *exec);
int				proccess_redr_out(t_redirect *redr, int fd, t_exec *exec);
int				proccess_redr_append(t_redirect *redr, int fd, t_exec *exec);
int				proccess_heredoc(t_redirect *redr);
int				check_hrdc_priority(t_redirect *redirect);
int				is_wild_card(char *s);
char			**allocate_wild_res(char *file_name, int *count);
char			**wildcard_expand(char *file_name);
int				ft_expand_redr_wild(t_ast_node *ast, t_exec *exec);
char			**handle_wildcard_expansion(t_ast_node *ast, int i);
int				match_wild_card(char *file_name, char *d_name);
int				count_wild_matches(char *file_name);
void			expand_args(t_ast_node *ast, t_exec *exec);
void			expand_ast_args(t_ast_node *ast, t_exec *exec);
void			filter_unquoted_empty_args(t_ast_node *ast, t_exec *exec);
void			prepare_ast_args(t_ast_node *ast, t_exec *exec);
void			close_heredoc_fds(t_ast_node **ast_pipes, int cmd_count);
void			copy_first_args(t_new_ast_args *new, t_ast_node *ast, int i);
void			copy_split_args(t_new_ast_args *new, t_ast_node *ast, 
					char **splited, int i);
void			copy_remaining_args(t_new_ast_args *new, t_ast_node *ast, 
					int i, int split_count);

//Parsing helpers
t_token_node	*ft_token_last(t_token_node *list);
void			ft_token_syntax_error(t_token_node *list, int *error);
void			ft_quotes_error(char quote_type);
void			ft_add_token(t_token_node **head, t_token_type type, 
					char *data, int spaced);
void			ft_print_paren_error(t_token_node *head, t_token_node *list,
					int *error);
t_token_node	*ft_handle_tokenize_error(t_token_node **head, t_exec *exec,
					int error);
void			ft_handle_quotes(char *input, int *i, t_token_node **head, 
					int *error);
void			ft_handle_str(char *input, int *i, t_token_node **head);
void			ft_handle_special_tokens(char *input, int *i, 
					t_token_node **head);
void			ft_handle_redirections(char *input, int *i, 
					t_token_node **head);
void			ft_handle_parenthesis(char *input, int *i, t_token_node **head);
void			ft_handle_pipe(char *input, int *i, t_token_node **head);
void			ft_valid_parentesis(t_token_node *list, int *error);
int				ft_check_complex_case(t_token_node *list, t_token_node *head);
void			ft_check_last_token(t_token_node *list, int *error);
void			ft_check_consecutive_paren(t_token_node *list, 
					t_token_node *head, int *error);
void			ft_check_consecutive_operators(t_token_node *list, int *error);
void			ft_consecutive_operators(t_token_node *list, int *error);
int				is_node_operator(t_token_node *node);
void			ft_is_quotes_spaced(char *input, int *i, int *is_spaced);
void			ft_add_token_quotes(char quote_type, t_token_node **head, 
					char *str, int is_spaced);
t_token_node	*ft_before_this_token(t_token_node *list, t_token_node *token);
void			ft_check_open_paren(t_token_node *head, t_token_node *list,
					int *error);
void			ft_put_token_error(t_token_node *list);
void			ft_check_close_paren(t_token_node *list, int *paren_count,
					int *error);
t_token_node	*ft_before_this_token(t_token_node *list, t_token_node *token);
void			ft_valid_redirections(t_token_node *list, int *error);
t_ast_node		*parse_logical_ops(t_token_node *tokens, t_exec *exec);
t_ast_node		*handle_logical_operators(t_ast_node *node, 
					t_token_node *tokens, t_exec *exec);
t_token_node	*find_op_at_level(t_token_node *tokens, t_token_type type1, 
					t_token_type type2);
t_token_node	*find_token_at_level(t_token_node *tokens, t_token_type type);
t_token_node	*extract_tokens(t_token_node *start, t_token_node *end);
void			free_ast_args(t_ast_node *node);
void			free_ast_node(t_ast_node *node);
void			free_collected_args(char **args, int *quote_types, 
					int *is_spaced);
char			**free_collected_args_and_return(char **args, int *qt, 
					int *spaced);
t_token_node	*free_token_list_and_return_null(t_token_node *list);
void			mark_redirection_tokens(t_token_node *redir, 
					t_token_node **last);
char			**allocate_args_memory(int count, int **quote_types, 
					int **is_spaced);
void			add_redirect_to_list(t_redirect **head, t_redirect **current, 
					t_redirect *redir);
int				get_quote_type(int token_type);

#endif