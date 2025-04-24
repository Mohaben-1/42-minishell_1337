NAME = minishell

CC = cc
CFLAGS = -Wall -Werror -Wextra
LIB_RL = -L/goinfre/mohaben-/homebrew/opt/readline/lib -lreadline
INCLUDE_RL = -I/goinfre/mohaben-/homebrew/opt/readline/include

SRC = minishell.c \
		built_in/cd.c built_in/export.c built_in/env.c built_in/unset.c  built_in/exit.c built_in/pwd.c built_in/echo.c \
		str_utils/str_utils1.c str_utils/str_utils2.c str_utils/str_utils3.c str_utils/ft_print.c \
		utils/utils.c utils/free.c utils/errors.c utils/errors2.c utils/ft_split.c utils/ft_itoa.c utils/signals.c utils/ft_split_whitespaces.c \
		execution/exec_ast.c  execution/exec_cmd.c execution/exec_ve.c execution/pipe.c execution/expand.c  execution/redirection.c execution/here_doc.c execution/wild_card.c \
		parsing/tokenizer.c parsing/ft_tree.c parsing/parse_cmd.c parsing/parse_redirection.c parsing/parse_subshell.c \
		exec_helpers/export_helper.c exec_helpers/exec_ve_helper.c exec_helpers/pipe_helper.c exec_helpers/pipe_helper2.c exec_helpers/expand_ast.c exec_helpers/filter_ast.c \
		exec_helpers/prepare_ast_args.c exec_helpers/redirection_helper.c exec_helpers/wild_card_helper.c exec_helpers/wild_card_helper2.c exec_helpers/env_helper.c \
		exec_helpers/expand_ast_args.c exec_helpers/expand_ast_args_helper.c \
		parse_helpers/tokenize_consecutive.c parse_helpers/tokenize_ops.c parse_helpers/tokenizer_errors.c parse_helpers/tokenizer_helper.c parse_helpers/tokenizer_helper2.c \
		parse_helpers/tree_helper.c parse_helpers/tree_helper2.c parse_helpers/free_tree.c parse_helpers/parse_redir_helper.c

SRC_BNS = minishell_bonus.c \
		built_in_bonus/cd_bonus.c built_in_bonus/export_bonus.c built_in_bonus/env_bonus.c built_in_bonus/unset_bonus.c  built_in_bonus/exit_bonus.c built_in_bonus/pwd_bonus.c built_in_bonus/echo_bonus.c \
		str_utils_bonus/str_utils1_bonus.c str_utils_bonus/str_utils2_bonus.c str_utils_bonus/str_utils3_bonus.c str_utils_bonus/ft_print_bonus.c \
		utils_bonus/utils_bonus.c utils_bonus/free_bonus.c utils_bonus/errors_bonus.c utils_bonus/errors2_bonus.c utils_bonus/ft_split_bonus.c utils_bonus/ft_itoa_bonus.c utils_bonus/signals_bonus.c utils_bonus/ft_split_whitespaces_bonus.c \
		execution_bonus/exec_ast_bonus.c  execution_bonus/exec_cmd_bonus.c execution_bonus/exec_ve_bonus.c execution_bonus/pipe_bonus.c execution_bonus/expand_bonus.c  execution_bonus/redirection_bonus.c execution_bonus/here_doc_bonus.c execution_bonus/wild_card_bonus.c \
		parsing_bonus/tokenizer_bonus.c parsing_bonus/ft_tree_bonus.c parsing_bonus/parse_cmd_bonus.c parsing_bonus/parse_redirection_bonus.c parsing_bonus/parse_subshell_bonus.c \
		exec_helpers_bonus/export_helper_bonus.c exec_helpers_bonus/exec_ve_helper_bonus.c exec_helpers_bonus/pipe_helper_bonus.c exec_helpers_bonus/pipe_helper2_bonus.c exec_helpers_bonus/expand_ast_bonus.c exec_helpers_bonus/filter_ast_bonus.c \
		exec_helpers_bonus/prepare_ast_args_bonus.c exec_helpers_bonus/redirection_helper_bonus.c exec_helpers_bonus/wild_card_helper_bonus.c exec_helpers_bonus/wild_card_helper2_bonus.c exec_helpers_bonus/env_helper_bonus.c \
		exec_helpers_bonus/expand_ast_args_bonus.c exec_helpers_bonus/expand_ast_args_helper_bonus.c \
		parse_helpers_bonus/tokenize_consecutive_bonus.c parse_helpers_bonus/tokenize_ops_bonus.c parse_helpers_bonus/tokenizer_errors_bonus.c parse_helpers_bonus/tokenizer_helper_bonus.c parse_helpers_bonus/tokenizer_helper2_bonus.c \
		parse_helpers_bonus/tree_helper_bonus.c parse_helpers_bonus/tree_helper2_bonus.c parse_helpers_bonus/free_tree_bonus.c parse_helpers_bonus/parse_redir_helper_bonus.c


OBJ = $(SRC:.c=.o)

OBJ_BNS = $(SRC_BNS:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	@rm -f $(OBJ_BNS)
	@$(CC) $(CFLAGS) $(OBJ) $(LIB_RL) -o $(NAME)
	@echo mandatory compiled


bonus: $(OBJ_BNS)
	@rm -f $(OBJ)
	@$(CC) $(CFLAGS) $(OBJ_BNS) $(LIB_RL) -o $(NAME)
	@touch bonus
	@echo bonus compiled


%_bonus.o: %_bonus.c  minishell_bonus.h
	@$(CC) $(CFLAGS) $(INCLUDE_RL) -c $< -o $@

%.o: %.c minishell.h
	@$(CC) $(CFLAGS) $(INCLUDE_RL) -c $< -o $@

clean:
	@rm -f $(OBJ) $(OBJ_BNS) bonus
	@echo clean

fclean: clean
	@rm -f $(NAME)
	@echo fclean

re: fclean all

.PHONY: all clean fclean re
