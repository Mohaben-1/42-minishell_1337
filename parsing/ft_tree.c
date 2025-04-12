/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_tree.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohaben- <mohaben-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 20:58:14 by ahouass           #+#    #+#             */
/*   Updated: 2025/04/12 18:24:11 by mohaben-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_ast_node *build_ast(t_token_node *tokens, t_exec *exec)
{
	if (!tokens)
		return (NULL);
	return (parse_logical_ops(tokens, exec));
}

/* Parse logical operators (&&, ||) */
t_ast_node *parse_logical_ops(t_token_node *tokens, t_exec *exec)
{
	t_token_node *op = find_op_at_level(tokens, token_and_and, token_or);
	
	if (op)
	{
		t_ast_node *node = create_ast_node(op->type == token_and_and ? AST_AND_AND : AST_OR_OR);
		
		// Get tokens before and after the operator
		t_token_node *left_tokens = extract_tokens(tokens, op);
		t_token_node *right_tokens = op->next;
		
		// Recursively parse both sides
		node->left = parse_logical_ops(left_tokens, exec);
		node->right = parse_logical_ops(right_tokens, exec);
		
		return node;
	}
	
	// If no logical operators, look for pipes
	return parse_pipes(tokens, exec);
}

/* Parse pipe operators */
t_ast_node *parse_pipes(t_token_node *tokens, t_exec *exec)
{
	t_token_node *pipe = find_token_at_level(tokens, token_pipe);
	
	if (pipe)
	{
		t_ast_node *node = create_ast_node(AST_PIPE);
		
		// Get tokens before and after the pipe
		t_token_node *left_tokens = extract_tokens(tokens, pipe);
		t_token_node *right_tokens = pipe->next;
		
		// Recursively parse both sides
		node->left = parse_pipes(left_tokens, exec);
		node->right = parse_pipes(right_tokens, exec);
		
		return node;
	}
	
	// Check for subshell (parentheses)
	if (tokens->type == token_paren_open)
		return parse_subshell(tokens, exec);
		
	// Parse as simple command if no operators found
	return parse_command(tokens, exec);
}

/* Parse a subshell (commands in parentheses) */
t_ast_node *parse_subshell(t_token_node *tokens, t_exec *exec)
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
	node->child = parse_logical_ops(inner_tokens, exec);
	
	// If there are tokens after closing parenthesis, handle them
	if (closing->next)
	{
		// Check for redirections first
		if (is_redirection(closing->next->type) && closing->next->next)
		{
			// Create a new command node to hold the redirection
			// t_ast_node *redir_node = create_ast_node(AST_COMMAND);
			
			// Temporarily modify tokens to parse redirections
			t_token_node *redir_tokens = closing->next;
			node->redirects = parse_redirections(&redir_tokens, exec);
			
			// Connect the subshell to the redirect command
			// redir_node->child = node;
			// node = redir_node;
			
			// Update closing->next to be after the redirection
			tmp = closing->next->next;
		}
		else
		{
			tmp = closing->next;
		}
		
		// Check for further operators after potential redirections
		if (tmp)
		{
			if (tmp->type == token_pipe)
			{
				t_ast_node *pipe_node = create_ast_node(AST_PIPE);
				pipe_node->left = node;
				pipe_node->right = parse_pipes(tmp->next, exec);
				return pipe_node;
			}
			else if (tmp->type == token_and_and || tmp->type == token_or)
			{
				t_ast_node *log_node = create_ast_node(tmp->type == token_and_and ? AST_AND_AND : AST_OR_OR);
				log_node->left = node;
				log_node->right = parse_logical_ops(tmp->next, exec);
				return log_node;
			}
		}
	}
	
	return node;
}

