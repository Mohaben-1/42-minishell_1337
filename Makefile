NAME = minishell

CC = cc
CFLAGS = -Wall -Werror -Wextra
LIB_RL = -L/goinfre/mohaben-/homebrew/opt/readline/lib -lreadline
INCLUDE_RL = -I/goinfre/mohaben-/homebrew/opt/readline/include

SRC = minishell.c get_next_line.c str_utils1.c str_utils2.c str_utils3.c utils.c ft_split.c execution.c free.c errors.c tokenizer.c ft_print.c \
		built_in/cd.c built_in/export.c built_in/env.c built_in/unset.c  built_in/exit.c built_in/pwd.c built_in/echo.c

OBJ = $(SRC:.c=.o)


$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LIB_RL) -o $(NAME)

all: $(NAME)

%.o: %.c minishell.h
	$(CC) $(CFLAGS) $(INCLUDE_RL) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re 
