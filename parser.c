#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"

struct npool {
	Node *pool;
	uint32_t p_index;
};

static void tree_iterate(Node *root, Token *tokens, int tokenc, Node **stack, int *stack_index, struct npool *nodes);
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
				root->token    = &tokens[i];
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
		case ASSIGNMENT:
			root->token    = &tokens[i];
			root->children = malloc(2 * sizeof(Node *));
			done = true;
			break;
		case SEMI_COLON:
			return;
		}
	}
}

Node tree_make(Token *tokens, int tokenc) {
	//Node *lines = malloc(tokenc * sizeof(Node));

	//int x;
	//for (x = 0; x < tokenc; x++)
	Node root = {NULL, 0, NULL, false};

	determine_operator(&root, tokens, tokenc, root.starting_token);

	Node **stack = malloc(tokenc * sizeof(Node *));
	int i;
	for (i = 0; i < tokenc; i++)
		stack[i] = NULL;

	int sp = 0;
	stack[sp++] = &root;

	struct npool nodes;
	nodes.pool = malloc(tokenc * sizeof(Node));
	nodes.p_index = 0;

	while (sp) {
		tree_iterate(stack[--sp], tokens, tokenc, stack, &sp, &nodes);
	}

	free(stack);

	return root;
}

static void tree_iterate(Node *root, Token *tokens, int tokenc, Node **stack, int *stack_index, struct npool *nodes) {
	/* XXX@FIXME Allocate nodes in bulk instead
	 * of in the loop body */
	int i, j;
	for (i = root->starting_token, j = 0; j < 2 && i < tokenc; i++) {
		switch (tokens[i].type) {
		case INTEGER: {
			//Node *child = malloc(sizeof(Node));
			Node *child = &nodes->pool[nodes->p_index++];
			printf("TOKEN: %s\n", tokens[i].x);

			child->token    = &tokens[i];
			child->children = NULL;

			root->children[j] = child;
			j++;
			break;
		}
		case OPEN_PARENTHESES: {
			//Node *child = malloc(sizeof(Node));
			Node *child = &nodes->pool[nodes->p_index++];

			int starting_token = i + 1;
			determine_operator(child, tokens, tokenc, starting_token);

			// Walk the token stream until we pass out the 
			// expression.
			int open_count, close_count;
			open_count  = 0;
			close_count = 0;
			do {
				switch (tokens[i].type) {
				case OPEN_PARENTHESES : open_count++ ; break;
				case CLOSE_PARENTHESES: close_count++; break;
				}

				i++;
			} while (open_count != close_count);

			child->starting_token = starting_token;
			//child->children       = malloc(2 * sizeof(Node));
			child->children_added = false;

			stack[(*stack_index)++] = child;
			root->children[j] = child;
			j++;
			break;
		}
		case IDENTIFIER_L: {
			//Node *left = malloc(sizeof(Node));
			Node *left = &nodes->pool[nodes->p_index++];

			left->token          = &tokens[i];
			left->children       = NULL;
			left->children_added = false;

			root->children[j]    = left;
			j++;

			Node *right = &nodes->pool[nodes->p_index++];
			int starting_token = i + 2;
			determine_operator(right, tokens, tokenc, starting_token);
			if (right->children) {
				right->starting_token  = starting_token;
				right->children_added  = false;

				stack[(*stack_index)++] = right;
				root->children[j]       = right;
				j++;
			} else {
				free(right);
			}
			break;
		}
		}
	}
}

void tree_print(Node *root) {
	Node **stack = malloc(100 * sizeof(Node *));

	int i;
	for (i = 0; i < 100; i++)
		stack[i] = NULL;

	printf("%s\n", root->token->x);

	int sp = 0;
	stack[sp++] = root;
	while (sp) {
		tree_print_iterate(stack[--sp], stack, &sp);
	}

	free(stack);
}

static void tree_print_iterate(Node *root, Node **stack, int *stack_index) {
	int i, x;
	for (i = 1, x = 0; root->children != NULL && root->children[i - 1] != NULL; --i, x++) {
		printf("%s ", root->children[x]->token->x);
		stack[(*stack_index)++] = root->children[i];
	}
}