/* Parse a simple command with its arguments and redirections */
t_ast_node *parse_command(t_token_node *tokens, t_exec *exec)
{
	if (!tokens)
		return NULL;
	
	t_ast_node *node = create_ast_node(AST_COMMAND);
	t_token_node *cmd_tokens = tokens;
	
	// Parse redirections first
	node->redirects = parse_redirections(&cmd_tokens, exec);
	
	// Count arguments (skip redirection tokens)
	int arg_count = count_args(cmd_tokens);
	
	if (arg_count > 0)
	{
		// Collect command and arguments with quote information
		node->args = collect_args(cmd_tokens, arg_count, &node->arg_quote_types, &node->arg_is_spaced, exec);
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
		new_node->spaced = tmp->spaced;
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
	
	node->e_type = type;
	node->args = NULL;
	node->arg_count = 0;
	node->arg_quote_types = NULL;
	node->arg_is_spaced = NULL;
	node->redirects = NULL;
	node->left = NULL;
	node->right = NULL;
	node->child = NULL;
	
	return node;
}

/* Parse redirections from token list */
t_redirect *parse_redirections(t_token_node **tokens, t_exec *exec)
{
    t_redirect *head = NULL;
    t_redirect *current = NULL;
    t_token_node *tmp = *tokens;
    t_token_node *prev = NULL;
    t_token_node *new_head = *tokens;
    (void)exec;
    
    while (tmp)
    {
        if (is_redirection(tmp->type) && tmp->next)
        {
            // Create a new redirection node
            t_redirect *redir = malloc(sizeof(t_redirect));
            if (!redir)
                return head;
            
            redir->type = tmp->type;
            if (redir->type == token_hrdc)
                redir->heredoc_fd = -1;

            // Get initial file name token
            t_token_node *file_token = tmp->next;
            
            // Set quoted status
            if (file_token->type == token_dquote)
                redir->quoted = token_dquote;
            else if (file_token->type == token_squote)
                redir->quoted = token_squote;
            else
                redir->quoted = 0;
                
            // Initialize with the first file token's data
            redir->file = strdup(file_token->data);
            
            // For normal redirections like "> t1 > t2", each file should have is_spaced=1
            // because there's a space between redirection operator and file
            redir->is_spaced = (file_token->spaced) ? 1 : 0;
            
            redir->next = NULL;
            
            // Add to redirection list
            if (!head)
                head = redir;
            else
                current->next = redir;
            
            current = redir;
            
            // Track the tokens we've processed
            t_token_node *last_joined = file_token;
            t_token_node *next_token = file_token->next;
            
            // Process additional non-spaced tokens
            while (next_token && 
                   !next_token->spaced && 
                   (next_token->type == token_cmd || 
                    next_token->type == token_dquote || 
                    next_token->type == token_squote))
            {
                // Create a new redirection node for this token
                t_redirect *additional = malloc(sizeof(t_redirect));
                if (!additional)
                    return head;
                
                additional->type = tmp->type;  // Same redirection type
                additional->heredoc_fd = -1;   // Initialize for consistency
                
                // Set quoted status for this token
                if (next_token->type == token_dquote)
                    additional->quoted = token_dquote;
                else if (next_token->type == token_squote)
                    additional->quoted = token_squote;
                else
                    additional->quoted = 0;
                
                additional->file = strdup(next_token->data);
                additional->is_spaced = 0;  // No space between joined parts
                additional->next = NULL;
                
                // Add to our redirection list
                current->next = additional;
                current = additional;
                
                // Remember this token was processed
                last_joined = next_token;
                next_token = next_token->next;
            }
            
            // Remove these tokens from command tokens
            t_token_node *to_remove = tmp;
            
            // Update links - skip all joined tokens
            if (prev)
                prev->next = last_joined->next;
            else
                new_head = last_joined->next;
            
            // Move to next token after all joined tokens
            tmp = last_joined->next;
            
            // Unlink removed tokens
            to_remove->next = NULL;
            // No need to free tokens here as they are part of the original list
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

char **collect_args(t_token_node *tokens, int count, int **quote_types, int **is_spaced, t_exec *exec)
{
	// Allocate memory for arguments array
	char **args = malloc(sizeof(char *) * (count + 1));
	if (!args)
		return NULL;
	
	*quote_types = malloc(sizeof(int) * count);
	if (!(*quote_types))
	{
		free(args);
		return NULL;
	}
	
	*is_spaced = malloc(sizeof(int) * count);
	if (!(*is_spaced))
	{
		free(*quote_types);
		free(args);
		return NULL;
	}
	
	// Initialize variables
	int i = 0;
	t_token_node *tmp = tokens;

	while (tmp && i < count)
	{
		if (tmp->type == token_cmd || tmp->type == token_dquote || tmp->type == token_squote)
		{
			char *expanded_token;

			(void)exec;
				expanded_token = ft_strdup(tmp->data);
			
			// Always create a new argument for each token
			args[i] = expanded_token;
			
			// Set the quote type
			if (tmp->type == token_dquote)
				(*quote_types)[i] = AST_DQUOTES;
			else if (tmp->type == token_squote)
				(*quote_types)[i] = AST_SQUOTES;
			else
				(*quote_types)[i] = 0;
			
			// Set the spaced flag
			(*is_spaced)[i] = tmp->spaced;
			
			i++;
		}
		
		tmp = tmp->next;
	}
	
	// NULL-terminate the array
	args[i] = NULL;
	
	return args;
}

/* Collect arguments into a string array */
// char **collect_args(t_token_node *tokens, int count, int **quote_types, t_exec *exec)
// {
// 	// Allocate memory for arguments array
// 	char **args = malloc(sizeof(char *) * (count + 1));
// 	if (!args)
// 		return NULL;
	
// 	*quote_types = malloc(sizeof(int) * count);
// 	if (!(*quote_types))
// 	{
// 		free(args);
// 		return NULL;
// 	}
	
// 	// Initialize variables
// 	int i = 0;
// 	t_token_node *tmp = tokens;
// 	char *current_arg = NULL;
// 	int current_quote_type = 0;
// 	int prev_was_arg = 0;

// 	while (tmp && i < count)
// 	{
// 		if (tmp->type == token_cmd || tmp->type == token_dquote || tmp->type == token_squote)
// 		{
// 			char *expanded_token;

// 			(void)exec;
// 			// if (ft_strchr(tmp->data, '$') && tmp->type != token_squote)
// 			// 	expanded_token = ft_expand(tmp->data, exec);
// 			// else
// 				expanded_token = ft_strdup(tmp->data);
			
// 			// Check if we should start a new argument or merge with previous
// 			if (!prev_was_arg || tmp->spaced)
// 			{
// 				// If we have a previous argument in progress, store it
// 				if (current_arg)
// 				{
// 					args[i] = current_arg;
// 					(*quote_types)[i] = current_quote_type;
// 					i++;
// 				}
				
// 				// Start a new argument
// 				current_arg = expanded_token;
				
// 				// Store the quote type of the first part
// 				if (tmp->type == token_dquote)
// 					current_quote_type = AST_DQUOTES;
// 				else if (tmp->type == token_squote)
// 					current_quote_type = AST_SQUOTES;
// 				else
// 					current_quote_type = 0;
// 			}
// 			else
// 			{
// 				char *merged = ft_strjoin(current_arg, expanded_token);
// 				free(current_arg);
// 				free(expanded_token);
// 				current_arg = merged;
// 			}
			
// 			prev_was_arg = 1;
// 		}
// 		else
// 		{
// 			// Non-argument token encountered, store current argument if any
// 			if (current_arg)
// 			{
// 				args[i] = current_arg;
// 				(*quote_types)[i] = current_quote_type;
// 				i++;
// 				current_arg = NULL;
// 			}
			
// 			prev_was_arg = 0;
// 		}
		
// 		tmp = tmp->next;
// 	}
// 	// Store the last argument if any
// 	if (current_arg)
// 	{
// 		args[i] = current_arg;
// 		(*quote_types)[i] = current_quote_type;
// 		i++;
// 	}
	
// 	// NULL-terminate the array
// 	args[i] = NULL;
	
// 	return args;
// }

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

// /* Print the AST with indentation for visualization */
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
	switch (ast->e_type)
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
			printf("UNKNOWN NODE TYPE: %d\n", ast->e_type);
	}
	
	// Recursively print children
	if (ast->left)
		print_ast(ast->left, indent_level + 1);
	if (ast->right)
		print_ast(ast->right, indent_level + 1);
	if (ast->child)
		print_ast(ast->child, indent_level + 1);
}










// void print_ast(t_ast_node *node, int depth)
// {
//     if (!node)
//         return;
	
//     // Create indentation string
//     char indent[256] = {0};
//     for (int i = 0; i < depth; i++)
//         strcat(indent, "  ");
	
//     // Print node type
//     switch (node->type)
//     {
//         case AST_AND_AND:
//             printf("%s&&\n", indent);
//             break;
//         case AST_OR_OR:
//             printf("%s||\n", indent);
//             break;
//         case AST_PIPE:
//             printf("%s|\n", indent);
//             break;
//         case AST_SUBSHELL:
//             printf("%s(Subshell)\n", indent);
//             break;
//         case AST_COMMAND:
//             printf("%sCommand:\n", indent);
//             break;
//         default:
//             printf("%sUnknown Node Type\n", indent);
//     }
	
//     // Print arguments for command nodes
//     if (node->type == AST_COMMAND)
//     {
//         for (int i = 0; i < node->arg_count; i++)
//         {
//             char quote_prefix[4] = "";
//             if (node->arg_quote_types && node->arg_quote_types[i] == AST_DQUOTES)
//                 strcpy(quote_prefix, "\"");
//             else if (node->arg_quote_types && node->arg_quote_types[i] == AST_SQUOTES)
//                 strcpy(quote_prefix, "'");
			
//             printf("%s  Arg %d: %s%s%s\n", indent, i, quote_prefix, 
//                    node->args[i] ? node->args[i] : "(null)", quote_prefix);
//         }
		
//         // Print redirections
//         t_redirect *redir = node->redirects;
//         while (redir)
//         {
//             const char *redir_type = "Unknown";
//             switch (redir->type)
//             {
//                 case token_in: redir_type = "<"; break;
//                 case token_out: redir_type = ">"; break;
//                 case token_hrdc: redir_type = "<<"; break;
//                 case token_appnd: redir_type = ">>"; break;
//             }
//             printf("%s  Redirect: %s %s%s%s\n", indent, redir_type, 
//                    redir->quoted ? "\"" : "", 
//                    redir->file ? redir->file : "(null)", 
//                    redir->quoted ? "\"" : "");
//             redir = redir->next;
//         }
//     }
	
//     // Recursively print child/left/right nodes based on node type
//     if (node->type == AST_SUBSHELL)
//     {
//         printf("%s  Subshell Content:\n", indent);
//         print_ast(node->child, depth + 2);
//     }
//     else if (node->type == AST_COMMAND)
//     {
//         // No further recursion for command nodes
//         return;
//     }
//     else 
//     {
//         // For logical operators, pipes, etc.
//         if (node->left)
//         {
//             printf("%s  Left:\n", indent);
//             print_ast(node->left, depth + 2);
//         }
		
//         if (node->right)
//         {
//             printf("%s  Right:\n", indent);
//             print_ast(node->right, depth + 2);
//         }
//     }
// }

// /* Wrapper function to print AST with initial depth of 0 */
// void print_abstract_syntax_tree(t_ast_node *root)
// {
//     if (!root)
//     {
//         printf("Empty Abstract Syntax Tree\n");
//         return;
//     }
	
//     printf("Abstract Syntax Tree:\n");
//     print_ast(root, 0);
// }
