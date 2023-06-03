#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "lexer.h"

struct Variable {
	int stack_offset;
};

static void cvisit(Node *root, Node **stack, int *sp, char *assembly, int *asm_size, int *current_stack_offset, int *register_index);

char *registers[] = {"%rax", "%rbx", "%rcx", "%rdx", "%rdi"};

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
	int register_index = 0;
	stack[sp++] = &root;
	while (sp) {
		cvisit(stack[--sp], stack, &sp, assembly, &asm_size, &current_stack_offset, &register_index);
		stack[sp] = NULL;
		//sleep(1);
	}

	free(stack);

	char *end = "\tmov %rax, %rdi\n\tcall exit\n\tadd $0x40, %rsp\n\tret\n";
	int end_length = strlen(end);
	memmove(&assembly[asm_size], end, end_length);
	asm_size += end_length;

	char *exit = "exit:\n"
		     "\tmov %rdi, %rbx\n"
		     "\tmov $0x1, %rax\n"
		     "\tint $0x80\n";
	int exit_length = strlen(exit);
	memmove(&assembly[asm_size], exit, exit_length);
	asm_size += exit_length;

	return assembly;
}

static void cvisit(Node *root, Node **stack, int *sp, char *assembly, int *asm_size, int *current_stack_offset, int *register_index) {
	if (!root->children || root->children_added) {
		printf("TOK: %s\n", root->token->x);
		printf("WHERE: %d\n", root->starting_token);
		switch (root->token->type) {
		case INTEGER:
			struct Variable x = {*current_stack_offset};
			sprintf(&assembly[*asm_size], "\tmovq $0x%x, %s\n", atoi(root->token->x), registers[(*register_index)++]);
			*asm_size += strlen(&assembly[*asm_size]);
			*current_stack_offset += 0x8;
			break;
		case OPERATOR:
			sprintf(&assembly[*asm_size], "\tadd %s, %s\n", registers[(*register_index) - 1], registers[(*register_index) - 2]);
			--(*register_index);
			*asm_size += strlen(&assembly[*asm_size]);
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

