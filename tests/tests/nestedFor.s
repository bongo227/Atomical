	.text
	.file	"../tests/tests/nestedFor.ll"
	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp0:
	.cfi_def_cfa_offset 16
.Ltmp1:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp2:
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	$0, -8(%rbp)
	movq	$0, -16(%rbp)
	xorl	%eax, %eax
	testb	%al, %al
	jne	.LBB0_5
# BB#1:
	xorl	%eax, %eax
	.align	16, 0x90
.LBB0_2:                                # %for
                                        # =>This Loop Header: Depth=1
                                        #     Child Loop BB0_3 Depth 2
	movq	%rsp, %rdx
	leaq	-16(%rdx), %rcx
	movq	%rcx, %rsp
	movq	$0, -16(%rdx)
	testb	%al, %al
	jne	.LBB0_4
	.align	16, 0x90
.LBB0_3:                                # %for1
                                        #   Parent Loop BB0_2 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	incq	-8(%rbp)
	movq	(%rcx), %rdx
	incq	%rdx
	movq	%rdx, (%rcx)
	cmpq	$41, %rdx
	jl	.LBB0_3
.LBB0_4:                                # %endfor
                                        #   in Loop: Header=BB0_2 Depth=1
	movq	-16(%rbp), %rcx
	incq	%rcx
	movq	%rcx, -16(%rbp)
	cmpq	$3, %rcx
	jl	.LBB0_2
.LBB0_5:                                # %endfor7
	movq	-8(%rbp), %rax
	movq	%rbp, %rsp
	popq	%rbp
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc


	.section	".note.GNU-stack","",@progbits
