#ifndef LEXER_H
#define LEXER_H

//@Fix change enum type when gcc supports
//C23
enum Type {
	/* Precedence */
	ASSIGNMENT,
	CLOSE_PARENTHESES,
	MINUS,
	ADD,
	DIVIDE,
	MULTIPLY,
	OPEN_PARENTHESES,
	PROC_CALL,

	PROC_DECLARATION,
	PROC_DEFINITION,
	OPEN_BRACE,
	CLOSE_BRACE,
	COMMA,
	INTEGER,
	DECLARATION_CHILD,
	IDENTIFIER_R,
	IDENTIFIER_L,
	SEMI_COLON,

	/* Data types */
	INT,
};

struct Token {
	enum Type type;
	char *x;
};

typedef struct Token Token;

#endif
