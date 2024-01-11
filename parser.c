#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "parser.h"
#include "lexer.h"
#include "gen.h"

#include "stb_ds.h"


//@Benchmark: try using table instead
static struct {char *key; enum Type value;} *vtypes = NULL;

static struct Token _SEMI_COLON = {SEMI_COLON, ";"};

#define precedence(op) op.type
Node **shunting(Token *tokens, int tokenc, uint32_t *l_size) {
	size_t tarrsz;

	struct {
		Token stack[256];
		uint32_t sp;
	} op;
	op.sp = 0;

	struct {
		Token *stack;
		uint32_t sp;
	} data;

	tarrsz = tokenc * sizeof(Token);
	data.stack = malloc(tarrsz * 2);
	memset(data.stack, 0, tarrsz * 2);
	data.sp = 0;

	int i, j, extra_tokens, extra_lines;
	size_t len;
	len = 0;
	Token next, topop;
	extra_tokens = 0;
	extra_lines  = 0;
	for (i = 0; i < tokenc; i++) {
		switch (tokens[i].type) {
		case ASSIGNMENT: 
		case CLOSE_PARENTHESES: case OPEN_PARENTHESES:
		case ADD:      case MINUS:
		case MULTIPLY: case DIVIDE:
			next  = tokens[i];
			if (op.sp == 0) {
				push(op, next);
				break;
			}
			
			topop = op.stack[op.sp - 1];
			if (precedence(next) < precedence(topop)) {
				push(data, topop);
				--op.sp;
			}
			push(op, next);
			break;
		case INTEGER: case IDENTIFIER_L: case IDENTIFIER_R:
			push(data, tokens[i]);
			break;
		case SEMI_COLON:
			len    = op.sp;
			for (j = 0; j < len; j++)
				push(data, op.stack[--op.sp]);

			push(data, tokens[i]);
			memset(op.stack, 0, len);
			break;
		case INT:
			shput(vtypes, tokens[i + 1].x, tokens[i].type);

			push(data, tokens[i + 1]);
			push(data, tokens[i]);
			push(data, _SEMI_COLON);
			extra_tokens += 2;

			if (tokens[i + 2].type != ASSIGNMENT) {
				i += 2;
			} else {
				*l_size      += 1;
				extra_tokens += 1;
			}
			break;
		}
	}
	tokenc  += extra_tokens;

	for (i = 0; i < tokenc; i++) {
		if (data.stack[i].x == NULL) {
			tokenc = i;
			break;
		}
		printf("%s", data.stack[i].x);
	}

	struct npool {
		Node *pool;
		uint32_t p_index;
	} nodes;

	nodes.pool = malloc(tokenc * sizeof(Node));
	memset(nodes.pool, 0, tokenc * sizeof(Node));
	nodes.p_index = 0;

	Node **lines = malloc(*l_size * sizeof(Node *));

	uint32_t line_index;
	int k;
	for (i = 0, line_index = 0; i < tokenc; i++) {
		switch (data.stack[i].type) {
		case ASSIGNMENT:
		case ADD:      case MINUS:
		case MULTIPLY: case DIVIDE: {
			Node *parent  = pool_stay(nodes);
			parent->flags = 0x0;
			parent->token = &data.stack[i];

			for (j = 1, k = nodes.p_index - 1; j >= 0; k--)  {
				if (!(nodes.pool[k].flags & NF_adopted)) {
					parent->children[j] = &nodes.pool[k];
					nodes.pool[k].flags |= NF_adopted;
					j--;
				}
			}
			nodes.p_index++;
			break;
		}
		case INTEGER: case IDENTIFIER_R: {
			Node *child  = pool(nodes);
			child->token = &data.stack[i];
			child->flags = 0x0;
			break;
		}
		case IDENTIFIER_L: {
			Node *child  = pool(nodes);
			child->token = &data.stack[i];
			child->flags = 0x0;
			child->dtype = shget(vtypes, data.stack[i].x);
			break;
		}
		case INT: {
			Node *parent  = pool_stay(nodes); 
			parent->token = &data.stack[i];
			parent->flags = 0x0;

			parent->children[0] = pool_offset(nodes, -1);
			parent->children[0]->flags = NF_adopted;
			nodes.p_index++;
			break;
		}
		case SEMI_COLON:
			lines[line_index++] = pool_offset(nodes, -1);
			break;
		}
	}

//	puts("Tokens:");
//	for (i = 0; i < nodes.p_index; i++) {
//		printf("%s", nodes.pool[i].token->x);
//	}
//	puts("\nLines:");
//	for (i = 0; i < line_index; i++) {
//		printf("%s\n", lines[i]->token->x);
//		printf("%s\n", lines[i]->children[0]->token->x);
//                                                     
//		printf("%s\n", lines[i]->children[1]->token->x);
//		putchar('\n');
//	}

	return lines;
}
