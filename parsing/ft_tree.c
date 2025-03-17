/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_tree.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 20:58:14 by ahouass           #+#    #+#             */
/*   Updated: 2025/03/17 15:35:54 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* Main function to build AST from tokens */
t_ast_node *build_ast(t_token_node *tokens)
{
    if (!tokens)
        return NULL;
        
    return parse_logical_ops(tokens);
}

/* Parse logical operators (&&, ||) */
t_ast_node *parse_logical_ops(t_token_node *tokens)
{
    t_token_node *op = find_op_at_level(tokens, token_and_and, token_or);
    
    if (op)
    {
        t_ast_node *node = create_ast_node(op->type == token_and_and ? AST_AND_AND : AST_OR_OR);
        
        // Get tokens before and after the operator
        t_token_node *left_tokens = extract_tokens(tokens, op);
        t_token_node *right_tokens = op->next;
        
        // Recursively parse both sides
        node->left = parse_logical_ops(left_tokens);
        node->right = parse_logical_ops(right_tokens);
        
        return node;
    }
    
    // If no logical operators, look for pipes
    return parse_pipes(tokens);
}

/* Parse pipe operators */
t_ast_node *parse_pipes(t_token_node *tokens)
{
    t_token_node *pipe = find_token_at_level(tokens, token_pipe);
    
    if (pipe)
    {
        t_ast_node *node = create_ast_node(AST_PIPE);
        
        // Get tokens before and after the pipe
        t_token_node *left_tokens = extract_tokens(tokens, pipe);
        t_token_node *right_tokens = pipe->next;
        
        // Recursively parse both sides
        node->left = parse_pipes(left_tokens);
        node->right = parse_pipes(right_tokens); 
        
        return node;
    }
    
    // Check for subshell (parentheses)
    if (tokens->type == token_paren_open)
        return parse_subshell(tokens);
        
    // Parse as simple command if no operators found
    return parse_command(tokens);
}

/* Parse a subshell (commands in parentheses) */
t_ast_node *parse_subshell(t_token_node *tokens)
{
    if (tokens->type != token_paren_open)
        return NULL;
    
    // Find matching closing parenthesis
    t_token_node *tmp = tokens->next;
    int paren_count = 1;
    t_token_node *closing = NULL;
    
    while (tmp && paren_count > 0)
    {
        if (tmp->type == token_paren_open)
            paren_count++;
        else if (tmp->type == token_paren_close)
        {
            paren_count--;
            if (paren_count == 0)
                closing = tmp;
        }
        tmp = tmp->next;
    }
    
    if (!closing)
        return NULL; // Unbalanced parentheses
    
    // Create subshell node
    t_ast_node *node = create_ast_node(AST_SUBSHELL);
    
    // Extract commands inside parentheses
    t_token_node *inner_tokens = extract_tokens(tokens->next, closing);
    
    // Parse content recursively
    node->child = parse_logical_ops(inner_tokens);
    
    // If there are tokens after closing parenthesis, handle them
    if (closing->next)
    {
        // Check for pipes or logical operators after the subshell
        if (closing->next->type == token_pipe)
        {
            t_ast_node *pipe_node = create_ast_node(AST_PIPE);
            pipe_node->left = node;
            pipe_node->right = parse_pipes(closing->next->next);
            return pipe_node;
        }
        else if (closing->next->type == token_and_and || closing->next->type == token_or)
        {
            t_ast_node *log_node = create_ast_node(closing->next->type == token_and_and ? AST_AND_AND : AST_OR_OR);
            log_node->left = node;
            log_node->right = parse_logical_ops(closing->next->next);
            return log_node;
        }
    }
    
    return node;
}

/* Parse a simple command with its arguments and redirections */
t_ast_node *parse_command(t_token_node *tokens)
{
    if (!tokens)
        return NULL;
    
    t_ast_node *node = create_ast_node(AST_COMMAND);
    t_token_node *cmd_tokens = tokens;
    
    // Parse redirections first
    node->redirects = parse_redirections(&cmd_tokens);
    
    // Count arguments (skip redirection tokens)
    int arg_count = count_args(cmd_tokens);
    
    if (arg_count > 0)
    {
        // Collect command and arguments with quote information
        node->args = collect_args(cmd_tokens, arg_count, &node->arg_quote_types);
        node->arg_count = arg_count;
    }
    else
    {
        // No command, only redirections
        node->args = NULL;
        node->arg_count = 0;
        node->arg_quote_types = NULL;
    }
    
    return node;
}

