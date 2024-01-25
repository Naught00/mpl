#ifndef PARSER_H
#define PARSER_H
#include <stdbool.h>
#include <stdint.h>
#include "lexer.h"

enum Node_Flags {
	NF_children_added = 0x1,
	NF_adopted        = 0x2,
};

struct Node {
	Token *token;

	//@FIXME non-bin ops
	struct Node **children;

	void *auxiliary;

	enum Type dtype;

	uint8_t childc;
	uint8_t flags;
};

struct variable {
	size_t   size;
	uint32_t offset;
};

struct procedure_call {
	size_t return_size;
	size_t *argsizes;
	uint32_t argc;
};

#define add_arg(p, v) p->args[p->argc++] = v
struct proc_hmv {
	enum Type return_type;
	uint32_t argc;
	enum Type *args;
};

struct vtypes_value {
	struct variable variable;
	enum Type data_type;
};

struct vtypes_hm {
	char *key;
	struct vtypes_value value;
};

extern size_t size_table[];

struct scope {
	struct vtypes_hm *vtypes;
	size_t frame_size;
	uint32_t offset;
};


typedef struct Node Node;

Node **shunting(Token *tokens, int tokenc, uint32_t *l_size);
Node tree(Token *tokens, int tokenc);
void tree_print(Node *root);

Node *tree_make(Token *tokens, int tokenc, int *lines, uint32_t l_size);

#endif
