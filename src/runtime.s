	.file	"runtime.c"
	.text
	.p2align 4
	.globl	updateFileName
	.type	updateFileName, @function
updateFileName:
.LFB11:
	.cfi_startproc
	movq	ADRESSES(%rip), %rdx
	movslq	NAME(%rip), %rax
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movq	%rdi, %rbx
	salq	$4, %rax
	addq	8(%rdx), %rax
	movq	(%rax), %rdi
	movq	8(%rax), %rsi
	call	neobject_destroy@PLT
	movq	%rbx, %rdi
	call	neo_str_create@PLT
	movq	%rdx, %rbx
	movq	%rax, %rcx
	movslq	NAME(%rip), %rdx
	movq	ADRESSES(%rip), %rax
	salq	$4, %rdx
	addq	8(%rax), %rdx
	movq	%rcx, (%rdx)
	movq	%rbx, 8(%rdx)
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE11:
	.size	updateFileName, .-updateFileName
	.p2align 4
	.globl	newContext
	.type	newContext, @function
newContext:
.LFB12:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movq	%rdi, %rbx
	call	ptrlist_create@PLT
	movq	%rbx, %rdi
	popq	%rbx
	.cfi_def_cfa_offset 8
	movq	%rax, %rsi
	jmp	ptrlist_append@PLT
	.cfi_endproc
.LFE12:
	.size	newContext, .-newContext
	.p2align 4
	.globl	deleteContext
	.type	deleteContext, @function
deleteContext:
.LFB13:
	.cfi_startproc
	pushq	%r14
	.cfi_def_cfa_offset 16
	.cfi_offset 14, -16
	pushq	%r13
	.cfi_def_cfa_offset 24
	.cfi_offset 13, -24
	movq	%rdi, %r13
	pushq	%r12
	.cfi_def_cfa_offset 32
	.cfi_offset 12, -32
	pushq	%rbp
	.cfi_def_cfa_offset 40
	.cfi_offset 6, -40
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	movq	(%rdi), %r14
	movq	(%r14), %rbx
	testq	%rbx, %rbx
	je	.L14
	cmpq	$0, 8(%r14)
	movl	16(%rbx), %edi
	movq	%r14, %r12
	jne	.L9
	jmp	.L15
	.p2align 4,,10
	.p2align 3
.L10:
	movq	%rbp, %r12
.L9:
	movq	(%rbx), %rsi
	movq	8(%rbx), %rdx
	movl	16(%rbx), %edi
	call	replace_var@PLT
	movq	%rbx, %rdi
	call	free@PLT
	movq	8(%r12), %rbp
	movq	%r12, %rdi
	call	free@PLT
	cmpq	$0, 8(%rbp)
	movq	0(%rbp), %rbx
	jne	.L10
	movq	(%rbx), %rsi
	movq	8(%rbx), %rdx
	movl	16(%rbx), %edi
	call	replace_var@PLT
	movq	%rbx, %rdi
	call	free@PLT
	movq	%rbp, %rdi
	call	free@PLT
.L8:
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 40
	movq	%r14, %rsi
	popq	%rbp
	.cfi_def_cfa_offset 32
	movq	%r13, %rdi
	popq	%r12
	.cfi_def_cfa_offset 24
	movl	$1, %edx
	popq	%r13
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	jmp	ptrlist_remove@PLT
	.p2align 4,,10
	.p2align 3
.L15:
	.cfi_restore_state
	movq	(%rbx), %rsi
	movq	8(%rbx), %rdx
	call	replace_var@PLT
	movq	%rbx, %rdi
	call	free@PLT
.L14:
	movq	%r14, %rdi
	call	free@PLT
	jmp	.L8
	.cfi_endproc
.LFE13:
	.size	deleteContext, .-deleteContext
	.p2align 4
	.globl	neoIsTrue
	.type	neoIsTrue, @function
neoIsTrue:
.LFB15:
	.cfi_startproc
	movl	CODE_ERROR(%rip), %eax
	testl	%eax, %eax
	jne	.L17
	cmpb	$8, %sil
	je	.L25
	cmpb	$4, %sil
	je	.L26
	movl	$20, CODE_ERROR(%rip)
.L17:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L25:
	jmp	neo_to_bool@PLT
	.p2align 4,,10
	.p2align 3
.L26:
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	call	neo_to_integer@PLT
	testq	%rax, %rax
	setne	%al
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE15:
	.size	neoIsTrue, .-neoIsTrue
	.p2align 4
	.globl	local
	.type	local, @function
local:
.LFB16:
	.cfi_startproc
	pushq	%r12
	.cfi_def_cfa_offset 16
	.cfi_offset 12, -16
	movq	%rsi, %r12
	pushq	%rbp
	.cfi_def_cfa_offset 24
	.cfi_offset 6, -24
	movl	%edi, %ebp
	movl	$24, %edi
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	call	malloc@PLT
	movl	%ebp, %edi
	movq	%rax, %rbx
	call	get_var_value@PLT
	movl	%ebp, 16(%rbx)
	movl	%ebp, %edi
	movq	%rdx, 8(%rbx)
	movq	%rax, (%rbx)
	call	get_absolute_address@PLT
	movq	%rax, %rdi
	call	var_reset@PLT
	movq	(%r12), %rdi
	movq	%rbx, %rsi
	popq	%rbx
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r12
	.cfi_def_cfa_offset 8
	jmp	ptrlist_append@PLT
	.cfi_endproc
.LFE16:
	.size	local, .-local
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	".ne"
	.text
	.p2align 4
	.globl	exec_aux
	.type	exec_aux, @function
exec_aux:
.LFB28:
	.cfi_startproc
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
	subq	$56, %rsp
	.cfi_def_cfa_offset 112
	movl	32(%rdi), %esi
	testl	%esi, %esi
	jle	.L85
	movq	%rdi, %rbp
	xorl	%ebx, %ebx
	leaq	.L34(%rip), %r13
	.p2align 4,,10
	.p2align 3
.L30:
	movq	24(%rbp), %rax
	leaq	0(,%rbx,8), %r12
	movq	(%rax,%rbx,8), %rdi
	movl	4(%rdi), %eax
	movl	%eax, LINENUMBER(%rip)
	movzbl	(%rdi), %eax
	subl	$16, %eax
	cmpb	$41, %al
	ja	.L32
	movzbl	%al, %eax
	movslq	0(%r13,%rax,4), %rax
	addq	%r13, %rax
	jmp	*%rax
	.section	.rodata
	.align 4
	.align 4
.L34:
	.long	.L42-.L34
	.long	.L32-.L34
	.long	.L41-.L34
	.long	.L32-.L34
	.long	.L32-.L34
	.long	.L32-.L34
	.long	.L32-.L34
	.long	.L32-.L34
	.long	.L32-.L34
	.long	.L32-.L34
	.long	.L32-.L34
	.long	.L40-.L34
	.long	.L39-.L34
	.long	.L38-.L34
	.long	.L32-.L34
	.long	.L32-.L34
	.long	.L32-.L34
	.long	.L32-.L34
	.long	.L32-.L34
	.long	.L37-.L34
	.long	.L32-.L34
	.long	.L32-.L34
	.long	.L32-.L34
	.long	.L32-.L34
	.long	.L32-.L34
	.long	.L32-.L34
	.long	.L32-.L34
	.long	.L32-.L34
	.long	.L36-.L34
	.long	.L32-.L34
	.long	.L35-.L34
	.long	.L32-.L34
	.long	.L32-.L34
	.long	.L32-.L34
	.long	.L32-.L34
	.long	.L32-.L34
	.long	.L32-.L34
	.long	.L32-.L34
	.long	.L32-.L34
	.long	.L32-.L34
	.long	.L32-.L34
	.long	.L33-.L34
	.text
	.p2align 4,,10
	.p2align 3
.L32:
	leaq	32(%rsp), %r14
	movq	%rdi, %rsi
	movq	%r14, %rdi
	call	eval_prolog
	movq	32(%rsp), %rdi
	movq	40(%rsp), %rsi
	call	neobject_destroy@PLT
.L124:
	movl	CODE_ERROR(%rip), %eax
	testl	%eax, %eax
	jne	.L85
.L92:
	addq	$1, %rbx
	cmpl	%ebx, 32(%rbp)
	jg	.L30
.L85:
	xorl	%r15d, %r15d
.L29:
	addq	$56, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	movl	%r15d, %eax
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
	.p2align 4,,10
	.p2align 3
.L33:
	.cfi_restore_state
	movq	24(%rdi), %rax
	movq	(%rax), %rax
	movq	24(%rax), %rdx
	movq	(%rdx), %rcx
	cmpb	$10, (%rcx)
	jne	.L128
	cmpl	$2, 32(%rax)
	jne	.L133
	movq	8(%rdx), %rsi
	leaq	32(%rsp), %r14
	movq	%r14, %rdi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %esi
	testl	%esi, %esi
	jne	.L85
	movzbl	40(%rsp), %eax
	cmpb	$-122, %al
	je	.L134
	cmpb	$-125, %al
	jne	.L90
	movq	24(%rbp), %rax
	movq	32(%rsp), %rsi
	movq	40(%rsp), %rdx
	movq	(%rax,%r12), %rdi
	call	execStatementForeachString
	movl	%eax, %r15d
.L89:
	movq	32(%rsp), %rdi
	movq	40(%rsp), %rsi
	call	neobject_destroy@PLT
	movl	%r15d, %eax
	orl	CODE_ERROR(%rip), %eax
	je	.L92
	jmp	.L29
	.p2align 4,,10
	.p2align 3
.L35:
	movl	atomic_counter(%rip), %r12d
	movl	$-1, atomic_counter(%rip)
	call	exec_aux
	addl	%r12d, atomic_counter(%rip)
	xorl	%edx, %edx
	movl	%eax, %r15d
	movl	atomic_counter(%rip), %eax
	testl	%eax, %eax
	cmovs	%edx, %eax
	movl	%eax, atomic_counter(%rip)
	movl	%r15d, %eax
	orl	CODE_ERROR(%rip), %eax
	je	.L92
	jmp	.L29
	.p2align 4,,10
	.p2align 3
.L36:
	movq	24(%rdi), %rax
	movq	(%rax), %rdi
	call	exec_aux
	movl	%eax, %r15d
	testl	%eax, %eax
	jne	.L29
	movq	24(%rbp), %rax
	movl	CODE_ERROR(%rip), %edx
	movq	(%rax,%r12), %r12
	movl	%edx, %eax
	cmpl	$1, %edx
	ja	.L135
.L44:
	testl	%eax, %eax
	je	.L92
	jmp	.L85
	.p2align 4,,10
	.p2align 3
.L37:
	movq	24(%rdi), %rax
	movq	(%rax), %r15
	movl	32(%r15), %edi
	call	nelist_create@PLT
	movl	32(%r15), %ecx
	movq	%rax, %rdx
	testl	%ecx, %ecx
	jle	.L96
	xorl	%r14d, %r14d
	jmp	.L97
	.p2align 4,,10
	.p2align 3
.L137:
	movq	%rdx, 8(%rsp)
	call	eval_prolog
	movl	CODE_ERROR(%rip), %edx
	testl	%edx, %edx
	movq	8(%rsp), %rdx
	jne	.L136
	addq	$1, %r14
	cmpl	%r14d, 32(%r15)
	jle	.L96
.L97:
	movq	24(%r15), %rax
	movq	%r14, %rdi
	salq	$4, %rdi
	addq	8(%rdx), %rdi
	movq	(%rax,%r14,8), %rsi
	testq	%rsi, %rsi
	jne	.L137
	pxor	%xmm0, %xmm0
	addq	$1, %r14
	movups	%xmm0, (%rdi)
	movb	$55, 8(%rdi)
	cmpl	%r14d, 32(%r15)
	jg	.L97
.L96:
	movq	24(%rbp), %rax
	movq	(%rax,%r12), %rax
	movq	48(%rax), %rsi
	movq	40(%rax), %rdi
	call	userFuncDefine@PLT
	movq	%rax, %r15
	movq	24(%rbp), %rax
	movq	%rdx, %r14
	movq	(%rax,%r12), %rax
	movq	8(%rax), %rdi
	call	get_var@PLT
	movq	%r15, %rsi
	movq	%r14, %rdx
	movl	%eax, %edi
	call	replace_var@PLT
	jmp	.L125
	.p2align 4,,10
	.p2align 3
.L38:
	movq	24(%rdi), %rax
	leaq	32(%rsp), %r14
	movq	%r14, %rdi
	movq	(%rax), %rsi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %r9d
	testl	%r9d, %r9d
	jne	.L85
.L131:
	movzbl	40(%rsp), %eax
	movq	32(%rsp), %rdi
	movq	40(%rsp), %rsi
	cmpb	$8, %al
	je	.L138
	cmpb	$4, %al
	je	.L139
	movl	$20, CODE_ERROR(%rip)
	call	neobject_destroy@PLT
.L125:
	movl	CODE_ERROR(%rip), %eax
	testl	%eax, %eax
	je	.L92
	jmp	.L85
	.p2align 4,,10
	.p2align 3
.L39:
	movq	24(%rdi), %rax
	movq	(%rax), %rax
	movq	24(%rax), %rax
	movq	(%rax), %rax
	cmpb	$10, (%rax)
	jne	.L128
	call	execStatementFor
	movl	%eax, %r15d
	orl	CODE_ERROR(%rip), %eax
	je	.L92
	jmp	.L29
	.p2align 4,,10
	.p2align 3
.L40:
	call	execConditionBlock
	movl	%eax, %r15d
	testl	%eax, %eax
	je	.L124
	jmp	.L29
	.p2align 4,,10
	.p2align 3
.L41:
	movl	16(%rdi), %eax
	cmpl	$1, %eax
	je	.L140
	cmpl	$2, %eax
	je	.L141
	cmpl	$3, %eax
	jne	.L125
	movl	32(%rdi), %edx
	testl	%edx, %edx
	je	.L142
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rcx
	cmpq	$0, (%rcx)
	je	.L67
	xorl	%r15d, %r15d
	testl	%edx, %edx
	jg	.L68
	jmp	.L125
	.p2align 4,,10
	.p2align 3
.L69:
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
.L68:
	movq	24(%rdi), %rdx
	movq	120(%rax), %rdi
	movq	(%rdx,%r15,8), %rdx
	movl	56(%rdx), %esi
	call	save_later@PLT
	movq	process_cycle(%rip), %rax
	movl	$24, %edi
	movq	(%rax), %rax
	movq	104(%rax), %rsi
	movq	24(%rbp), %rax
	movq	(%rax,%r12), %rax
	movq	%rsi, 16(%rsp)
	movq	24(%rax), %rax
	movq	(%rax,%r15,8), %rax
	addq	$1, %r15
	movl	56(%rax), %eax
	movl	%eax, 8(%rsp)
	call	malloc@PLT
	movl	8(%rsp), %edi
	movq	%rax, %r14
	call	get_var_value@PLT
	movq	%rax, (%r14)
	movl	8(%rsp), %eax
	movq	%rdx, 8(%r14)
	movl	%eax, 16(%r14)
	movl	%eax, %edi
	call	get_absolute_address@PLT
	movq	%rax, %rdi
	call	var_reset@PLT
	movq	16(%rsp), %rsi
	movq	(%rsi), %rdi
	movq	%r14, %rsi
	call	ptrlist_append@PLT
	movq	24(%rbp), %rax
	movq	(%rax,%r12), %rdi
	cmpl	%r15d, 32(%rdi)
	jg	.L69
	jmp	.L125
	.p2align 4,,10
	.p2align 3
