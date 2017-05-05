	.text
	.file	"../tests/tests/arrayInit.ll"
	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %entry
	movq	$100, -24(%rsp)
	movq	$20, -16(%rsp)
	movq	$3, -8(%rsp)
	movq	-24(%rsp), %rax
	movq	-16(%rsp), %rcx
	leaq	3(%rax,%rcx), %rax
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc


	.section	".note.GNU-stack","",@progbits
