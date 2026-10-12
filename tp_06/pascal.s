	.file	"pascal.c"
	.text
	.p2align 4
	.globl	fact
	.type	fact, @function
fact:
.LFB23:
	.cfi_startproc
	endbr64
	testl	%edi, %edi
	jle	.L4
	leal	1(%rdi), %esi
	andl	$1, %edi
	movl	$1, %eax
	movl	$1, %edx
	je	.L3
	movl	$2, %eax
	cmpl	%esi, %eax
	je	.L1
	.p2align 4,,10
	.p2align 3
.L3:
	imull	%eax, %edx
	leal	1(%rax), %ecx
	addl	$2, %eax
	imull	%ecx, %edx
	cmpl	%esi, %eax
	jne	.L3
.L1:
	movl	%edx, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L4:
	movl	$1, %edx
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE23:
	.size	fact, .-fact
	.p2align 4
	.globl	binom
	.type	binom, @function
binom:
.LFB24:
	.cfi_startproc
	endbr64
	testl	%edi, %edi
	jle	.L18
	testl	%esi, %esi
	jle	.L18
	leal	-1(%rdi), %eax
	movl	$2, %edx
	andl	$1, %eax
	movl	%eax, %ecx
	movl	$1, %eax
	cmpl	$2, %edi
	jl	.L38
	testl	%ecx, %ecx
	je	.L14
	movl	$2, %eax
	movl	$3, %edx
	cmpl	$3, %edi
	jl	.L38
	.p2align 4,,10
	.p2align 3
.L14:
	imull	%edx, %eax
	leal	1(%rdx), %ecx
	addl	$2, %edx
	imull	%ecx, %eax
	cmpl	%edx, %edi
	jge	.L14
.L38:
	leal	-1(%rsi), %edx
	movl	$1, %ecx
	andl	$1, %edx
	movl	%edx, %r8d
	movl	$2, %edx
	cmpl	$2, %esi
	jl	.L39
	testl	%r8d, %r8d
	je	.L15
	movl	$2, %ecx
	movl	$3, %edx
	cmpl	$3, %esi
	jl	.L39
	.p2align 4,,10
	.p2align 3
.L15:
	imull	%edx, %ecx
	leal	1(%rdx), %r8d
	addl	$2, %edx
	imull	%r8d, %ecx
	cmpl	%edx, %esi
	jge	.L15
.L39:
	subl	%esi, %edi
	testl	%edi, %edi
	jle	.L16
	leal	1(%rdi), %r8d
	andl	$1, %edi
	movl	$1, %edx
	movl	$1, %esi
	je	.L17
	movl	$2, %edx
	cmpl	$2, %r8d
	je	.L41
	.p2align 4,,10
	.p2align 3
.L17:
	imull	%edx, %esi
	leal	1(%rdx), %edi
	addl	$2, %edx
	imull	%edi, %esi
	cmpl	%edx, %r8d
	jne	.L17
.L41:
	imull	%esi, %ecx
.L16:
	cltd
	idivl	%ecx
	ret
	.p2align 4,,10
	.p2align 3
.L18:
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE24:
	.size	binom, .-binom
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"1  "
.LC1:
	.string	"%d  "
	.text
	.p2align 4
	.globl	triangle_pascal
	.type	triangle_pascal, @function
triangle_pascal:
.LFB25:
	.cfi_startproc
	endbr64
	pushq	%r15
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	pushq	%r14
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	pushq	%r13
	.cfi_def_cfa_offset 32
	.cfi_offset 13, -32
	pushq	%r12
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	subq	$24, %rsp
	.cfi_def_cfa_offset 80
	movl	%esi, 12(%rsp)
	cmpl	%edi, %esi
	jl	.L42
	testl	%esi, %esi
	movl	%edi, %ebx
	leal	1(%rdi), %edi
	setle	%al
	movl	%edi, 8(%rsp)
	movl	%edi, %r14d
	leaq	.LC1(%rip), %rbp
	testb	%al, %al
	jne	.L42
	.p2align 4,,10
	.p2align 3
.L53:
	xorl	%eax, %eax
	leaq	.LC0(%rip), %rsi
	movl	$2, %edi
	call	__printf_chk@PLT
	testl	%ebx, %ebx
	jle	.L45
	movl	%ebx, %edi
	movl	$2, %r13d
	.p2align 4,,10
	.p2align 3
.L46:
	movl	$1, %ecx
	movl	$1, %eax
	testb	$1, %r14b
	jne	.L47
	movl	$2, %ecx
	cmpl	%r14d, %ecx
	je	.L77
	.p2align 4,,10
	.p2align 3
.L47:
	imull	%ecx, %eax
	leal	1(%rcx), %edx
	addl	$2, %ecx
	imull	%edx, %eax
	cmpl	%r14d, %ecx
	jne	.L47
.L77:
	movl	$1, %ecx
	movl	$1, %esi
	movl	%r13d, %r15d
	testb	$1, %r13b
	jne	.L48
	movl	$2, %ecx
	cmpl	%r13d, %ecx
	je	.L76
	.p2align 4,,10
	.p2align 3
.L48:
	imull	%ecx, %esi
	leal	1(%rcx), %edx
	addl	$2, %ecx
	imull	%edx, %esi
	cmpl	%r13d, %ecx
	jne	.L48
.L76:
	leal	-1(%rdi), %r12d
	testl	%r12d, %r12d
	jle	.L49
	movl	$1, %edx
	movl	$1, %r10d
	testb	$1, %dil
	jne	.L50
	movl	$2, %edx
	cmpl	$2, %edi
	je	.L75
	.p2align 4,,10
	.p2align 3
.L50:
	imull	%edx, %r10d
	leal	1(%rdx), %r11d
	addl	$2, %edx
	imull	%r11d, %r10d
	cmpl	%edx, %edi
	jne	.L50
.L75:
	imull	%r10d, %esi
.L49:
	cltd
	movl	$2, %edi
	addl	$1, %r13d
	idivl	%esi
	movq	%rbp, %rsi
	movl	%eax, %edx
	xorl	%eax, %eax
	call	__printf_chk@PLT
	cmpl	%ebx, %r15d
	jg	.L51
	movl	%r12d, %edi
	jmp	.L46
	.p2align 4,,10
	.p2align 3
.L51:
	movl	$10, %edi
	addl	$1, %r14d
	call	putchar@PLT
	movl	8(%rsp), %eax
	movl	%eax, %ebx
	cmpl	%eax, 12(%rsp)
	jl	.L42
.L54:
	movl	8(%rsp), %eax
	addl	$1, %eax
	movl	%eax, 8(%rsp)
	jmp	.L53
.L42:
	addq	$24, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	popq	%rbx
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
	ret
.L45:
	.cfi_restore_state
	movl	$10, %edi
	addl	$1, %r14d
	call	putchar@PLT
	movl	8(%rsp), %ebx
	jmp	.L54
	.cfi_endproc
.LFE25:
	.size	triangle_pascal, .-triangle_pascal
	.section	.text.startup,"ax",@progbits
	.p2align 4
	.globl	main
	.type	main, @function
main:
.LFB26:
	.cfi_startproc
	endbr64
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movl	$12, %esi
	movl	$1, %edi
	call	triangle_pascal
	xorl	%eax, %eax
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE26:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