.L42:
	movl	16(%rdi), %r15d
	cmpl	$3, %r15d
	je	.L125
	jmp	.L29
	.p2align 4,,10
	.p2align 3
.L135:
	movl	32(%r12), %edi
	cmpl	$1, %edi
	jle	.L85
	movl	$1, 8(%rsp)
	movq	24(%r12), %rsi
	movl	$8, %r14d
.L54:
	movq	(%rsi,%r14), %rax
	movq	24(%rax), %rax
	movq	(%rax), %rax
	movl	32(%rax), %ecx
	testl	%ecx, %ecx
	jne	.L143
.L46:
	movl	$0, CODE_ERROR(%rip)
	movslq	8(%rsp), %r14
	movq	(%rsi,%r14,8), %rax
	movq	24(%rax), %rax
	movq	8(%rax), %rdi
	call	exec_aux
	movl	%eax, %r15d
	orl	CODE_ERROR(%rip), %eax
	je	.L92
	jmp	.L29
	.p2align 4,,10
	.p2align 3
.L143:
	jle	.L47
	xorl	%r15d, %r15d
	jmp	.L53
	.p2align 4,,10
	.p2align 3
.L50:
	movq	24(%r12), %rsi
	addq	$1, %r15
	movq	(%rsi,%r14), %rax
	movq	24(%rax), %rax
	movq	(%rax), %rax
	cmpl	%r15d, 32(%rax)
	jle	.L144
.L53:
	movq	24(%rax), %rax
	movq	(%rax,%r15,8), %rax
	cmpb	$20, 48(%rax)
	jne	.L145
	movq	40(%rax), %rdi
	movq	48(%rax), %rsi
	call	get_exception_code@PLT
	movslq	CODE_ERROR(%rip), %rdi
	movq	exceptions_err(%rip), %rsi
	movq	%rdi, %rdx
	cmpl	%eax, (%rsi,%rdi,4)
	je	.L49
	testl	%edi, %edi
	jns	.L50
	addl	%edi, %eax
	jne	.L50
.L51:
	movq	$0, EXCEPTION(%rip)
.L122:
	movq	24(%r12), %rsi
	jmp	.L46
	.p2align 4,,10
	.p2align 3
.L144:
	movl	32(%r12), %edi
.L47:
	addl	$1, 8(%rsp)
	movl	8(%rsp), %eax
	addq	$8, %r14
	cmpl	%edi, %eax
	jl	.L54
	movl	%edx, %eax
	jmp	.L44
	.p2align 4,,10
	.p2align 3
.L138:
	movb	$8, %sil
	call	neo_to_bool@PLT
	movl	%eax, %r15d
.L73:
	movq	32(%rsp), %rdi
	movq	40(%rsp), %rsi
	call	neobject_destroy@PLT
	testb	%r15b, %r15b
	je	.L125
	movq	24(%rbp), %rax
	movq	(%rax,%r12), %rax
	movq	24(%rax), %rax
	movq	8(%rax), %rdi
	call	exec_aux
	movl	CODE_ERROR(%rip), %r8d
	movl	%eax, %r15d
	testl	%r8d, %r8d
	jne	.L85
	cmpl	$1, %eax
	ja	.L77
	movq	24(%rbp), %rax
	movq	%r14, %rdi
	movq	(%rax,%r12), %rax
	movq	24(%rax), %rax
	movq	(%rax), %rsi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %edi
	testl	%edi, %edi
	je	.L131
	xorl	%r15d, %r15d
	jmp	.L29
	.p2align 4,,10
	.p2align 3
.L136:
	movq	%rdx, %rdi
	leal	-1(%r14), %esi
	call	nelist_destroy_until@PLT
	xorl	%edx, %edx
	jmp	.L96
	.p2align 4,,10
	.p2align 3
.L139:
	movb	$4, %sil
	call	neo_to_integer@PLT
	testq	%rax, %rax
	setne	%r15b
	jmp	.L73
	.p2align 4,,10
	.p2align 3
.L134:
	movq	24(%rbp), %rax
	movq	32(%rsp), %rsi
	movq	40(%rsp), %rdx
	movq	(%rax,%r12), %rdi
	call	execStatementForeachList
	movl	%eax, %r15d
	jmp	.L89
	.p2align 4,,10
	.p2align 3
.L145:
	movl	$78, CODE_ERROR(%rip)
	xorl	%r15d, %r15d
	jmp	.L29
	.p2align 4,,10
	.p2align 3
.L49:
	testl	%edi, %edi
	jns	.L122
	jmp	.L51
	.p2align 4,,10
	.p2align 3
.L77:
	cmpl	$4, %eax
	je	.L29
	jmp	.L125
	.p2align 4,,10
	.p2align 3
.L141:
	movl	32(%rdi), %r10d
	xorl	%r15d, %r15d
	leaq	32(%rsp), %r14
	testl	%r10d, %r10d
	jg	.L64
	jmp	.L125
	.p2align 4,,10
	.p2align 3
.L146:
	movl	NAME(%rip), %esi
	movq	ADRESSES(%rip), %rdi
	call	nelist_nth@PLT
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	neo_to_string@PLT
	movq	%rax, %rdi
	call	strdup@PLT
	movq	32(%rsp), %rdi
	movq	40(%rsp), %rsi
	movq	%rax, 16(%rsp)
	call	neo_to_string@PLT
	leaq	.LC0(%rip), %rsi
	movq	%rax, %rdi
	call	addStr@PLT
	movq	40(%rsp), %rsi
	movq	32(%rsp), %rdi
	movq	%rax, 8(%rsp)
	call	neo_to_string@PLT
	movq	%rax, %rdi
	call	strdup@PLT
	movq	ADRESSES(%rip), %rdx
	movq	%rax, 24(%rsp)
	movslq	NAME(%rip), %rax
	salq	$4, %rax
	addq	8(%rdx), %rax
	movq	(%rax), %rdi
	movq	8(%rax), %rsi
	call	neobject_destroy@PLT
	movq	24(%rsp), %rdi
	call	neo_str_create@PLT
	movq	%rax, %rsi
	movq	%rdx, %rdi
	movq	ADRESSES(%rip), %rax
	movslq	NAME(%rip), %rdx
	salq	$4, %rdx
	addq	8(%rax), %rdx
	movq	%rsi, (%rdx)
	movq	%rdi, 8(%rdx)
	movq	8(%rsp), %rdi
	call	importFile@PLT
	movq	8(%rsp), %rdi
	call	free@PLT
	movq	ADRESSES(%rip), %rdx
	movslq	NAME(%rip), %rax
	salq	$4, %rax
	addq	8(%rdx), %rax
	movq	(%rax), %rdi
	movq	8(%rax), %rsi
	call	neobject_destroy@PLT
	movq	16(%rsp), %rdi
	call	neo_str_create@PLT
	movq	%rax, %rsi
	movq	%rdx, %rdi
	movq	ADRESSES(%rip), %rax
	movslq	NAME(%rip), %rdx
	salq	$4, %rdx
	addq	8(%rax), %rdx
	movq	%rsi, (%rdx)
	movq	%rdi, 8(%rdx)
	movq	32(%rsp), %rdi
	movq	40(%rsp), %rsi
	call	neobject_destroy@PLT
	movl	CODE_ERROR(%rip), %r11d
	testl	%r11d, %r11d
	jne	.L85
	call	gc_mark_and_sweep@PLT
	movq	24(%rbp), %rax
	addq	$1, %r15
	movq	(%rax,%r12), %rdi
	cmpl	%r15d, 32(%rdi)
	jle	.L125
.L64:
	movq	24(%rdi), %rax
	movq	%r14, %rdi
	movq	(%rax,%r15,8), %rsi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %eax
	testl	%eax, %eax
	je	.L146
	jmp	.L85
.L128:
	movl	$22, CODE_ERROR(%rip)
	jmp	.L85
.L140:
	cmpl	$1, 32(%rdi)
	jle	.L57
	movl	$21, CODE_ERROR(%rip)
	jmp	.L85
.L133:
	movl	$109, CODE_ERROR(%rip)
	jmp	.L85
.L90:
	movq	32(%rsp), %rdi
	movq	40(%rsp), %rsi
	call	neobject_destroy@PLT
	movl	$109, CODE_ERROR(%rip)
	jmp	.L85
.L57:
	movq	RETURN_VALUE(%rip), %rdi
	movq	8+RETURN_VALUE(%rip), %rsi
	call	neo_is_void@PLT
	testb	%al, %al
	jne	.L58
	movl	$99, CODE_ERROR(%rip)
	jmp	.L85
.L58:
	movq	24(%rbp), %rax
	movq	(%rax,%r12), %rax
	movl	32(%rax), %edx
	testl	%edx, %edx
	jne	.L59
	call	neo_none_create@PLT
	movq	%rax, RETURN_VALUE(%rip)
	movq	%rdx, 8+RETURN_VALUE(%rip)
.L60:
	movl	$4, %r15d
	jmp	.L29
.L67:
	movl	$70, CODE_ERROR(%rip)
	jmp	.L85
.L142:
	movl	$69, CODE_ERROR(%rip)
	jmp	.L85
.L59:
	movq	24(%rax), %rax
	leaq	RETURN_VALUE(%rip), %rdi
	movq	(%rax), %rsi
	call	eval_prolog
	jmp	.L60
	.cfi_endproc
.LFE28:
	.size	exec_aux, .-exec_aux
	.section	.rodata.str1.1
.LC1:
	.string	"__local_args__"
	.text
	.p2align 4
	.globl	callUserFunc
	.type	callUserFunc, @function
callUserFunc:
.LFB20:
	.cfi_startproc
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
	movq	%rdx, %r12
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	movq	%rdi, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	movq	%rcx, %rbx
	subq	$40, %rsp
	.cfi_def_cfa_offset 96
	movq	process_cycle(%rip), %rax
	movq	%rsi, 16(%rsp)
	movq	(%rax), %rax
	movq	104(%rax), %r13
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%r13, %rdi
	call	ptrlist_append@PLT
	movq	process_cycle(%rip), %rax
	cmpb	$0, 20(%rbp)
	movq	(%rax), %rax
	movq	120(%rax), %rax
	movq	(%rax), %rax
	movq	%rax, 24(%rsp)
	jne	.L148
	movl	16(%rbp), %eax
.L149:
	xorl	%ebx, %ebx
	testl	%eax, %eax
	jg	.L151
	jmp	.L159
	.p2align 4,,10
	.p2align 3
.L154:
	addq	$1, %rbx
	cmpl	%ebx, 16(%rbp)
	jle	.L159
.L151:
	movq	16(%rsp), %rdi
	movl	%ebx, %esi
	call	nelist_nth@PLT
	movq	%rax, %rdi
	movq	%rdx, %rsi
	call	neo_copy@PLT
	movq	%rax, %r13
	leaq	0(,%rbx,4), %rax
	movq	%rdx, (%rsp)
	movq	%rax, 8(%rsp)
	movq	8(%rbp), %rax
	movl	(%rax,%rbx,4), %esi
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	call	save_later@PLT
	movq	process_cycle(%rip), %rax
	movl	$24, %edi
	movq	(%rax), %rax
	movq	104(%rax), %r12
	movq	8(%rbp), %rax
	movl	(%rax,%rbx,4), %r14d
	call	malloc@PLT
	movl	%r14d, %edi
	movq	%rax, %r15
	call	get_var_value@PLT
	movl	%r14d, 16(%r15)
	movl	%r14d, %edi
	movq	%rdx, 8(%r15)
	movq	%rax, (%r15)
	call	get_absolute_address@PLT
	movq	%rax, %rdi
	call	var_reset@PLT
	movq	(%r12), %rdi
	movq	%r15, %rsi
	call	ptrlist_append@PLT
	movq	8(%rbp), %rax
	movq	(%rsp), %rdx
	movq	%r13, %rsi
	movl	(%rax,%rbx,4), %edi
	call	replace_var@PLT
	movl	CODE_ERROR(%rip), %eax
	testl	%eax, %eax
	je	.L154
	movl	$4, %r12d
	cmpl	$1, %ebx
	jle	.L158
	.p2align 4,,10
	.p2align 3
.L157:
	movq	8(%rbp), %rax
	movl	(%rax,%r12), %edi
	addq	$4, %r12
	call	free_var@PLT
	movq	8(%rsp), %rax
	cmpq	%rax, %r12
	jne	.L157
.L158:
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rdi
	call	deleteContext
	movq	process_cycle(%rip), %rax
	movq	24(%rsp), %rsi
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	call	partialRestore@PLT
	addq	$40, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	xorl	%eax, %eax
	movl	$55, %edx
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
	.p2align 4,,10
	.p2align 3
.L159:
	.cfi_restore_state
	movq	32(%rbp), %rdi
	call	exec_aux
	movl	%eax, %ebx
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rdi
	call	deleteContext
	movq	process_cycle(%rip), %rax
	movq	24(%rsp), %rsi
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	call	partialRestore@PLT
	movl	CODE_ERROR(%rip), %edx
	testl	%edx, %edx
	jne	.L168
	cmpl	$4, %ebx
	jne	.L160
	pxor	%xmm0, %xmm0
	movq	8+RETURN_VALUE(%rip), %rdx
	movq	RETURN_VALUE(%rip), %rax
	movups	%xmm0, RETURN_VALUE(%rip)
	movb	$55, 8+RETURN_VALUE(%rip)
	addq	$40, %rsp
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
	.p2align 4,,10
	.p2align 3
.L148:
	.cfi_restore_state
	movq	%rbx, %rsi
	movq	%r12, %rdi
	call	neo_list_len@PLT
	leaq	.LC1(%rip), %rdi
	movl	%eax, %r15d
	call	get_var@PLT
	movl	%eax, %r13d
	movq	process_cycle(%rip), %rax
	movl	%r13d, %esi
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	call	save_later@PLT
	movq	process_cycle(%rip), %rax
	movl	$24, %edi
	movq	(%rax), %rax
	movq	104(%rax), %rcx
	movq	%rcx, (%rsp)
	call	malloc@PLT
	movl	%r13d, %edi
	movq	%rax, %r14
	call	get_var_value@PLT
	movl	%r13d, 16(%r14)
	movl	%r13d, %edi
	movq	%rdx, 8(%r14)
	movq	%rax, (%r14)
	call	get_absolute_address@PLT
	movq	%rax, %rdi
	call	var_reset@PLT
	movq	(%rsp), %rcx
	movq	%r14, %rsi
	movq	(%rcx), %rdi
	call	ptrlist_append@PLT
	movq	%r12, %rsi
	movq	%rbx, %rdx
	movl	%r13d, %edi
	call	replace_var@PLT
	movl	16(%rbp), %eax
	testl	%r15d, %r15d
	jle	.L149
	movq	16(%rsp), %rcx
	movl	%eax, 16(%rcx)
	jmp	.L149
	.p2align 4,,10
	.p2align 3
