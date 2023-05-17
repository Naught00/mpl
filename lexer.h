#ifndef LEXER_H
#define LEXER_H

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

void token_delete(Token *tokens, int tokenc);
void token_print(Token *tokens, int tokenc);

#endif
