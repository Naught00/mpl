#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "parser.h"
#include "lexer.h"

struct npool {
	Node *pool;
	uint32_t p_index;
};

//@FIXME precompute
uint32_t precedence(Token op) {
	switch (op.x[0]) {
	case '(': return 5;
	case ')': return 5;
	case '*': return 4;
	case '/': return 3;
	case '+': return 2;
	case '-': return 1;
	case '=': return 0;
	}
}

Node **shunting(Token *tokens, int tokenc, int *line_starts, uint32_t l_size) {
	uint32_t o_index, d_index;
	size_t tarrsz;

	Token opstack[256];
	o_index = 0;

	tarrsz = tokenc * sizeof(Token);
	Token *dstack = malloc(tarrsz);
	memset(dstack, 0, tarrsz);
	d_index = 0;

	uint32_t i, j, len;
	len = 0;
	Token next, topop;
	for (i = 0; i < tokenc; i++) {
		switch (tokens[i].type) {
		case OPERATOR: case ASSIGNMENT:
			next  = tokens[i];
			if (o_index == 0) {
				opstack[o_index++] = next;
				break;
			}
			
			topop = opstack[o_index - 1];
			if (precedence(next) < precedence(topop)) {
				dstack[d_index++] = topop;
				--o_index;
			}
			opstack[o_index++] = next;
			break;
		case INTEGER: case IDENTIFIER_L: case IDENTIFIER_R:
			dstack[d_index++] = tokens[i];
			break;
		case SEMI_COLON:
			len    = o_index;
			for (j = 0; j < len; j++)
				dstack[d_index++] = opstack[--o_index];

			dstack[d_index++] = tokens[i];
			memset(opstack, 0, len);
			break;
		}
	}


	for (i = 0; i < tokenc; i++) {
		printf("%s", dstack[i].x);
	}
	putchar('\n');
	struct npool nodes;
	nodes.pool = malloc(tokenc * sizeof(Node));
	nodes.p_index = 0;

	Node **lines = malloc(l_size * sizeof(Node *));

	uint32_t line_index;
	for (i = 0, line_index = 0; i < tokenc; i++) {
		switch (dstack[i].type) {
		case OPERATOR: case ASSIGNMENT:
			Node *parent     = &nodes.pool[nodes.p_index];
			parent->flags    = 0x0;
			parent->token    = &dstack[i];

			parent->children = &nodes.pool[nodes.p_index - 2]; 
			parent->childc   = 2;
			for (j = 0; j < 2; j++) {
				parent->children[j].flags |= N_parented;
			}
			nodes.p_index++;
			break;
		case INTEGER: case IDENTIFIER_L: case IDENTIFIER_R:
			Node *child  = &nodes.pool[nodes.p_index++];
			child->token = &dstack[i];
			child->flags = 0x0;
			break;
		case SEMI_COLON:
			lines[line_index++] = &nodes.pool[nodes.p_index - 1];
			break;
		}
	}

	for (i = 0; i < tokenc - 3; i++) {
		printf("%s", nodes.pool[i].token->x);
	}
	putchar('\n');
	for (i = 0; i < line_index; i++) {
		printf("%s\n", lines[i]->token->x);
		printf("%s\n", lines[i]->children[0].token->x);

		printf("%s\n", lines[i]->children[1].token->x);
	}
	return lines;
}