.L168:
	addq	$40, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	xorl	%eax, %eax
	movl	$55, %edx
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
	.p2align 4,,10
	.p2align 3
.L160:
	.cfi_restore_state
	addq	$40, %rsp
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
	jmp	neo_none_create@PLT
	.cfi_endproc
.LFE20:
	.size	callUserFunc, .-callUserFunc
	.p2align 4
	.globl	callUserMethod
	.type	callUserMethod, @function
callUserMethod:
.LFB21:
	.cfi_startproc
	pushq	%r15
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	pushq	%r14
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	pushq	%r13
	.cfi_def_cfa_offset 32
	.cfi_offset 13, -32
	movq	%r8, %r13
	pushq	%r12
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	movq	%rdi, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	movq	%rcx, %rbx
	subq	$56, %rsp
	.cfi_def_cfa_offset 112
	movq	process_cycle(%rip), %rax
	movq	%rsi, 40(%rsp)
	movq	(%rax), %rax
	movq	%rdx, 24(%rsp)
	movq	104(%rax), %r12
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%r12, %rdi
	call	ptrlist_append@PLT
	movq	process_cycle(%rip), %rax
	cmpb	$0, 20(%rbp)
	movq	(%rax), %rax
	movq	120(%rax), %rax
	movq	(%rax), %rax
	movq	%rax, 32(%rsp)
	jne	.L170
	movl	16(%rbp), %eax
.L171:
	xorl	%ebx, %ebx
	testl	%eax, %eax
	jg	.L173
	jmp	.L181
	.p2align 4,,10
	.p2align 3
.L176:
	addq	$1, %rbx
	cmpl	%ebx, 16(%rbp)
	jle	.L181
.L173:
	movq	24(%rsp), %rdi
	movl	%ebx, %esi
	call	nelist_nth@PLT
	movq	%rax, %rdi
	movq	%rdx, %rsi
	call	neo_copy@PLT
	movq	%rax, %r13
	leaq	0(,%rbx,4), %rax
	movq	%rdx, 8(%rsp)
	movq	%rax, 16(%rsp)
	movq	8(%rbp), %rax
	movl	(%rax,%rbx,4), %esi
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	call	save_later@PLT
	movq	process_cycle(%rip), %rax
	movl	$24, %edi
	movq	(%rax), %rax
	movq	104(%rax), %r12
	movq	8(%rbp), %rax
	movl	(%rax,%rbx,4), %r14d
	call	malloc@PLT
	movl	%r14d, %edi
	movq	%rax, %r15
	call	get_var_value@PLT
	movl	%r14d, 16(%r15)
	movl	%r14d, %edi
	movq	%rdx, 8(%r15)
	movq	%rax, (%r15)
	call	get_absolute_address@PLT
	movq	%rax, %rdi
	call	var_reset@PLT
	movq	(%r12), %rdi
	movq	%r15, %rsi
	call	ptrlist_append@PLT
	movq	8(%rbp), %rax
	movq	8(%rsp), %rdx
	movq	%r13, %rsi
	movl	(%rax,%rbx,4), %edi
	call	replace_var@PLT
	movl	CODE_ERROR(%rip), %eax
	testl	%eax, %eax
	je	.L176
	movl	$4, %r12d
	cmpl	$1, %ebx
	jle	.L174
	.p2align 4,,10
	.p2align 3
.L179:
	movq	8(%rbp), %rax
	movl	(%rax,%r12), %edi
	addq	$4, %r12
	call	free_var@PLT
	movq	16(%rsp), %rax
	cmpq	%rax, %r12
	jne	.L179
.L174:
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rdi
	call	deleteContext
	movq	process_cycle(%rip), %rax
	movq	32(%rsp), %rsi
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	call	partialRestore@PLT
	addq	$56, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	xorl	%eax, %eax
	movl	$55, %edx
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
	.p2align 4,,10
	.p2align 3
.L181:
	.cfi_restore_state
	movq	32(%rbp), %rdi
	call	exec_aux
	movl	CODE_ERROR(%rip), %edx
	movl	%eax, %ebx
	testl	%edx, %edx
	jne	.L174
	movq	8(%rbp), %rax
	movl	(%rax), %edi
	call	get_var_value@PLT
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	neo_copy@PLT
	movq	%rax, %r12
	movq	process_cycle(%rip), %rax
	movq	%rdx, %r13
	movq	(%rax), %rax
	movq	104(%rax), %rdi
	call	deleteContext
	movq	process_cycle(%rip), %rax
	movq	32(%rsp), %rsi
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	call	partialRestore@PLT
	movq	40(%rsp), %r15
	movq	(%r15), %rdi
	movq	8(%r15), %rsi
	call	neobject_destroy@PLT
	movq	%r12, (%r15)
	movq	%r13, 8(%r15)
	cmpl	$4, %ebx
	jne	.L182
	pxor	%xmm0, %xmm0
	movq	8+RETURN_VALUE(%rip), %rdx
	movq	RETURN_VALUE(%rip), %rax
	movups	%xmm0, RETURN_VALUE(%rip)
	movb	$55, 8+RETURN_VALUE(%rip)
	addq	$56, %rsp
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
	.p2align 4,,10
	.p2align 3
.L170:
	.cfi_restore_state
	movq	%r13, %rsi
	movq	%rbx, %rdi
	call	neo_list_len@PLT
	leaq	.LC1(%rip), %rdi
	movl	%eax, %r15d
	call	get_var@PLT
	movl	%eax, %r12d
	movq	process_cycle(%rip), %rax
	movl	%r12d, %esi
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	call	save_later@PLT
	movq	process_cycle(%rip), %rax
	movl	$24, %edi
	movq	(%rax), %rax
	movq	104(%rax), %rcx
	movq	%rcx, 8(%rsp)
	call	malloc@PLT
	movl	%r12d, %edi
	movq	%rax, %r14
	call	get_var_value@PLT
	movl	%r12d, 16(%r14)
	movl	%r12d, %edi
	movq	%rdx, 8(%r14)
	movq	%rax, (%r14)
	call	get_absolute_address@PLT
	movq	%rax, %rdi
	call	var_reset@PLT
	movq	8(%rsp), %rcx
	movq	%r14, %rsi
	movq	(%rcx), %rdi
	call	ptrlist_append@PLT
	movq	%rbx, %rsi
	movq	%r13, %rdx
	movl	%r12d, %edi
	call	replace_var@PLT
	movl	16(%rbp), %eax
	testl	%r15d, %r15d
	jle	.L171
	movq	24(%rsp), %rcx
	movl	%eax, 16(%rcx)
	jmp	.L171
	.p2align 4,,10
	.p2align 3
.L182:
	addq	$56, %rsp
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
	jmp	neo_none_create@PLT
	.cfi_endproc
.LFE21:
	.size	callUserMethod, .-callUserMethod
	.p2align 4
	.globl	eval_aux
	.type	eval_aux, @function
eval_aux:
.LFB22:
	.cfi_startproc
	pushq	%r15
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	pushq	%r14
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	movq	%rdi, %r14
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
	subq	$88, %rsp
	.cfi_def_cfa_offset 144
	movl	4(%rdi), %eax
	movl	%eax, LINENUMBER(%rip)
	movzbl	(%rdi), %eax
	cmpb	$54, %al
	ja	.L191
	testb	%al, %al
	je	.L192
	cmpb	$54, %al
	ja	.L192
	leaq	.L194(%rip), %rdx
	movslq	(%rdx,%rax,4), %rax
	addq	%rdx, %rax
	jmp	*%rax
	.section	.rodata
	.align 4
	.align 4
.L194:
	.long	.L192-.L194
	.long	.L199-.L194
	.long	.L198-.L194
	.long	.L192-.L194
	.long	.L196-.L194
	.long	.L196-.L194
	.long	.L192-.L194
	.long	.L192-.L194
	.long	.L196-.L194
	.long	.L192-.L194
	.long	.L197-.L194
	.long	.L192-.L194
	.long	.L192-.L194
	.long	.L192-.L194
	.long	.L192-.L194
	.long	.L192-.L194
	.long	.L192-.L194
	.long	.L196-.L194
	.long	.L192-.L194
	.long	.L192-.L194
	.long	.L196-.L194
	.long	.L192-.L194
	.long	.L192-.L194
	.long	.L192-.L194
	.long	.L192-.L194
	.long	.L192-.L194
	.long	.L192-.L194
	.long	.L192-.L194
	.long	.L192-.L194
	.long	.L192-.L194
	.long	.L192-.L194
	.long	.L192-.L194
	.long	.L192-.L194
	.long	.L192-.L194
	.long	.L192-.L194
	.long	.L192-.L194
	.long	.L192-.L194
	.long	.L192-.L194
	.long	.L192-.L194
	.long	.L192-.L194
	.long	.L192-.L194
	.long	.L192-.L194
	.long	.L192-.L194
	.long	.L192-.L194
	.long	.L192-.L194
	.long	.L192-.L194
	.long	.L192-.L194
	.long	.L192-.L194
	.long	.L192-.L194
	.long	.L192-.L194
	.long	.L192-.L194
	.long	.L192-.L194
	.long	.L195-.L194
	.long	.L192-.L194
	.long	.L193-.L194
	.text
	.p2align 4,,10
	.p2align 3
.L191:
	cmpb	$-122, %al
	je	.L200
	jbe	.L415
	cmpb	$-77, %al
	jne	.L192
	movl	32(%rdi), %edi
	xorl	%ebx, %ebx
	call	nelist_create@PLT
	movl	32(%r14), %r8d
	movq	%rax, %rbp
	testl	%r8d, %r8d
	jg	.L318
.L321:
	movl	16(%r14), %edi
	addq	$88, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	movq	%rbp, %rsi
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
	jmp	neo_container_create@PLT
	.p2align 4,,10
	.p2align 3
.L319:
	.cfi_restore_state
	addq	$1, %rbx
	cmpl	%ebx, 32(%r14)
	jle	.L321
.L318:
	movq	24(%r14), %rax
	movq	%rbx, %rdi
	salq	$4, %rdi
	addq	8(%rbp), %rdi
	movq	(%rax,%rbx,8), %rax
	movq	24(%rax), %rax
	movq	(%rax), %rsi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %edi
	testl	%edi, %edi
	je	.L319
	leal	-1(%rbx), %esi
	movq	%rbp, %rdi
	call	nelist_destroy_until@PLT
	xorl	%eax, %eax
	movl	$55, %edx
	jmp	.L395
	.p2align 4,,10
	.p2align 3
.L415:
	cmpb	$-128, %al
	jne	.L416
	movq	40(%rdi), %rdi
	movq	48(%r14), %rsi
	call	neo_is_void@PLT
	testb	%al, %al
	jne	.L417
.L239:
	movzbl	48(%r14), %eax
	cmpb	$-128, %al
	je	.L418
	leal	79(%rax), %edx
	cmpb	$1, %dl
	jbe	.L419
	cmpb	$55, %al
	je	.L420
	movl	$9, CODE_ERROR(%rip)
.L413:
	movq	40(%r14), %rdi
	movq	48(%r14), %rsi
.L409:
	call	neobject_destroy@PLT
	pxor	%xmm0, %xmm0
	movups	%xmm0, 40(%r14)
	movb	$55, 48(%r14)
	jmp	.L410
	.p2align 4,,10
	.p2align 3
.L416:
	cmpb	$-125, %al
	jne	.L192
.L196:
	movq	40(%r14), %rdi
	movq	48(%r14), %rsi
.L412:
	addq	$88, %rsp
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
	jmp	neo_copy@PLT
	.p2align 4,,10
	.p2align 3
.L192:
	.cfi_restore_state
	movl	$19, CODE_ERROR(%rip)
.L410:
	xorl	%eax, %eax
	movl	$55, %edx
.L395:
	addq	$88, %rsp
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
	.p2align 4,,10
	.p2align 3
.L197:
	.cfi_restore_state
	movl	56(%rdi), %edi
	call	get_var_value@PLT
	movq	%rax, %rdi
	movq	%rdx, %rsi
	cmpb	$55, %dl
	jne	.L412
	movl	$5, CODE_ERROR(%rip)
	jmp	.L410
	.p2align 4,,10
	.p2align 3
.L195:
	movq	24(%rdi), %rax
	leaq	64(%rsp), %rdi
	movq	(%rax), %rsi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %esi
	testl	%esi, %esi
	jne	.L410
	cmpb	$-77, 72(%rsp)
	movq	64(%rsp), %rdi
	movq	72(%rsp), %rsi
	jne	.L421
	call	neo_to_container@PLT
	movl	16(%r14), %esi
	movq	%rax, %rbx
	cmpl	$-1, %esi
	je	.L422
.L325:
	movq	%rbx, %rdi
	call	get_container_field@PLT
	movq	%rax, %rdi
	movq	%rdx, %rsi
	call	neo_copy@PLT
.L403:
	movq	%rax, %rbp
	movq	%rdx, %rbx
.L404:
	movq	64(%rsp), %rdi
	movq	72(%rsp), %rsi
	call	neobject_destroy@PLT
	movq	%rbp, %rax
	movq	%rbx, %rdx
	jmp	.L395
	.p2align 4,,10
	.p2align 3
.L193:
	cmpl	$1, 32(%rdi)
	jle	.L326
	movl	$101, CODE_ERROR(%rip)
	xorl	%eax, %eax
	movl	$55, %edx
	jmp	.L395
	.p2align 4,,10
	.p2align 3
.L199:
	movq	24(%rdi), %rax
	movq	(%rax), %rdi
	call	get_address
	movl	CODE_ERROR(%rip), %r10d
	movq	(%rax), %r12
	movq	8(%rax), %rbp
	movzbl	8(%rax), %ebx
	testl	%r10d, %r10d
	jne	.L410
	movq	24(%r14), %rax
	leaq	64(%rsp), %rdi
	movq	8(%rax), %rsi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %r9d
	testl	%r9d, %r9d
	jne	.L410
	cmpb	$-122, %bl
	je	.L311
	cmpb	$-125, %bl
	je	.L311
	movl	$15, CODE_ERROR(%rip)
	xorl	%eax, %eax
	movl	$55, %edx
	jmp	.L395
	.p2align 4,,10
	.p2align 3
.L198:
	movl	32(%rdi), %edi
	cmpl	$1, %edi
	je	.L423
	cmpl	$2, %edi
	je	.L424
.L214:
	call	nelist_create@PLT
	movl	32(%r14), %r10d
	movq	%rax, %rbp
	testl	%r10d, %r10d
	jle	.L236
	xorl	%ebx, %ebx
	jmp	.L237
	.p2align 4,,10
	.p2align 3
