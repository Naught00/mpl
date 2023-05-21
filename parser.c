#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"

void tree_iterate(Node *root, Token *tokens, int tokenc, int *token_index, Node **stack, int stack_index);
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
	for (i = token_index; i < tokenc && !done; i++) {
		switch (tokens[i].type) {
		case OPERATOR: 
			root->token = &tokens[i];
			root->children = malloc(2 * sizeof(Node *));
			done = true;
		}
	}
}

Node tree_make(Token *tokens, int tokenc) {
	Node root = {NULL, NULL};

	int token_index = 0;
	determine_operator(&root, tokens, tokenc, token_index);

	Node **stack = malloc(tokenc * sizeof(Node *));

	int i;
	for (i = 0; i < tokenc; i++)
		stack[i] = NULL;

	stack[0] = &root;

	for (i = 0; i < tokenc; i++) {
		if (stack[i] == NULL) {
			break;
		}
		tree_iterate(stack[i], tokens, tokenc, &token_index, stack, i);
	}
	return root;
}

void tree_iterate(Node *root, Token *tokens, int tokenc, int *token_index, Node **stack, int stack_index) {
	int i, j;
	for (i = *token_index, j = 0; j < 2 && *token_index < tokenc; i++) {
		switch (tokens[i].type) {
		case INTEGER: {
			Node *child = malloc(sizeof(Node));

			child->token = &tokens[i];
			child->children = NULL;

			root->children[j] = child;
			j++;
			break;
		}
		case PARENTHESES: {
			Node *child = malloc(sizeof(Node));

			determine_operator(child, tokens, tokenc, i);

			child->children = malloc(2 * sizeof(Node));

			stack[stack_index + 1] = child;
			root->children[j] = child;
			j++;
			break;
		}
		}
	}

	*token_index = i;
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
