#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "lexer.h"

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"

#define LEFT  0
#define RIGHT 1

static struct {char *key; int value;} *variables = NULL;

static void cvisit(Node *root, Node **stack, uint32_t *sp, char *assembly, int *asm_size, int *current_stack_offset, int *register_index);

char *registers[] = {"%rax", "%rbx", "%rcx", "%rdx", "%rdi"};

char *compile(Node **tree, uint32_t l_size, int tokenc) {
	int asm_size;
	asm_size = 0;
	char *assembly      = malloc(tokenc * 100);
	memset(assembly, 0, tokenc * 100);

	char *start = ".section .text\n.global _start\n_start:\n\tsub $0x40, %rsp\n";
	int start_length = strlen(start);
	memmove(assembly, start, start_length);
	asm_size += start_length;

	Node **stack = malloc(tokenc * sizeof(Node *));
	int i;
	for (i = 0; i < tokenc; i++) {
		stack[i] = NULL;
	}

	int current_stack_offset = 0x0;
	int register_index = 0;

	uint32_t sp;
	for (i = 0; i < l_size; i++) {
		sp = 0;
		stack[sp++] = tree[i];
		while (sp) {
			cvisit(stack[--sp], stack, &sp, assembly, &asm_size, &current_stack_offset, &register_index);
			stack[sp] = NULL;
		}
	}

	free(stack);

	char *end = "\tmovq %rax, %rdi\n\tcall exit\n\tadd $0x40, %rsp\n\tret\n";
	int end_length = strlen(end);
	memmove(&assembly[asm_size], end, end_length);
	asm_size += end_length;

	char *exit = "exit:\n"
		     "\tmovq %rdi, %rbx\n"
		     "\tmovq $0x3c, %rax\n"
		     "\tsyscall\n";
	int exit_length = strlen(exit);
	memmove(&assembly[asm_size], exit, exit_length);
	asm_size += exit_length;

	return assembly;
}

static void cvisit(Node *root, Node **stack, uint32_t *sp, char *assembly, int *asm_size, int *current_stack_offset, int *register_index) {
	if (!root->children || root->flags & NF_children_added) {
		switch (root->token->type) {
		case INTEGER:
			stbsp_sprintf(&assembly[*asm_size], "\tmovq $0x%x, %s\n", atoi(root->token->x), registers[(*register_index)++]);
			*asm_size += strlen(&assembly[*asm_size]);
			break;
		case OPERATOR:
			stbsp_sprintf(&assembly[*asm_size], "\tadd %s, %s\n", registers[(*register_index) - 1], registers[(*register_index) - 2]);
			*asm_size += 16;

			--(*register_index);
			break;
		case IDENTIFIER_R: {
			int variable_offset = shget(variables, root->token->x);

			stbsp_sprintf(&assembly[*asm_size], "\tmovq 0x%x(%rsp), %s\n", variable_offset, registers[(*register_index)++]);
			*asm_size += strlen(&assembly[*asm_size]);
			break;
		}
		case ASSIGNMENT:
			shput(variables, root->children[LEFT]->token->x, *current_stack_offset);

			stbsp_sprintf(&assembly[*asm_size], "\tmovq %s, 0x%x(%rsp)\n", registers[--(*register_index)], *current_stack_offset);
			*asm_size += strlen(&assembly[*asm_size]);

			*current_stack_offset += 0x8;
			break;
		}
		return;
	}

	stack[(*sp)++] = root;
	root->flags   |= NF_children_added;
	
	int i;
	for (i = 1; i > -1; i--) {
		if (root->children[i]) {
			stack[(*sp)++] = root->children[i];
		}
	}
}
