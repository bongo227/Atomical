	.text
	.file	"../tests/tests/bubblesort.ll"
	.globl	sort
	.align	16, 0x90
	.type	sort,@function
sort:                                   # @sort
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
	subq	$64, %rsp
	movq	%r9, -8(%rbp)
	movq	%r8, -16(%rbp)
	movq	%rcx, -24(%rbp)
	movq	%rdx, -32(%rbp)
	movq	%rsi, -40(%rbp)
	movq	$5, -48(%rbp)
	movq	$0, -56(%rbp)
	movq	-48(%rbp), %rax
	decq	%rax
	testq	%rax, %rax
	jle	.LBB0_5
	.align	16, 0x90
.LBB0_1:                                # %for
                                        # =>This Loop Header: Depth=1
                                        #     Child Loop BB0_2 Depth 2
	movq	%rsp, %rcx
	leaq	-16(%rcx), %rax
	movq	%rax, %rsp
	movq	$0, -16(%rcx)
	movq	-48(%rbp), %rcx
	subq	-56(%rbp), %rcx
	decq	%rcx
	testq	%rcx, %rcx
	jle	.LBB0_4
	.align	16, 0x90
.LBB0_2:                                # %for1
                                        #   Parent Loop BB0_1 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	movq	(%rax), %rcx
	movq	-40(%rbp,%rcx,8), %rdx
	cmpq	-32(%rbp,%rcx,8), %rdx
	jle	.LBB0_3
# BB#6:                                 # %if
                                        #   in Loop: Header=BB0_2 Depth=2
	movq	(%rax), %rcx
	movq	-40(%rbp,%rcx,8), %rcx
	movq	%rsp, %rdx
	leaq	-16(%rdx), %rsp
	movq	%rcx, -16(%rdx)
	movq	(%rax), %rcx
	movq	-32(%rbp,%rcx,8), %rsi
	movq	%rsi, -40(%rbp,%rcx,8)
	movq	(%rax), %rcx
	movq	-16(%rdx), %rdx
	movq	%rdx, -32(%rbp,%rcx,8)
.LBB0_3:                                # %endBlock
                                        #   in Loop: Header=BB0_2 Depth=2
	movq	(%rax), %rcx
	incq	%rcx
	movq	%rcx, (%rax)
	movq	-48(%rbp), %rdx
	subq	-56(%rbp), %rdx
	decq	%rdx
	cmpq	%rdx, %rcx
	jl	.LBB0_2
.LBB0_4:                                # %endfor
                                        #   in Loop: Header=BB0_1 Depth=1
	movq	$5, -48(%rbp)
	movq	-56(%rbp), %rax
	incq	%rax
	movq	%rax, -56(%rbp)
	movq	-48(%rbp), %rcx
	decq	%rcx
	cmpq	%rcx, %rax
	jl	.LBB0_1
.LBB0_5:                                # %endfor30
	movq	-40(%rbp), %r8
	movq	-32(%rbp), %rcx
	movq	-24(%rbp), %rdx
	movq	-16(%rbp), %rsi
	movq	-8(%rbp), %rax
	movq	%rax, 32(%rdi)
	movq	%rsi, 24(%rdi)
	movq	%rdx, 16(%rdi)
	movq	%rcx, 8(%rdi)
	movq	%r8, (%rdi)
	movq	%rdi, %rax
	movq	%rbp, %rsp
	popq	%rbp
	retq
.Lfunc_end0:
	.size	sort, .Lfunc_end0-sort
	.cfi_endproc

	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %entry
	subq	$88, %rsp
.Ltmp3:
	.cfi_def_cfa_offset 96
	movq	$54, 48(%rsp)
	movq	$2, 56(%rsp)
	movq	$42, 64(%rsp)
	movq	$5, 72(%rsp)
	movq	$6, 80(%rsp)
	movq	72(%rsp), %r8
	movq	64(%rsp), %rcx
	movq	48(%rsp), %rsi
	movq	56(%rsp), %rdx
	leaq	8(%rsp), %rdi
	movl	$6, %r9d
	callq	sort
	movq	8(%rsp), %rax
	movq	16(%rsp), %rcx
	movq	24(%rsp), %rdx
	movq	32(%rsp), %rsi
	movq	40(%rsp), %rdi
	movq	%rdi, 80(%rsp)
	movq	%rsi, 72(%rsp)
	movq	%rdx, 64(%rsp)
	movq	%rcx, 56(%rsp)
	movq	%rax, 48(%rsp)
	movq	48(%rsp), %rax
	xorl	%ecx, %ecx
	cmpq	56(%rsp), %rax
	movq	$-1, %rax
	movl	$0, %edx
	cmovlq	%rax, %rdx
	cmpq	64(%rsp), %rdx
	movl	$0, %edx
	cmovlq	%rax, %rdx
	cmpq	72(%rsp), %rdx
	movl	$0, %edx
	cmovlq	%rax, %rdx
	cmpq	80(%rsp), %rdx
	cmovlq	%rax, %rcx
	cmpq	88(%rsp), %rcx
	jge	.LBB1_1
# BB#2:                                 # %if
	movl	$123, %eax
	addq	$88, %rsp
	retq
.LBB1_1:                                # %endBlock
	xorl	%eax, %eax
	addq	$88, %rsp
	retq
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
	.cfi_endproc


	.section	".note.GNU-stack","",@progbits
