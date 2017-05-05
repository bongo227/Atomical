	.text
	.file	"../tests/tests/gcd.ll"
	.globl	gcd
	.align	16, 0x90
	.type	gcd,@function
gcd:                                    # @gcd
	.cfi_startproc
# BB#0:                                 # %entry
	subq	$24, %rsp
.Ltmp0:
	.cfi_def_cfa_offset 32
	movq	%rdi, 16(%rsp)
	movq	%rsi, 8(%rsp)
	testq	%rsi, %rsi
	je	.LBB0_2
# BB#1:                                 # %endBlock
	movq	8(%rsp), %rdi
	movq	16(%rsp), %rax
	cqto
	idivq	%rdi
	movq	%rdx, %rsi
	callq	gcd
	addq	$24, %rsp
	retq
.LBB0_2:                                # %if
	movq	16(%rsp), %rax
	addq	$24, %rsp
	retq
.Lfunc_end0:
	.size	gcd, .Lfunc_end0-gcd
	.cfi_endproc

	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rax
.Ltmp1:
	.cfi_def_cfa_offset 16
	movl	$1529, %edi             # imm = 0x5F9
	movl	$14039, %esi            # imm = 0x36D7
	callq	gcd
	popq	%rcx
	retq
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
	.cfi_endproc


	.section	".note.GNU-stack","",@progbits
