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
	.section	.rodata
.LC1:
	.string	"test"
	.text
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$304, %rsp
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movl	$10, -300(%rbp)
	movl	$-2, -296(%rbp)
	movl	-300(%rbp), %eax
	imull	-296(%rbp), %eax
	movl	%eax, -292(%rbp)
	movq	$90, -288(%rbp)
	movq	-288(%rbp), %rax
	imulq	%rax, %rax
	movq	%rax, -280(%rbp)
	movl	$0, -300(%rbp)
	jmp	.L5
.L6:
	movl	-300(%rbp), %eax
	cltq
	movb	$102, -272(%rbp,%rax)
	addl	$1, -300(%rbp)
.L5:
	cmpl	$255, -300(%rbp)
	jle	.L6
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	call	*puts@GOTPCREL(%rip)
	movl	$0, %eax
	movq	-8(%rbp), %rdx
	subq	%fs:40, %rdx
	je	.L8
	call	*__stack_chk_fail@GOTPCREL(%rip)
.L8:
	leave
	ret
	.size	main, .-main
	.globl	oog_function
	.type	oog_function, @function
oog_function:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	$10, -4(%rbp)
	cmpl	$0, -4(%rbp)
	je	.L10
	movl	$0, %eax
	jmp	.L11
.L10:
	movl	$1, %eax
.L11:
	leave
	ret
	.size	oog_function, .-oog_function
	.section	.rodata
	.align 4
.LC0:
	.long	1084227584
	.ident	"GCC: (GNU) 13.2.1 20230801"
	.section	.note.GNU-stack,"",@progbits
