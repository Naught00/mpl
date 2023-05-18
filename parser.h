#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"

struct Node {
	Token *token;
	struct Node *left;
	struct Node *right;
};

typedef struct Node Node;

Node tree(Token *tokens, int tokenc);
void tree_print(Node root);

#endif
