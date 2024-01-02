#ifndef PARSER_H
#define PARSER_H
#include <stdbool.h>
#include <stdint.h>
#include "lexer.h"

struct Node {
	Token *token;
	int starting_token;
	struct Node *children;
	uint32_t childc;

	bool children_added;
};

typedef struct Node Node;

Node tree(Token *tokens, int tokenc);
void tree_print(Node *root);

Node *tree_make(Token *tokens, int tokenc, int *lines, uint32_t l_size);

#endif
