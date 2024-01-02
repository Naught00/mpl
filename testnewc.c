#include "lexer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

//int main(void) {
//	char *test = "test";
//	char *test2 = malloc(1024);
//
//	strcat(test2, test);
//
//	int *p = malloc(10);
//	printf("%u\n", p);
//
//}
//

//@FIXME precompute
uint32_t precedence(char op) {
	switch (op) {
	case '(': return 5;
	case ')': return 5;
	case '*': return 4;
	case '/': return 3;
	case '+': return 2;
	case '-': return 1;
	case '=': return 0;
	}
}


char *shunting(Token *tokens, int tokenc, int *line_starts, uint32_t l_size) {
	char opstack[256];
	memset(opstack, 0, 256);
	uint32_t o_index = 0;

	struct npool nodes;
	nodes.pool = malloc(tokenc * sizeof(Node));
	nodes.p_index = 0;

	Node **lines = malloc(l_size * sizeof(Node));

	char dstack[256];
	memset(dstack, 0, 256);
	uint32_t d_index = 0;

	char fstack[256];
	memset(fstack, 0, 256);
	uint32_t f_index = 0;

	uint32_t i, len, x;
	len = 0;
	char next, topop;
	for (i = 0; i < tokenc; i++) {
		switch (tokens[i].type) {
		case OPERATOR: case ASSIGNMENT:
			next   = tokens[i].x[0];
			if (o_index == 0) {
				opstack[o_index++] = next;
				break;
			}
			
			topop  = opstack[o_index - 1];
			if (precedence(next) < precedence(topop)) {
				dstack[d_index++] = topop;
				--o_index;
			}
			opstack[o_index++] = next;
			break;
		case INTEGER: case IDENTIFIER_L:
			//@FIXME get this from elsewhere
			len    = strlen(tokens[i].x);
			memmove(&dstack[d_index], tokens[i].x, len);
			d_index += len;
			break;
		case SEMI_COLON:
			len    = o_index;
			for (x = 0; x < len; x++)
				dstack[d_index++] = opstack[--o_index];

			dstack[d_index++] = ';';
			memmove(&fstack[f_index], dstack, d_index);

			f_index         += d_index;
			memset(dstack,  0, d_index);
			memset(opstack, 0, o_index);

			d_index       = 0;
			o_index       = 0;
			break;
		}
	}

	puts(fstack);
}
