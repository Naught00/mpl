#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "parser.h"
#include "out.h"

#define MAX_VSIZE 256

static void token_delete(Token *tokens, int tokenc);
static void token_print(Token *tokens, int tokenc);

int main(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "No arguments supplied!\n");
		return 1;
	}

	FILE *f = fopen(argv[1], "r");
	if (f == NULL) {
		fprintf(stderr, "Invalid filename! errno: %d\n", errno);
		return 2;
	}

	fseek(f, 0, SEEK_END);
	long program_length = ftell(f);
	rewind(f);

	char *program = malloc(program_length);
	fread(program, 1, program_length, f);
	fclose(f);
	printf("%s", program);

	Token *tokens = malloc(program_length * sizeof(Token));

	int i;
	char *reprs     = malloc(program_length * sizeof(char) * 10);
	memset(reprs, 0, program_length * sizeof(char) * 10);
	uint32_t r_index = 0;

	int tokenc, r;
	char ch;
	bool lvalue      = true;
	int *lines       = malloc(program_length * sizeof(int));
	uint32_t l_index = 0;
	lines[l_index++] = 0;
	for (i = 0, tokenc = 0; i < program_length; i++) {
		ch = program[i];

		switch (ch) {
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9': {
			char *repr = &reprs[r_index];
			r = 0;

			while (isdigit(program[i])) {
				repr[r++] = program[i];
				i++;
			}
			i--;

			repr[r] = '\0';
			printf("repr: %s\n", repr);
			r_index += r + 1;

			Token token = {INTEGER, repr};
			tokens[tokenc] = token;
			tokenc++;
			break;
		}
		case '+': case '-':
		case '*': case '/': {
			char *repr = &reprs[r_index];
			repr[0] = ch;
			repr[1] = '\0';
			r_index += 2;

			Token token = {OPERATOR, repr};
			tokens[tokenc] = token;
			tokenc++;
			break;
		}
		case '=': {
			char *repr = &reprs[r_index];
			repr[0] = ch;
			repr[1] = '\0';
			r_index += 2;

			Token token = {ASSIGNMENT, repr};
			tokens[tokenc] = token;
			tokenc++;
			break;
		}
		case '(': {
			printf("%c\n", ch);
			char *repr = &reprs[r_index];
			repr[0] = ch;
			repr[1] = '\0';
			r_index += 2;

			enum Type type;
			type = OPEN_PARENTHESES;

			Token token = {type, repr};
			tokens[tokenc] = token;
			tokenc++;
			break;
		}
		case ')': {
			printf("%c\n", ch);
			char *repr = &reprs[r_index];
			repr[0] = ch;
			repr[1] = '\0';
			r_index += 2;

			enum Type type;
			type = CLOSE_PARENTHESES;

			Token token = {type, repr};
			tokens[tokenc] = token;
			tokenc++;
			break;
		}
		case ';': {
			char *repr = &reprs[r_index];
			repr[0] = ch;
			repr[1] = '\0';
			r_index += 2;

			Token token = {SEMI_COLON, repr};
			tokens[tokenc] = token;
			tokenc++;

			lvalue = true;
			lines[l_index++] = tokenc;
			break;
		}
		default: {
			if (!isalpha(ch)) 
				break;

			char *identifier = &reprs[r_index];
			r = 0;

			while isalpha(program[i]) {
				identifier[r++] = program[i];
				i++;
			}
			i--;
			identifier[r] = '\0';
			r_index += r + 1;

			Token token = {.x = identifier};
			if (lvalue) {
				token.type = IDENTIFIER_L; 
				lvalue = false;
			} else {
				token.type = IDENTIFIER_R;
			}

			tokens[tokenc] = token;
			tokenc++;
			break;
		}
		}
	}

	free(program);
	token_print(tokens, tokenc);

	l_index--;
	//Node *tree = tree_make(tokens, tokenc, lines, l_index);
	Node **tree  = shunting(tokens, tokenc, lines, l_index);
	//tree_print(tree);
	
	char *assembly;
	assembly = compile(tree, l_index, tokenc);
	//printf("%s", assembly);

	//FILE *as = fopen("/tmp/a.s", "w+");
	//if (as == NULL) {
	//	fprintf(stderr, "Error creating temporary assembly file! %d\n", errno);
	//	return 3;
	//}

	//fprintf(as, "%s", assembly);
	//fclose(as);

	//system("as -o /tmp/tmp.o /tmp/a.s");
	//system("ld /tmp/tmp.o -o a.out");
	//remove("/tmp/a.s");
	//remove("/tmp/tmp.o");

	//free(assembly);
	//token_delete(tokens, tokenc);
	//@TODO free the tree
}

static void token_delete(Token *tokens, int tokenc) {
	int i;
	for (i = 0; i < tokenc; i++) {
		free(tokens[i].x);
	}
	free(tokens);
}

static void token_print(Token *tokens, int tokenc) {
	int i;
	for (i = 0; i < tokenc; i++) {        
		switch (tokens[i].type) {
		case INTEGER: printf("INTEGER: %s\n", tokens[i].x); break;
		case ASSIGNMENT: printf("ASSIGNMENT: %s\n", tokens[i].x); break;
		case OPERATOR: printf("OPERATOR: %s\n", tokens[i].x); break;
		case IDENTIFIER_L: printf("IDENTIFIER_L: %s\n", tokens[i].x); break;
		case IDENTIFIER_R: printf("IDENTIFIER_R: %s\n", tokens[i].x); break;
		case OPEN_PARENTHESES: printf("Open PARENTHESES: %s\n", tokens[i].x); break;
		case CLOSE_PARENTHESES: printf("Close PARENTHESES: %s\n", tokens[i].x); break;
		case SEMI_COLON: printf("Semi-colon: %s\n", tokens[i].x); break;
		}
	}
}