.L426:
	call	eval_prolog
	movl	CODE_ERROR(%rip), %r9d
	testl	%r9d, %r9d
	jne	.L425
.L235:
	addq	$1, %rbx
	cmpl	%ebx, 32(%r14)
	jle	.L236
.L237:
	movq	24(%r14), %rax
	movq	%rbx, %rdi
	salq	$4, %rdi
	addq	8(%rbp), %rdi
	movq	(%rax,%rbx,8), %rsi
	testq	%rsi, %rsi
	jne	.L426
	pxor	%xmm0, %xmm0
	movups	%xmm0, (%rdi)
	movb	$55, 8(%rdi)
	jmp	.L235
	.p2align 4,,10
	.p2align 3
.L425:
	movq	%rbp, %rdi
	leal	-1(%rbx), %esi
	xorl	%ebp, %ebp
	call	nelist_destroy_until@PLT
.L236:
	movl	CODE_ERROR(%rip), %r8d
	testl	%r8d, %r8d
	jne	.L410
	addq	$88, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	movq	%rbp, %rdi
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
	jmp	neo_list_convert@PLT
	.p2align 4,,10
	.p2align 3
.L200:
	.cfi_restore_state
	movl	32(%rdi), %edi
	jmp	.L214
	.p2align 4,,10
	.p2align 3
.L417:
	movq	24(%r14), %rax
	leaq	40(%r14), %rdi
	movq	(%rax), %rsi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %edi
	testl	%edi, %edi
	je	.L239
	jmp	.L410
	.p2align 4,,10
	.p2align 3
.L421:
	call	neobject_destroy@PLT
	xorl	%eax, %eax
	movl	$55, %edx
	movl	$80, CODE_ERROR(%rip)
	jmp	.L395
	.p2align 4,,10
	.p2align 3
.L424:
	movslq	16(%r14), %rcx
	movq	OPERANDES(%rip), %rdx
	movl	(%rdx,%rcx,4), %edx
	testb	$1, %dh
	jne	.L427
	cmpl	$37, %ecx
	je	.L428
	cmpl	$35, %ecx
	je	.L429
	movq	24(%r14), %rax
	movq	(%rax), %rsi
	testb	$8, %dl
	jne	.L430
	testb	$16, %dl
	jne	.L431
	andl	$64, %edx
	je	.L226
	movq	%rsi, %rdi
	call	get_address
	movl	CODE_ERROR(%rip), %r12d
	movq	%rax, %rbx
	testl	%r12d, %r12d
	jne	.L410
	movq	24(%r14), %rax
	movq	8(%rax), %rdi
	call	get_address
	movl	CODE_ERROR(%rip), %ebp
	movq	%rax, %rsi
	testl	%ebp, %ebp
	jne	.L410
	movslq	16(%r14), %rdx
	leaq	OPFONC(%rip), %rax
	movq	%rbx, %rdi
	movq	(%rax,%rdx,8), %rax
.L411:
	addq	$88, %rsp
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
	jmp	*%rax
	.p2align 4,,10
	.p2align 3
.L420:
	.cfi_restore_state
	movl	$8, CODE_ERROR(%rip)
	jmp	.L413
	.p2align 4,,10
	.p2align 3
.L326:
	movq	24(%rdi), %rax
	leaq	64(%rsp), %rdi
	movq	(%rax), %rsi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %ecx
	movq	64(%rsp), %rdi
	movq	72(%rsp), %rsi
	testl	%ecx, %ecx
	jne	.L432
	movzbl	72(%rsp), %eax
	movq	%rsi, %rdx
	cmpb	$8, %al
	je	.L433
	cmpb	$4, %al
	je	.L434
	movl	$20, CODE_ERROR(%rip)
.L332:
	call	neobject_destroy@PLT
	movl	atomic_counter(%rip), %edx
	testl	%edx, %edx
	js	.L408
	movl	$0, atomic_counter(%rip)
.L408:
	movq	process_cycle(%rip), %rax
	movl	$55, %edx
	movq	(%rax), %rax
	movq	%r14, 72(%rax)
	movq	$-1, %rax
	jmp	.L395
	.p2align 4,,10
	.p2align 3
.L423:
	movq	24(%r14), %rdx
	movslq	16(%r14), %rax
	movq	(%rdx), %rbx
	cmpl	$39, %eax
	je	.L435
	movq	OPERANDES(%rip), %rdx
	movl	(%rdx,%rax,4), %edx
	testb	$1, %dh
	jne	.L436
	andl	$33, %edx
	je	.L210
	movq	%rbx, %rdi
	call	get_address
	movl	CODE_ERROR(%rip), %edx
	testl	%edx, %edx
	jne	.L410
	movslq	16(%r14), %rcx
	leaq	OPFONC(%rip), %rdx
	movq	%rax, %rdi
	movq	(%rdx,%rcx,8), %rdx
	addq	$88, %rsp
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
	jmp	*%rdx
	.p2align 4,,10
	.p2align 3
.L311:
	.cfi_restore_state
	cmpb	$4, 72(%rsp)
	movq	64(%rsp), %rdi
	movq	72(%rsp), %rsi
	je	.L312
	call	neobject_destroy@PLT
	xorl	%eax, %eax
	movl	$55, %edx
	movl	$16, CODE_ERROR(%rip)
	jmp	.L395
	.p2align 4,,10
	.p2align 3
.L419:
	movq	40(%r14), %rdi
	movq	48(%r14), %rsi
	call	neo_to_userfunc@PLT
	movq	%rax, %rbx
	movq	40(%r14), %rax
	movl	16(%rbx), %r8d
	movzbl	20(%rbx), %r11d
	movq	%rax, 16(%rsp)
	movq	48(%r14), %rax
	movq	%rax, (%rsp)
	movzbl	48(%r14), %eax
	movb	%al, 35(%rsp)
	movq	24(%r14), %rax
	movq	8(%rax), %rax
	movl	32(%rax), %ecx
	cmpl	%r8d, %ecx
	jg	.L251
	testl	%ecx, %ecx
	jle	.L437
.L252:
	movq	24(%rax), %rdi
	movl	$0, 36(%rsp)
	xorl	%eax, %eax
	jmp	.L259
	.p2align 4,,10
	.p2align 3
.L256:
	addq	$1, %rax
	cmpl	%eax, %ecx
	jle	.L253
.L259:
	movq	(%rdi,%rax,8), %rdx
	cmpb	$2, (%rdx)
	jne	.L256
	cmpl	$37, 16(%rdx)
	jne	.L256
	testl	%r8d, %r8d
	jle	.L256
	movq	24(%rdx), %rdx
	movq	8(%rbx), %rsi
	movq	(%rdx), %rdx
	movl	56(%rdx), %r10d
	xorl	%edx, %edx
	jmp	.L257
	.p2align 4,,10
	.p2align 3
.L258:
	addl	$1, %edx
	addq	$4, %rsi
	cmpl	%edx, %r8d
	je	.L256
.L257:
	cmpl	%r10d, (%rsi)
	jne	.L258
	movl	%r8d, %esi
	subl	24(%rbx), %esi
	cmpl	%edx, %esi
	jg	.L256
	cmpl	%edx, %r8d
	setg	%dl
	cmpb	$1, %dl
	sbbl	$-1, 36(%rsp)
	jmp	.L256
	.p2align 4,,10
	.p2align 3
.L418:
	movdqu	40(%r14), %xmm3
	movq	48(%r14), %rsi
	movq	40(%r14), %rdi
	movaps	%xmm3, (%rsp)
	call	neo_to_function@PLT
	pxor	%xmm0, %xmm0
	movq	%rax, %rbp
	movq	24(%r14), %rax
	movups	%xmm0, 40(%r14)
	movb	$55, 48(%r14)
	movq	8(%rax), %r13
	movl	32(%r13), %edi
	call	nelist_create@PLT
	movl	32(%r13), %esi
	movq	%rax, %r12
	testl	%esi, %esi
	jle	.L244
	xorl	%ebx, %ebx
	jmp	.L245
	.p2align 4,,10
	.p2align 3
.L439:
	call	eval_prolog
	movl	CODE_ERROR(%rip), %ecx
	testl	%ecx, %ecx
	jne	.L438
.L243:
	addq	$1, %rbx
	cmpl	%ebx, 32(%r13)
	jle	.L244
.L245:
	movq	24(%r13), %rax
	movq	%rbx, %rdi
	salq	$4, %rdi
	addq	8(%r12), %rdi
	movq	(%rax,%rbx,8), %rsi
	testq	%rsi, %rsi
	jne	.L439
	pxor	%xmm0, %xmm0
	movups	%xmm0, (%rdi)
	movb	$55, 8(%rdi)
	jmp	.L243
.L438:
	movq	%r12, %rdi
	leal	-1(%rbx), %esi
	xorl	%r12d, %r12d
	call	nelist_destroy_until@PLT
.L244:
	movdqa	(%rsp), %xmm4
	movl	CODE_ERROR(%rip), %edx
	movups	%xmm4, 40(%r14)
	testl	%edx, %edx
	jne	.L413
	movq	%r12, %rsi
	movq	%rbp, %rdi
	call	funcArgsCheck@PLT
	testb	%al, %al
	je	.L440
	movq	48(%r14), %rsi
	movq	40(%r14), %rdi
	movq	%r12, %rdx
	call	functionCall@PLT
	movq	%r12, %rdi
	movq	%rax, %rbp
	movq	%rdx, %rbx
	call	nelist_destroy@PLT
	movq	40(%r14), %rdi
	movq	48(%r14), %rsi
	call	neobject_destroy@PLT
	movl	CODE_ERROR(%rip), %eax
	pxor	%xmm0, %xmm0
	movups	%xmm0, 40(%r14)
	movb	$55, 48(%r14)
	testl	%eax, %eax
	jne	.L441
	movl	4(%r14), %eax
	movq	%rbx, %rdx
	movl	%eax, LINENUMBER(%rip)
	movq	%rbp, %rax
	jmp	.L395
.L437:
	xorl	%eax, %eax
	movl	%eax, 36(%rsp)
.L253:
	testb	%r11b, %r11b
	jne	.L336
.L260:
	movl	%r8d, %edi
	call	nelist_create@PLT
	movslq	16(%rax), %rdx
	movq	%rax, %r13
	testl	%edx, %edx
	jle	.L265
	movq	8(%rax), %rax
	salq	$4, %rdx
	addq	%rax, %rdx
	.p2align 4,,10
	.p2align 3
.L264:
	pxor	%xmm0, %xmm0
	addq	$16, %rax
	movups	%xmm0, -16(%rax)
	movb	$55, -8(%rax)
	cmpq	%rax, %rdx
	jne	.L264
.L265:
	movl	16(%rbx), %eax
	movl	%eax, %ebp
	subl	24(%rbx), %ebp
	movslq	%ebp, %r15
	salq	$4, %r15
	cmpl	%ebp, %eax
	jle	.L263
	.p2align 4,,10
	.p2align 3
.L269:
	testl	%ebp, %ebp
	jne	.L268
	cmpb	$-79, 48(%r14)
	je	.L414
.L268:
	movq	40(%rbx), %rdi
	movl	%ebp, %esi
	addl	$1, %ebp
	call	nelist_nth@PLT
	movq	%rax, %rdi
	movq	%rdx, %rsi
	call	neo_copy@PLT
	movq	8(%r13), %rcx
	movq	%rax, (%rcx,%r15)
	movq	%rdx, 8(%rcx,%r15)
	addq	$16, %r15
	cmpl	%ebp, 16(%rbx)
	jg	.L269
.L263:
	movq	$0, 40(%rsp)
	movq	24(%r14), %rdi
	xorl	%ebp, %ebp
	movq	8(%rdi), %rsi
	movl	32(%rsi), %edx
	testl	%edx, %edx
	jg	.L266
	jmp	.L267
	.p2align 4,,10
	.p2align 3
.L270:
	addq	$1, %rbp
	cmpl	%ebp, %edx
	jle	.L267
.L266:
	movq	24(%rsi), %rax
	leaq	0(,%rbp,8), %r15
	movq	(%rax,%rbp,8), %rax
	cmpb	$2, (%rax)
	jne	.L270
	cmpl	$37, 16(%rax)
	jne	.L270
	movq	24(%rax), %rax
	movq	(%rax), %rax
	cmpb	$10, (%rax)
	jne	.L274
	movl	16(%rbx), %r8d
	testl	%r8d, %r8d
	jle	.L338
	movl	56(%rax), %ecx
	xorl	%r12d, %r12d
	movq	8(%rbx), %rax
	jmp	.L273
	.p2align 4,,10
	.p2align 3
.L275:
	addl	$1, %r12d
	addq	$4, %rax
	cmpl	%r8d, %r12d
	je	.L274
.L273:
	cmpl	%ecx, (%rax)
	jne	.L275
.L272:
	cmpl	%r8d, %r12d
	je	.L274
	movslq	%r12d, %rcx
	subl	24(%rbx), %r8d
	movq	8(%r13), %rax
	salq	$4, %rcx
	cmpl	%r12d, %r8d
	jle	.L277
	movq	(%rax,%rcx), %rdi
	movq	8(%rax,%rcx), %rsi
	movq	%rcx, 24(%rsp)
	call	neo_is_void@PLT
	testb	%al, %al
	je	.L278
	movq	24(%r14), %rax
	movq	24(%rsp), %rcx
	movq	8(%rax), %rsi
.L277:
	pxor	%xmm2, %xmm2
	movups	%xmm2, 40(%r14)
	movb	$55, 48(%r14)
	movq	24(%rsi), %rax
	addq	8(%r13), %rcx
	movq	(%rax,%r15), %rax
	movq	%rcx, %rdi
	movq	24(%rax), %rax
	movq	8(%rax), %rsi
	call	eval_prolog
	movq	(%rsp), %rax
	movb	35(%rsp), %al
	movq	16(%rsp), %rcx
	movq	%rax, (%rsp)
	movq	%rax, 48(%r14)
	movl	CODE_ERROR(%rip), %eax
	movq	%rcx, 40(%r14)
	testl	%eax, %eax
	jne	.L442
	movq	24(%r14), %rdi
	cmpb	$-79, 35(%rsp)
	movq	8(%rdi), %rsi
	jne	.L399
	testl	%r12d, %r12d
	je	.L280
.L399:
	movl	32(%rsi), %edx
	jmp	.L270
.L432:
	call	neobject_destroy@PLT
	xorl	%eax, %eax
	movl	$55, %edx
	jmp	.L395
.L312:
	call	neo_to_integer@PLT
	movq	64(%rsp), %rdi
	movq	72(%rsp), %rsi
	movq	%rax, %r13
	call	neobject_destroy@PLT
	cmpb	$-122, %bl
	je	.L313
	testq	%r13, %r13
	js	.L315
	movb	$-125, %bpl
	movq	%r12, %rdi
	movq	%rbp, %rsi
	call	neo_to_string@PLT
	movq	%rax, %rdi
	call	strlen@PLT
	cmpq	%rax, %r13
	jnb	.L315
	movq	%rbp, %rsi
	movq	%r12, %rdi
	call	neo_to_string@PLT
	movsbl	(%rax,%r13), %edi
	call	charToString@PLT
	movq	%rax, %rdi
	call	neo_str_create@PLT
	movl	%edx, %ecx
	movb	%cl, %dl
	jmp	.L395
