#include <stdlib.h>
#include <stdio.h>
#include "lexer.h"
#include "parser.h"

Node tree(Token *tokens, int tokenc) {
	Node root;

	int i;
	for (i = 0; i < 5; i++) {
		switch (tokens[i].type) {
		case OPERATOR: 
			root.token = &tokens[i];

			Node *left = malloc(sizeof(Node));
			Node *right = malloc(sizeof(Node));

			left->token = &tokens[i - 1];
			left->left = NULL;
			left->right = NULL;

			right->token = &tokens[i + 1];
			right->left = NULL;
			right->right = NULL;

			root.left = left;
			root.right = right;
		}
	}

	return root;
}

void tree_print(Node root) {
	int i;
	printf("\t%s\n       / \\\n", root.token->x);
	printf("      %s", root.left->token->x);
	printf("  %s\n", root.right->token->x);
}
