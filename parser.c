#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"

static void tree_iterate(Node *root, Token *tokens, int tokenc, Node **stack, int *stack_index);
static void determine_operator(Node *root, Token *tokens, int tokenc, int token_index);
static void tree_print_iterate(Node *root, Node **stack, int *stack_index);

static void determine_operator(Node *root, Token *tokens, int tokenc, int token_index) {
	int i;
	bool done = false;
	int open_count, close_count;
	for (i = token_index, open_count = 0, close_count = 0; i < tokenc && !done; i++) {
		switch (tokens[i].type) {
		case OPERATOR: 
			if (open_count == close_count) {
				root->token = &tokens[i];
				root->children = malloc(2 * sizeof(Node *));
				done = true;
			}
			break;
		case OPEN_PARENTHESES: 
			open_count++;
			break;
		case CLOSE_PARENTHESES: 
			close_count++;
			break;
		}
	}
}

Node tree_make(Token *tokens, int tokenc) {
	Node root = {NULL, 0, NULL};

	determine_operator(&root, tokens, tokenc, root.starting_token);

	Node **stack = malloc(tokenc * sizeof(Node *));

	int i;
	for (i = 0; i < tokenc; i++)
		stack[i] = NULL;

	int sp = 0;
	stack[sp++] = &root;
	for (i = 0; sp != 0; i++) {
		tree_iterate(stack[--sp], tokens, tokenc, stack, &sp);
	}

	free(stack);

	return root;
}

static void tree_iterate(Node *root, Token *tokens, int tokenc, Node **stack, int *stack_index) {
	int i, j;
	for (i = root->starting_token, j = 0; j < 2 && i < tokenc; i++) {
		switch (tokens[i].type) {
		case INTEGER: {
			Node *child = malloc(sizeof(Node));
			printf("TOKEN: %s\n", tokens[i].x);

			child->token = &tokens[i];
			child->children = NULL;

			root->children[j] = child;
			j++;
			break;
		}
		case OPEN_PARENTHESES: {
			Node *child = malloc(sizeof(Node));

			int starting_token = i + 1;
			determine_operator(child, tokens, tokenc, starting_token);

			// Walk the token stream until we pass out the 
			// expression.
			int open_count, close_count;
			open_count = 0;
			close_count = 0;
			do {
				switch (tokens[i].type) {
				case OPEN_PARENTHESES: open_count++; break;
				case CLOSE_PARENTHESES: close_count++; break;
				}

				i++;
			} while (open_count != close_count);

			child->starting_token = starting_token;
			child->children = malloc(2 * sizeof(Node));

			stack[(*stack_index)++] = child;
			root->children[j] = child;
			j++;
			break;
		}
		}
	}
}

void tree_print(Node *root) {
	Node **stack = malloc(100);

	int i;
	for (i = 0; i < 100; i++)
		stack[i] = NULL;

	printf("%s\n", root->token->x);

	int sp = 0;
	stack[sp++] = root;
	for (i = 0; sp != 0; i++) {
		tree_print_iterate(stack[--sp], stack, &sp);
	}

	free(stack);
}

static void tree_print_iterate(Node *root, Node **stack, int *stack_index) {
	int i;
	for (i = 0; root->children != NULL && root->children[i] != NULL; i++) {
		printf("%s\n", root->children[i]->token->x);
		stack[(*stack_index)++] = root->children[i];
	}
}
