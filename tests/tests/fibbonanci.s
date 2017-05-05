	.text
	.file	"../tests/tests/fibbonanci.ll"
	.globl	fib
	.align	16, 0x90
	.type	fib,@function
fib:                                    # @fib
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbx
.Ltmp0:
	.cfi_def_cfa_offset 16
	subq	$16, %rsp
.Ltmp1:
	.cfi_def_cfa_offset 32
.Ltmp2:
	.cfi_offset %rbx, -16
	movq	%rdi, 8(%rsp)
	cmpq	$2, %rdi
	jge	.LBB0_1
# BB#2:                                 # %if
	movq	8(%rsp), %rax
	jmp	.LBB0_3
.LBB0_1:                                # %endBlock
	movq	8(%rsp), %rdi
	decq	%rdi
	callq	fib
	movq	%rax, %rbx
	movq	8(%rsp), %rdi
	addq	$-2, %rdi
	callq	fib
	addq	%rbx, %rax
.LBB0_3:                                # %if
	addq	$16, %rsp
	popq	%rbx
	retq
.Lfunc_end0:
	.size	fib, .Lfunc_end0-fib
	.cfi_endproc

	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rax
.Ltmp3:
	.cfi_def_cfa_offset 16
	movl	$12, %edi
	callq	fib
	popq	%rcx
	retq
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
	.cfi_endproc


	.section	".note.GNU-stack","",@progbits