.L436:
	leaq	OPFONC(%rip), %rdx
	movq	%rbx, %rdi
	movq	(%rdx,%rax,8), %rax
	addq	$88, %rsp
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
	jmp	*%rax
.L210:
	.cfi_restore_state
	leaq	64(%rsp), %rdi
	movq	%rbx, %rsi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %eax
	testl	%eax, %eax
	jne	.L410
	movslq	16(%r14), %rdx
	leaq	OPFONC(%rip), %rax
	movq	64(%rsp), %rdi
	movq	72(%rsp), %rsi
	call	*(%rax,%rdx,8)
	jmp	.L403
.L251:
	testb	%r11b, %r11b
	je	.L443
	testl	%ecx, %ecx
	jg	.L252
	xorl	%eax, %eax
	movl	%eax, 36(%rsp)
.L336:
	movl	24(%rbx), %eax
	movl	%r8d, %edx
	movl	36(%rsp), %esi
	subl	%eax, %edx
	addl	%ecx, %eax
	subl	%esi, %eax
	cmpl	%edx, %ecx
	cmovg	%eax, %r8d
	jmp	.L260
.L267:
	movl	16(%r13), %eax
	xorl	%r15d, %r15d
	testl	%eax, %eax
	jg	.L282
	jmp	.L444
	.p2align 4,,10
	.p2align 3
.L285:
	addq	$1, %r15
	cmpl	%r15d, %eax
	jle	.L445
.L282:
	movq	%r15, %rax
	movl	%r15d, %ebp
	salq	$4, %rax
	addq	8(%r13), %rax
	movq	(%rax), %rdi
	movq	8(%rax), %rsi
	call	neo_is_void@PLT
	testb	%al, %al
	movl	16(%r13), %eax
	je	.L285
.L284:
	movq	24(%r14), %rdx
	movq	8(%rdx), %r8
	movl	32(%r8), %edi
	cmpl	%eax, %r15d
	jge	.L283
	xorl	%ebp, %ebp
	jmp	.L286
	.p2align 4,,10
	.p2align 3
.L288:
	addq	$1, %rbp
	cmpl	%eax, %r15d
	jge	.L283
.L286:
	cmpl	%ebp, %edi
	jle	.L283
	movq	24(%r8), %rdx
	leaq	0(,%rbp,8), %rcx
	movq	%rcx, 24(%rsp)
	movq	(%rdx,%rbp,8), %rsi
	cmpb	$2, (%rsi)
	jne	.L287
	cmpl	$37, 16(%rsi)
	je	.L288
.L287:
	pxor	%xmm1, %xmm1
	movslq	%r15d, %rdx
	movups	%xmm1, 40(%r14)
	salq	$4, %rdx
	movb	$55, 48(%r14)
	movq	8(%r13), %rdi
	movq	%rdx, %r12
	addq	%rdx, %rdi
	call	eval_prolog
	movq	(%rsp), %rax
	movb	35(%rsp), %al
	movq	16(%rsp), %rcx
	movq	%rax, (%rsp)
	movq	%rax, 48(%r14)
	movl	CODE_ERROR(%rip), %eax
	movq	%rcx, 40(%r14)
	testl	%eax, %eax
	jne	.L446
	cmpb	$-79, 35(%rsp)
	jne	.L290
	testl	%r15d, %r15d
	je	.L447
.L290:
	cmpl	%r15d, 16(%r13)
	jg	.L291
	jmp	.L292
	.p2align 4,,10
	.p2align 3
.L293:
	addl	$1, %r15d
	addq	$16, %r12
	cmpl	%r15d, 16(%r13)
	jle	.L292
.L291:
	movq	8(%r13), %rax
	movq	(%rax,%r12), %rdi
	movq	8(%rax,%r12), %rsi
	call	neo_is_void@PLT
	testb	%al, %al
	je	.L293
	movq	24(%r14), %rdx
	movl	16(%r13), %eax
	movq	8(%rdx), %r8
	movl	32(%r8), %edi
	jmp	.L288
	.p2align 4,,10
	.p2align 3
.L292:
	movq	24(%r14), %rax
	movq	8(%rax), %rax
	movl	32(%rax), %edi
.L283:
	movl	16(%rbx), %eax
	cmpl	%edi, %eax
	jg	.L448
.L295:
	xorl	%ecx, %ecx
	cmpb	$0, 20(%rbx)
	movl	$55, %r8d
	je	.L299
	movl	16(%r13), %ebp
	movl	36(%rsp), %edx
	movl	%ebp, %ecx
	subl	%edx, %ecx
	movl	%eax, %edx
	subl	24(%rbx), %edx
	cmpl	%edx, %ecx
	jg	.L300
	xorl	%edi, %edi
	call	nelist_create@PLT
	movq	%rax, %r15
.L301:
	movq	%r15, %rdi
	call	neo_list_convert@PLT
	movq	%rax, %rcx
	movq	%rdx, %r8
.L299:
	pxor	%xmm0, %xmm0
	cmpb	$-79, 35(%rsp)
	movups	%xmm0, 40(%r14)
	movb	$55, 48(%r14)
	je	.L449
	movq	%rcx, %rdx
	movq	%rbx, %rdi
	movq	%r8, %rcx
	movq	%r13, %rsi
	call	callUserFunc
	movq	%rax, %rbp
	movq	%rdx, %rbx
.L305:
	movq	16(%rsp), %rdi
	movq	(%rsp), %rax
	movb	35(%rsp), %al
	movq	%rdi, 40(%r14)
	movq	%rax, 48(%r14)
	movq	%rax, %rsi
	call	neobject_destroy@PLT
	pxor	%xmm0, %xmm0
	movq	%r13, %rdi
	movups	%xmm0, 40(%r14)
	movb	$55, 48(%r14)
	call	nelist_destroy@PLT
	movl	CODE_ERROR(%rip), %r11d
	xorl	%eax, %eax
	movl	$55, %edx
	testl	%r11d, %r11d
	cmove	%rbp, %rax
	cmove	%rbx, %rdx
	jmp	.L395
.L451:
	movq	40(%rbx), %rdi
	movl	%ebp, %esi
	call	nelist_nth@PLT
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	neo_copy@PLT
	movq	8(%r13), %rcx
	movq	%rax, (%rcx,%r15)
	movq	8(%r13), %rax
	movq	%rdx, 8(%rcx,%r15)
	movq	(%rax,%r15), %rdi
	movq	8(%rax,%r15), %rsi
	call	neo_is_void@PLT
	testb	%al, %al
	jne	.L450
	testq	%rbp, %rbp
	jne	.L296
.L414:
	movl	$110, CODE_ERROR(%rip)
	movq	%r13, %rdi
	call	nelist_destroy@PLT
	jmp	.L413
.L427:
	movq	24(%r14), %rax
	movq	8(%rax), %rsi
	movq	(%rax), %rdi
	leaq	OPFONC(%rip), %rax
	movq	(%rax,%rcx,8), %rax
	jmp	.L411
.L435:
	movl	$64, %edi
	call	malloc@PLT
	movq	%rax, %rbp
	movzbl	(%rbx), %eax
	leaq	40(%rbp), %rdi
	movb	%al, 0(%rbp)
	movq	8(%rbx), %rax
	movq	%rax, 8(%rbp)
	movl	16(%rbx), %eax
	movl	%eax, 16(%rbp)
	movq	24(%rbx), %rax
	movq	32(%rbx), %rdx
	movq	%rax, 24(%rbp)
	movq	(%rax), %rsi
	movq	%rdx, 32(%rbp)
	call	eval_prolog
	movl	CODE_ERROR(%rip), %ecx
	testl	%ecx, %ecx
	jne	.L401
	cmpb	$-78, 48(%rbp)
	je	.L208
	movq	40(%rbp), %rdi
	movq	48(%rbp), %rsi
	movl	$100, CODE_ERROR(%rip)
	call	neobject_destroy@PLT
.L401:
	movq	%rbp, %rdi
	call	free@PLT
	xorl	%eax, %eax
	movl	$55, %edx
	jmp	.L395
.L422:
	movq	8(%r14), %rsi
	movq	%rax, %rdi
	call	get_field_index@PLT
	movq	8(%r14), %rdi
	movl	%eax, 16(%r14)
	call	free@PLT
	movq	$0, 8(%r14)
	movl	16(%r14), %esi
	jmp	.L325
.L447:
	movq	24(%r14), %rax
	movq	24(%rsp), %rcx
	movq	8(%rax), %rax
	movq	24(%rax), %rax
	movq	(%rax,%rcx), %rdi
	call	get_address
	movq	%rax, 40(%rsp)
	jmp	.L290
.L440:
	movl	$14, CODE_ERROR(%rip)
	movq	%r12, %rdi
	call	nelist_destroy@PLT
	jmp	.L413
.L315:
	movl	$18, CODE_ERROR(%rip)
	xorl	%eax, %eax
	movl	$55, %edx
	jmp	.L395
.L445:
	leal	1(%rbp), %r15d
	jmp	.L284
.L428:
	movl	$95, CODE_ERROR(%rip)
	xorl	%eax, %eax
	movl	$55, %edx
	jmp	.L395
.L430:
	movq	%rsi, %rdi
	call	get_address
	movq	%rax, %rbx
	movl	CODE_ERROR(%rip), %eax
	testl	%eax, %eax
	jne	.L410
	movq	24(%r14), %rax
	leaq	64(%rsp), %rdi
	movq	8(%rax), %rsi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %r15d
	testl	%r15d, %r15d
	jne	.L410
	movslq	16(%r14), %rcx
	leaq	OPFONC(%rip), %rax
	movq	64(%rsp), %rsi
	movq	%rbx, %rdi
	movq	72(%rsp), %rdx
	call	*(%rax,%rcx,8)
	jmp	.L403
.L441:
	movq	%rbp, %rdi
	movq	%rbx, %rsi
	call	neobject_destroy@PLT
	xorl	%eax, %eax
	movl	$55, %edx
	jmp	.L395
.L429:
	movl	$92, CODE_ERROR(%rip)
	xorl	%eax, %eax
	movl	$55, %edx
	jmp	.L395
.L448:
	testl	%eax, %eax
	jle	.L295
	xorl	%ebp, %ebp
.L298:
	movq	8(%r13), %rax
	movq	%rbp, %r15
	salq	$4, %r15
	movq	(%rax,%r15), %rdi
	movq	8(%rax,%r15), %rsi
	call	neo_is_void@PLT
	testb	%al, %al
	jne	.L451
.L296:
	movl	16(%rbx), %eax
	addq	$1, %rbp
	cmpl	%ebp, %eax
	jg	.L298
	jmp	.L295
	.p2align 4,,10
	.p2align 3
.L226:
	leaq	48(%rsp), %rdi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %ebx
	testl	%ebx, %ebx
	jne	.L452
	movq	24(%r14), %rax
	leaq	64(%rsp), %rdi
	movq	8(%rax), %rsi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %r11d
	testl	%r11d, %r11d
	jne	.L453
	movslq	16(%r14), %r8
	leaq	OPFONC(%rip), %rax
	movq	48(%rsp), %rdi
	movq	56(%rsp), %rsi
	movq	64(%rsp), %rdx
	movq	72(%rsp), %rcx
	call	*(%rax,%r8,8)
	movq	48(%rsp), %rdi
	movq	56(%rsp), %rsi
	movq	%rax, %rbp
	movq	%rdx, %rbx
	call	neobject_destroy@PLT
	jmp	.L404
.L433:
	movb	$8, %dl
	movq	%rdx, %rsi
	call	neo_to_bool@PLT
.L330:
	movq	64(%rsp), %rdi
	movq	72(%rsp), %rsi
	testb	%al, %al
	je	.L332
	call	neobject_destroy@PLT
	call	neo_none_create@PLT
	jmp	.L395
.L313:
	movq	%r12, %rdi
	movq	%rbp, %rsi
	call	neo_list_len@PLT
	cltq
	cmpq	%r13, %rax
	jle	.L315
	testq	%r13, %r13
	js	.L315
	movb	$-122, %bpl
	movq	%r12, %rdi
	movl	%r13d, %edx
	movq	%rbp, %rsi
	call	neo_list_nth@PLT
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	neo_copy@PLT
	movl	%edx, %ecx
	movb	%cl, %dl
	jmp	.L395
.L431:
	leaq	64(%rsp), %rdi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %r13d
	testl	%r13d, %r13d
	jne	.L410
	movq	24(%r14), %rax
	movq	8(%rax), %rdi
	call	get_address
	cmpl	$0, CODE_ERROR(%rip)
	movq	%rax, %rdx
	jne	.L410
	movslq	16(%r14), %rcx
	leaq	OPFONC(%rip), %rax
	movq	64(%rsp), %rdi
	movq	72(%rsp), %rsi
	call	*(%rax,%rcx,8)
	jmp	.L403
.L443:
	movl	$6, CODE_ERROR(%rip)
	movq	16(%rsp), %rdi
	movq	(%rsp), %rsi
	jmp	.L409
.L208:
	movq	%rbp, %rdi
	xorl	%edx, %edx
	movl	$1, %esi
	call	create_new_process@PLT
	addq	$88, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	popq	%rbx
	.cfi_def_cfa_offset 48
	movl	%eax, %edi
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
	jmp	neo_promise_create@PLT
.L280:
	.cfi_restore_state
	movq	24(%rsi), %rax
	movq	(%rax,%r15), %rax
	movq	24(%rax), %rax
	movq	8(%rax), %rdi
	call	get_address
	movq	24(%r14), %rdi
	movq	%rax, 40(%rsp)
	movq	8(%rdi), %rsi
	jmp	.L399
.L446:
	movq	%r13, %rdi
	call	nelist_destroy@PLT
	movq	40(%r14), %rdi
	movq	48(%r14), %rsi
	call	neobject_destroy@PLT
	pxor	%xmm6, %xmm6
	xorl	%eax, %eax
	movl	$55, %edx
	movups	%xmm6, 40(%r14)
	movb	$55, 48(%r14)
	jmp	.L395
.L434:
	movb	$4, %dl
	movq	%rdx, %rsi
	call	neo_to_integer@PLT
	testq	%rax, %rax
	setne	%al
	jmp	.L330
.L449:
	movq	40(%rsp), %rsi
	movq	%rbx, %rdi
	movq	%r13, %rdx
	call	callUserMethod
	movq	%rax, %rbp
	movq	%rdx, %rbx
	jmp	.L305
.L300:
	subl	%eax, %ebp
	movl	%ebp, %edi
	call	nelist_create@PLT
	movq	%rax, %r15
	testl	%ebp, %ebp
	jle	.L301
	movslq	%ebp, %r12
	xorl	%ebp, %ebp
	.p2align 4,,10
	.p2align 3
