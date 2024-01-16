.text
.globl _start
.type  exit @function
_start:
	movq $0x1, %rdi
	call write
	call exit
exit:
        movq %rdi, %rbx
        movq $0x3c, %rax
        syscall


.data
.globl msg
.globl len
msg:
	.ascii "Hello, world\n"
	len = . - msg
