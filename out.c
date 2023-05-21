#include "parser.h"
#include "lexer.h"
#include <stdio.h>

//void compile(Node root) {
//	int i;
//	Node *operating = &root;
//
//	printf("\n");
//	for (i = 0; i < 1; i++) {
//		switch (operating->token->x[0]) {
//		case '*':
//			if (operating->right->token->type == OPERATOR) {
//				switch (operating->right->token->x[0]) {
//				case '+': printf("ADD %s, %s\n", operating->right->left->token->x, operating->right->right->token->x);
//				}
//			}
//
//			printf("MULT %s\n", operating->left->token->x);
//		}
//	}
//}
