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

#include "stb_ds.h"

static void token_delete(Token *tokens, int tokenc);
static void token_print(Token *tokens, int tokenc);

static struct {char *key; enum Type value;} *types;

void init_types() {
	shput(types, "int", INT);
}

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

	#ifdef DEBUG
	puts(program);
	#endif

	init_types();

	Token *tokens = malloc(program_length * sizeof(Token));

	char *reprs     = malloc(program_length * sizeof(char) * 10);
	memset(reprs, 0, program_length * sizeof(char) * 10);
	uint32_t r_index = 0;

	int tokenc, r;
	char ch, *repr, *identifier;

	bool lvalue = true, declaration = false;

	uint32_t l_index = 0;
	enum Type data_type;
	int i, openc = 1, closec = 1;
	for (i = 0, tokenc = 0; i < program_length; i++) {
		ch = program[i];

		switch (ch) {
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9': {
			repr = &reprs[r_index];
			r = 0;

			while (isdigit(program[i])) {
				repr[r++] = program[i];
				i++;
			}
			i--;

			repr[r] = '\0';
			r_index += r + 1;

			Token token = {INTEGER, repr};
			tokens[tokenc] = token;
			tokenc++;
			break;
		}
		case '+': {
			repr = &reprs[r_index];
			repr[0] = ch;
			repr[1] = '\0';
			r_index += 2;

			Token token = {ADD, repr};
			tokens[tokenc] = token;
			tokenc++;
			break;
		}
		case '-': {
			repr = &reprs[r_index];
			repr[0] = ch;
			repr[1] = '\0';
			r_index += 2;

			Token token = {MINUS, repr};
			tokens[tokenc] = token;
			tokenc++;
			break;
		}
		case '*': {
			repr = &reprs[r_index];
			repr[0] = ch;
			repr[1] = '\0';
			r_index += 2;

			Token token = {MULTIPLY, repr};
			tokens[tokenc] = token;
			tokenc++;
			break;
		}
		case '/': {
			repr = &reprs[r_index];
			repr[0] = ch;
			repr[1] = '\0';
			r_index += 2;

			Token token = {DIVIDE, repr};
			tokens[tokenc] = token;
			tokenc++;
			break;
		}
		case '=': {
			repr = &reprs[r_index];
			repr[0] = ch;
			repr[1] = '\0';
			r_index += 2;

			Token token = {ASSIGNMENT, repr};
			tokens[tokenc] = token;
			tokenc++;
			break;
		}
		case '(': {
			openc++;
			if (openc == 1)  break;
			repr = &reprs[r_index];
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
			openc--;
			if (openc == 0)  break;

			repr = &reprs[r_index];
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
		case '{': {
			repr = &reprs[r_index];
			repr[0] = ch;
			repr[1] = '\0';
			r_index += 2;

			enum Type type;
			type = OPEN_BRACE;

			Token token = {type, repr};
			tokens[tokenc] = token;
			tokenc++;
			break;
		} 
		case '}': {
			repr = &reprs[r_index];
			repr[0] = ch;
			repr[1] = '\0';
			r_index += 2;

			Token token = {CLOSE_BRACE, repr};
			tokens[tokenc] = token;
			tokenc++;
			break;
		} 
		case ';': {
			repr = &reprs[r_index];
			repr[0] = ch;
			repr[1] = '\0';
			r_index += 2;

			Token token = {SEMI_COLON, repr};
			tokens[tokenc] = token;
			tokenc++;

			lvalue = true;
			l_index++;
			break;
		}
		default: {
			if (!isalpha(ch)) 
				break;

			identifier = &reprs[r_index];
			r = 0;
			while (isalpha(program[i])
			       || program[i] == '_'
			       || isdigit(program[i])
			       ) {
				identifier[r++] = program[i];
				i++;
			}
			i--;
			identifier[r] = '\0';
			r_index += r + 1;

			Token token = {.x = identifier};
			data_type   = shget(types, identifier);
			if (data_type) {
				token.type  = data_type;
				declaration = true;
				lvalue      = true;
			} else if (program[i + 1] == '(' && declaration) {
				token.type = PROC_DECLARATION;
				declaration = false;
			} else if (program[i + 1] == '(') {
				token.type = PROC_CALL;
				lvalue  = false;
				openc = 0;
				closec = 0;
			} else if (lvalue && declaration) {
				token.type  = DECLARATION_CHILD; 
				lvalue      = false;
				declaration = false;
			} else if (lvalue) {
				token.type  = IDENTIFIER_L; 
				lvalue      = false;
			} else {
				token.type  = IDENTIFIER_R;
			}

			tokens[tokenc] = token;
			tokenc++;
			break;
		}
		}
	}

	free(program);

	Node **tree  = shunting(tokens, tokenc, &l_index);
	
	char *assembly;
	assembly = compile(tree, l_index, tokenc);
	#ifdef DEBUG
	printf("%s", assembly);
	#endif

	FILE *as = fopen("/tmp/a.s", "w+");
	if (as == NULL) {
		fprintf(stderr, "Error creating temporary assembly file! %d\n", errno);
		return 3;
	}

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
		//case OPERATOR: printf("OPERATOR: %s\n", tokens[i].x); break;
		case IDENTIFIER_L: printf("IDENTIFIER_L: %s\n", tokens[i].x); break;
		case IDENTIFIER_R: printf("IDENTIFIER_R: %s\n", tokens[i].x); break;
		case DECLARATION_CHILD: printf("decler: %s\n", tokens[i].x); break;
		case OPEN_PARENTHESES: printf("Open PARENTHESES: %s\n", tokens[i].x); break;
		case CLOSE_PARENTHESES: printf("Close PARENTHESES: %s\n", tokens[i].x); break;
		case SEMI_COLON: printf("Semi-colon: %s\n", tokens[i].x); break;
		}
	}
}
