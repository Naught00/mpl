	.file	"gcctest.c"
	.text
	.globl	test_function
	.type	test_function, @function
test_function:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$320, %rsp
	movl	%edi, -292(%rbp)
	movl	%esi, -296(%rbp)
	movl	%edx, -300(%rbp)
	movl	%ecx, -304(%rbp)
	movl	%r8d, -308(%rbp)
	movl	%r9d, -312(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movl	$10, -284(%rbp)
	movl	$10, -280(%rbp)
	movss	.LC0(%rip), %xmm0
	movss	%xmm0, -276(%rbp)
	movl	56(%rbp), %eax
	movl	%eax, -284(%rbp)
	movl	-292(%rbp), %eax
	imull	%eax, %eax
	movq	-8(%rbp), %rdx
	subq	%fs:40, %rdx
	je	.L3
	call	*__stack_chk_fail@GOTPCREL(%rip)
.L3:
	leave
	ret
	.size	test_function, .-test_function
	.globl	oog_function
	.type	oog_function, @function
oog_function:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movl	%edi, -20(%rbp)
	movl	%esi, -24(%rbp)
	movl	%edx, -28(%rbp)
	movl	$10, -4(%rbp)
	cmpl	$0, -4(%rbp)
	je	.L5
	movl	$0, %eax
	jmp	.L6
.L5:
	movl	$1, %eax
.L6:
	leave
	ret
	.size	oog_function, .-oog_function
	.globl	struct_function
	.type	struct_function, @function
struct_function:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	%edi, %eax
	movq	%rsi, %rcx
	movq	%rcx, %rdx
	movq	%rax, -16(%rbp)
	movq	%rdx, -8(%rbp)
	movl	$0, %eax
	leave
	ret
	.size	struct_function, .-struct_function
	.globl	struct_function2
	.type	struct_function2, @function
struct_function2:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$8, %rsp
	movq	%rdi, -8(%rbp)
	movl	$0, %eax
	leave
	ret
	.size	struct_function2, .-struct_function2
	.section	.rodata
.LC1:
	.string	"test"
	.text
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$336, %rsp
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movl	$10, -328(%rbp)
	movl	$-2, -324(%rbp)
	movl	-328(%rbp), %eax
	imull	-324(%rbp), %eax
	movl	%eax, -320(%rbp)
	movl	$10, -288(%rbp)
	movq	$100, -280(%rbp)
	movl	$10, -296(%rbp)
	movl	$100, -292(%rbp)
	movq	-296(%rbp), %rax
	movq	%rax, %rdi
	call	struct_function2
	movq	$90, -312(%rbp)
	movq	-312(%rbp), %rax
	imulq	%rax, %rax
	movq	%rax, -304(%rbp)
	movl	$0, -328(%rbp)
	jmp	.L12
.L13:
	movl	-328(%rbp), %eax
	cltq
	movb	$102, -272(%rbp,%rax)
	addl	$1, -328(%rbp)
.L12:
	cmpl	$255, -328(%rbp)
	jle	.L13
	movl	-328(%rbp), %r9d
	movl	-328(%rbp), %r8d
	movl	-328(%rbp), %ecx
	movl	-328(%rbp), %edx
	movl	-328(%rbp), %esi
	movl	-328(%rbp), %eax
	movl	-328(%rbp), %edi
	pushq	%rdi
	movl	-328(%rbp), %edi
	pushq	%rdi
	movl	-328(%rbp), %edi
	pushq	%rdi
	movl	-328(%rbp), %edi
	pushq	%rdi
	movl	-328(%rbp), %edi
	pushq	%rdi
	movl	-328(%rbp), %edi
	pushq	%rdi
	movl	%eax, %edi
	call	test_function
	addq	$48, %rsp
	movl	%eax, -316(%rbp)
	movl	-328(%rbp), %edx
	movl	-328(%rbp), %ecx
	movl	-328(%rbp), %eax
	movl	%ecx, %esi
	movl	%eax, %edi
	call	oog_function
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	call	*puts@GOTPCREL(%rip)
	movl	-316(%rbp), %eax
	movq	-8(%rbp), %rdx
	subq	%fs:40, %rdx
	je	.L15
	call	*__stack_chk_fail@GOTPCREL(%rip)
.L15:
	leave
	ret
	.size	main, .-main
	.section	.rodata
	.align 4
.LC0:
	.long	1084227584
	.ident	"GCC: (GNU) 13.2.1 20230801"
	.section	.note.GNU-stack,"",@progbits
