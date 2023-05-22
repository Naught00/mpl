#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"

void tree_iterate(Node *root, Token *tokens, int tokenc, Node **stack, int stack_index);
void determine_operator(Node *root, Token *tokens, int tokenc, int token_index);

//Node tree(Token *tokens, int tokenc) {
//	Node root = {NULL, NULL};
//	Node *operating;
//
//	operating = &root;
//
//	int i;
//	for (i = 0; i < tokenc; i++) {
//		switch (tokens[i].type) {
//		case OPERATOR: 
//			operating->token = &tokens[i];
//
//			Node *left = malloc(sizeof(Node));
//			Node *right = malloc(sizeof(Node));
//
//			left->token = &tokens[i - 1];
//			left->left = NULL;
//			left->right = NULL;
//
//			if (tokens[i + 1].type == PARENTHESES) {
//				right->token = &tokens[i + 3];
//				right->left = NULL;
//				right->right = NULL;
//			} else {
//				right->token = &tokens[i + 1];
//				right->left = NULL;
//				right->right = NULL;
//			}
//
//			operating->left = left;
//			operating->right = right;
//			operating = right;
//		}
//	}
//
//	return root;
//}

//Node tree_alt(Token *tokens, int tokenc) {
//	Node root = {NULL, NULL};
//	root->children = malloc(10 * sizeof(Node));
//
//	int i, j;
//	for (i = 0, j = 0; i < tokenc; i++) {
//		switch (tokens[i].type) {
//		case INTEGER: 
//			//operating->token = &tokens[i];
//
//			Node *child = malloc(sizeof(Node));
//
//			child->token = &tokens[i + 1];
//			child->children = malloc(2 * sizeof(Node));
//
//			Node *numchild = malloc(sizeof(Node));
//
//			numchild->token = &tokens[i];
//			numchild->children = NULL;
//
//			root->children[j] = child;
//			j++;
//
//		}
//		case OPERATOR:
//	}
//
//	return root;
//}

void determine_operator(Node *root, Token *tokens, int tokenc, int token_index) {
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

	stack[0] = &root;

	for (i = 0; i < tokenc; i++) {
		if (stack[i] == NULL) {
			break;
		}
		tree_iterate(stack[i], tokens, tokenc, stack, i);
	}

	free(stack);

	return root;
}

void tree_iterate(Node *root, Token *tokens, int tokenc, Node **stack, int stack_index) {
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

			stack[stack_index + (j + 1)] = child;
			root->children[j] = child;
			j++;
			break;
		}
		}
	}
}

//void tree_print(Node root) {
//	int i, x;
//
//	Node *operating = &root;
//	for (i = 0; i < 2; i++) {
//		if (i == 0) {
//			printf("\t%s\n       / \\\n", operating->token->x);
//		} else {
//			printf("\t\n         / \\\n", operating->token->x);
//		}
//		printf("      ");
//		for (x = 0; x < i; x++) {
//			printf(" ");
//		}
//		printf("%s", operating->left->token->x);
//		printf("  %s", operating->right->token->x);
//		operating = operating->right;
//	}
//}
