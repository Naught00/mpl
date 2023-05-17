#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

enum Type {
	OPERATOR,
	INTEGER,
};

struct Token {
	enum Type type;
	char x;
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

	int i, tokenc;
	char ch;
	for (i = 0, tokenc = 0; i < program_length; i++) {
		ch = program[i];

		if (isdigit(ch)) {
			Token token = {INTEGER, ch};
			tokens[tokenc] = token;
			tokenc++;
			printf("integer\n");
		} else if (is_operator(ch)) {
			Token token = {OPERATOR, ch};
			tokens[tokenc] = token;
			tokenc++;
			printf("operator\n");
		}
	}
	for (i = 0; i < tokenc; i++) {        
		printf("%c\n", tokens[i].x);
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
