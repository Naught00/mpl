#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "out.h"

bool is_operator(char ch);
bool is_paren(char ch);

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
	printf("%s\n", program);

	/* tokenc is always <= program_length
	 */
	Token *tokens = malloc(program_length * sizeof(Token));

	int i, tokenc, r;
	char ch;
	for (i = 0, tokenc = 0; i < program_length; i++) {
		ch = program[i];

		if (isdigit(ch)) {
			char *repr = malloc(10);
			memset(repr, 0, 10 * sizeof(char));
			r = 0;

			while (isdigit(program[i])) {
				repr[r] = program[i];
				r++;
				i++;
			}
			/* Loop is over: undo the overshoot
			 */
			i--;

			repr[r + 1] = '\0';

			Token token = {INTEGER, repr};
			tokens[tokenc] = token;
			tokenc++;

		} else if (is_operator(ch)) {
			char *repr = malloc(2);
			repr[0] = ch;
			repr[1] = '\0';

			Token token = {OPERATOR, repr};
			tokens[tokenc] = token;
			tokenc++;
		} else if (ch == '=') {
			char *repr = malloc(2);
			repr[0] = ch;
			repr[1] = '\0';

			Token token = {ASSIGNMENT, repr};
			tokens[tokenc] = token;
			tokenc++;
		} else if (isalpha(ch)) {
			char *identifier = malloc(10);
			memset(identifier, 0, 10 * sizeof(char));
			r = 0;

			while isalpha(program[i]) {
				identifier[r] = program[i];
				r++;
				i++;
			}
			identifier[r + 1] = '\0';

			Token token = {IDENTIFIER, identifier};
			tokens[tokenc] = token;
			tokenc++;
		} else if (is_paren(ch)) {
			printf("%c\n", ch);
			char *repr = malloc(2);
			repr[0] = ch;
			repr[1] = '\0';

			Token token = {PARENTHESES, repr};
			tokens[tokenc] = token;
			tokenc++;
		}

	}
	/* We have already incremented tokenc so
	 * it is the correct size 
	 */
	token_print(tokens, tokenc);

	Node ast = tree(tokens, tokenc);
	tree_print(ast);
	compile(ast);

	free(program);
	token_delete(tokens, tokenc);
}

bool is_operator(char ch) {
	switch (ch) {
	case '+': return true;
	case '-': return true;
	case '*': return true;
	case '/': return true;
	default : return false;
	}
}

bool is_paren(char ch) {
	switch (ch) {
	case '(': return true;
	case ')': return true;

	default : return false;
	}
}

void token_delete(Token *tokens, int tokenc) {
	int i;
	for (i = 0; i < tokenc; i++) {
		free(tokens[i].x);
	}
	free(tokens);
}

void token_print(Token *tokens, int tokenc) {
	int i;
	for (i = 0; i < tokenc; i++) {        
		switch (tokens[i].type) {
		case INTEGER: printf("INTEGER: %s\n", tokens[i].x); break;
		case ASSIGNMENT: printf("ASSIGNMENT: %s\n", tokens[i].x); break;
		case OPERATOR: printf("OPERATOR: %s\n", tokens[i].x); break;
		case IDENTIFIER: printf("IDENTIFIER: %s\n", tokens[i].x); break;
		case PARENTHESES: printf("PARENTHESES: %s\n", tokens[i].x); break;
		}
	}
}