/* Find an operator (type1 or type2) at the current nesting level */
t_token_node *find_op_at_level(t_token_node *tokens, t_token_type type1, t_token_type type2)
{
    t_token_node *tmp = tokens;
    t_token_node *last_op = NULL;
    int paren_level = 0;
    
    while (tmp)
    {
        if (tmp->type == token_paren_open)
            paren_level++;
        else if (tmp->type == token_paren_close)
            paren_level--;
        
        // Only consider operators at the current level (not inside parentheses)
        if (paren_level == 0 && (tmp->type == type1 || tmp->type == type2))
            last_op = tmp;
        
        tmp = tmp->next;
    }
    
    return last_op;
}

/* Find a token of specific type at the current nesting level */
t_token_node *find_token_at_level(t_token_node *tokens, t_token_type type)
{
    t_token_node *tmp = tokens;
    t_token_node *last_token = NULL;
    int paren_level = 0;
    
    while (tmp)
    {
        if (tmp->type == token_paren_open)
            paren_level++;
        else if (tmp->type == token_paren_close)
            paren_level--;
        
        // Only consider tokens at the current level
        if (paren_level == 0 && tmp->type == type)
            last_token = tmp;
        
        tmp = tmp->next;
    }
    
    return last_token;
}

/* Extract tokens between start and end (exclusive of end) */
t_token_node *extract_tokens(t_token_node *start, t_token_node *end)
{
    if (!start || start == end)
        return NULL;
    
    // Clone tokens from start to end (not including end)
    t_token_node *result = NULL;
    t_token_node *current = NULL;
    t_token_node *tmp = start;
    
    while (tmp && tmp != end)
    {
        t_token_node *new_node = malloc(sizeof(t_token_node));
        if (!new_node)
            return NULL;
        
        new_node->type = tmp->type;
        new_node->data = ft_strdup(tmp->data);
        new_node->next = NULL;
        
        if (!result)
            result = new_node;
        else
            current->next = new_node;
        
        current = new_node;
        tmp = tmp->next;
    }
    
    return result;
}

/* Create a new AST node with the specified type */
t_ast_node *create_ast_node(int type)
{
    t_ast_node *node = malloc(sizeof(t_ast_node));
    if (!node)
        return NULL;
    
    node->type = type;
    node->args = NULL;
    node->arg_count = 0;
    node->arg_quote_types = NULL;  // Initialize new field
    node->redirects = NULL;
    node->left = NULL;
    node->right = NULL;
    node->child = NULL;
    
    return node;
}

/* Parse redirections from token list */
t_redirect *parse_redirections(t_token_node **tokens)
{
    t_redirect *head = NULL;
    t_redirect *current = NULL;
    t_token_node *tmp = *tokens;
    t_token_node *prev = NULL;
    t_token_node *new_head = *tokens;
    
    while (tmp)
    {
        if (is_redirection(tmp->type) && tmp->next)
        {
            // Create new redirection node
            t_redirect *redir = malloc(sizeof(t_redirect));
            if (!redir)
                return head;
            
            redir->type = tmp->type;
            redir->file = strdup(tmp->next->data);
            redir->next = NULL;
            
            // Add to redirection list
            if (!head)
                head = redir;
            else
                current->next = redir;
            
            current = redir;
            
            // Remove these tokens from command tokens
            t_token_node *to_remove = tmp;
            t_token_node *file_to_remove = tmp->next;
            
            // Update links
            if (prev)
                prev->next = file_to_remove->next;
            else
                new_head = file_to_remove->next;
            
            // Move to next token after redirection file
            tmp = file_to_remove->next;
            
            // Free removed tokens (in real code, you'd want to handle this differently)
            // For now, just unlink them
            to_remove->next = NULL;
            file_to_remove->next = NULL;
        }
        else
        {
            prev = tmp;
            tmp = tmp->next;
        }
    }
    
    *tokens = new_head;
    return head;
}

/* Check if token type is a redirection */
int is_redirection(t_token_type type)
{
    return (type == token_in || type == token_out || 
            type == token_hrdc || type == token_appnd);
}

