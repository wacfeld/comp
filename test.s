	.file	"test.c"
	.text
	.globl	s
	.data
	.type	s, @object
	.size	s, 5
s:
	.ascii	"hello"
	.globl	t
	.section	.rodata
.LC0:
	.string	"hi"
	.section	.data.rel.local,"aw"
	.align 8
	.type	t, @object
	.size	t, 8
t:
	.quad	.LC0
	.text
	.globl	f
	.type	f, @function
f:
.LFB0:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	addl	$1, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	f, .-f
	.section	.rodata
.LC1:
	.string	"etuhonut"
	.text
	.globl	main
	.type	main, @function
main:
.LFB1:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	leaq	.LC1(%rip), %rax
	movq	%rax, -40(%rbp)
	movabsq	$8026950989380150632, %rax
	movabsq	$8031437039870504821, %rdx
	movq	%rax, -32(%rbp)
	movq	%rdx, -24(%rbp)
	movw	$117, -16(%rbp)
	movl	$1, -48(%rbp)
	movl	-48(%rbp), %eax
	movl	%eax, %edi
	call	f
	movl	%eax, -44(%rbp)
	nop
	movq	-8(%rbp), %rcx
	xorq	%fs:40, %rcx
	je	.L4
	call	__stack_chk_fail@PLT
.L4:
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
