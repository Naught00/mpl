#ifndef LEXER_H
#define LEXER_H

//@Fix change enum type when gcc supports
//C23
enum Type {
	/* Precedence */
	ASSIGNMENT,
	RETURN,
	CLOSE_PARENTHESES,
	ADD,
	MINUS,
	DIVIDE,
	MULTIPLY,
	PROC_CALL,
	OPEN_PARENTHESES,

	PROC_DECLARATION,
	PROC_DEFINITION,
	LOAD_ARG,
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

	/* Keywords */
};

enum Flags {
	LF_lvalue = 0x1,
	LF_declaration = 0x2,
	LF_in_proc = 0x4,
	LF_hanging = 0x8,
};

struct Token {
	enum Type type;
	char *x;
};

typedef struct Token Token;

#endif
