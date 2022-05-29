	.file	"test.c"
	.text
	.globl	fun
	.type	fun, @function

fun:
	pushq	%rbp
	movq	%rsp, %rbp
	movl	%edi, -20(%rbp) ; c
	movl	$5, -8(%rbp) ; x
	movl	$6, -4(%rbp) ; y

	movl	-4(%rbp), %eax
	addl	%eax, -8(%rbp)
	movl	-8(%rbp), %edx
	movl	-20(%rbp), %eax
	addl	%edx, %eax
	popq	%rbp
	ret

.LFE0:
	.size	fun, .-fun
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp

	movl	$6, %edi
	call	fun
	movl	%eax, -4(%rbp)
	movl	$1, %eax
	

	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 9.4.0-1ubuntu1~20.04.1) 9.4.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	 1f - 0f
	.long	 4f - 1f
	.long	 5
0:
	.string	 "GNU"
1:
	.align 8
	.long	 0xc0000002
	.long	 3f - 2f
2:
	.long	 0x3
3:
	.align 8
4:
