	.file	"test.c"
	.text
	.data
	.align 2
	.type	.Lubsan_type0, @object
	.size	.Lubsan_type0, 10
.Lubsan_type0:
	.value	0
	.value	11
	.string	"'int'"
	.align 2
	.type	.Lubsan_type1, @object
	.size	.Lubsan_type1, 14
.Lubsan_type1:
	.value	0
	.value	11
	.string	"'int [5]'"
	.section	.rodata
.LC0:
	.string	"test.c"
	.section	.data.rel.local,"aw"
	.align 32
	.type	.Lubsan_data0, @object
	.size	.Lubsan_data0, 32
.Lubsan_data0:
	.quad	.LC0
	.long	10
	.long	18
	.quad	.Lubsan_type1
	.quad	.Lubsan_type0
	.section	.rodata
.LC1:
	.string	"%d\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movl	$1, -32(%rbp)
	movl	$2, -28(%rbp)
	movl	$3, -24(%rbp)
	movl	$4, -20(%rbp)
	movl	$5, -16(%rbp)
	movq	$-1, %rsi
	leaq	.Lubsan_data0(%rip), %rdi
	call	__ubsan_handle_out_of_bounds@PLT
	movl	-36(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC1(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$0, %eax
	movq	-8(%rbp), %rdx
	xorq	%fs:40, %rdx
	je	.L3
	call	__stack_chk_fail@PLT
.L3:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
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