.L303:
	movl	16(%rbx), %esi
	movq	%r13, %rdi
	addl	%ebp, %esi
	call	nelist_nth@PLT
	movq	%rdx, %rdi
	movq	%rbp, %rdx
	movq	%rax, %rsi
	addq	$1, %rbp
	salq	$4, %rdx
	addq	8(%r15), %rdx
	movq	%rsi, (%rdx)
	movq	%rdi, 8(%rdx)
	cmpq	%r12, %rbp
	jne	.L303
	jmp	.L301
.L338:
	xorl	%r12d, %r12d
	jmp	.L272
.L274:
	movq	%r13, %rdi
	call	nelist_destroy@PLT
	movq	40(%r14), %rdi
	movq	48(%r14), %rsi
	call	neobject_destroy@PLT
	pxor	%xmm0, %xmm0
	xorl	%eax, %eax
	movl	$55, %edx
	movups	%xmm0, 40(%r14)
	movl	$93, CODE_ERROR(%rip)
	movb	$55, 48(%r14)
	jmp	.L395
.L452:
	movq	48(%rsp), %rdi
	movq	56(%rsp), %rsi
	call	neobject_destroy@PLT
	xorl	%eax, %eax
	movl	$55, %edx
	jmp	.L395
.L453:
	movq	48(%rsp), %rdi
	movq	56(%rsp), %rsi
	call	neobject_destroy@PLT
	movq	64(%rsp), %rdi
	movq	72(%rsp), %rsi
	call	neobject_destroy@PLT
	xorl	%eax, %eax
	movl	$55, %edx
	jmp	.L395
.L444:
	movq	8(%rdi), %rax
	movl	32(%rax), %edi
	jmp	.L283
.L442:
	movq	%r13, %rdi
	call	nelist_destroy@PLT
	movq	40(%r14), %rdi
	movq	48(%r14), %rsi
	call	neobject_destroy@PLT
	pxor	%xmm5, %xmm5
	xorl	%eax, %eax
	movl	$55, %edx
	movups	%xmm5, 40(%r14)
	movb	$55, 48(%r14)
	jmp	.L395
.L278:
	movq	%r13, %rdi
	call	nelist_destroy@PLT
	movq	40(%r14), %rdi
	movq	48(%r14), %rsi
	call	neobject_destroy@PLT
	pxor	%xmm0, %xmm0
	xorl	%eax, %eax
	movl	$55, %edx
	movups	%xmm0, 40(%r14)
	movl	$94, CODE_ERROR(%rip)
	movb	$55, 48(%r14)
	jmp	.L395
.L450:
	movq	40(%r14), %rdi
	movq	48(%r14), %rsi
	movl	$7, CODE_ERROR(%rip)
	call	neobject_destroy@PLT
	pxor	%xmm0, %xmm0
	movq	%r13, %rdi
	movups	%xmm0, 40(%r14)
	movb	$55, 48(%r14)
	call	nelist_destroy@PLT
	xorl	%eax, %eax
	movl	$55, %edx
	jmp	.L395
	.cfi_endproc
.LFE22:
	.size	eval_aux, .-eval_aux
	.p2align 4
	.globl	eval_prolog
	.type	eval_prolog, @function
eval_prolog:
.LFB18:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rdi, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	movq	%rsi, %rbx
	subq	$8, %rsp
	.cfi_def_cfa_offset 32
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rdi
	testq	%rsi, %rsi
	je	.L463
.L455:
	movq	(%rax), %rdi
	movl	atomic_counter(%rip), %edx
	movq	%rbx, 72(%rdi)
	movq	%rbp, 80(%rdi)
	testl	%edx, %edx
	jne	.L457
	movl	ATOMIC_TIME(%rip), %edx
	cmpq	8(%rax), %rax
	je	.L457
	movl	%edx, atomic_counter(%rip)
	call	unloadCurrentProcess@PLT
	movq	process_cycle(%rip), %rdi
	call	loadNextLivingProcess@PLT
	movq	16(%rax), %rdx
	movq	(%rax), %rdi
	movq	%rax, process_cycle(%rip)
	movq	(%rdx), %rsi
	call	switch_registers@PLT
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	80(%rax), %rbp
	movq	72(%rax), %rbx
.L462:
	movl	atomic_counter(%rip), %edx
.L457:
	subl	$1, %edx
	movq	%rbx, %rdi
	movl	%edx, atomic_counter(%rip)
	call	eval_aux
	movq	%rdx, 8(%rbp)
	cmpb	$55, 8(%rbp)
	movq	%rax, 0(%rbp)
	je	.L464
.L458:
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	movq	%rbp, %rax
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L463:
	.cfi_restore_state
	movq	16(%rax), %rax
	movl	ATOMIC_TIME(%rip), %edx
	movq	(%rax), %rsi
	movl	%edx, atomic_counter(%rip)
	call	switch_registers@PLT
	movq	process_cycle(%rip), %rdi
	movq	(%rdi), %rax
	movq	80(%rax), %rbp
	movq	72(%rax), %rbx
	call	processCycle_clean@PLT
	movl	CODE_ERROR(%rip), %eax
	testl	%eax, %eax
	je	.L462
	pxor	%xmm0, %xmm0
	movups	%xmm0, 0(%rbp)
	movb	$55, 8(%rbp)
	jmp	.L458
	.p2align 4,,10
	.p2align 3
.L464:
	cmpq	$-1, 0(%rbp)
	movq	process_cycle(%rip), %rax
	jne	.L458
	jmp	.L455
	.cfi_endproc
.LFE18:
	.size	eval_prolog, .-eval_prolog
	.p2align 4
	.globl	isTrue
	.type	isTrue, @function
isTrue:
.LFB14:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movq	%rdi, %rsi
	subq	$16, %rsp
	.cfi_def_cfa_offset 32
	movq	%rsp, %rdi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %eax
	movq	(%rsp), %rdi
	movq	8(%rsp), %rsi
	testl	%eax, %eax
	jne	.L466
	movzbl	8(%rsp), %eax
	movq	%rsi, %rdx
	cmpb	$8, %al
	je	.L471
	cmpb	$4, %al
	je	.L472
	movl	$20, CODE_ERROR(%rip)
.L466:
	xorl	%ebx, %ebx
.L468:
	call	neobject_destroy@PLT
	addq	$16, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 16
	movl	%ebx, %eax
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L471:
	.cfi_restore_state
	movb	$8, %dl
	movq	%rdx, %rsi
	call	neo_to_bool@PLT
	movq	(%rsp), %rdi
	movq	8(%rsp), %rsi
	movl	%eax, %ebx
	jmp	.L468
	.p2align 4,,10
	.p2align 3
.L472:
	movb	$4, %dl
	movq	%rdx, %rsi
	call	neo_to_integer@PLT
	movq	(%rsp), %rdi
	movq	8(%rsp), %rsi
	testq	%rax, %rax
	setne	%bl
	jmp	.L468
	.cfi_endproc
.LFE14:
	.size	isTrue, .-isTrue
	.section	.rodata.str1.1
.LC2:
	.string	"TERMIN\303\211!"
	.text
	.p2align 4
	.globl	launch_process
	.type	launch_process, @function
launch_process:
.LFB17:
	.cfi_startproc
	subq	$24, %rsp
	.cfi_def_cfa_offset 32
	movq	process_cycle(%rip), %rdi
	call	processCycle_clean@PLT
	movq	process_cycle(%rip), %rax
	movq	%rsp, %rdi
	movq	(%rax), %rax
	movb	$1, 64(%rax)
	movq	72(%rax), %rsi
	call	eval_prolog
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rdi
	call	process_preRemove@PLT
	movq	process_cycle(%rip), %rdi
	movq	PROMISES_CNT(%rip), %rdx
	movq	(%rdi), %rax
	movslq	112(%rax), %rax
	movq	(%rdx,%rax,8), %rdx
	movl	(%rdx), %edx
	testl	%edx, %edx
	je	.L474
	movq	PROMISES(%rip), %rcx
	movdqa	(%rsp), %xmm0
	movq	%rax, %rdx
	salq	$4, %rdx
	addq	8(%rcx), %rdx
	movups	%xmm0, (%rdx)
.L475:
	movq	PROCESS_FINISH(%rip), %rdx
	movl	$1, (%rdx,%rax,4)
	call	processCycle_isActive@PLT
	testb	%al, %al
	jne	.L483
.L476:
	call	reset_stack_and_registers@PLT
	movq	process_cycle(%rip), %rdi
	jmp	.L477
	.p2align 4,,10
	.p2align 3
.L478:
	movq	process_cycle(%rip), %rdi
	call	processCycle_remove@PLT
	movq	%rax, process_cycle(%rip)
	movq	%rax, %rdi
.L477:
	call	processCycle_isEmpty@PLT
	testb	%al, %al
	je	.L478
	leaq	.LC2(%rip), %rdi
	call	puts@PLT
	addq	$24, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L474:
	.cfi_restore_state
	movq	(%rsp), %rdi
	movq	8(%rsp), %rsi
	call	neobject_destroy@PLT
	movq	process_cycle(%rip), %rdi
	movq	(%rdi), %rax
	movslq	112(%rax), %rax
	jmp	.L475
	.p2align 4,,10
	.p2align 3
.L483:
	movq	process_cycle(%rip), %rdi
	call	loadNextLivingProcess@PLT
	xorl	%esi, %esi
	xorl	%edi, %edi
	movq	%rax, process_cycle(%rip)
	call	eval_prolog
	jmp	.L476
	.cfi_endproc
.LFE17:
	.size	launch_process, .-launch_process
	.p2align 4
	.globl	treeToList
	.type	treeToList, @function
treeToList:
.LFB19:
	.cfi_startproc
	pushq	%r12
	.cfi_def_cfa_offset 16
	.cfi_offset 12, -16
	pushq	%rbp
	.cfi_def_cfa_offset 24
	.cfi_offset 6, -24
	movq	%rdi, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	movl	32(%rdi), %edi
	call	nelist_create@PLT
	movl	32(%rbp), %edx
	movq	%rax, %r12
	testl	%edx, %edx
	jle	.L484
	xorl	%ebx, %ebx
	jmp	.L489
	.p2align 4,,10
	.p2align 3
.L493:
	call	eval_prolog
	movl	CODE_ERROR(%rip), %eax
	testl	%eax, %eax
	jne	.L492
	addq	$1, %rbx
	cmpl	%ebx, 32(%rbp)
	jle	.L484
.L489:
	movq	24(%rbp), %rax
	movq	%rbx, %rdi
	salq	$4, %rdi
	addq	8(%r12), %rdi
	movq	(%rax,%rbx,8), %rsi
	testq	%rsi, %rsi
	jne	.L493
	pxor	%xmm0, %xmm0
	addq	$1, %rbx
	movups	%xmm0, (%rdi)
	movb	$55, 8(%rdi)
	cmpl	%ebx, 32(%rbp)
	jg	.L489
.L484:
	movq	%r12, %rax
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r12
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L492:
	.cfi_restore_state
	leal	-1(%rbx), %esi
	movq	%r12, %rdi
	xorl	%r12d, %r12d
	call	nelist_destroy_until@PLT
	movq	%r12, %rax
	popq	%rbx
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r12
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE19:
	.size	treeToList, .-treeToList
	.p2align 4
	.globl	get_address
	.type	get_address, @function
get_address:
.LFB23:
	.cfi_startproc
	pushq	%r13
	.cfi_def_cfa_offset 16
	.cfi_offset 13, -16
	pushq	%r12
	.cfi_def_cfa_offset 24
	.cfi_offset 12, -24
	pushq	%rbp
	.cfi_def_cfa_offset 32
	.cfi_offset 6, -32
	pushq	%rbx
	.cfi_def_cfa_offset 40
	.cfi_offset 3, -40
	movq	%rdi, %rbx
	subq	$40, %rsp
	.cfi_def_cfa_offset 80
	movzbl	(%rdi), %eax
	cmpb	$10, %al
	je	.L522
	cmpb	$1, %al
	je	.L523
	cmpb	$52, %al
	je	.L524
	movl	$89, CODE_ERROR(%rip)
.L521:
	xorl	%eax, %eax
.L494:
	addq	$40, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 40
	popq	%rbx
	.cfi_def_cfa_offset 32
	popq	%rbp
	.cfi_def_cfa_offset 24
	popq	%r12
	.cfi_def_cfa_offset 16
	popq	%r13
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L524:
	.cfi_restore_state
	movq	24(%rdi), %rax
	leaq	16(%rsp), %rdi
	movq	(%rax), %rsi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %eax
	testl	%eax, %eax
	jne	.L521
	cmpb	$-77, 24(%rsp)
	movq	16(%rsp), %rdi
	movq	24(%rsp), %rsi
	je	.L510
	call	neobject_destroy@PLT
	movl	$80, CODE_ERROR(%rip)
	jmp	.L521
	.p2align 4,,10
	.p2align 3
.L523:
	movq	24(%rdi), %rax
	movq	(%rax), %rdi
	call	get_address
	movl	CODE_ERROR(%rip), %ecx
	movq	(%rax), %r13
	movq	8(%rax), %r12
	movzbl	8(%rax), %ebp
	testl	%ecx, %ecx
	jne	.L521
	movq	24(%rbx), %rax
	leaq	16(%rsp), %rdi
	movq	8(%rax), %rsi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %edx
	testl	%edx, %edx
	jne	.L521
	cmpb	$-122, %bpl
	je	.L500
	cmpb	$-125, %bpl
	je	.L500
	movl	$15, CODE_ERROR(%rip)
	jmp	.L521
	.p2align 4,,10
	.p2align 3
.L522:
	movl	56(%rdi), %edi
	addq	$40, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 40
	popq	%rbx
	.cfi_def_cfa_offset 32
	popq	%rbp
	.cfi_def_cfa_offset 24
	popq	%r12
	.cfi_def_cfa_offset 16
	popq	%r13
	.cfi_def_cfa_offset 8
	jmp	get_absolute_address@PLT
	.p2align 4,,10
	.p2align 3
.L510:
	.cfi_restore_state
	call	neo_to_container@PLT
	movl	16(%rbx), %esi
	movq	%rax, %rbp
	cmpl	$-1, %esi
	je	.L525
.L512:
	movq	%rbp, %rdi
	call	get_container_field_addr@PLT
	movq	16(%rsp), %rdi
	movq	24(%rsp), %rsi
	movq	%rax, 8(%rsp)
	call	neobject_destroy@PLT
	movq	8(%rsp), %rax
	jmp	.L494
	.p2align 4,,10
	.p2align 3
.L500:
	cmpb	$4, 24(%rsp)
	movq	16(%rsp), %rdi
	movq	24(%rsp), %rsi
	je	.L501
	call	neobject_destroy@PLT
	movl	$16, CODE_ERROR(%rip)
	jmp	.L521
	.p2align 4,,10
	.p2align 3
