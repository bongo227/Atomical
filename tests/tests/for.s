	.text
	.file	"../tests/tests/for.ll"
	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %entry
	movq	$0, -8(%rsp)
	movq	$0, -16(%rsp)
	xorl	%eax, %eax
	testb	%al, %al
	jne	.LBB0_2
	.align	16, 0x90
.LBB0_1:                                # %for
                                        # =>This Inner Loop Header: Depth=1
	incq	-8(%rsp)
	movq	-16(%rsp), %rax
	incq	%rax
	movq	%rax, -16(%rsp)
	cmpq	$123, %rax
	jl	.LBB0_1
.LBB0_2:                                # %endfor
	movq	-8(%rsp), %rax
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc


	.section	".note.GNU-stack","",@progbits
