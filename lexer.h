#ifndef LEXER_H
#define LEXER_H

enum Type {
	OPERATOR,
	INTEGER,
	ASSIGNMENT,
	IDENTIFIER,
	OPEN_PARENTHESES,
	CLOSE_PARENTHESES,
	SEMI_COLON,
};

struct Token {
	enum Type type;
	char *x;
};

typedef struct Token Token;

#endif
