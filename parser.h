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
	enum Type dtype;

	//@FIXME non-bin ops
	struct Node *children[2];
	//uint32_t childc;

	uint8_t flags;

	void *auxiliary;
};

struct vtypes_hm {
	char *key;
	enum Type value;
};

extern size_t size_table[];

// @Fix Doesn't need to be in scope struct
// we should dump hash maps before entering 
// code gen
struct scope {
	struct vtypes_hm *vtypes;
	size_t frame_size;
};


typedef struct Node Node;

Node **shunting(Token *tokens, int tokenc, uint32_t *l_size);
Node tree(Token *tokens, int tokenc);
void tree_print(Node *root);

Node *tree_make(Token *tokens, int tokenc, int *lines, uint32_t l_size);

#endif
