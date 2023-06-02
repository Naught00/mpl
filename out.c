#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "lexer.h"

struct Variable {
	int stack_offset;
};

static void cvisit(Node *root, Node **stack, int *sp, char *assembly, int *asm_size, int *current_stack_offset);

char *registers[] = {"%rax", "%rbx", "%rcx", "%rdx", "%rdi"};

//void compile(Node root) {
//	int i;
//	Node *operating = &root;
//
//	printf("\n");
//	for (i = 0; i < 1; i++) {
//		switch (operating->token->x[0]) {
//		case '*':
//			if (operating->right->token->type == OPERATOR) {
//				switch (operating->right->token->x[0]) {
//				case '+': printf("ADD %s, %s\n", operating->right->left->token->x, operating->right->right->token->x);
//				}
//			}
//
//			printf("MULT %s\n", operating->left->token->x);
//		}
//	}
//}
//

char *compile(Node root, int tokenc) {
	int asm_size;
	asm_size = 0;
	char *assembly = malloc(10000);
	memset(assembly, 0, tokenc);

	char *start = ".section .text\n.global _start\n_start:\n\tsub $0x40, %rsp\n";
	int start_length = strlen(start);
	memmove(assembly, start, strlen(start));
	asm_size += start_length;

	/* @FIXME Store nodes directly? */
	Node **stack = malloc(tokenc * sizeof(Node *));

	int i;
	for (i = 0; i < tokenc; i++) {
		stack[i] = NULL;
	}

	int current_stack_offset = 0x0;
	int sp = 0;
	stack[sp++] = &root;
	while (sp) {
		cvisit(stack[--sp], stack, &sp, assembly, &asm_size, &current_stack_offset);
		stack[sp] = NULL;
		//sleep(1);
	}

	free(stack);

	char *end = "\tcall exit\n\tadd $0x40, %rsp\n\tret\n";
	int end_length = strlen(end);
	memmove(&assembly[asm_size], end, end_length);
	asm_size += end_length;

	char *exit = "exit:\n"
		     "\tmov $0x10, %rbx\n"
		     "\tmov $0x1, %rax\n"
		     "\tint $0x80\n";
	int exit_length = strlen(exit);
	memmove(&assembly[asm_size], exit, exit_length);
	asm_size += exit_length;

	return assembly;
}

static void cvisit(Node *root, Node **stack, int *sp, char *assembly, int *asm_size, int *current_stack_offset) {
	if (!root->children || root->children_added) {
		//printf("TOK: %s\n", root->token->x);
		switch (root->token->type) {
		case INTEGER:
			struct Variable x = {*current_stack_offset};
			sprintf(&assembly[*asm_size], "\tmovq $0x%x, 0x%x(%rsp)\n", atoi(root->token->x), x.stack_offset);
			*asm_size += strlen(&assembly[*asm_size]);
			*current_stack_offset += 0x8;
			break;
		case OPERATOR:
			printf("OPER: %s\n", root->token->x);
			break;
		}
		return;
	}

	stack[(*sp)++] = root;
	root->children_added = true;
	
	int i;
	for (i = 1; i > -1; i--) {
		if (root->children[i]) {
			stack[(*sp)++] = root->children[i];
		}
	}
}

