#ifndef LEXER_H
#define LEXER_H

enum Type {
	/* Precedence */
	ASSIGNMENT,
	CLOSE_PARENTHESES,
	MINUS,
	ADD,
	DIVIDE,
	MULTIPLY,
	OPEN_PARENTHESES,

	OPERATOR,
	INTEGER,
	IDENTIFIER_R,
	IDENTIFIER_L,
	SEMI_COLON,
};

struct Token {
	enum Type type;
	char *x;
};

typedef struct Token Token;

#endif