.L525:
	movq	8(%rbx), %rsi
	movq	%rax, %rdi
	call	get_field_index@PLT
	movq	8(%rbx), %rdi
	movl	%eax, 16(%rbx)
	call	free@PLT
	movq	$0, 8(%rbx)
	movl	16(%rbx), %esi
	jmp	.L512
	.p2align 4,,10
	.p2align 3
.L501:
	call	neo_to_integer@PLT
	movq	16(%rsp), %rdi
	movq	24(%rsp), %rsi
	movq	%rax, %rbx
	call	neobject_destroy@PLT
	cmpb	$-122, %bpl
	je	.L502
	testq	%rbx, %rbx
	js	.L504
	movb	$-125, %r12b
	movq	%r13, %rdi
	movq	%r12, %rsi
	call	neo_to_string@PLT
	movq	%rax, %rdi
	call	strlen@PLT
	cmpq	%rax, %rbx
	jnb	.L504
	movl	$105, CODE_ERROR(%rip)
	jmp	.L521
.L504:
	movl	$18, CODE_ERROR(%rip)
	jmp	.L521
.L502:
	movq	%r13, %rdi
	movq	%r12, %rsi
	call	neo_list_len@PLT
	testq	%rbx, %rbx
	js	.L504
	cltq
	cmpq	%rbx, %rax
	jle	.L504
	movb	$-122, %r12b
	movq	%r13, %rdi
	movq	%r12, %rsi
	call	neo_to_list@PLT
	movl	%ebx, %esi
	movq	%rax, %rdi
	call	nelist_nth_addr@PLT
	jmp	.L494
	.cfi_endproc
.LFE23:
	.size	get_address, .-get_address
	.p2align 4
	.globl	execConditionBlock
	.type	execConditionBlock, @function
execConditionBlock:
.LFB24:
	.cfi_startproc
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
	movq	%rdi, %r12
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	subq	$40, %rsp
	.cfi_def_cfa_offset 96
	movq	process_cycle(%rip), %rax
	movl	32(%r12), %r14d
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	movq	(%rdi), %rax
	movq	%rax, 8(%rsp)
	testl	%r14d, %r14d
	jle	.L527
	xorl	%ebx, %ebx
	leaq	16(%rsp), %r14
.L555:
	movq	24(%r12), %rax
	movslq	%ebx, %r15
	movq	%r14, %rdi
	leaq	0(,%r15,8), %rbp
	movq	(%rax,%r15,8), %rax
	movq	24(%rax), %rax
	movq	(%rax), %rsi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %r13d
	testl	%r13d, %r13d
	jne	.L529
	movzbl	24(%rsp), %eax
	movq	16(%rsp), %rdi
	movq	24(%rsp), %rsi
	cmpb	$8, %al
	je	.L565
	cmpb	$4, %al
	je	.L566
	movl	$20, CODE_ERROR(%rip)
	call	neobject_destroy@PLT
	movl	CODE_ERROR(%rip), %r11d
	testl	%r11d, %r11d
	jne	.L529
.L534:
	movl	32(%r12), %edx
	addl	$1, %ebx
	cmpl	%edx, %ebx
	jge	.L541
	addq	$8, %rbp
	xorl	%r15d, %r15d
	jmp	.L542
	.p2align 4,,10
	.p2align 3
.L548:
	movl	32(%r12), %edx
	addl	$1, %ebx
	addq	$8, %rbp
	cmpl	%ebx, %edx
	jle	.L540
.L542:
	movq	24(%r12), %rcx
	movq	(%rcx,%rbp), %rax
	cmpb	$31, (%rax)
	jne	.L567
	movq	24(%rax), %rax
	movq	%r14, %rdi
	movq	(%rax), %rsi
	call	eval_prolog
	movq	24(%rsp), %rsi
	movq	16(%rsp), %rdi
	movl	CODE_ERROR(%rip), %r8d
	movzbl	24(%rsp), %eax
	movq	%rsi, %rdx
	testl	%r8d, %r8d
	jne	.L543
	cmpb	$8, %al
	je	.L568
	cmpb	$4, %al
	je	.L569
	movl	$20, CODE_ERROR(%rip)
.L543:
	call	neobject_destroy@PLT
	movl	CODE_ERROR(%rip), %eax
	testl	%eax, %eax
	je	.L548
.L529:
	xorl	%r15d, %r15d
.L526:
	addq	$40, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	movl	%r15d, %eax
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
	.p2align 4,,10
	.p2align 3
.L568:
	.cfi_restore_state
	movb	$8, %dl
	movq	%rdx, %rsi
	call	neo_to_bool@PLT
.L545:
	movq	24(%rsp), %rsi
	movq	16(%rsp), %rdi
	movb	%al, 7(%rsp)
	call	neobject_destroy@PLT
	movl	CODE_ERROR(%rip), %esi
	testl	%esi, %esi
	jne	.L529
	movl	%r15d, %r13d
	movzbl	7(%rsp), %eax
	xorl	$1, %r13d
	andb	%al, %r13b
	je	.L548
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	104(%rax), %r15
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%r15, %rdi
	call	ptrlist_append@PLT
	movq	24(%r12), %rax
	movq	(%rax,%rbp), %rax
	movq	24(%rax), %rax
	movq	8(%rax), %rdi
	call	exec_aux
	movl	%eax, %r15d
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rdi
	call	deleteContext
	movl	CODE_ERROR(%rip), %ecx
	testl	%ecx, %ecx
	jne	.L529
	testl	%r15d, %r15d
	jne	.L526
	movl	32(%r12), %edx
	addl	$1, %ebx
	movl	%r13d, %r15d
	addq	$8, %rbp
	cmpl	%ebx, %edx
	jg	.L542
	.p2align 4,,10
	.p2align 3
.L540:
	cmpl	%edx, %ebx
	jl	.L555
	.p2align 4,,10
	.p2align 3
.L541:
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	120(%rax), %rdi
.L527:
	movq	8(%rsp), %rsi
	xorl	%r15d, %r15d
	call	partialRestore@PLT
	jmp	.L526
	.p2align 4,,10
	.p2align 3
.L569:
	movb	$4, %dl
	movq	%rdx, %rsi
	call	neo_to_integer@PLT
	testq	%rax, %rax
	setne	%al
	jmp	.L545
	.p2align 4,,10
	.p2align 3
.L567:
	cmpl	%ebx, %edx
	jle	.L540
	movslq	%ebx, %rbp
	movq	(%rcx,%rbp,8), %rax
	cmpb	$32, (%rax)
	jne	.L540
	testb	%r15b, %r15b
	je	.L553
.L554:
	addl	$1, %ebx
	cmpl	%edx, %ebx
	jl	.L555
	jmp	.L541
	.p2align 4,,10
	.p2align 3
.L565:
	movb	$8, %sil
	call	neo_to_bool@PLT
	movl	%eax, %r13d
.L531:
	movq	16(%rsp), %rdi
	movq	24(%rsp), %rsi
	call	neobject_destroy@PLT
	movl	CODE_ERROR(%rip), %r10d
	testl	%r10d, %r10d
	jne	.L529
	testb	%r13b, %r13b
	je	.L534
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rbp
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%rbp, %rdi
	call	ptrlist_append@PLT
	movq	24(%r12), %rax
	movq	(%rax,%r15,8), %rax
	movq	24(%rax), %rax
	movq	8(%rax), %rdi
	call	exec_aux
	movl	%eax, %r15d
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rdi
	call	deleteContext
	movl	CODE_ERROR(%rip), %r9d
	testl	%r9d, %r9d
	jne	.L529
	testl	%r15d, %r15d
	jne	.L526
	movl	32(%r12), %edx
	addl	$1, %ebx
	cmpl	%ebx, %edx
	jle	.L541
	movq	24(%r12), %rcx
	movslq	%ebx, %rbx
	jmp	.L537
	.p2align 4,,10
	.p2align 3
.L539:
	addq	$1, %rbx
	cmpl	%ebx, %edx
	jle	.L570
.L537:
	movq	(%rcx,%rbx,8), %rax
	movl	%ebx, %esi
	cmpb	$30, (%rax)
	jne	.L539
	cmpl	%edx, %ebx
	je	.L541
.L564:
	cmpl	%edx, %ebx
	jl	.L555
	jmp	.L541
	.p2align 4,,10
	.p2align 3
.L570:
	leal	1(%rsi), %ebx
	cmpl	%edx, %ebx
	jne	.L564
	jmp	.L541
.L566:
	movb	$4, %sil
	call	neo_to_integer@PLT
	testq	%rax, %rax
	setne	%r13b
	jmp	.L531
.L553:
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	104(%rax), %r13
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%r13, %rdi
	call	ptrlist_append@PLT
	movq	24(%r12), %rax
	movq	(%rax,%rbp,8), %rdi
	call	exec_aux
	movl	%eax, %r15d
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rdi
	call	deleteContext
	movl	CODE_ERROR(%rip), %edx
	testl	%edx, %edx
	jne	.L529
	testl	%r15d, %r15d
	jne	.L526
	movl	32(%r12), %edx
	jmp	.L554
	.cfi_endproc
.LFE24:
	.size	execConditionBlock, .-execConditionBlock
	.p2align 4
	.globl	execStatementFor
	.type	execStatementFor, @function
execStatementFor:
.LFB25:
	.cfi_startproc
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
	movq	%rdi, %rbx
	subq	$88, %rsp
	.cfi_def_cfa_offset 144
	movq	24(%rdi), %rax
	movq	(%rax), %rdx
	movl	32(%rdx), %eax
	cmpl	$4, %eax
	je	.L602
	cmpl	$3, %eax
	je	.L603
	cmpl	$2, %eax
	je	.L604
	movl	$108, CODE_ERROR(%rip)
.L577:
	xorl	%r15d, %r15d
.L571:
	addq	$88, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	movl	%r15d, %eax
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
	.p2align 4,,10
	.p2align 3
.L604:
	.cfi_restore_state
	xorl	%edi, %edi
	call	neo_integer_create@PLT
	movq	%rdx, 40(%rsp)
	movl	CODE_ERROR(%rip), %edx
	movq	%rax, 32(%rsp)
	testl	%edx, %edx
	jne	.L577
	movq	24(%rbx), %rax
	leaq	48(%rsp), %rdi
	movq	(%rax), %rax
	movq	24(%rax), %rax
	movq	8(%rax), %rsi
	call	eval_prolog
.L581:
	movl	$1, %ebp
.L576:
	movl	CODE_ERROR(%rip), %r15d
	movq	32(%rsp), %rdi
	movq	40(%rsp), %rsi
	testl	%r15d, %r15d
	jne	.L601
	cmpb	$4, 40(%rsp)
	jne	.L585
	cmpb	$4, 56(%rsp)
	je	.L586
.L585:
	call	neobject_destroy@PLT
	movq	48(%rsp), %rdi
	movq	56(%rsp), %rsi
	call	neobject_destroy@PLT
	movl	$10, CODE_ERROR(%rip)
	jmp	.L577
	.p2align 4,,10
	.p2align 3
.L602:
	movq	24(%rdx), %rax
	leaq	64(%rsp), %rdi
	movq	24(%rax), %rsi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %edi
	testl	%edi, %edi
	jne	.L577
	cmpb	$4, 72(%rsp)
	movq	64(%rsp), %rdi
	movq	72(%rsp), %rsi
	je	.L574
	call	neobject_destroy@PLT
	movl	$108, CODE_ERROR(%rip)
	jmp	.L577
	.p2align 4,,10
	.p2align 3
.L603:
	movq	24(%rdx), %rax
	leaq	32(%rsp), %rdi
	movq	8(%rax), %rsi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %ecx
	testl	%ecx, %ecx
	jne	.L577
	movq	24(%rbx), %rax
	leaq	48(%rsp), %rdi
	movq	(%rax), %rax
	movq	24(%rax), %rax
	movq	16(%rax), %rsi
	call	eval_prolog
	jmp	.L581
	.p2align 4,,10
	.p2align 3
.L574:
	call	neo_to_integer@PLT
	movq	64(%rsp), %rdi
	movq	72(%rsp), %rsi
	movq	%rax, %rbp
	call	neobject_destroy@PLT
	movq	24(%rbx), %rax
	leaq	32(%rsp), %rdi
	movq	(%rax), %rax
	movq	24(%rax), %rax
	movq	8(%rax), %rsi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %esi
	testl	%esi, %esi
	jne	.L577
	movq	24(%rbx), %rax
	leaq	48(%rsp), %rdi
	movq	(%rax), %rax
	movq	24(%rax), %rax
	movq	16(%rax), %rsi
	call	eval_prolog
	jmp	.L576
	.p2align 4,,10
	.p2align 3
.L586:
	call	neo_to_integer@PLT
	movq	32(%rsp), %rdi
	movq	40(%rsp), %rsi
	movq	%rax, 8(%rsp)
	call	neobject_destroy@PLT
	movq	24(%rbx), %rax
	movq	(%rax), %rax
	movq	24(%rax), %rax
	movq	(%rax), %rax
	cmpb	$10, (%rax)
	je	.L587
	movl	$111, CODE_ERROR(%rip)
	movq	48(%rsp), %rdi
	movq	56(%rsp), %rsi
.L601:
	call	neobject_destroy@PLT
	jmp	.L577
	.p2align 4,,10
	.p2align 3
.L587:
	movl	56(%rax), %r12d
	movq	process_cycle(%rip), %rax
	movl	8(%rsp), %r14d
	movq	(%rax), %rax
	movq	104(%rax), %r13
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%r13, %rdi
	call	ptrlist_append@PLT
	movq	process_cycle(%rip), %rax
	movl	%r12d, %esi
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	movq	(%rdi), %rax
	movq	%rax, 16(%rsp)
	call	save_later@PLT
	movq	process_cycle(%rip), %rax
	movl	$24, %edi
	movq	(%rax), %rax
	movq	104(%rax), %rsi
	movq	%rsi, 24(%rsp)
	call	malloc@PLT
	movl	%r12d, %edi
	movq	%rax, %r13
	call	get_var_value@PLT
	movl	%r12d, 16(%r13)
	movl	%r12d, %edi
	movq	%rdx, 8(%r13)
	movq	%rax, 0(%r13)
	call	get_absolute_address@PLT
	movq	%rax, %rdi
	call	var_reset@PLT
	movq	24(%rsp), %rsi
	movq	(%rsi), %rdi
	movq	%r13, %rsi
	call	ptrlist_append@PLT
	movq	48(%rsp), %rdi
	movq	56(%rsp), %rsi
	call	neo_to_integer@PLT
	movq	48(%rsp), %rdi
	movq	56(%rsp), %rsi
	movq	%rax, 24(%rsp)
	movl	%eax, %r13d
	call	neobject_destroy@PLT
	movq	24(%rsp), %rax
	cmpl	%eax, 8(%rsp)
	jl	.L591
	jmp	.L599
	.p2align 4,,10
	.p2align 3
.L589:
	movslq	%r15d, %rax
	cmpq	$1, %rax
	ja	.L590
	addl	%ebp, %r14d
	cmpl	%r14d, %r13d
	jle	.L599
