#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "parser.h"
#include "lexer.h"
#include "gen.h"

#include "stb_ds.h"

/* @Fix experiment with this higher
 *  10 is quite low */
#define MAX_FRAME_DEPTH 10

size_t size_table[] = {[INT] 4};

static const struct Token _LOAD_ARG = {LOAD_ARG, ","};

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

	int i, j;
	uint32_t proc_token = 0;
	size_t len;
	len = 0;
	Token next, topop;
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
		case OPEN_BRACE: case CLOSE_BRACE:
			push(data, tokens[i]);
			break;
		case PROC_DECLARATION:
			j = i;
			while (tokens[j].type != CLOSE_PARENTHESES) {
				j++;
			}

			if (tokens[j + 1].type == OPEN_BRACE) {
				data.stack[data.sp - 2].type = PROC_DEFINITION;
				push(data, tokens[j + 1]);
			}

			i += 2;
			while (tokens[i].type != CLOSE_PARENTHESES) {
				if (tokens[i].type == COMMA) {
					i++;
					continue;
				}
				tokens[i + 1].type = DECLARATION_CHILD;
				push(data, tokens[i + 1]);
				push(data, tokens[i]);
				push(data, _LOAD_ARG);
				i += 2;
			}
			i++;
			break;
		case INT:
			push(data, tokens[i + 1]);
			push(data, tokens[i]);

			if (tokens[i + 1].type == PROC_DECLARATION) {
				break;
			}

			data.stack[data.sp - 2].type = DECLARATION_CHILD;

			switch (tokens[i + 2].type) {
			case SEMI_COLON:
				i += 2;
				break;
			case ASSIGNMENT:
				break;
			default:
				break;
			}
			break;
		case SEMI_COLON:
			len    = op.sp;
			for (j = 0; j < len; j++)
				push(data, op.stack[--op.sp]);

			push(data, tokens[i]);
			op.sp = 0;
			break;
		}
	}

	#ifdef DEBUG
	printf("tokenc: %d\n", tokenc);
	for (i = 0; i < data.sp; i++) {
		if (tokens[i].x == NULL) {
			break;
		}
		printf("%s", tokens[i].x);
	}
	printf("DSP::::...%d\n", data.sp);
	putchar('\n');

	for (i = 0; i < data.sp; i++) {
		if (data.stack[i].x == NULL) {
			break;
		}
		printf("%s", data.stack[i].x);
	}
	printf("i:%d\n", i);
	#endif

	struct npool {
		Node *pool;
		uint32_t p_index;
	} nodes;
	nodes.pool = malloc(data.sp * sizeof(Node));
	memset(nodes.pool, 0, data.sp * sizeof(Node));
	nodes.p_index = 0;

	struct spool {
		struct scope *pool;
		uint32_t p_index;
	} scopes;
	scopes.pool    = malloc(data.sp * sizeof(struct scope));
	scopes.p_index = 0;

	struct rbp_offset_pool {
		uint32_t *pool;
		uint32_t p_index;
	} offsets;
	//@Fix lower memory footprint: get size from lexer
	offsets.pool    = malloc(data.sp * sizeof(uint32_t));
	offsets.p_index = 0;

	struct variable_pool {
		struct variable *pool;
		uint32_t p_index;
	} variables;
	variables.pool    = malloc(data.sp * sizeof(uint32_t));
	variables.p_index = 0;

	Node **lines = malloc(*l_size * sizeof(Node *) * 3);

	struct scope *curr_scope;
	struct scope *prev_scope;
	curr_scope  = pool(scopes);
	curr_scope->vtypes     = NULL;
	curr_scope->frame_size = 0;

	struct {
		char *key; 
		enum Type value;
	} *procedures = NULL;

	struct vtypes_value value; /* for types hashmap (see parser.h) */
	uint32_t curr_offset = 0x0;
	uint32_t line_index;
	int k, prev;
	for (i = 0, line_index = 0; i < data.sp; i++) {
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
		case INTEGER: {
			Node *child  = pool(nodes);
			child->token = &data.stack[i];
			child->flags = 0x0;
			break;
		}
		case DECLARATION_CHILD:
			break;
		/* lvalues */
		case IDENTIFIER_L: case IDENTIFIER_R: {
			Node *child  = pool(nodes);
			child->token = &data.stack[i];
			child->flags = 0x0;

			value = shget(curr_scope->vtypes, data.stack[i].x);
			child->dtype     = value.data_type; 
			child->auxiliary = pool(variables);

			*(struct variable *) child->auxiliary = value.variable;
			break;
		}
		case PROC_DECLARATION: case PROC_DEFINITION: {
			Node *parent  = pool(nodes);
			parent->token = &data.stack[i];
			parent->flags = 0x0;

			lines[line_index++] = parent;
			break;
		}
		case LOAD_ARG: {
			Node *parent  = pool(nodes);
			parent->token = &data.stack[i];
			parent->flags = 0x0;

			value = shget(curr_scope->vtypes, data.stack[i - 2].x);

			parent->auxiliary = pool(variables);
			*(struct variable *) parent->auxiliary = value.variable; 

			lines[line_index++] = parent;
			break;
		}
		case INT: {
			prev = i - 1;
			value.data_type = INT; 
			value.variable.size = size_table[INT]; 

			if (stack_index(data, prev).type == DECLARATION_CHILD) {
				curr_offset += value.variable.size;
				value.variable.offset = curr_offset;

				shput(curr_scope->vtypes, stack_index(data, prev).x, value);
				curr_scope->frame_size += value.variable.size;
				break;
			}

			shput(procedures, stack_index(data, prev).x, INT);
			break;
		}
		case OPEN_BRACE: {
			Node *parent  = pool(nodes); 
			parent->token = &data.stack[i];
			parent->flags = 0x0;

			prev_scope = curr_scope;
			curr_scope = pool(scopes);
			curr_scope->vtypes     = NULL;
			curr_scope->frame_size = 0;

			parent->auxiliary = (void *) &curr_scope->frame_size;

			for (j = 0; j < shlen(prev_scope->vtypes); j++) {
				shput(curr_scope->vtypes,
				          prev_scope->vtypes[j].key,
				          prev_scope->vtypes[j].value);
			}
			lines[line_index++] = parent;
			break;
		}
		case CLOSE_BRACE: {
			Node *parent  = pool(nodes); 
			parent->token = &data.stack[i];
			parent->flags = 0x0;

			parent->auxiliary = (void *) &curr_scope->frame_size;

			pool_freetop(scopes);
			curr_scope = pool_offset(scopes, -1);
			lines[line_index++] = parent;
			break;
		}
		case SEMI_COLON:
			lines[line_index++] = pool_offset(nodes, -1);
			break;
		}
	}

	*l_size = line_index;

	#ifdef DEBUG
	puts("Tokens:");
	printf("%d\n", nodes.p_index);
	for (i = 0; i < nodes.p_index; i++) {
		printf("%s", nodes.pool[i].token->x);
	}
	puts("\nLines:");
	printf("line_index:%d\n", line_index);
	for (i = 0; i < line_index; i++) {
		printf("lines: %s\n", lines[i]->token->x);
		//printf("lines: %s\n", lines[i]->children[0]->token->x);
		putchar('\n');
	}
	#endif

	return lines;
}
