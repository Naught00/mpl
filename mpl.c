#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

enum Type {
	OPERATOR,
	INTEGER,
	ASSIGNMENT,
	IDENTIFIER,
};

struct Token {
	enum Type type;
	char *x;
};

typedef struct Token Token;

bool is_operator(char ch);

Token tokens[100] = {0};

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

	int i, tokenc, r;
	char ch;
	for (i = 0, tokenc = 0; i < program_length; i++) {
		ch = program[i];

		if (isdigit(ch)) {
			char *repr = malloc(10);
			memset(repr, 1, 10);
			r = 0;

			while (isdigit(program[i])) {
				repr[r] = program[i];
				r++;
				i++;
			}
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
			memset(identifier, 1, 10);
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
		}

	}
	for (i = 0; i < tokenc; i++) {        
		switch (tokens[i].type) {
		case INTEGER: printf("INTEGER: %s\n", tokens[i].x); break;
		case ASSIGNMENT: printf("ASSIGNMENT: %s\n", tokens[i].x); break;
		case OPERATOR: printf("OPERATOR: %s\n", tokens[i].x); break;
		case IDENTIFIER: printf("IDENTIFIER: %s\n", tokens[i].x); break;
		}
	}
	free(program);
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