.L591:
	movslq	%r14d, %rdi
	call	neo_integer_create@PLT
	movl	%r12d, %edi
	movq	%rax, %rsi
	call	replace_var@PLT
	movq	24(%rbx), %rax
	movq	8(%rax), %rdi
	call	exec_aux
	movl	%eax, %r15d
	movl	CODE_ERROR(%rip), %eax
	testl	%eax, %eax
	je	.L589
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rdi
	call	deleteContext
	movq	process_cycle(%rip), %rax
	movq	16(%rsp), %rsi
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	call	partialRestore@PLT
	jmp	.L577
.L599:
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rdi
	call	deleteContext
	movq	process_cycle(%rip), %rax
	movq	16(%rsp), %rsi
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	call	partialRestore@PLT
	jmp	.L571
.L590:
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rdi
	call	deleteContext
	movq	process_cycle(%rip), %rax
	movq	16(%rsp), %rsi
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	call	partialRestore@PLT
	cmpl	$2, %r15d
	je	.L577
	jmp	.L571
	.cfi_endproc
.LFE25:
	.size	execStatementFor, .-execStatementFor
	.p2align 4
	.globl	execStatementForeachList
	.type	execStatementForeachList, @function
execStatementForeachList:
.LFB26:
	.cfi_startproc
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
	movq	%rdi, %rbp
	movq	%rsi, %rdi
	movq	%rdx, %rsi
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	subq	$24, %rsp
	.cfi_def_cfa_offset 80
	call	neo_to_list@PLT
	movq	%rax, %rbx
	movq	24(%rbp), %rax
	movq	(%rax), %rax
	movq	24(%rax), %rax
	movq	(%rax), %rax
	cmpb	$10, (%rax)
	je	.L606
	movl	$111, CODE_ERROR(%rip)
.L607:
	xorl	%eax, %eax
.L605:
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
	.p2align 4,,10
	.p2align 3
.L606:
	.cfi_restore_state
	movl	56(%rax), %r12d
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	104(%rax), %r13
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%r13, %rdi
	call	ptrlist_append@PLT
	movq	process_cycle(%rip), %rax
	movl	%r12d, %esi
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	movq	(%rdi), %r13
	call	save_later@PLT
	movq	process_cycle(%rip), %rax
	movl	$24, %edi
	movq	(%rax), %rax
	movq	104(%rax), %r15
	call	malloc@PLT
	movl	%r12d, %edi
	movq	%rax, %r14
	call	get_var_value@PLT
	movl	%r12d, 16(%r14)
	movl	%r12d, %edi
	movq	%rdx, 8(%r14)
	movq	%rax, (%r14)
	call	get_absolute_address@PLT
	movq	%rax, %rdi
	call	var_reset@PLT
	movq	(%r15), %rdi
	movq	%r14, %rsi
	call	ptrlist_append@PLT
	movl	16(%rbx), %ecx
	testl	%ecx, %ecx
	jle	.L622
	xorl	%r14d, %r14d
	jmp	.L612
	.p2align 4,,10
	.p2align 3
.L609:
	movslq	%eax, %rdx
	cmpq	$1, %rdx
	ja	.L611
	movslq	16(%rbx), %rdx
	addq	$1, %r14
	cmpq	%r14, %rdx
	jle	.L623
.L612:
	movl	%r14d, %esi
	movq	%rbx, %rdi
	call	nelist_nth@PLT
	movq	%rax, %rdi
	movq	%rdx, %rsi
	call	neo_copy@PLT
	movl	%r12d, %edi
	movq	%rax, %rsi
	call	replace_var@PLT
	movq	24(%rbp), %rax
	movq	8(%rax), %rdi
	call	exec_aux
	movl	CODE_ERROR(%rip), %edx
	testl	%edx, %edx
	je	.L609
.L622:
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rdi
	call	deleteContext
	movq	process_cycle(%rip), %rax
	movq	%r13, %rsi
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	call	partialRestore@PLT
	jmp	.L607
	.p2align 4,,10
	.p2align 3
.L611:
	movq	process_cycle(%rip), %rdx
	movl	%eax, 12(%rsp)
	movq	(%rdx), %rdx
	movq	104(%rdx), %rdi
	call	deleteContext
	movq	process_cycle(%rip), %rdx
	movq	%r13, %rsi
	movq	(%rdx), %rdx
	movq	120(%rdx), %rdi
	call	partialRestore@PLT
	movl	12(%rsp), %eax
	cmpl	$2, %eax
	je	.L607
	jmp	.L605
	.p2align 4,,10
	.p2align 3
.L623:
	movq	process_cycle(%rip), %rdx
	movl	%eax, 12(%rsp)
	movq	(%rdx), %rdx
	movq	104(%rdx), %rdi
	call	deleteContext
	movq	process_cycle(%rip), %rdx
	movq	%r13, %rsi
	movq	(%rdx), %rdx
	movq	120(%rdx), %rdi
	call	partialRestore@PLT
	movl	12(%rsp), %eax
	addq	$24, %rsp
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
	.cfi_endproc
.LFE26:
	.size	execStatementForeachList, .-execStatementForeachList
	.p2align 4
	.globl	execStatementForeachString
	.type	execStatementForeachString, @function
execStatementForeachString:
.LFB27:
	.cfi_startproc
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
	movq	%rdi, %r12
	movq	%rsi, %rdi
	movq	%rdx, %rsi
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	subq	$24, %rsp
	.cfi_def_cfa_offset 80
	call	neo_to_string@PLT
	movq	%rax, %rdi
	movq	%rax, %rbx
	call	strlen@PLT
	movq	%rax, %rbp
	movq	24(%r12), %rax
	movq	(%rax), %rax
	movq	24(%rax), %rax
	movq	(%rax), %rax
	cmpb	$10, (%rax)
	je	.L625
	movl	$111, CODE_ERROR(%rip)
.L626:
	xorl	%eax, %eax
.L624:
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
	.p2align 4,,10
	.p2align 3
.L625:
	.cfi_restore_state
	movl	56(%rax), %r13d
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	104(%rax), %r14
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%r14, %rdi
	call	ptrlist_append@PLT
	movq	process_cycle(%rip), %rax
	movl	%r13d, %esi
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	movq	(%rdi), %rax
	movq	%rax, (%rsp)
	call	save_later@PLT
	movq	process_cycle(%rip), %rax
	movl	$24, %edi
	movq	(%rax), %rax
	movq	104(%rax), %r14
	call	malloc@PLT
	movl	%r13d, %edi
	movq	%rax, %r15
	call	get_var_value@PLT
	movl	%r13d, 16(%r15)
	movl	%r13d, %edi
	movq	%rdx, 8(%r15)
	movq	%rax, (%r15)
	call	get_absolute_address@PLT
	movq	%rax, %rdi
	call	var_reset@PLT
	movq	(%r14), %rdi
	movq	%r15, %rsi
	call	ptrlist_append@PLT
	testl	%ebp, %ebp
	jle	.L641
	movslq	%ebp, %rbp
	addq	%rbx, %rbp
	jmp	.L631
	.p2align 4,,10
	.p2align 3
.L628:
	movslq	%eax, %rdx
	cmpq	$1, %rdx
	ja	.L630
	addq	$1, %rbx
	cmpq	%rbp, %rbx
	je	.L642
.L631:
	movsbl	(%rbx), %edi
	call	charToString@PLT
	movq	%rax, %rdi
	call	neo_str_create@PLT
	movl	%r13d, %edi
	movq	%rax, %rsi
	call	replace_var@PLT
	movq	24(%r12), %rax
	movq	8(%rax), %rdi
	call	exec_aux
	movl	CODE_ERROR(%rip), %edx
	testl	%edx, %edx
	je	.L628
.L641:
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rdi
	call	deleteContext
	movq	process_cycle(%rip), %rax
	movq	(%rsp), %rsi
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	call	partialRestore@PLT
	jmp	.L626
	.p2align 4,,10
	.p2align 3
.L630:
	movq	process_cycle(%rip), %rdx
	movl	%eax, 12(%rsp)
	movq	(%rdx), %rdx
	movq	104(%rdx), %rdi
	call	deleteContext
	movq	process_cycle(%rip), %rdx
	movq	(%rsp), %rsi
	movq	(%rdx), %rdx
	movq	120(%rdx), %rdi
	call	partialRestore@PLT
	movl	12(%rsp), %eax
	cmpl	$2, %eax
	je	.L626
	jmp	.L624
	.p2align 4,,10
	.p2align 3
.L642:
	movq	process_cycle(%rip), %rdx
	movl	%eax, 12(%rsp)
	movq	(%rdx), %rdx
	movq	104(%rdx), %rdi
	call	deleteContext
	movq	process_cycle(%rip), %rdx
	movq	(%rsp), %rsi
	movq	(%rdx), %rdx
	movq	120(%rdx), %rdi
	call	partialRestore@PLT
	movl	12(%rsp), %eax
	addq	$24, %rsp
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
	.cfi_endproc
.LFE27:
	.size	execStatementForeachString, .-execStatementForeachString
	.p2align 4
	.globl	initRuntime
	.type	initRuntime, @function
initRuntime:
.LFB29:
	.cfi_startproc
	movl	$1, %edx
	xorl	%esi, %esi
	xorl	%edi, %edi
	jmp	create_new_process@PLT
	.cfi_endproc
.LFE29:
	.size	initRuntime, .-initRuntime
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC3:
	.string	"saut direct \303\240 eval_prolog apr\303\250s la fin du processus principal"
	.text
	.p2align 4
	.globl	exitRuntime
	.type	exitRuntime, @function
exitRuntime:
.LFB30:
	.cfi_startproc
	subq	$24, %rsp
	.cfi_def_cfa_offset 32
	movq	process_cycle(%rip), %rax
	pxor	%xmm0, %xmm0
	movaps	%xmm0, (%rsp)
	movq	(%rax), %rdi
	movq	PROCESS_FINISH(%rip), %rax
	movb	$55, 8(%rsp)
	movslq	112(%rdi), %rdx
	movl	$1, (%rax,%rdx,4)
	call	process_preRemove@PLT
	movq	process_cycle(%rip), %rdi
	call	processCycle_isActive@PLT
	testb	%al, %al
	jne	.L652
.L645:
	movq	process_cycle(%rip), %rdi
	jmp	.L646
	.p2align 4,,10
	.p2align 3
.L647:
	movq	process_cycle(%rip), %rdi
	call	processCycle_remove@PLT
	movq	%rax, process_cycle(%rip)
	movq	%rax, %rdi
.L646:
	call	processCycle_isEmpty@PLT
	testb	%al, %al
	je	.L647
	movq	PROMISES_CNT(%rip), %rax
	movq	(%rax), %rax
	movl	$0, (%rax)
	movq	PROCESS_FINISH(%rip), %rax
	movl	$1, (%rax)
	addq	$24, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L652:
	.cfi_restore_state
	leaq	.LC3(%rip), %rdi
	call	puts@PLT
	call	save_stack_and_registers@PLT
	movq	process_cycle(%rip), %rdi
	call	loadNextLivingProcess@PLT
	xorl	%esi, %esi
	xorl	%edi, %edi
	movq	%rax, process_cycle(%rip)
	call	eval_prolog
	jmp	.L645
	.cfi_endproc
.LFE30:
	.size	exitRuntime, .-exitRuntime
	.p2align 4
	.globl	exec
	.type	exec, @function
exec:
.LFB31:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movl	$1, %edx
	movq	%rdi, %rbx
	xorl	%esi, %esi
	xorl	%edi, %edi
	call	create_new_process@PLT
	movq	%rbx, %rdi
	call	exec_aux
	popq	%rbx
	.cfi_def_cfa_offset 8
	jmp	exitRuntime
	.cfi_endproc
.LFE31:
	.size	exec, .-exec
	.p2align 4
	.globl	eval
	.type	eval, @function
eval:
.LFB32:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movl	$1, %edx
	movq	%rdi, %rbx
	xorl	%esi, %esi
	xorl	%edi, %edi
	subq	$16, %rsp
	.cfi_def_cfa_offset 32
	call	create_new_process@PLT
	movq	%rsp, %rdi
	movq	%rbx, %rsi
	call	eval_prolog
	call	exitRuntime
	movq	(%rsp), %rax
	movq	8(%rsp), %rdx
	addq	$16, %rsp
	.cfi_def_cfa_offset 16
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE32:
	.size	eval, .-eval
	.globl	BUILTINSFONC
	.section	.data.rel,"aw"
	.align 32
	.type	BUILTINSFONC, @object
	.size	BUILTINSFONC, 432
BUILTINSFONC:
	.quad	_print_
	.quad	_input_
	.quad	_nbr_
	.quad	_str_
	.quad	_len_
	.quad	_substring_
	.quad	_exit_
	.quad	_append_
	.quad	_remove_
	.quad	_insert_
	.quad	_type_
	.quad	_reverse_
	.quad	_eval_
	.quad	_clear_
	.quad	_help_
	.quad	_randint_
	.quad	_failwith_
	.quad	_time_
	.quad	_assert_
	.quad	_output_
	.quad	_chr_
	.quad	_ord_
	.quad	_list_comp_
	.quad	_create_exception_
	.quad	_raise_
	.quad	_int_
	.quad	_index_
	.quad	_replace_
	.quad	_count_
	.quad	_list_
	.quad	_sort_asc_
	.quad	_sort_desc_
	.quad	_sin_
	.quad	_cos_
	.quad	_tan_
	.quad	_deg_
	.quad	_rad_
	.quad	_sqrt_
	.quad	_ln_
	.quad	_exp_
	.quad	_log_
	.quad	_log2_
	.quad	_round_
	.quad	_abs_
	.quad	_ceil_
	.quad	_floor_
	.quad	_readFile_
	.quad	_writeFile_
	.quad	_setFunctionDoc_
	.quad	_setAtomicTime_
	.quad	_copy_
	.quad	_load_namespace_
	.quad	_gc_
	.quad	_setColor_
	.globl	OPFONC
	.align 32
	.type	OPFONC, @object
	.size	OPFONC, 320
OPFONC:
	.quad	_and
	.quad	_or
	.quad	_xor
	.quad	_add
	.quad	_mul
	.quad	_sub
	.quad	_div
	.quad	_pow
	.quad	_equal
	.quad	_notEq
	.quad	_infEqual
	.quad	_supEqual
	.quad	_inf
	.quad	_sup
	.quad	_affectNone
	.quad	_addEqual
	.quad	_subEqual
	.quad	_mulEqual
	.quad	_divEqual
	.quad	_incr
	.quad	_decr
	.quad	_not
	.quad	_mod
	.quad	_eucl
	.quad	_ref
	.quad	_goIn
	.quad	_deref
	.quad	_minus
	.quad	_del
	.quad	_affect
	.quad	0
	.quad	_exponent
	.quad	_implique
	.quad	_in
	.quad	0
	.quad	0
	.quad	_swap
	.quad	0
	.quad	0
	.quad	0
	.ident	"GCC: (Debian 12.2.0-14+deb12u1) 12.2.0"
	.section	.note.GNU-stack,"",@progbits
