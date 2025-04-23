NAME = minishell

CC = cc
CFLAGS = -Wall -Werror -Wextra #-g -fsanitize=address,undefined
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
		parse_helpers/tokenize_consecutive.c parse_helpers/tokenize_ops.c parse_helpers/tokenizer_errors.c parse_helpers/tokenizer_helper.c parse_helpers/tokenizer_helper2.c \
		parse_helpers/tree_helper.c parse_helpers/tree_helper2.c parse_helpers/free_tree.c parse_helpers/parse_redir_helper.c \
		exec_helpers/expand_ast_args.c

OBJ = $(SRC:.c=.o)


$(NAME): $(OBJ)
	@$(CC) $(CFLAGS) $(OBJ) $(LIB_RL) -o $(NAME)
	@echo compiled

all: $(NAME)

%.o: %.c minishell.h
	@$(CC) $(CFLAGS) $(INCLUDE_RL) -c $< -o $@

clean:
	@rm -f $(OBJ)
	@echo clean

fclean: clean
	@rm -f $(NAME)
	@echo fclean

re: fclean all

.PHONY: all clean fclean re 
