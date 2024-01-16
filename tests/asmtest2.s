.text
.globl write
write: 
	movq $0x1, %rax

	movq $0x1, %rdi
	movq $msg, %rsi
	movq $len, %rdx
	syscall
	ret
