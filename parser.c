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

static const struct Token _SEMI_COLON = {SEMI_COLON, ";"};

#define precedence(op) op.type
Node **shunting(Token *tokens, int tokenc, uint32_t *l_size) {
	size_t tarrsz;

	struct {
		Token stack[256];
		uint32_t sp;
	} op;
	op.sp = 0;

	/* @Bencmhark: array of pointer */
	struct {
		Token *stack;
		uint32_t sp;
	} data;

	tarrsz = tokenc * sizeof(Token);
	data.stack = malloc(tarrsz * 2);
	memset(data.stack, 0, tarrsz * 2);
	data.sp = 0;

	int i, j, extra_tokens, extra_lines;
	uint32_t proc_token = 0;
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
		case OPEN_BRACE: case CLOSE_BRACE:
			push(data, tokens[i]);
			push(data, _SEMI_COLON);

			extra_tokens += 1;
			*l_size += 1;
			break;
		case PROC_DECLARATION:
			proc_token =  data.sp;

		//	shput(vtypes, tokens[i].x, tokens[i - 1].type);
			push(data, tokens[i]);
			i += 2;
			while (tokens[i].type != CLOSE_PARENTHESES) {
				if (tokens[i].type == COMMA) {
					i++;
					continue;
				}
				push(data, tokens[i + 1]);
				push(data, tokens[i]);
				i += 2;
			}

			if (tokens[i + 1].type == OPEN_BRACE) {
				push(data, _SEMI_COLON);
				extra_tokens += 1;
				
				data.stack[proc_token].type = PROC_DEFINITION;
				*l_size += 1;
			}
			
			// chop off () and type;
			extra_tokens -= 3;
			break;
		case INT:
			if (tokens[i + 1].type == PROC_DECLARATION) {
				break;
			}

			//shput(vtypes, tokens[i + 1].x, tokens[i].type);

			push(data, tokens[i + 1]);
			push(data, tokens[i]);
			push(data, _SEMI_COLON);

			switch (tokens[i + 2].type) {
			case SEMI_COLON:
				i += 2;
				break;
			default:
				*l_size      += 1;
				extra_tokens += 2;
				break;
			}
			break;
		case SEMI_COLON:
			len    = op.sp;
			for (j = 0; j < len; j++)
				push(data, op.stack[--op.sp]);

			push(data, tokens[i]);
			memset(op.stack, 0, len);
			break;
		}
	}
	tokenc  += extra_tokens;

	#ifdef DEBUG
	printf("tokenc: %d\n", tokenc);
	printf("extra_tokens: %d\n", extra_tokens);
	for (i = 0; i < tokenc; i++) {
		if (tokens[i].x == NULL) {
			break;
		}
		printf("%s", tokens[i].x);
	}
	putchar('\n');

	for (i = 0; i < tokenc; i++) {
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
	nodes.pool = malloc(tokenc * sizeof(Node));
	memset(nodes.pool, 0, tokenc * sizeof(Node));
	nodes.p_index = 0;

	struct spool {
		struct scope *pool;
		uint32_t p_index;
	} scopes;
	scopes.pool = malloc(tokenc * sizeof(struct scope));
	scopes.p_index = 0;

	Node **lines = malloc(*l_size * sizeof(Node *));

	struct scope *curr_scope;
	struct scope *prev_scope;
	curr_scope = pool(scopes);
	curr_scope->vtypes     = NULL;
	curr_scope->frame_size = 0;
	//@Fix global scope

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
			child->dtype = shget(curr_scope->vtypes, data.stack[i].x);
			break;
		}
		case PROC_DECLARATION: case PROC_DEFINITION: {
			Node *parent  = pool(nodes);
			parent->token = &data.stack[i];
			parent->flags = 0x0;
			//@Fix sort out types for function calls
			//shput(*curr_vtypes, data.stack[i].x, );
			//shget(vtypes, data.stack[i].x);

			i++;
			/* arguments */
			for (k = 0; data.stack[i].type != SEMI_COLON; k++, i++) {
				puts(data.stack[i].x);
				Node *arg  = pool(nodes);
				arg->flags = NF_adopted;
				arg->token = &data.stack[i];
				i++;

				Node *dec  = pool(nodes);
				dec->flags = 0x0;
				dec->token = &data.stack[i];
				dec->children[0] = arg;

				parent->children[k] = dec;

			}
			lines[line_index++] = parent;
			break;
		}
		case INT: {
			Node *parent  = pool_stay(nodes); 
			parent->token = &data.stack[i];
			parent->flags = 0x0;

			parent->children[0] = pool_offset(nodes, -1);
			parent->children[0]->flags = NF_adopted;

			shput(curr_scope->vtypes, parent->children[0]->token->x, INT);
			curr_scope->frame_size += size_table[INT];

			nodes.p_index++;
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

			for (j = 0; j < shlen(prev_scope); j++) {
				shput(curr_scope->vtypes,
				          prev_scope->vtypes[j].key,
				          prev_scope->vtypes[j].value);
			}
			break;
		}
		case CLOSE_BRACE: {
			Node *parent  = pool(nodes); 
			parent->token = &data.stack[i];
			parent->flags = 0x0;

			parent->auxiliary = (void *) &curr_scope->frame_size;

			pool_freetop(scopes);
			curr_scope = pool_offset(scopes, -1);
			break;
		}
		case SEMI_COLON:
			lines[line_index++] = pool_offset(nodes, -1);
			break;
		}
	}

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

	return lines;
}
