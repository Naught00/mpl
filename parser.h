#ifndef PARSER_H
#define PARSER_H
#include <stdbool.h>
#include <stdint.h>
#include "lexer.h"

enum Node_Flags {
	NF_children_added = 0x1,
	NF_adopted       = 0x2,
};

struct Node {
	Token *token;
	int starting_token;

	//@FIXME non-bin ops
	struct Node *children[2];
	//uint32_t childc;

	uint8_t flags;
};

typedef struct Node Node;

Node **shunting(Token *tokens, int tokenc, uint32_t l_size);
Node tree(Token *tokens, int tokenc);
void tree_print(Node *root);

Node *tree_make(Token *tokens, int tokenc, int *lines, uint32_t l_size);

#endif
