	.text
	.file	"../tests/tests/unary.ll"
	.globl	neg
	.align	16, 0x90
	.type	neg,@function
neg:                                    # @neg
	.cfi_startproc
# BB#0:                                 # %entry
	movq	%rdi, -8(%rsp)
	negq	%rdi
	movq	%rdi, %rax
	retq
.Lfunc_end0:
	.size	neg, .Lfunc_end0-neg
	.cfi_endproc

	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rax
.Ltmp0:
	.cfi_def_cfa_offset 16
	movq	$-123, %rdi
	callq	neg
	popq	%rcx
	retq
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
	.cfi_endproc


	.section	".note.GNU-stack","",@progbits
