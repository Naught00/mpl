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
//@TEST
#include "testnewc.c"

struct narrpool {
	Node **pool;
	uint32_t p_index;
};

struct narrpool narrs;

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
				root->children = &narrs.pool[narrs.p_index += 2];
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
			root->children = &narrs.pool[narrs.p_index += 2];
			done = true;
			break;
		case SEMI_COLON:
			return;
		}
	}
}

Node *tree_make(Token *tokens, int tokenc, int *line_starts, uint32_t l_size) {
	shunting(tokens, tokenc, line_starts, l_size);
	exit(10);
	Node *lines = malloc(l_size * sizeof(Node));

	//@FIXME Make this not needed
	narrs.pool    = malloc(tokenc * sizeof(Node *));
	narrs.p_index = 0;

	int i;
	for (i = 0; i < l_size; i++) {
		lines[i] = (Node) {NULL, line_starts[i], NULL, false};
		printf("ls: %d\n", line_starts[i]);
		determine_operator(&lines[i], tokens, tokenc, lines[i].starting_token);
	}

	Node **stack = malloc(tokenc * sizeof(Node *));
	for (i = 0; i < tokenc; i++)
		stack[i] = NULL;

	struct npool nodes;
	nodes.pool = malloc(tokenc * sizeof(Node));
	nodes.p_index = 0;

	int sp;
	for (i = 0; i < l_size; i++) {
		sp = 0;
		stack[sp++] = &lines[i];
		while (sp) {
			tree_iterate(stack[--sp], tokens, tokenc, stack, &sp, &nodes);
		}
	}

	free(stack);

	return lines;
}

static void tree_iterate(Node *root, Token *tokens, int tokenc, Node **stack, int *stack_index, struct npool *nodes) {
	int i, j;
	for (i = root->starting_token, j = 0; j < 2 && i < tokenc; i++) {
		switch (tokens[i].type) {
		case INTEGER: {
			Node *child = &nodes->pool[nodes->p_index++];
			printf("TOKEN: %s\n", tokens[i].x);

			child->token    = &tokens[i];
			child->children = NULL;

			root->children[j] = child;
			j++;
			break;
		}
		case OPEN_PARENTHESES: {
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
			child->children_added = false;

			stack[(*stack_index)++] = child;
			root->children[j] = child;
			j++;
			break;
		}
		case IDENTIFIER_L: {
			Node *left = &nodes->pool[nodes->p_index++];

			left->token          = &tokens[i];
			puts(left->token->x);
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
				//free(right);
			}
			break;
		}
		case IDENTIFIER_R: {
			Node *child = &nodes->pool[nodes->p_index++];
			printf("TOKEN: %s\n", tokens[i].x);

			child->token    = &tokens[i];
			child->children = NULL;

			root->children[j] = child;
			j++;
			break;
		}
		}
	}
	printf("pindex: %d\n", nodes->p_index);
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
