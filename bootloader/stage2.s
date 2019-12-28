	.text
	.file	"stage2.c"
                                        # Start of file scope inline assembly
	.code16

                                        # End of file scope inline assembly
	.globl	main                    # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
# %bb.0:
	pushl	%ebp
	movl	%esp, %ebp
	xorl	%eax, %eax
	popl	%ebp
	retl
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
                                        # -- End function
	.globl	print                   # -- Begin function print
	.p2align	4, 0x90
	.type	print,@function
print:                                  # @print
# %bb.0:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%eax
	movl	8(%ebp), %eax
	#APP
	movb	$14, %ah
	movb	$0, %bh
	movb	$7, %bl

	#NO_APP
	movl	$0, -4(%ebp)
.LBB1_1:                                # =>This Inner Loop Header: Depth=1
	movl	8(%ebp), %eax
	movl	-4(%ebp), %ecx
	movsbl	(%eax,%ecx), %eax
	cmpl	$0, %eax
	je	.LBB1_3
# %bb.2:                                #   in Loop: Header=BB1_1 Depth=1
	movl	8(%ebp), %eax
	addl	-4(%ebp), %eax
	#APP
	movb	(%eax), %al
	int	$16
	#NO_APP
	movl	-4(%ebp), %eax
	addl	$1, %eax
	movl	%eax, -4(%ebp)
	jmp	.LBB1_1
.LBB1_3:
	addl	$4, %esp
	popl	%ebp
	retl
.Lfunc_end1:
	.size	print, .Lfunc_end1-print
                                        # -- End function

	.ident	"clang version 9.0.0 (tags/RELEASE_900/final)"
	.section	".note.GNU-stack","",@progbits
	.addrsig
