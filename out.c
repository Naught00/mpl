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

static void cvisit(Node *root, Node **stack, uint32_t *sp, char *assembly, int *asm_size, int *current_stack_offset, int *register_index);

char *registersq[] = {"%rax", "%rbx", "%rcx", "%rdx", "%rdi"};
char *registersl[] = {"%eax", "%ebx", "%ecx", "%edx", "%edi"};

char *compile(Node **tree, uint32_t l_size, int tokenc) {
	int asm_size;
	asm_size = 0;
	char *assembly = malloc(tokenc * 100);
	memset(assembly, 0, tokenc * 100);

	char *start = ".section .text\n"
	       	      ".global _start\n"
		      "_start:\n"
		      "\tsub $0x40, %rsp\n";

	int start_length = strlen(start);
	memmove(assembly, start, start_length);
	asm_size += start_length;

	Node **stack = malloc(tokenc * sizeof(Node *));
	int i;
	for (i = 0; i < tokenc; i++) {
		stack[i] = NULL;
	}

	int current_stack_offset = 0x0;
	int register_index       = 0;
	uint32_t sp;
	printf("lsize: %d\n", l_size);
	for (i = 0; i < l_size; i++) {
		sp = 0;
		stack[sp++] = tree[i];
		while (sp) {
			cvisit(stack[--sp], stack, &sp,
				       	assembly, &asm_size,
				       	&current_stack_offset,
				       	&register_index
			      );
			stack[sp] = NULL;
		}
	}
	free(stack);

	char *end = "\tmovq %rax, %rdi\n"
		    "\tcall exit\n"
		    "\tadd $0x40, %rsp\n"
		    "\tret\n";
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

static inline void cvisit(Node *root, Node **stack, uint32_t *sp, 
		          char *assembly, int *asm_size, 
		          int *current_stack_offset, int *register_index) 
{
	int variable_offset;

	if (!root->children || root->flags & NF_children_added) {
		switch (root->token->type) {
		case INTEGER:
			stbsp_sprintf(&assembly[*asm_size], 
					"\tmovl $0x%x, %s\n",
				       	atoi(root->token->x),
				       	registersl[*register_index]
				     );
			*register_index += 1;

			*asm_size += strlen(&assembly[*asm_size]);
			break;
		case ADD:      case MINUS:
		case MULTIPLY: case DIVIDE:
			stbsp_sprintf(&assembly[*asm_size],
					"\tadd %s, %s\n",
				       	registersl[*register_index - 1],
				       	registersl[*register_index - 2]
				     );

			*asm_size += 16;

			*register_index -= 1;
			break;
		case IDENTIFIER_R:
			variable_offset = *(uint32_t *) root->auxiliary;

			stbsp_sprintf(&assembly[*asm_size],
				       	"\tmovl -0x%x(%%rbp), %s\n",
				       	variable_offset, 
				        registersl[*register_index]
				     );
			*register_index += 1;

			*asm_size += strlen(&assembly[*asm_size]);
			break;
		case INT:
			break;
		case OPEN_BRACE:
			stbsp_sprintf(&assembly[*asm_size],
				       	"\tsub $0x%x, %%rsp\n",
					*((size_t *) root->auxiliary)
				     );
			*asm_size += strlen(&assembly[*asm_size]);
			break;
		case CLOSE_BRACE:
			stbsp_sprintf(&assembly[*asm_size],
				       	"\tadd $0x%x, %%rsp\n",
					*((size_t *) root->auxiliary)
				     );
			*asm_size += strlen(&assembly[*asm_size]);
			break;
		case PROC_DECLARATION:
			break;
		case PROC_DEFINITION:
			stbsp_sprintf(&assembly[*asm_size],
				       	"%s:\n"
					"\tpushq %%rbp\n"
					"\tmovq %%rsp, %%rbp\n",
					root->token->x
				     );
			*asm_size += strlen(&assembly[*asm_size]);
			break;
		case ASSIGNMENT:
			variable_offset = *(uint32_t *) root->children[0]->auxiliary; 

			stbsp_sprintf(&assembly[*asm_size],
				       	"\tmovl %s, -0x%x(%%rbp)\n",
				       	registersl[--*register_index],
				       	variable_offset
				     );

			*asm_size += strlen(&assembly[*asm_size]);
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
