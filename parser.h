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
	enum Type dtype;

	//@FIXME non-bin ops
	struct Node *children[2];

	uint8_t flags;
	void *auxiliary;
};

/* 
   Value is a 64 bit integer containing a signed 32 bit 
   integer for the data Type enum (see lexer.h) and unsigned 
   32bit integer for the base pointer offset: -$0xX(%rbp)

                       1                   2                   3
   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                       enum Type type                        |  
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                       uint32_t offset                       | 
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

 */
 
struct vtypes_hm {
	char *key;
	uint64_t value;
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
