#include <stdio.h>
#include <string.h>

#include "parser.h"
#include "lexer.h"

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"

#define LEFT  0
#define RIGHT 1

static void cvisit(Node *root, Node **stack, uint32_t *sp, char *assembly, int *asm_size, bool *returning, int *register_index);

#define take(r)    r->registers[r->r_index++]
#define release(r) r->r_index--

#define release_all(r) r.r_index = 0

static char *registersq[] = {"%rax", "%rbx", "%rcx", "%rdx", "%rdi"};
static char *registersl[] = {"%eax", "%ebx", "%ecx", "%edx", "%edi"};

struct reg_stacks {
	int r_index;
	char *registers[];
};

//static struct reg_stacks registersq = 
//{0, {"%rax", "%rbx", "%rcx", "%rdx", "%rdi"}};
//
//static struct reg_stacks registersq = 
//{0, {"%eax", "%ebx", "%ecx", "%edx", "%edi"}};

static struct reg_stacks arg_regsq = 
{0, {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"}};

static struct reg_stacks arg_regsl = 
{0, {"%edi", "%esi", "%edx", "%ecx", "%e8", "%e9"}};

static struct reg_stacks *curr_argstack;

//~Fix think about updating tokenc.
char *compile(Node **tree, uint32_t l_size, int tokenc) {
	int asm_size;
	asm_size = 0;
	char *assembly = malloc(tokenc * 100);
	memset(assembly, 0, tokenc * 100);

	char *start = ".section .text\n"
	       	      ".globl main\n";

	int start_length = strlen(start);
	memmove(assembly, start, start_length);
	asm_size += start_length;

	Node **stack = malloc(tokenc * sizeof(Node *));
	int i;
	for (i = 0; i < tokenc; i++) {
		stack[i] = NULL;
	}

	int register_index  = 0;
	bool returning      = false;
	uint32_t sp;
	for (i = 0; i < l_size; i++) {
		sp = 0;
		stack[sp++] = tree[i];
		register_index = 0;
		while (sp) {
			cvisit(stack[--sp], stack, &sp,
				       	assembly, &asm_size,
				       	&returning,
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

	//char *exit = "exit:\n"
	//	     "\tmovq %rdi, %rbx\n"
	//	     "\tmovq $0x3c, %rax\n"
	//	     "\tsyscall\n";
	//int exit_length = strlen(exit);
	//memmove(&assembly[asm_size], exit, exit_length);
	//asm_size += exit_length;

	return assembly;
}

static inline void cvisit(Node *root, Node **stack, uint32_t *sp, 
		          char *assembly, int *asm_size, 
		          bool *returning, int *register_index) 
{
	int variable_offset;
	struct procedure_call *proc_call;
	size_t frame_size;
	int temp_storage;
	char *reg, *argreg, *retreg;
	int i, j;

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
			variable_offset = ((struct variable *) root->auxiliary)->offset;

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
			frame_size  = *(size_t *) root->auxiliary;
			frame_size += 16 - frame_size % 16;
			stbsp_sprintf(&assembly[*asm_size],
				       	"\tsub $0x%lx, %%rsp\n",
					frame_size
				     );
			*asm_size += strlen(&assembly[*asm_size]);
			break;
		case CLOSE_BRACE:
			if (*returning) {
				stbsp_sprintf(&assembly[*asm_size],
						"\tleave\n"
						"\tret\n"
					     );
				*asm_size += strlen(&assembly[*asm_size]);
				break;
			}

			frame_size  = *(size_t *) root->auxiliary;
			frame_size += 16 - frame_size % 16;
			stbsp_sprintf(&assembly[*asm_size],
				       	"\tadd $0x%lx, %%rsp\n",
					frame_size
				     );
			*asm_size += strlen(&assembly[*asm_size]);

			break;
		case PROC_DECLARATION:
			break;
		case PROC_DEFINITION:
			*returning = false;

			stbsp_sprintf(&assembly[*asm_size],
				       	"%s:\n"
					"\tpushq %%rbp\n"
					"\tmovq %%rsp, %%rbp\n",
					root->token->x
				     );
			*asm_size += strlen(&assembly[*asm_size]);
			
			release_all(arg_regsq);
			release_all(arg_regsl);
			break;
		case LOAD_ARG:
			if (((struct variable *) root->auxiliary)->size == 8)
				curr_argstack = &arg_regsq;
			else 
				curr_argstack = &arg_regsl;

			stbsp_sprintf(&assembly[*asm_size],
				       	"\tmov %s, -0x%x(%%rbp)\n",
					take(curr_argstack),
					((struct variable *) root->auxiliary)->offset
				     );
			*asm_size += strlen(&assembly[*asm_size]);
			break;
		case PROC_CALL:
			proc_call = root->auxiliary;
			for (i = 0, j = *register_index - proc_call->argc; i < proc_call->argc; i++, j++) {
				if (proc_call->argsizes[i] == 8) {
					argreg = arg_regsq.registers[i];
					reg    = registersq[j];
				} else {
					argreg = arg_regsl.registers[i];
					reg    = registersl[j];
				}

				stbsp_sprintf(&assembly[*asm_size],
						"\tmov %s, %s\n",
						reg, argreg
					     );
				*asm_size += strlen(&assembly[*asm_size]);
			}

			*register_index -= proc_call->argc;
			if (*register_index) {
				temp_storage = 0;
				stbsp_sprintf(&assembly[*asm_size],
						"\tsub $0x30, %%rsp\n"
					     );
				*asm_size += strlen(&assembly[*asm_size]);

				for (i = 0; i < *register_index; i++) {
					stbsp_sprintf(&assembly[*asm_size],
							"\tmov %s, 0x%x(%%rsp)\n",  
							registersq[i], temp_storage
						     );
					*asm_size += strlen(&assembly[*asm_size]);

					temp_storage += 0x8;
				}

			}

			stbsp_sprintf(&assembly[*asm_size],
					"\tcall %s\n", root->token->x
				     );
			*asm_size += strlen(&assembly[*asm_size]);

			if (proc_call->return_size == 8)
				retreg = registersq[0];
			else
				retreg = registersl[0];

			stbsp_sprintf(&assembly[*asm_size],
					"\tmov %s, %s\n", 
					retreg, registersl[*register_index]
				     );
			*asm_size += strlen(&assembly[*asm_size]);

			if (*register_index) {
				temp_storage = 0;

				for (i = 0; i < *register_index; i++) {
					stbsp_sprintf(&assembly[*asm_size],
							"\tmov 0x%x(%%rsp), %s\n",  
							temp_storage, registersq[i]
						     );
					*asm_size += strlen(&assembly[*asm_size]);

					temp_storage += 0x8;
				}

				stbsp_sprintf(&assembly[*asm_size],
						"\tadd $0x30, %%rsp\n"
					     );
				*asm_size += strlen(&assembly[*asm_size]);
			}

			*register_index += 1;
			break;
		case RETURN:
			*returning = true;
			break;
		case ASSIGNMENT:
			puts(root->children[0]->token->x);
			variable_offset = ((struct variable *) root->
							       children[0]->
							       auxiliary)->
				                               offset;

			stbsp_sprintf(&assembly[*asm_size],
				       	"\tmovl %s, -0x%x(%%rbp)\n",
				       	registersl[--*register_index],
				       	variable_offset
				     );


			*asm_size += strlen(&assembly[*asm_size]);
			break;
		}
		return;
	}

	stack[(*sp)++] = root;
	root->flags   |= NF_children_added;
	
	for (i = 1; i > -1; i--) {
		if (root->children[i]) {
			stack[(*sp)++] = root->children[i];
		}
	}
}