/* Count number of argument tokens */
int count_args(t_token_node *tokens)
{
    int count = 0;
    t_token_node *tmp = tokens;
    
    while (tmp)
    {
        if (tmp->type == token_cmd || tmp->type == token_dquote || 
            tmp->type == token_squote)
            count++;
        tmp = tmp->next;
    }
    
    return count;
}

/* Collect arguments into a string array */
char **collect_args(t_token_node *tokens, int count, int **quote_types)
{
    char **args = malloc(sizeof(char *) * (count + 1));
    if (!args)
        return NULL;
    
    // Allocate memory for quote types
    *quote_types = malloc(sizeof(int) * count);
    if (!(*quote_types))
    {
        free(args);
        return NULL;
    }
    
    int i = 0;
    t_token_node *tmp = tokens;
    
    while (tmp && i < count)
    {
        if (tmp->type == token_cmd || tmp->type == token_dquote || 
            tmp->type == token_squote)
        {
            args[i] = strdup(tmp->data);
            
            // Store quote type information
            if (tmp->type == token_dquote)
                (*quote_types)[i] = AST_DQUOTES;
            else if (tmp->type == token_squote)
                (*quote_types)[i] = AST_SQUOTES;
            else
                (*quote_types)[i] = 0;  // No quotes
            
            i++;
        }
        tmp = tmp->next;
    }
    
    args[i] = NULL;  // NULL-terminate the array
    return args;
}

/* Get the length of a token list */
int token_list_len(t_token_node *tokens)
{
    int len = 0;
    t_token_node *tmp = tokens;
    
    while (tmp)
    {
        len++;
        tmp = tmp->next;
    }
    
    return len;
}

/* Free the AST recursively */
void free_ast(t_ast_node *ast)
{
    if (!ast)
        return;
    
    // Free child nodes
    free_ast(ast->left);
    free_ast(ast->right);
    free_ast(ast->child);
    
    // Free arguments
    if (ast->args)
    {
        for (int i = 0; i < ast->arg_count; i++)
            free(ast->args[i]);
        free(ast->args);
    }
    
    // Free quote types array
    if (ast->arg_quote_types)
        free(ast->arg_quote_types);
    
    // Free redirections
    t_redirect *redir = ast->redirects;
    while (redir)
    {
        t_redirect *next = redir->next;
        free(redir->file);
        free(redir);
        redir = next;
    }
    
    // Free the node itself
    free(ast);
}

/* Print the AST with indentation for visualization */
void print_ast(t_ast_node *ast, int indent_level)
{
    if (!ast)
        return;
    
    // Create indentation string
    char indent[100] = {0};
    for (int i = 0; i < indent_level; i++)
        strcat(indent, "  ");
    
    // Print node type
    printf("%s", indent);
    switch (ast->type)
    {
        case AST_COMMAND:
            printf("COMMAND: ");
            if (ast->args && ast->arg_count > 0)
            {
                for (int i = 0; i < ast->arg_count; i++)
                    printf("%s ", ast->args[i]);
            }
            else
                printf("(no command)");
            printf("\n");
            
            // Print redirections if any
            t_redirect *redir = ast->redirects;
            while (redir)
            {
                printf("%s  REDIR: ", indent);
                switch (redir->type)
                {
                    case token_in:
                        printf("< %s\n", redir->file);
                        break;
                    case token_out:
                        printf("> %s\n", redir->file);
                        break;
                    case token_appnd:
                        printf(">> %s\n", redir->file);
                        break;
                    case token_hrdc:
                        printf("<< %s\n", redir->file);
                        break;
                    default:
                        printf("Unknown redirection type\n");
                }
                redir = redir->next;
            }
            break;
            
        case AST_PIPE:
            printf("PIPE\n");
            break;
            
        case AST_AND_AND:
            printf("AND (&&)\n");
            break;
            
        case AST_OR_OR:
            printf("OR (||)\n");
            break;
            
        case AST_SUBSHELL:
            printf("SUBSHELL\n");
            break;
            
        default:
            printf("UNKNOWN NODE TYPE: %d\n", ast->type);
    }
    
    // Recursively print children
    if (ast->left)
        print_ast(ast->left, indent_level + 1);
    if (ast->right)
        print_ast(ast->right, indent_level + 1);
    if (ast->child)
        print_ast(ast->child, indent_level + 1);
}
