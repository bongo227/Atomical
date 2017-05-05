	.text
	.file	"../tests/tests/arraySum.ll"
	.globl	sum
	.align	16, 0x90
	.type	sum,@function
sum:                                    # @sum
	.cfi_startproc
# BB#0:                                 # %entry
	movq	%rdx, -8(%rsp)
	movq	%rsi, -16(%rsp)
	movq	%rdi, -24(%rsp)
	movq	$0, -32(%rsp)
	movq	$0, -40(%rsp)
	xorl	%eax, %eax
	testb	%al, %al
	jne	.LBB0_2
	.align	16, 0x90
.LBB0_1:                                # %for
                                        # =>This Inner Loop Header: Depth=1
	movq	-40(%rsp), %rax
	movq	-24(%rsp,%rax,8), %rax
	addq	%rax, -32(%rsp)
	movq	-40(%rsp), %rax
	incq	%rax
	movq	%rax, -40(%rsp)
	cmpq	$3, %rax
	jl	.LBB0_1
.LBB0_2:                                # %endfor
	movq	-32(%rsp), %rax
	retq
.Lfunc_end0:
	.size	sum, .Lfunc_end0-sum
	.cfi_endproc

	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %entry
	subq	$24, %rsp
.Ltmp0:
	.cfi_def_cfa_offset 32
	movq	$100, (%rsp)
	movq	$20, 8(%rsp)
	movq	$3, 16(%rsp)
	movq	(%rsp), %rdi
	movq	8(%rsp), %rsi
	movl	$3, %edx
	callq	sum
	addq	$24, %rsp
	retq
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
	.cfi_endproc


	.section	".note.GNU-stack","",@progbits
