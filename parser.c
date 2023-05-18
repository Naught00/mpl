#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "lexer.h"
#include "parser.h"

Node tree(Token *tokens, int tokenc) {
	Node root = {NULL, NULL, NULL};
	Node *operating;

	operating = &root;

	int i;
	for (i = 0; i < 5; i++) {
		switch (tokens[i].type) {
		case OPERATOR: 
			operating->token = &tokens[i];

			Node *left = malloc(sizeof(Node));
			Node *right = malloc(sizeof(Node));

			left->token = &tokens[i - 1];
			left->left = NULL;
			left->right = NULL;

			if (tokens[i + 1].type == PARENTHESES) {
				right->token = &tokens[i + 3];
				right->left = NULL;
				right->right = NULL;
			} else {
				right->token = &tokens[i + 1];
				right->left = NULL;
				right->right = NULL;
			}

			operating->left = left;
			operating->right = right;
			operating = right;
		}
	}

	return root;
}

void tree_print(Node root) {
	int i, x;

	Node *operating = &root;
	for (i = 0; i < 2; i++) {
		if (i == 0) {
			printf("\t%s\n       / \\\n", operating->token->x);
		} else {
			printf("\t\n         / \\\n", operating->token->x);
		}
		printf("      ");
		for (x = 0; x < i; x++) {
			printf(" ");
		}
		printf("%s", operating->left->token->x);
		printf("  %s", operating->right->token->x);
		operating = operating->right;
	}
}

bool expression(Token token) {
	switch (token.type) {
	case INTEGER: return true; break;
	case OPERATOR: return true; break;
	}
}
