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
	.globl	allocate_new_stack
	.type	allocate_new_stack, @function
allocate_new_stack:
.LFB16:
	.cfi_startproc
	movl	$8388608, %edi
	jmp	malloc@PLT
	.cfi_endproc
.LFE16:
	.size	allocate_new_stack, .-allocate_new_stack
	.p2align 4
	.globl	set_stack_pointer
	.type	set_stack_pointer, @function
set_stack_pointer:
.LFB17:
	.cfi_startproc
	testq	%rsi, %rsi
	je	.L28
	addq	$8388608, %rsi
	andq	$-16, %rsi
	leaq	-8(%rsi), %rax
	movq	%rax, 48(%rdi)
	movq	launch_process_unaligned@GOTPCREL(%rip), %rax
	movq	%rax, -8(%rsi)
.L28:
	ret
	.cfi_endproc
.LFE17:
	.size	set_stack_pointer, .-set_stack_pointer
	.p2align 4
	.globl	local
	.type	local, @function
local:
.LFB18:
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
.LFE18:
	.size	local, .-local
	.p2align 4
	.globl	processCycle_create
	.type	processCycle_create, @function
processCycle_create:
.LFB19:
	.cfi_startproc
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movl	$24, %edi
	call	malloc@PLT
	pxor	%xmm0, %xmm0
	movq	$0, 16(%rax)
	movups	%xmm0, (%rax)
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE19:
	.size	processCycle_create, .-processCycle_create
	.p2align 4
	.globl	save_later
	.type	save_later, @function
save_later:
.LFB20:
	.cfi_startproc
	pushq	%r12
	.cfi_def_cfa_offset 16
	.cfi_offset 12, -16
	movq	%rdi, %rax
	xorl	%ecx, %ecx
	pushq	%rbp
	.cfi_def_cfa_offset 24
	.cfi_offset 6, -24
	movq	%rdi, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	movl	%esi, %ebx
	testq	%rdi, %rdi
	jne	.L38
	jmp	.L39
	.p2align 4,,10
	.p2align 3
.L54:
	testb	%cl, %cl
	jne	.L37
	movq	8(%rax), %rax
	cmpl	%ebx, 16(%rdx)
	sete	%cl
	testq	%rax, %rax
	je	.L40
.L38:
	movq	(%rax), %rdx
	testq	%rdx, %rdx
	jne	.L54
.L40:
	testb	%cl, %cl
	je	.L39
.L37:
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
.L39:
	.cfi_restore_state
	movl	$24, %edi
	call	malloc@PLT
	movl	%ebx, %edi
	movq	%rax, %r12
	call	get_var_value@PLT
	movl	%ebx, 16(%r12)
	movq	%r12, %rsi
	movq	%rbp, %rdi
	movq	%rax, (%r12)
	movq	%rdx, 8(%r12)
	popq	%rbx
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r12
	.cfi_def_cfa_offset 8
	jmp	ptrlist_append@PLT
	.cfi_endproc
.LFE20:
	.size	save_later, .-save_later
	.p2align 4
	.globl	switchGlobalLocalVariables
	.type	switchGlobalLocalVariables, @function
switchGlobalLocalVariables:
.LFB21:
	.cfi_startproc
	testq	%rdi, %rdi
	je	.L67
	pushq	%r13
	.cfi_def_cfa_offset 16
	.cfi_offset 13, -16
	pushq	%r12
	.cfi_def_cfa_offset 24
	.cfi_offset 12, -24
	pushq	%rbp
	.cfi_def_cfa_offset 32
	.cfi_offset 6, -32
	movq	%rdi, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 40
	.cfi_offset 3, -40
	subq	$8, %rsp
	.cfi_def_cfa_offset 48
	jmp	.L56
	.p2align 4,,10
	.p2align 3
.L58:
	movl	16(%rbx), %edi
	movq	(%rbx), %r13
	movq	8(%rbx), %r12
	call	get_var_value@PLT
	movl	16(%rbx), %edi
	movq	%r13, %rsi
	movq	%rdx, 8(%rbx)
	movq	%r12, %rdx
	movq	%rax, (%rbx)
	call	set_var@PLT
	movq	8(%rbp), %rbp
	testq	%rbp, %rbp
	je	.L55
.L56:
	movq	0(%rbp), %rbx
	testq	%rbx, %rbx
	jne	.L58
.L55:
	addq	$8, %rsp
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
.L67:
	.cfi_restore 3
	.cfi_restore 6
	.cfi_restore 12
	.cfi_restore 13
	ret
	.cfi_endproc
.LFE21:
	.size	switchGlobalLocalVariables, .-switchGlobalLocalVariables
	.p2align 4
	.globl	partialRestore
	.type	partialRestore, @function
partialRestore:
.LFB22:
	.cfi_startproc
	cmpq	(%rdi), %rsi
	je	.L73
	pushq	%r12
	.cfi_def_cfa_offset 16
	.cfi_offset 12, -16
	movq	%rsi, %r12
	pushq	%rbp
	.cfi_def_cfa_offset 24
	.cfi_offset 6, -24
	movq	%rdi, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	.p2align 4,,10
	.p2align 3
.L70:
	movq	%rbp, %rdi
	call	ptrlist_pop@PLT
	movl	16(%rax), %edi
	movq	(%rax), %rsi
	movq	%rax, %rbx
	movq	8(%rax), %rdx
	call	set_var@PLT
	movq	%rbx, %rdi
	call	free@PLT
	cmpq	%r12, 0(%rbp)
	jne	.L70
	popq	%rbx
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r12
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L73:
	.cfi_restore 3
	.cfi_restore 6
	.cfi_restore 12
	ret
	.cfi_endproc
.LFE22:
	.size	partialRestore, .-partialRestore
	.p2align 4
	.globl	nextProcess
	.type	nextProcess, @function
nextProcess:
.LFB23:
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
	movq	8(%rdi), %rax
	testq	%rax, %rax
	jne	.L94
.L77:
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 40
	movq	%r13, %rax
	popq	%rbp
	.cfi_def_cfa_offset 32
	popq	%r12
	.cfi_def_cfa_offset 24
	popq	%r13
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L94:
	.cfi_restore_state
	movq	(%rdi), %rdx
	movq	112(%rdx), %rbp
	testq	%rbp, %rbp
	jne	.L78
	jmp	.L79
	.p2align 4,,10
	.p2align 3
.L80:
	movl	16(%rbx), %edi
	movq	(%rbx), %r14
	movq	8(%rbx), %r12
	call	get_var_value@PLT
	movl	16(%rbx), %edi
	movq	%r14, %rsi
	movq	%rdx, 8(%rbx)
	movq	%r12, %rdx
	movq	%rax, (%rbx)
	call	set_var@PLT
	movq	8(%rbp), %rbp
	testq	%rbp, %rbp
	je	.L92
.L78:
	movq	0(%rbp), %rbx
	testq	%rbx, %rbx
	jne	.L80
.L92:
	movq	8(%r13), %rax
.L79:
	movq	(%rax), %rdx
	movq	112(%rdx), %rbp
	testq	%rbp, %rbp
	jne	.L81
	jmp	.L95
	.p2align 4,,10
	.p2align 3
.L83:
	movl	16(%rbx), %edi
	movq	(%rbx), %r14
	movq	8(%rbx), %r12
	call	get_var_value@PLT
	movl	16(%rbx), %edi
	movq	%r14, %rsi
	movq	%rdx, 8(%rbx)
	movq	%r12, %rdx
	movq	%rax, (%rbx)
	call	set_var@PLT
	movq	8(%rbp), %rbp
	testq	%rbp, %rbp
	je	.L93
.L81:
	movq	0(%rbp), %rbx
	testq	%rbx, %rbx
	jne	.L83
.L93:
	movq	8(%r13), %r13
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 40
	popq	%rbp
	.cfi_def_cfa_offset 32
	popq	%r12
	.cfi_def_cfa_offset 24
	movq	%r13, %rax
	popq	%r13
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
.L95:
	.cfi_restore_state
	movq	%rax, %r13
	jmp	.L77
	.cfi_endproc
.LFE23:
	.size	nextProcess, .-nextProcess
	.p2align 4
	.globl	processCycle_add
	.type	processCycle_add, @function
processCycle_add:
.LFB24:
	.cfi_startproc
	pushq	%r14
	.cfi_def_cfa_offset 16
	.cfi_offset 14, -16
	movl	%edx, %r14d
	pushq	%r13
	.cfi_def_cfa_offset 24
	.cfi_offset 13, -24
	movl	%ecx, %r13d
	pushq	%r12
	.cfi_def_cfa_offset 32
	.cfi_offset 12, -32
	movq	%rdi, %r12
	movl	$120, %edi
	pushq	%rbp
	.cfi_def_cfa_offset 40
	.cfi_offset 6, -40
	movq	%rsi, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	subq	$16, %rsp
	.cfi_def_cfa_offset 64
	call	malloc@PLT
	movq	%rax, %rbx
	call	ptrlist_create@PLT
	movl	%r14d, 104(%rbx)
	movq	%rax, 96(%rbx)
	call	ptrlist_create@PLT
	movq	%rbp, 64(%rbx)
	movq	%rbx, %xmm0
	movq	%rax, 112(%rbx)
	xorl	%eax, %eax
	testb	%r13b, %r13b
	movq	8(%r12), %r13
	cmove	%rax, %rbp
	cmpq	$0, (%r12)
	movq	$0, 88(%rbx)
	movq	%r13, %xmm1
	movq	$0, 72(%rbx)
	movq	%rbp, 80(%rbx)
	punpcklqdq	%xmm1, %xmm0
	je	.L107
.L98:
	cmpq	%r13, %r12
	je	.L108
.L99:
	movl	$8388608, %edi
	movaps	%xmm0, (%rsp)
	call	malloc@PLT
	movl	$24, %edi
	movq	%rax, 88(%rbx)
	movq	%rax, %rbp
	call	malloc@PLT
	movdqa	(%rsp), %xmm0
	movups	%xmm0, (%rax)
	movq	%rax, 16(%r13)
	movq	%rax, 8(%r12)
	movq	%r12, 16(%rax)
.L101:
	testq	%rbp, %rbp
	je	.L96
	addq	$8388608, %rbp
	andq	$-16, %rbp
	leaq	-8(%rbp), %rax
	movq	%rax, 48(%rbx)
	movq	launch_process_unaligned@GOTPCREL(%rip), %rax
	movq	%rax, -8(%rbp)
.L96:
	addq	$16, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 48
	movq	%rbx, %rax
	popq	%rbx
	.cfi_def_cfa_offset 40
	popq	%rbp
	.cfi_def_cfa_offset 32
	popq	%r12
	.cfi_def_cfa_offset 24
	popq	%r13
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L108:
	.cfi_restore_state
	cmpq	%r12, 16(%r12)
	jne	.L99
	movl	$8388608, %edi
	call	malloc@PLT
	movl	$24, %edi
	movq	%rax, 88(%rbx)
	movq	%rax, %rbp
	call	malloc@PLT
	movq	%rax, %xmm0
	movq	%rbx, (%rax)
	punpcklqdq	%xmm0, %xmm0
	movq	%r12, 16(%rax)
	movq	%r12, 8(%rax)
	movups	%xmm0, 8(%r12)
	jmp	.L101
	.p2align 4,,10
	.p2align 3
.L107:
	testq	%r13, %r13
	jne	.L98
	cmpq	$0, 16(%r12)
	jne	.L99
	movq	%rbx, (%r12)
	movq	%r12, 8(%r12)
	movq	%r12, 16(%r12)
	jmp	.L96
	.cfi_endproc
.LFE24:
	.size	processCycle_add, .-processCycle_add
	.p2align 4
	.globl	processCycle_remove
	.type	processCycle_remove, @function
processCycle_remove:
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
	movq	%rdi, %r13
	pushq	%r12
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	subq	$8, %rsp
	.cfi_def_cfa_offset 64
	movq	(%rdi), %r14
	movq	112(%r14), %rbp
	testq	%rbp, %rbp
	jne	.L110
	jmp	.L113
	.p2align 4,,10
	.p2align 3
.L114:
	movl	16(%rbx), %edi
	movq	(%rbx), %r15
	movq	8(%rbx), %r12
	call	get_var_value@PLT
	movl	16(%rbx), %edi
	movq	%r15, %rsi
	movq	%rdx, 8(%rbx)
	movq	%r12, %rdx
	movq	%rax, (%rbx)
	call	set_var@PLT
	movq	8(%rbp), %rbp
	testq	%rbp, %rbp
	je	.L113
.L110:
	movq	0(%rbp), %rbx
	testq	%rbx, %rbx
	jne	.L114
.L113:
	movq	96(%r14), %rdi
	call	free@PLT
	movq	80(%r14), %rax
	testq	%rax, %rax
	je	.L112
	movq	40(%rax), %rdi
	movq	48(%rax), %rsi
	call	neobject_destroy@PLT
	movq	80(%r14), %rdi
	call	free@PLT
.L112:
	movq	88(%r14), %rdi
	testq	%rdi, %rdi
	je	.L115
	call	free@PLT
.L115:
	movq	112(%r14), %rdi
	movl	$1, %edx
	movl	$1, %esi
	call	ptrlist_destroy@PLT
	movq	%r14, %rdi
	call	free@PLT
	movq	8(%r13), %r14
	cmpq	%r14, %r13
	je	.L135
	movq	16(%r13), %rax
	cmpq	%r14, %rax
	je	.L136
.L117:
	movq	%r14, 8(%rax)
	movq	%r13, %rdi
	movq	%rax, 16(%r14)
	call	free@PLT
.L119:
	movq	(%r14), %rax
	movq	112(%rax), %rbp
	testq	%rbp, %rbp
	jne	.L120
	jmp	.L109
	.p2align 4,,10
	.p2align 3
.L122:
	movl	16(%rbx), %edi
	movq	(%rbx), %r13
	movq	8(%rbx), %r12
	call	get_var_value@PLT
	movl	16(%rbx), %edi
	movq	%r13, %rsi
	movq	%rdx, 8(%rbx)
	movq	%r12, %rdx
	movq	%rax, (%rbx)
	call	set_var@PLT
	movq	8(%rbp), %rbp
	testq	%rbp, %rbp
	je	.L109
.L120:
	movq	0(%rbp), %rbx
	testq	%rbx, %rbx
	jne	.L122
.L109:
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	movq	%r14, %rax
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
.L136:
	.cfi_restore_state
	movq	%r13, %rdi
	call	free@PLT
	movq	%r14, %xmm0
	punpcklqdq	%xmm0, %xmm0
	movups	%xmm0, 8(%r14)
	jmp	.L119
	.p2align 4,,10
	.p2align 3
.L135:
	movq	16(%r14), %rax
	cmpq	%r14, %rax
	jne	.L117
	movq	%r14, %rdi
	xorl	%r14d, %r14d
	call	free@PLT
	jmp	.L109
	.cfi_endproc
.LFE25:
	.size	processCycle_remove, .-processCycle_remove
	.p2align 4
	.globl	create_new_process
	.type	create_new_process, @function
create_new_process:
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
	movq	%rdi, %r12
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	movl	%esi, %ebp
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	subq	$8, %rsp
	.cfi_def_cfa_offset 64
	movq	PROMISES(%rip), %r8
	movl	16(%r8), %r9d
	testl	%r9d, %r9d
	jle	.L143
	movq	PROCESS_FINISH(%rip), %rdx
	movq	PROMISES_CNT(%rip), %rdi
	movslq	%r9d, %rcx
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L139:
	movl	(%rdx,%rax,4), %r10d
	movl	%eax, %ebx
	testl	%r10d, %r10d
	je	.L141
	movq	(%rdi,%rax,8), %rsi
	movl	(%rsi), %esi
	testl	%esi, %esi
	jle	.L146
.L141:
	addq	$1, %rax
	cmpq	%rax, %rcx
	jne	.L139
	addl	$1, %ebx
	movslq	%ebx, %rax
	leaq	0(,%rax,8), %r14
	leaq	0(,%rax,4), %r13
.L140:
	salq	$4, %rax
	movq	%rax, %r15
	cmpl	%ebx, %r9d
	jle	.L138
.L142:
	movq	8(%r8), %rax
	movq	(%rax,%r15), %rdi
	movq	8(%rax,%r15), %rsi
	call	neobject_destroy@PLT
	pxor	%xmm0, %xmm0
	movzbl	%bpl, %ecx
	movl	%ebx, %edx
	movq	PROMISES(%rip), %rax
	movq	process_cycle(%rip), %rdi
	movq	%r12, %rsi
	addq	8(%rax), %r15
	movq	PROMISES_CNT(%rip), %rax
	movups	%xmm0, (%r15)
	movb	$55, 8(%r15)
	movq	(%rax,%r14), %rax
	movl	$1, (%rax)
	movq	PROCESS_FINISH(%rip), %rax
	movl	$0, (%rax,%r13)
	call	processCycle_add
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	movl	%ebx, %eax
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
.L143:
	.cfi_restore_state
	xorl	%r13d, %r13d
	xorl	%r14d, %r14d
	xorl	%r15d, %r15d
	xorl	%ebx, %ebx
	.p2align 4,,10
	.p2align 3
.L138:
	movq	%r8, %rdi
	movl	$55, %edx
	xorl	%esi, %esi
	call	nelist_append@PLT
	movl	$4, %edi
	call	malloc@PLT
	leaq	PROMISES_CNT(%rip), %rdi
	movq	%rax, %rsi
	call	intptrlist_append@PLT
	xorl	%esi, %esi
	leaq	PROCESS_FINISH(%rip), %rdi
	call	intlist_append@PLT
	movq	PROMISES(%rip), %r8
	jmp	.L142
	.p2align 4,,10
	.p2align 3
.L146:
	leaq	0(,%rax,4), %r13
	leaq	0(,%rax,8), %r14
	jmp	.L140
	.cfi_endproc
.LFE26:
	.size	create_new_process, .-create_new_process
	.p2align 4
	.globl	ptrlist_destroy_noinline
	.type	ptrlist_destroy_noinline, @function
ptrlist_destroy_noinline:
.LFB27:
	.cfi_startproc
	movzbl	%dl, %edx
	movzbl	%sil, %esi
	jmp	ptrlist_destroy@PLT
	.cfi_endproc
.LFE27:
	.size	ptrlist_destroy_noinline, .-ptrlist_destroy_noinline
	.p2align 4
	.globl	nextProcess_noinline
	.type	nextProcess_noinline, @function
nextProcess_noinline:
.LFB28:
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
	movq	8(%rdi), %rax
	testq	%rax, %rax
	jne	.L166
.L149:
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 40
	movq	%r13, %rax
	popq	%rbp
	.cfi_def_cfa_offset 32
	popq	%r12
	.cfi_def_cfa_offset 24
	popq	%r13
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L166:
	.cfi_restore_state
	movq	(%rdi), %rdx
	movq	112(%rdx), %rbp
	testq	%rbp, %rbp
	jne	.L150
	jmp	.L151
	.p2align 4,,10
	.p2align 3
.L152:
	movl	16(%rbx), %edi
	movq	(%rbx), %r14
	movq	8(%rbx), %r12
	call	get_var_value@PLT
	movl	16(%rbx), %edi
	movq	%r14, %rsi
	movq	%rdx, 8(%rbx)
	movq	%r12, %rdx
	movq	%rax, (%rbx)
	call	set_var@PLT
	movq	8(%rbp), %rbp
	testq	%rbp, %rbp
	je	.L164
.L150:
	movq	0(%rbp), %rbx
	testq	%rbx, %rbx
	jne	.L152
.L164:
	movq	8(%r13), %rax
.L151:
	movq	(%rax), %rdx
	movq	112(%rdx), %rbp
	testq	%rbp, %rbp
	jne	.L153
	jmp	.L167
	.p2align 4,,10
	.p2align 3
.L155:
	movl	16(%rbx), %edi
	movq	(%rbx), %r14
	movq	8(%rbx), %r12
	call	get_var_value@PLT
	movl	16(%rbx), %edi
	movq	%r14, %rsi
	movq	%rdx, 8(%rbx)
	movq	%r12, %rdx
	movq	%rax, (%rbx)
	call	set_var@PLT
	movq	8(%rbp), %rbp
	testq	%rbp, %rbp
	je	.L165
.L153:
	movq	0(%rbp), %rbx
	testq	%rbx, %rbx
	jne	.L155
.L165:
	movq	8(%r13), %r13
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 40
	popq	%rbp
	.cfi_def_cfa_offset 32
	popq	%r12
	.cfi_def_cfa_offset 24
	movq	%r13, %rax
	popq	%r13
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
.L167:
	.cfi_restore_state
	movq	%rax, %r13
	jmp	.L149
	.cfi_endproc
.LFE28:
	.size	nextProcess_noinline, .-nextProcess_noinline
	.p2align 4
	.globl	neobject_destroy_noinline
	.type	neobject_destroy_noinline, @function
neobject_destroy_noinline:
.LFB29:
	.cfi_startproc
	jmp	neobject_destroy@PLT
	.cfi_endproc
.LFE29:
	.size	neobject_destroy_noinline, .-neobject_destroy_noinline
	.p2align 4
	.globl	processCycle_remove_noinline
	.type	processCycle_remove_noinline, @function
processCycle_remove_noinline:
.LFB30:
	.cfi_startproc
	jmp	processCycle_remove
	.cfi_endproc
.LFE30:
	.size	processCycle_remove_noinline, .-processCycle_remove_noinline
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	".ne"
	.text
	.p2align 4
	.globl	exec_aux
	.type	exec_aux, @function
exec_aux:
.LFB42:
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
	jle	.L231
	movq	%rdi, %rbp
	xorl	%ebx, %ebx
	leaq	.L175(%rip), %r13
	.p2align 4,,10
	.p2align 3
.L171:
	movq	24(%rbp), %rcx
	leaq	0(,%rbx,8), %r12
	movq	(%rcx,%rbx,8), %rdi
	movl	4(%rdi), %eax
	movl	%eax, LINENUMBER(%rip)
	movzbl	(%rdi), %eax
	subl	$16, %eax
	cmpb	$41, %al
	ja	.L173
	movzbl	%al, %eax
	movslq	0(%r13,%rax,4), %rax
	addq	%r13, %rax
	jmp	*%rax
	.section	.rodata
	.align 4
	.align 4
.L175:
	.long	.L183-.L175
	.long	.L173-.L175
	.long	.L182-.L175
	.long	.L173-.L175
	.long	.L173-.L175
	.long	.L173-.L175
	.long	.L173-.L175
	.long	.L173-.L175
	.long	.L173-.L175
	.long	.L173-.L175
	.long	.L173-.L175
	.long	.L181-.L175
	.long	.L180-.L175
	.long	.L179-.L175
	.long	.L173-.L175
	.long	.L173-.L175
	.long	.L173-.L175
	.long	.L173-.L175
	.long	.L173-.L175
	.long	.L178-.L175
	.long	.L173-.L175
	.long	.L173-.L175
	.long	.L173-.L175
	.long	.L173-.L175
	.long	.L173-.L175
	.long	.L173-.L175
	.long	.L173-.L175
	.long	.L173-.L175
	.long	.L177-.L175
	.long	.L173-.L175
	.long	.L176-.L175
	.long	.L173-.L175
	.long	.L173-.L175
	.long	.L173-.L175
	.long	.L173-.L175
	.long	.L173-.L175
	.long	.L173-.L175
	.long	.L173-.L175
	.long	.L173-.L175
	.long	.L173-.L175
	.long	.L173-.L175
	.long	.L174-.L175
	.text
	.p2align 4,,10
	.p2align 3
.L173:
	leaq	32(%rsp), %r14
	movq	%rdi, %rsi
	movq	%r14, %rdi
	call	eval_prolog
	movq	32(%rsp), %rdi
	movq	40(%rsp), %rsi
	call	neobject_destroy@PLT
.L279:
	movl	CODE_ERROR(%rip), %eax
	testl	%eax, %eax
	jne	.L231
.L238:
	addq	$1, %rbx
	cmpl	%ebx, 32(%rbp)
	jg	.L171
.L231:
	xorl	%r15d, %r15d
.L170:
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
.L174:
	.cfi_restore_state
	movq	24(%rdi), %rax
	movq	(%rax), %rax
	movq	24(%rax), %rdx
	movq	(%rdx), %rcx
	cmpb	$10, (%rcx)
	jne	.L283
	cmpl	$2, 32(%rax)
	jne	.L288
	movq	8(%rdx), %rsi
	leaq	32(%rsp), %r14
	movq	%r14, %rdi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %esi
	testl	%esi, %esi
	jne	.L231
	movzbl	40(%rsp), %eax
	cmpb	$-122, %al
	je	.L289
	cmpb	$-125, %al
	jne	.L236
	movq	24(%rbp), %rax
	movq	32(%rsp), %rsi
	movq	40(%rsp), %rdx
	movq	(%rax,%r12), %rdi
	call	execStatementForeachString
	movl	%eax, %r15d
.L235:
	movq	32(%rsp), %rdi
	movq	40(%rsp), %rsi
	call	neobject_destroy@PLT
	movl	%r15d, %eax
	orl	CODE_ERROR(%rip), %eax
	je	.L238
	jmp	.L170
	.p2align 4,,10
	.p2align 3
.L176:
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
	je	.L238
	jmp	.L170
	.p2align 4,,10
	.p2align 3
.L177:
	movq	24(%rdi), %rax
	movq	(%rax), %rdi
	call	exec_aux
	movl	%eax, %r15d
	testl	%eax, %eax
	jne	.L170
	movq	24(%rbp), %rax
	movl	CODE_ERROR(%rip), %edx
	movq	(%rax,%r12), %r12
	movl	%edx, %eax
	cmpl	$1, %edx
	ja	.L290
.L185:
	testl	%eax, %eax
	je	.L238
	jmp	.L231
	.p2align 4,,10
	.p2align 3
.L178:
	movq	24(%rdi), %rax
	movq	(%rax), %r15
	movl	32(%r15), %edi
	call	nelist_create@PLT
	movl	32(%r15), %ecx
	movq	%rax, %rdx
	testl	%ecx, %ecx
	jle	.L242
	xorl	%r14d, %r14d
	jmp	.L243
	.p2align 4,,10
	.p2align 3
.L292:
	movq	%rdx, 8(%rsp)
	call	eval_prolog
	movl	CODE_ERROR(%rip), %edx
	testl	%edx, %edx
	movq	8(%rsp), %rdx
	jne	.L291
	addq	$1, %r14
	cmpl	%r14d, 32(%r15)
	jle	.L242
.L243:
	movq	24(%r15), %rax
	movq	%r14, %rdi
	salq	$4, %rdi
	addq	8(%rdx), %rdi
	movq	(%rax,%r14,8), %rsi
	testq	%rsi, %rsi
	jne	.L292
	pxor	%xmm0, %xmm0
	addq	$1, %r14
	movups	%xmm0, (%rdi)
	movb	$55, 8(%rdi)
	cmpl	%r14d, 32(%r15)
	jg	.L243
.L242:
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
	jmp	.L280
	.p2align 4,,10
	.p2align 3
.L179:
	movq	24(%rdi), %rax
	leaq	32(%rsp), %r14
	movq	%r14, %rdi
	movq	(%rax), %rsi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %r9d
	testl	%r9d, %r9d
	jne	.L231
.L286:
	movzbl	40(%rsp), %eax
	movq	32(%rsp), %rdi
	movq	40(%rsp), %rsi
	cmpb	$8, %al
	je	.L293
	cmpb	$4, %al
	je	.L294
	movl	$20, CODE_ERROR(%rip)
	call	neobject_destroy@PLT
.L280:
	movl	CODE_ERROR(%rip), %eax
	testl	%eax, %eax
	je	.L238
	jmp	.L231
	.p2align 4,,10
	.p2align 3
.L180:
	movq	24(%rdi), %rax
	movq	(%rax), %rax
	movq	24(%rax), %rax
	movq	(%rax), %rax
	cmpb	$10, (%rax)
	jne	.L283
	call	execStatementFor
	movl	%eax, %r15d
	orl	CODE_ERROR(%rip), %eax
	je	.L238
	jmp	.L170
	.p2align 4,,10
	.p2align 3
.L181:
	call	execConditionBlock
	movl	%eax, %r15d
	testl	%eax, %eax
	je	.L279
	jmp	.L170
	.p2align 4,,10
	.p2align 3
.L182:
	movl	16(%rdi), %eax
	cmpl	$1, %eax
	je	.L295
	cmpl	$2, %eax
	je	.L296
	cmpl	$3, %eax
	jne	.L280
	movl	32(%rdi), %eax
	testl	%eax, %eax
	je	.L297
	movq	process_cycle(%rip), %rdx
	movq	(%rdx), %r10
	movq	96(%r10), %rdx
	cmpq	$0, (%rdx)
	je	.L208
	xorl	%r14d, %r14d
	testl	%eax, %eax
	jle	.L280
	.p2align 4,,10
	.p2align 3
.L209:
	movq	24(%rdi), %rax
	movq	112(%r10), %r8
	leaq	0(,%r14,8), %r15
	xorl	%esi, %esi
	movq	(%rax,%r14,8), %rax
	movl	56(%rax), %r9d
	movq	%r8, %rax
	testq	%r8, %r8
	jne	.L210
	jmp	.L211
	.p2align 4,,10
	.p2align 3
.L298:
	testb	%sil, %sil
	jne	.L213
	movq	8(%rax), %rax
	cmpl	%r9d, 16(%rdx)
	sete	%sil
	testq	%rax, %rax
	je	.L212
.L210:
	movq	(%rax), %rdx
	testq	%rdx, %rdx
	jne	.L298
.L212:
	testb	%sil, %sil
	je	.L211
.L213:
	movq	(%rcx,%r12), %rax
	movq	96(%r10), %r8
	movl	$24, %edi
	addq	$1, %r14
	movq	24(%rax), %rax
	movq	%r8, 16(%rsp)
	movq	(%rax,%r15), %rax
	movl	56(%rax), %eax
	movl	%eax, 8(%rsp)
	call	malloc@PLT
	movl	8(%rsp), %edi
	movq	%rax, %r15
	call	get_var_value@PLT
	movq	%rax, (%r15)
	movl	8(%rsp), %eax
	movq	%rdx, 8(%r15)
	movl	%eax, 16(%r15)
	movl	%eax, %edi
	call	get_absolute_address@PLT
	movq	%rax, %rdi
	call	var_reset@PLT
	movq	16(%rsp), %r8
	movq	%r15, %rsi
	movq	(%r8), %rdi
	call	ptrlist_append@PLT
	movq	24(%rbp), %rcx
	movq	(%rcx,%r12), %rdi
	cmpl	%r14d, 32(%rdi)
	jle	.L280
	movq	process_cycle(%rip), %rax
	movq	(%rax), %r10
	jmp	.L209
	.p2align 4,,10
	.p2align 3
.L183:
	movl	16(%rdi), %r15d
	cmpl	$3, %r15d
	je	.L280
	jmp	.L170
	.p2align 4,,10
	.p2align 3
.L290:
	movl	32(%r12), %edi
	cmpl	$1, %edi
	jle	.L231
	movl	$1, 8(%rsp)
	movq	24(%r12), %rsi
	movl	$8, %r14d
.L195:
	movq	(%rsi,%r14), %rax
	movq	24(%rax), %rax
	movq	(%rax), %rax
	movl	32(%rax), %ecx
	testl	%ecx, %ecx
	jne	.L299
.L187:
	movl	$0, CODE_ERROR(%rip)
	movslq	8(%rsp), %r14
	movq	(%rsi,%r14,8), %rax
	movq	24(%rax), %rax
	movq	8(%rax), %rdi
	call	exec_aux
	movl	%eax, %r15d
	orl	CODE_ERROR(%rip), %eax
	je	.L238
	jmp	.L170
	.p2align 4,,10
	.p2align 3
.L299:
	jle	.L188
	xorl	%r15d, %r15d
	jmp	.L194
	.p2align 4,,10
	.p2align 3
.L191:
	movq	24(%r12), %rsi
	addq	$1, %r15
	movq	(%rsi,%r14), %rax
	movq	24(%rax), %rax
	movq	(%rax), %rax
	cmpl	%r15d, 32(%rax)
	jle	.L300
.L194:
	movq	24(%rax), %rax
	movq	(%rax,%r15,8), %rax
	cmpb	$20, 48(%rax)
	jne	.L301
	movq	40(%rax), %rdi
	movq	48(%rax), %rsi
	call	get_exception_code@PLT
	movslq	CODE_ERROR(%rip), %rdi
	movq	exceptions_err(%rip), %rsi
	movq	%rdi, %rdx
	cmpl	%eax, (%rsi,%rdi,4)
	je	.L190
	testl	%edi, %edi
	jns	.L191
	addl	%edi, %eax
	jne	.L191
.L192:
	movq	$0, EXCEPTION(%rip)
.L277:
	movq	24(%r12), %rsi
	jmp	.L187
	.p2align 4,,10
	.p2align 3
.L300:
	movl	32(%r12), %edi
.L188:
	addl	$1, 8(%rsp)
	movl	8(%rsp), %eax
	addq	$8, %r14
	cmpl	%edi, %eax
	jl	.L195
	movl	%edx, %eax
	jmp	.L185
	.p2align 4,,10
	.p2align 3
.L293:
	movb	$8, %sil
	call	neo_to_bool@PLT
	movl	%eax, %r15d
.L219:
	movq	32(%rsp), %rdi
	movq	40(%rsp), %rsi
	call	neobject_destroy@PLT
	testb	%r15b, %r15b
	je	.L280
	movq	24(%rbp), %rax
	movq	(%rax,%r12), %rax
	movq	24(%rax), %rax
	movq	8(%rax), %rdi
	call	exec_aux
	movl	CODE_ERROR(%rip), %r8d
	movl	%eax, %r15d
	testl	%r8d, %r8d
	jne	.L231
	cmpl	$1, %eax
	ja	.L223
	movq	24(%rbp), %rax
	movq	%r14, %rdi
	movq	(%rax,%r12), %rax
	movq	24(%rax), %rax
	movq	(%rax), %rsi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %edi
	testl	%edi, %edi
	je	.L286
	xorl	%r15d, %r15d
	jmp	.L170
	.p2align 4,,10
	.p2align 3
.L291:
	movq	%rdx, %rdi
	leal	-1(%r14), %esi
	call	nelist_destroy_until@PLT
	xorl	%edx, %edx
	jmp	.L242
	.p2align 4,,10
	.p2align 3
.L294:
	movb	$4, %sil
	call	neo_to_integer@PLT
	testq	%rax, %rax
	setne	%r15b
	jmp	.L219
	.p2align 4,,10
	.p2align 3
.L289:
	movq	24(%rbp), %rax
	movq	32(%rsp), %rsi
	movq	40(%rsp), %rdx
	movq	(%rax,%r12), %rdi
	call	execStatementForeachList
	movl	%eax, %r15d
	jmp	.L235
	.p2align 4,,10
	.p2align 3
.L211:
	movl	$24, %edi
	movq	%r8, 24(%rsp)
	movl	%r9d, 8(%rsp)
	call	malloc@PLT
	movl	8(%rsp), %edi
	movq	%rax, 16(%rsp)
	call	get_var_value@PLT
	movq	16(%rsp), %rsi
	movl	8(%rsp), %r9d
	movq	24(%rsp), %rdi
	movq	%rax, (%rsi)
	movq	%rdx, 8(%rsi)
	movl	%r9d, 16(%rsi)
	call	ptrlist_append@PLT
	movq	process_cycle(%rip), %rax
	movq	24(%rbp), %rcx
	movq	(%rax), %r10
	jmp	.L213
	.p2align 4,,10
	.p2align 3
.L301:
	movl	$78, CODE_ERROR(%rip)
	xorl	%r15d, %r15d
	jmp	.L170
	.p2align 4,,10
	.p2align 3
.L190:
	testl	%edi, %edi
	jns	.L277
	jmp	.L192
	.p2align 4,,10
	.p2align 3
.L223:
	cmpl	$4, %eax
	je	.L170
	jmp	.L280
	.p2align 4,,10
	.p2align 3
.L296:
	movl	32(%rdi), %r10d
	xorl	%r15d, %r15d
	leaq	32(%rsp), %r14
	testl	%r10d, %r10d
	jg	.L205
	jmp	.L280
	.p2align 4,,10
	.p2align 3
.L302:
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
	jne	.L231
	call	gc_mark_and_sweep@PLT
	movq	24(%rbp), %rax
	addq	$1, %r15
	movq	(%rax,%r12), %rdi
	cmpl	%r15d, 32(%rdi)
	jle	.L280
.L205:
	movq	24(%rdi), %rax
	movq	%r14, %rdi
	movq	(%rax,%r15,8), %rsi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %eax
	testl	%eax, %eax
	je	.L302
	jmp	.L231
.L283:
	movl	$22, CODE_ERROR(%rip)
	jmp	.L231
.L295:
	cmpl	$1, 32(%rdi)
	jle	.L198
	movl	$21, CODE_ERROR(%rip)
	jmp	.L231
.L288:
	movl	$109, CODE_ERROR(%rip)
	jmp	.L231
.L236:
	movq	32(%rsp), %rdi
	movq	40(%rsp), %rsi
	call	neobject_destroy@PLT
	movl	$109, CODE_ERROR(%rip)
	jmp	.L231
.L198:
	movq	RETURN_VALUE(%rip), %rdi
	movq	8+RETURN_VALUE(%rip), %rsi
	call	neo_is_void@PLT
	testb	%al, %al
	jne	.L199
	movl	$99, CODE_ERROR(%rip)
	jmp	.L231
.L199:
	movq	24(%rbp), %rax
	movq	(%rax,%r12), %rax
	movl	32(%rax), %edx
	testl	%edx, %edx
	jne	.L200
	call	neo_none_create@PLT
	movq	%rax, RETURN_VALUE(%rip)
	movq	%rdx, 8+RETURN_VALUE(%rip)
.L201:
	movl	$4, %r15d
	jmp	.L170
.L208:
	movl	$70, CODE_ERROR(%rip)
	jmp	.L231
.L297:
	movl	$69, CODE_ERROR(%rip)
	jmp	.L231
.L200:
	movq	24(%rax), %rax
	leaq	RETURN_VALUE(%rip), %rdi
	movq	(%rax), %rsi
	call	eval_prolog
	jmp	.L201
	.cfi_endproc
.LFE42:
	.size	exec_aux, .-exec_aux
	.section	.rodata.str1.1
.LC1:
	.string	"__local_args__"
	.text
	.p2align 4
	.globl	callUserFunc
	.type	callUserFunc, @function
callUserFunc:
.LFB34:
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
	movq	%rcx, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	movq	%rdi, %rbx
	subq	$56, %rsp
	.cfi_def_cfa_offset 112
	movq	process_cycle(%rip), %rax
	movq	%rsi, 32(%rsp)
	movq	(%rax), %rax
	movq	96(%rax), %r13
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%r13, %rdi
	call	ptrlist_append@PLT
	movq	process_cycle(%rip), %rax
	cmpb	$0, 20(%rbx)
	movq	(%rax), %rax
	movq	112(%rax), %rax
	movq	(%rax), %rax
	movq	%rax, 24(%rsp)
	jne	.L360
	movl	16(%rbx), %eax
.L311:
	xorl	%r15d, %r15d
	testl	%eax, %eax
	jle	.L327
	.p2align 4,,10
	.p2align 3
.L324:
	movq	32(%rsp), %rdi
	movl	%r15d, %esi
	movl	%r15d, 44(%rsp)
	leaq	0(,%r15,4), %r13
	call	nelist_nth@PLT
	movq	%rax, %rdi
	movq	%rdx, %rsi
	call	neo_copy@PLT
	xorl	%esi, %esi
	movq	%rax, 8(%rsp)
	movq	8(%rbx), %rax
	movq	%rdx, 16(%rsp)
	movl	(%rax,%r15,4), %r12d
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rdi
	movq	112(%rdi), %rbp
	movq	%rbp, %rax
	testq	%rbp, %rbp
	jne	.L312
	jmp	.L313
	.p2align 4,,10
	.p2align 3
.L361:
	testb	%sil, %sil
	jne	.L315
	movq	8(%rax), %rax
	cmpl	%r12d, 16(%rdx)
	sete	%sil
	testq	%rax, %rax
	je	.L314
.L312:
	movq	(%rax), %rdx
	testq	%rdx, %rdx
	jne	.L361
.L314:
	testb	%sil, %sil
	je	.L313
.L315:
	movq	96(%rdi), %rbp
	movl	$24, %edi
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
	movq	0(%rbp), %rdi
	movq	%r14, %rsi
	call	ptrlist_append@PLT
	movq	8(%rbx), %rax
	movq	16(%rsp), %rdx
	movq	8(%rsp), %rsi
	movl	(%rax,%r13), %edi
	call	replace_var@PLT
	movl	CODE_ERROR(%rip), %edx
	testl	%edx, %edx
	jne	.L362
	addq	$1, %r15
	cmpl	%r15d, 16(%rbx)
	jg	.L324
.L327:
	movq	32(%rbx), %rdi
	call	exec_aux
	movl	%eax, %r12d
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	call	deleteContext
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	112(%rax), %rbp
	movq	24(%rsp), %rax
	cmpq	0(%rbp), %rax
	je	.L326
	.p2align 4,,10
	.p2align 3
.L325:
	movq	%rbp, %rdi
	call	ptrlist_pop@PLT
	movl	16(%rax), %edi
	movq	(%rax), %rsi
	movq	%rax, %rbx
	movq	8(%rax), %rdx
	call	set_var@PLT
	movq	%rbx, %rdi
	call	free@PLT
	movq	24(%rsp), %rax
	cmpq	0(%rbp), %rax
	jne	.L325
.L326:
	movl	CODE_ERROR(%rip), %eax
	testl	%eax, %eax
	jne	.L328
	cmpl	$4, %r12d
	jne	.L330
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
.L362:
	.cfi_restore_state
	cmpl	$1, 44(%rsp)
	movl	$4, %ebp
	jle	.L322
	.p2align 4,,10
	.p2align 3
.L321:
	movq	8(%rbx), %rax
	movl	(%rax,%rbp), %edi
	addq	$4, %rbp
	call	free_var@PLT
	cmpq	%r13, %rbp
	jne	.L321
.L322:
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	call	deleteContext
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	112(%rax), %rbp
	movq	24(%rsp), %rax
	cmpq	0(%rbp), %rax
	je	.L328
	.p2align 4,,10
	.p2align 3
.L319:
	movq	%rbp, %rdi
	call	ptrlist_pop@PLT
	movl	16(%rax), %edi
	movq	(%rax), %rsi
	movq	%rax, %rbx
	movq	8(%rax), %rdx
	call	set_var@PLT
	movq	%rbx, %rdi
	call	free@PLT
	movq	24(%rsp), %rax
	cmpq	0(%rbp), %rax
	jne	.L319
.L328:
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
.L313:
	.cfi_restore_state
	movl	$24, %edi
	call	malloc@PLT
	movl	%r12d, %edi
	movq	%rax, %r14
	call	get_var_value@PLT
	movl	%r12d, 16(%r14)
	movq	%rbp, %rdi
	movq	%r14, %rsi
	movq	%rax, (%r14)
	movq	%rdx, 8(%r14)
	call	ptrlist_append@PLT
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rdi
	movq	8(%rbx), %rax
	movl	(%rax,%r13), %r12d
	jmp	.L315
.L360:
	movq	%rbp, %rsi
	movq	%r12, %rdi
	call	neo_list_len@PLT
	leaq	.LC1(%rip), %rdi
	movl	%eax, %r13d
	call	get_var@PLT
	xorl	%ecx, %ecx
	movl	%eax, %r14d
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rsi
	movq	112(%rsi), %r15
	movq	%r15, %rax
	testq	%r15, %r15
	jne	.L305
	jmp	.L306
	.p2align 4,,10
	.p2align 3
.L363:
	testb	%cl, %cl
	jne	.L308
	movq	8(%rax), %rax
	cmpl	%r14d, 16(%rdx)
	sete	%cl
	testq	%rax, %rax
	je	.L307
.L305:
	movq	(%rax), %rdx
	testq	%rdx, %rdx
	jne	.L363
.L307:
	testb	%cl, %cl
	je	.L306
.L308:
	movq	96(%rsi), %rcx
	movl	$24, %edi
	movq	%rcx, 8(%rsp)
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
	movq	8(%rsp), %rcx
	movq	%r15, %rsi
	movq	(%rcx), %rdi
	call	ptrlist_append@PLT
	movq	%r12, %rsi
	movq	%rbp, %rdx
	movl	%r14d, %edi
	call	replace_var@PLT
	movl	16(%rbx), %eax
	testl	%r13d, %r13d
	jle	.L311
	movq	32(%rsp), %rcx
	movl	%eax, 16(%rcx)
	jmp	.L311
.L330:
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
	jmp	neo_none_create@PLT
.L306:
	.cfi_restore_state
	movl	$24, %edi
	call	malloc@PLT
	movl	%r14d, %edi
	movq	%rax, 8(%rsp)
	call	get_var_value@PLT
	movq	8(%rsp), %rsi
	movq	%r15, %rdi
	movq	%rax, (%rsi)
	movq	%rdx, 8(%rsi)
	movl	%r14d, 16(%rsi)
	call	ptrlist_append@PLT
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rsi
	jmp	.L308
	.cfi_endproc
.LFE34:
	.size	callUserFunc, .-callUserFunc
	.p2align 4
	.globl	callUserMethod
	.type	callUserMethod, @function
callUserMethod:
.LFB35:
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
	movq	%rcx, %r12
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	movq	%r8, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	movq	%rdi, %rbx
	subq	$56, %rsp
	.cfi_def_cfa_offset 112
	movq	process_cycle(%rip), %rax
	movq	%rsi, 40(%rsp)
	movq	(%rax), %rax
	movq	%rdx, 16(%rsp)
	movq	96(%rax), %r13
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%r13, %rdi
	call	ptrlist_append@PLT
	movq	process_cycle(%rip), %rax
	cmpb	$0, 20(%rbx)
	movq	(%rax), %rax
	movq	112(%rax), %rax
	movq	(%rax), %rax
	movq	%rax, 24(%rsp)
	jne	.L427
	movl	16(%rbx), %eax
.L372:
	xorl	%r15d, %r15d
	testl	%eax, %eax
	jle	.L388
	.p2align 4,,10
	.p2align 3
.L385:
	movq	16(%rsp), %rdi
	movl	%r15d, %esi
	movl	%r15d, 36(%rsp)
	leaq	0(,%r15,4), %r13
	call	nelist_nth@PLT
	movq	%rax, %rdi
	movq	%rdx, %rsi
	call	neo_copy@PLT
	xorl	%esi, %esi
	movq	%rax, (%rsp)
	movq	8(%rbx), %rax
	movq	%rdx, 8(%rsp)
	movl	(%rax,%r15,4), %r12d
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rdi
	movq	112(%rdi), %rbp
	movq	%rbp, %rax
	testq	%rbp, %rbp
	jne	.L373
	jmp	.L374
	.p2align 4,,10
	.p2align 3
.L428:
	testb	%sil, %sil
	jne	.L376
	movq	8(%rax), %rax
	cmpl	%r12d, 16(%rdx)
	sete	%sil
	testq	%rax, %rax
	je	.L375
.L373:
	movq	(%rax), %rdx
	testq	%rdx, %rdx
	jne	.L428
.L375:
	testb	%sil, %sil
	je	.L374
.L376:
	movq	96(%rdi), %rbp
	movl	$24, %edi
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
	movq	0(%rbp), %rdi
	movq	%r14, %rsi
	call	ptrlist_append@PLT
	movq	8(%rbx), %rax
	movq	8(%rsp), %rdx
	movq	(%rsp), %rsi
	movl	(%rax,%r13), %edi
	call	replace_var@PLT
	movl	CODE_ERROR(%rip), %edx
	testl	%edx, %edx
	jne	.L429
	addq	$1, %r15
	cmpl	%r15d, 16(%rbx)
	jg	.L385
.L388:
	movq	32(%rbx), %rdi
	call	exec_aux
	movl	%eax, %ebp
	movl	CODE_ERROR(%rip), %eax
	testl	%eax, %eax
	jne	.L430
	movq	8(%rbx), %rax
	movl	(%rax), %edi
	call	get_var_value@PLT
	movq	%rax, %rdi
	movq	%rdx, %rsi
	call	neo_copy@PLT
	movq	%rax, %r12
	movq	process_cycle(%rip), %rax
	movq	%rdx, %r13
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	call	deleteContext
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	112(%rax), %rbx
	movq	24(%rsp), %rax
	cmpq	(%rbx), %rax
	je	.L394
	.p2align 4,,10
	.p2align 3
.L391:
	movq	%rbx, %rdi
	call	ptrlist_pop@PLT
	movl	16(%rax), %edi
	movq	(%rax), %rsi
	movq	%rax, %r14
	movq	8(%rax), %rdx
	call	set_var@PLT
	movq	%r14, %rdi
	call	free@PLT
	movq	24(%rsp), %rax
	cmpq	(%rbx), %rax
	jne	.L391
.L394:
	movq	40(%rsp), %rbx
	movq	(%rbx), %rdi
	movq	8(%rbx), %rsi
	call	neobject_destroy@PLT
	movq	%r12, (%rbx)
	movq	%r13, 8(%rbx)
	cmpl	$4, %ebp
	jne	.L431
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
.L374:
	.cfi_restore_state
	movl	$24, %edi
	call	malloc@PLT
	movl	%r12d, %edi
	movq	%rax, %r14
	call	get_var_value@PLT
	movl	%r12d, 16(%r14)
	movq	%rbp, %rdi
	movq	%r14, %rsi
	movq	%rax, (%r14)
	movq	%rdx, 8(%r14)
	call	ptrlist_append@PLT
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rdi
	movq	8(%rbx), %rax
	movl	(%rax,%r13), %r12d
	jmp	.L376
.L429:
	cmpl	$1, 36(%rsp)
	movl	$4, %ebp
	jle	.L383
	.p2align 4,,10
	.p2align 3
.L382:
	movq	8(%rbx), %rax
	movl	(%rax,%rbp), %edi
	addq	$4, %rbp
	call	free_var@PLT
	cmpq	%r13, %rbp
	jne	.L382
.L383:
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	call	deleteContext
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	112(%rax), %rbp
	movq	24(%rsp), %rax
	cmpq	0(%rbp), %rax
	je	.L390
	.p2align 4,,10
	.p2align 3
.L380:
	movq	%rbp, %rdi
	call	ptrlist_pop@PLT
	movl	16(%rax), %edi
	movq	(%rax), %rsi
	movq	%rax, %rbx
	movq	8(%rax), %rdx
	call	set_var@PLT
	movq	%rbx, %rdi
	call	free@PLT
	movq	24(%rsp), %rax
	cmpq	0(%rbp), %rax
	jne	.L380
.L390:
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
.L427:
	.cfi_restore_state
	movq	%rbp, %rsi
	movq	%r12, %rdi
	call	neo_list_len@PLT
	leaq	.LC1(%rip), %rdi
	movl	%eax, %r13d
	call	get_var@PLT
	xorl	%ecx, %ecx
	movl	%eax, %r14d
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rsi
	movq	112(%rsi), %r15
	movq	%r15, %rax
	testq	%r15, %r15
	jne	.L366
	jmp	.L367
	.p2align 4,,10
	.p2align 3
.L432:
	testb	%cl, %cl
	jne	.L369
	movq	8(%rax), %rax
	cmpl	%r14d, 16(%rdx)
	sete	%cl
	testq	%rax, %rax
	je	.L368
.L366:
	movq	(%rax), %rdx
	testq	%rdx, %rdx
	jne	.L432
.L368:
	testb	%cl, %cl
	je	.L367
.L369:
	movq	96(%rsi), %rcx
	movl	$24, %edi
	movq	%rcx, (%rsp)
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
	movq	(%rsp), %rcx
	movq	%r15, %rsi
	movq	(%rcx), %rdi
	call	ptrlist_append@PLT
	movq	%r12, %rsi
	movq	%rbp, %rdx
	movl	%r14d, %edi
	call	replace_var@PLT
	movl	16(%rbx), %eax
	testl	%r13d, %r13d
	jle	.L372
	movq	16(%rsp), %rcx
	movl	%eax, 16(%rcx)
	jmp	.L372
.L430:
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	call	deleteContext
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	112(%rax), %rbp
	movq	24(%rsp), %rax
	cmpq	0(%rbp), %rax
	je	.L390
	.p2align 4,,10
	.p2align 3
.L389:
	movq	%rbp, %rdi
	call	ptrlist_pop@PLT
	movl	16(%rax), %edi
	movq	(%rax), %rsi
	movq	%rax, %rbx
	movq	8(%rax), %rdx
	call	set_var@PLT
	movq	%rbx, %rdi
	call	free@PLT
	movq	24(%rsp), %rax
	cmpq	0(%rbp), %rax
	jne	.L389
	jmp	.L390
.L431:
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
	jmp	neo_none_create@PLT
.L367:
	.cfi_restore_state
	movl	$24, %edi
	call	malloc@PLT
	movl	%r14d, %edi
	movq	%rax, (%rsp)
	call	get_var_value@PLT
	movq	(%rsp), %rsi
	movq	%r15, %rdi
	movq	%rax, (%rsi)
	movq	%rdx, 8(%rsi)
	movl	%r14d, 16(%rsi)
	call	ptrlist_append@PLT
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rsi
	jmp	.L369
	.cfi_endproc
.LFE35:
	.size	callUserMethod, .-callUserMethod
	.p2align 4
	.globl	eval_aux
	.type	eval_aux, @function
eval_aux:
.LFB36:
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
	ja	.L434
	testb	%al, %al
	je	.L435
	cmpb	$54, %al
	ja	.L435
	leaq	.L437(%rip), %rdx
	movslq	(%rdx,%rax,4), %rax
	addq	%rdx, %rax
	jmp	*%rax
	.section	.rodata
	.align 4
	.align 4
.L437:
	.long	.L435-.L437
	.long	.L442-.L437
	.long	.L441-.L437
	.long	.L435-.L437
	.long	.L439-.L437
	.long	.L439-.L437
	.long	.L435-.L437
	.long	.L435-.L437
	.long	.L439-.L437
	.long	.L435-.L437
	.long	.L440-.L437
	.long	.L435-.L437
	.long	.L435-.L437
	.long	.L435-.L437
	.long	.L435-.L437
	.long	.L435-.L437
	.long	.L435-.L437
	.long	.L439-.L437
	.long	.L435-.L437
	.long	.L435-.L437
	.long	.L439-.L437
	.long	.L435-.L437
	.long	.L435-.L437
	.long	.L435-.L437
	.long	.L435-.L437
	.long	.L435-.L437
	.long	.L435-.L437
	.long	.L435-.L437
	.long	.L435-.L437
	.long	.L435-.L437
	.long	.L435-.L437
	.long	.L435-.L437
	.long	.L435-.L437
	.long	.L435-.L437
	.long	.L435-.L437
	.long	.L435-.L437
	.long	.L435-.L437
	.long	.L435-.L437
	.long	.L435-.L437
	.long	.L435-.L437
	.long	.L435-.L437
	.long	.L435-.L437
	.long	.L435-.L437
	.long	.L435-.L437
	.long	.L435-.L437
	.long	.L435-.L437
	.long	.L435-.L437
	.long	.L435-.L437
	.long	.L435-.L437
	.long	.L435-.L437
	.long	.L435-.L437
	.long	.L435-.L437
	.long	.L438-.L437
	.long	.L435-.L437
	.long	.L436-.L437
	.text
	.p2align 4,,10
	.p2align 3
.L434:
	cmpb	$-122, %al
	je	.L443
	jbe	.L658
	cmpb	$-77, %al
	jne	.L435
	movl	32(%rdi), %edi
	xorl	%ebx, %ebx
	call	nelist_create@PLT
	movl	32(%r14), %r8d
	movq	%rax, %rbp
	testl	%r8d, %r8d
	jg	.L561
.L564:
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
.L562:
	.cfi_restore_state
	addq	$1, %rbx
	cmpl	%ebx, 32(%r14)
	jle	.L564
.L561:
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
	je	.L562
	leal	-1(%rbx), %esi
	movq	%rbp, %rdi
	call	nelist_destroy_until@PLT
	xorl	%eax, %eax
	movl	$55, %edx
	jmp	.L638
	.p2align 4,,10
	.p2align 3
.L658:
	cmpb	$-128, %al
	jne	.L659
	movq	40(%rdi), %rdi
	movq	48(%r14), %rsi
	call	neo_is_void@PLT
	testb	%al, %al
	jne	.L660
.L482:
	movzbl	48(%r14), %eax
	cmpb	$-128, %al
	je	.L661
	leal	79(%rax), %edx
	cmpb	$1, %dl
	jbe	.L662
	cmpb	$55, %al
	je	.L663
	movl	$9, CODE_ERROR(%rip)
.L656:
	movq	40(%r14), %rdi
	movq	48(%r14), %rsi
.L652:
	call	neobject_destroy@PLT
	pxor	%xmm0, %xmm0
	movups	%xmm0, 40(%r14)
	movb	$55, 48(%r14)
	jmp	.L653
	.p2align 4,,10
	.p2align 3
.L659:
	cmpb	$-125, %al
	jne	.L435
.L439:
	movq	40(%r14), %rdi
	movq	48(%r14), %rsi
.L655:
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
.L435:
	.cfi_restore_state
	movl	$19, CODE_ERROR(%rip)
.L653:
	xorl	%eax, %eax
	movl	$55, %edx
.L638:
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
.L440:
	.cfi_restore_state
	movl	56(%rdi), %edi
	call	get_var_value@PLT
	movq	%rax, %rdi
	movq	%rdx, %rsi
	cmpb	$55, %dl
	jne	.L655
	movl	$5, CODE_ERROR(%rip)
	jmp	.L653
	.p2align 4,,10
	.p2align 3
.L438:
	movq	24(%rdi), %rax
	leaq	64(%rsp), %rdi
	movq	(%rax), %rsi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %esi
	testl	%esi, %esi
	jne	.L653
	cmpb	$-77, 72(%rsp)
	movq	64(%rsp), %rdi
	movq	72(%rsp), %rsi
	jne	.L664
	call	neo_to_container@PLT
	movl	16(%r14), %esi
	movq	%rax, %rbx
	cmpl	$-1, %esi
	je	.L665
.L568:
	movq	%rbx, %rdi
	call	get_container_field@PLT
	movq	%rax, %rdi
	movq	%rdx, %rsi
	call	neo_copy@PLT
.L646:
	movq	%rax, %rbp
	movq	%rdx, %rbx
.L647:
	movq	64(%rsp), %rdi
	movq	72(%rsp), %rsi
	call	neobject_destroy@PLT
	movq	%rbp, %rax
	movq	%rbx, %rdx
	jmp	.L638
	.p2align 4,,10
	.p2align 3
.L436:
	cmpl	$1, 32(%rdi)
	jle	.L569
	movl	$101, CODE_ERROR(%rip)
	xorl	%eax, %eax
	movl	$55, %edx
	jmp	.L638
	.p2align 4,,10
	.p2align 3
.L442:
	movq	24(%rdi), %rax
	movq	(%rax), %rdi
	call	get_address
	movl	CODE_ERROR(%rip), %r10d
	movq	(%rax), %r12
	movq	8(%rax), %rbp
	movzbl	8(%rax), %ebx
	testl	%r10d, %r10d
	jne	.L653
	movq	24(%r14), %rax
	leaq	64(%rsp), %rdi
	movq	8(%rax), %rsi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %r9d
	testl	%r9d, %r9d
	jne	.L653
	cmpb	$-122, %bl
	je	.L554
	cmpb	$-125, %bl
	je	.L554
	movl	$15, CODE_ERROR(%rip)
	xorl	%eax, %eax
	movl	$55, %edx
	jmp	.L638
	.p2align 4,,10
	.p2align 3
.L441:
	movl	32(%rdi), %edi
	cmpl	$1, %edi
	je	.L666
	cmpl	$2, %edi
	je	.L667
.L457:
	call	nelist_create@PLT
	movl	32(%r14), %r10d
	movq	%rax, %rbp
	testl	%r10d, %r10d
	jle	.L479
	xorl	%ebx, %ebx
	jmp	.L480
	.p2align 4,,10
	.p2align 3
.L669:
	call	eval_prolog
	movl	CODE_ERROR(%rip), %r9d
	testl	%r9d, %r9d
	jne	.L668
.L478:
	addq	$1, %rbx
	cmpl	%ebx, 32(%r14)
	jle	.L479
.L480:
	movq	24(%r14), %rax
	movq	%rbx, %rdi
	salq	$4, %rdi
	addq	8(%rbp), %rdi
	movq	(%rax,%rbx,8), %rsi
	testq	%rsi, %rsi
	jne	.L669
	pxor	%xmm0, %xmm0
	movups	%xmm0, (%rdi)
	movb	$55, 8(%rdi)
	jmp	.L478
	.p2align 4,,10
	.p2align 3
.L668:
	movq	%rbp, %rdi
	leal	-1(%rbx), %esi
	xorl	%ebp, %ebp
	call	nelist_destroy_until@PLT
.L479:
	movl	CODE_ERROR(%rip), %r8d
	testl	%r8d, %r8d
	jne	.L653
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
.L443:
	.cfi_restore_state
	movl	32(%rdi), %edi
	jmp	.L457
	.p2align 4,,10
	.p2align 3
.L660:
	movq	24(%r14), %rax
	leaq	40(%r14), %rdi
	movq	(%rax), %rsi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %edi
	testl	%edi, %edi
	je	.L482
	jmp	.L653
	.p2align 4,,10
	.p2align 3
.L664:
	call	neobject_destroy@PLT
	xorl	%eax, %eax
	movl	$55, %edx
	movl	$80, CODE_ERROR(%rip)
	jmp	.L638
	.p2align 4,,10
	.p2align 3
.L667:
	movslq	16(%r14), %rcx
	movq	OPERANDES(%rip), %rdx
	movl	(%rdx,%rcx,4), %edx
	testb	$1, %dh
	jne	.L670
	cmpl	$37, %ecx
	je	.L671
	cmpl	$35, %ecx
	je	.L672
	movq	24(%r14), %rax
	movq	(%rax), %rsi
	testb	$8, %dl
	jne	.L673
	testb	$16, %dl
	jne	.L674
	andl	$64, %edx
	je	.L469
	movq	%rsi, %rdi
	call	get_address
	movl	CODE_ERROR(%rip), %r12d
	movq	%rax, %rbx
	testl	%r12d, %r12d
	jne	.L653
	movq	24(%r14), %rax
	movq	8(%rax), %rdi
	call	get_address
	movl	CODE_ERROR(%rip), %ebp
	movq	%rax, %rsi
	testl	%ebp, %ebp
	jne	.L653
	movslq	16(%r14), %rdx
	leaq	OPFONC(%rip), %rax
	movq	%rbx, %rdi
	movq	(%rax,%rdx,8), %rax
.L654:
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
.L663:
	.cfi_restore_state
	movl	$8, CODE_ERROR(%rip)
	jmp	.L656
	.p2align 4,,10
	.p2align 3
.L569:
	movq	24(%rdi), %rax
	leaq	64(%rsp), %rdi
	movq	(%rax), %rsi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %ecx
	movq	64(%rsp), %rdi
	movq	72(%rsp), %rsi
	testl	%ecx, %ecx
	jne	.L675
	movzbl	72(%rsp), %eax
	movq	%rsi, %rdx
	cmpb	$8, %al
	je	.L676
	cmpb	$4, %al
	je	.L677
	movl	$20, CODE_ERROR(%rip)
.L575:
	call	neobject_destroy@PLT
	movl	atomic_counter(%rip), %edx
	testl	%edx, %edx
	js	.L651
	movl	$0, atomic_counter(%rip)
.L651:
	movq	process_cycle(%rip), %rax
	movl	$55, %edx
	movq	(%rax), %rax
	movq	%r14, 64(%rax)
	movq	$-1, %rax
	jmp	.L638
	.p2align 4,,10
	.p2align 3
.L666:
	movq	24(%r14), %rdx
	movslq	16(%r14), %rax
	movq	(%rdx), %rbx
	cmpl	$39, %eax
	je	.L678
	movq	OPERANDES(%rip), %rdx
	movl	(%rdx,%rax,4), %edx
	testb	$1, %dh
	jne	.L679
	andl	$33, %edx
	je	.L453
	movq	%rbx, %rdi
	call	get_address
	movl	CODE_ERROR(%rip), %edx
	testl	%edx, %edx
	jne	.L653
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
.L554:
	.cfi_restore_state
	cmpb	$4, 72(%rsp)
	movq	64(%rsp), %rdi
	movq	72(%rsp), %rsi
	je	.L555
	call	neobject_destroy@PLT
	xorl	%eax, %eax
	movl	$55, %edx
	movl	$16, CODE_ERROR(%rip)
	jmp	.L638
	.p2align 4,,10
	.p2align 3
.L662:
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
	jg	.L494
	testl	%ecx, %ecx
	jle	.L680
.L495:
	movq	24(%rax), %rdi
	movl	$0, 36(%rsp)
	xorl	%eax, %eax
	jmp	.L502
	.p2align 4,,10
	.p2align 3
.L499:
	addq	$1, %rax
	cmpl	%eax, %ecx
	jle	.L496
.L502:
	movq	(%rdi,%rax,8), %rdx
	cmpb	$2, (%rdx)
	jne	.L499
	cmpl	$37, 16(%rdx)
	jne	.L499
	testl	%r8d, %r8d
	jle	.L499
	movq	24(%rdx), %rdx
	movq	8(%rbx), %rsi
	movq	(%rdx), %rdx
	movl	56(%rdx), %r10d
	xorl	%edx, %edx
	jmp	.L500
	.p2align 4,,10
	.p2align 3
.L501:
	addl	$1, %edx
	addq	$4, %rsi
	cmpl	%edx, %r8d
	je	.L499
.L500:
	cmpl	%r10d, (%rsi)
	jne	.L501
	movl	%r8d, %esi
	subl	24(%rbx), %esi
	cmpl	%edx, %esi
	jg	.L499
	cmpl	%edx, %r8d
	setg	%dl
	cmpb	$1, %dl
	sbbl	$-1, 36(%rsp)
	jmp	.L499
	.p2align 4,,10
	.p2align 3
.L661:
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
	jle	.L487
	xorl	%ebx, %ebx
	jmp	.L488
	.p2align 4,,10
	.p2align 3
.L682:
	call	eval_prolog
	movl	CODE_ERROR(%rip), %ecx
	testl	%ecx, %ecx
	jne	.L681
.L486:
	addq	$1, %rbx
	cmpl	%ebx, 32(%r13)
	jle	.L487
.L488:
	movq	24(%r13), %rax
	movq	%rbx, %rdi
	salq	$4, %rdi
	addq	8(%r12), %rdi
	movq	(%rax,%rbx,8), %rsi
	testq	%rsi, %rsi
	jne	.L682
	pxor	%xmm0, %xmm0
	movups	%xmm0, (%rdi)
	movb	$55, 8(%rdi)
	jmp	.L486
.L681:
	movq	%r12, %rdi
	leal	-1(%rbx), %esi
	xorl	%r12d, %r12d
	call	nelist_destroy_until@PLT
.L487:
	movdqa	(%rsp), %xmm4
	movl	CODE_ERROR(%rip), %edx
	movups	%xmm4, 40(%r14)
	testl	%edx, %edx
	jne	.L656
	movq	%r12, %rsi
	movq	%rbp, %rdi
	call	funcArgsCheck@PLT
	testb	%al, %al
	je	.L683
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
	jne	.L684
	movl	4(%r14), %eax
	movq	%rbx, %rdx
	movl	%eax, LINENUMBER(%rip)
	movq	%rbp, %rax
	jmp	.L638
.L680:
	xorl	%eax, %eax
	movl	%eax, 36(%rsp)
.L496:
	testb	%r11b, %r11b
	jne	.L579
.L503:
	movl	%r8d, %edi
	call	nelist_create@PLT
	movslq	16(%rax), %rdx
	movq	%rax, %r13
	testl	%edx, %edx
	jle	.L508
	movq	8(%rax), %rax
	salq	$4, %rdx
	addq	%rax, %rdx
	.p2align 4,,10
	.p2align 3
.L507:
	pxor	%xmm0, %xmm0
	addq	$16, %rax
	movups	%xmm0, -16(%rax)
	movb	$55, -8(%rax)
	cmpq	%rax, %rdx
	jne	.L507
.L508:
	movl	16(%rbx), %eax
	movl	%eax, %ebp
	subl	24(%rbx), %ebp
	movslq	%ebp, %r15
	salq	$4, %r15
	cmpl	%ebp, %eax
	jle	.L506
	.p2align 4,,10
	.p2align 3
.L512:
	testl	%ebp, %ebp
	jne	.L511
	cmpb	$-79, 48(%r14)
	je	.L657
.L511:
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
	jg	.L512
.L506:
	movq	$0, 40(%rsp)
	movq	24(%r14), %rdi
	xorl	%ebp, %ebp
	movq	8(%rdi), %rsi
	movl	32(%rsi), %edx
	testl	%edx, %edx
	jg	.L509
	jmp	.L510
	.p2align 4,,10
	.p2align 3
.L513:
	addq	$1, %rbp
	cmpl	%ebp, %edx
	jle	.L510
.L509:
	movq	24(%rsi), %rax
	leaq	0(,%rbp,8), %r15
	movq	(%rax,%rbp,8), %rax
	cmpb	$2, (%rax)
	jne	.L513
	cmpl	$37, 16(%rax)
	jne	.L513
	movq	24(%rax), %rax
	movq	(%rax), %rax
	cmpb	$10, (%rax)
	jne	.L517
	movl	16(%rbx), %r8d
	testl	%r8d, %r8d
	jle	.L581
	movl	56(%rax), %ecx
	xorl	%r12d, %r12d
	movq	8(%rbx), %rax
	jmp	.L516
	.p2align 4,,10
	.p2align 3
.L518:
	addl	$1, %r12d
	addq	$4, %rax
	cmpl	%r8d, %r12d
	je	.L517
.L516:
	cmpl	%ecx, (%rax)
	jne	.L518
.L515:
	cmpl	%r8d, %r12d
	je	.L517
	movslq	%r12d, %rcx
	subl	24(%rbx), %r8d
	movq	8(%r13), %rax
	salq	$4, %rcx
	cmpl	%r12d, %r8d
	jle	.L520
	movq	(%rax,%rcx), %rdi
	movq	8(%rax,%rcx), %rsi
	movq	%rcx, 24(%rsp)
	call	neo_is_void@PLT
	testb	%al, %al
	je	.L521
	movq	24(%r14), %rax
	movq	24(%rsp), %rcx
	movq	8(%rax), %rsi
.L520:
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
	jne	.L685
	movq	24(%r14), %rdi
	cmpb	$-79, 35(%rsp)
	movq	8(%rdi), %rsi
	jne	.L642
	testl	%r12d, %r12d
	je	.L523
.L642:
	movl	32(%rsi), %edx
	jmp	.L513
.L675:
	call	neobject_destroy@PLT
	xorl	%eax, %eax
	movl	$55, %edx
	jmp	.L638
.L555:
	call	neo_to_integer@PLT
	movq	64(%rsp), %rdi
	movq	72(%rsp), %rsi
	movq	%rax, %r13
	call	neobject_destroy@PLT
	cmpb	$-122, %bl
	je	.L556
	testq	%r13, %r13
	js	.L558
	movb	$-125, %bpl
	movq	%r12, %rdi
	movq	%rbp, %rsi
	call	neo_to_string@PLT
	movq	%rax, %rdi
	call	strlen@PLT
	cmpq	%rax, %r13
	jnb	.L558
	movq	%rbp, %rsi
	movq	%r12, %rdi
	call	neo_to_string@PLT
	movsbl	(%rax,%r13), %edi
	call	charToString@PLT
	movq	%rax, %rdi
	call	neo_str_create@PLT
	movl	%edx, %ecx
	movb	%cl, %dl
	jmp	.L638
.L679:
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
.L453:
	.cfi_restore_state
	leaq	64(%rsp), %rdi
	movq	%rbx, %rsi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %eax
	testl	%eax, %eax
	jne	.L653
	movslq	16(%r14), %rdx
	leaq	OPFONC(%rip), %rax
	movq	64(%rsp), %rdi
	movq	72(%rsp), %rsi
	call	*(%rax,%rdx,8)
	jmp	.L646
.L494:
	testb	%r11b, %r11b
	je	.L686
	testl	%ecx, %ecx
	jg	.L495
	xorl	%eax, %eax
	movl	%eax, 36(%rsp)
.L579:
	movl	24(%rbx), %eax
	movl	%r8d, %edx
	movl	36(%rsp), %esi
	subl	%eax, %edx
	addl	%ecx, %eax
	subl	%esi, %eax
	cmpl	%edx, %ecx
	cmovg	%eax, %r8d
	jmp	.L503
.L510:
	movl	16(%r13), %eax
	xorl	%r15d, %r15d
	testl	%eax, %eax
	jg	.L525
	jmp	.L687
	.p2align 4,,10
	.p2align 3
.L528:
	addq	$1, %r15
	cmpl	%r15d, %eax
	jle	.L688
.L525:
	movq	%r15, %rax
	movl	%r15d, %ebp
	salq	$4, %rax
	addq	8(%r13), %rax
	movq	(%rax), %rdi
	movq	8(%rax), %rsi
	call	neo_is_void@PLT
	testb	%al, %al
	movl	16(%r13), %eax
	je	.L528
.L527:
	movq	24(%r14), %rdx
	movq	8(%rdx), %r8
	movl	32(%r8), %edi
	cmpl	%eax, %r15d
	jge	.L526
	xorl	%ebp, %ebp
	jmp	.L529
	.p2align 4,,10
	.p2align 3
.L531:
	addq	$1, %rbp
	cmpl	%eax, %r15d
	jge	.L526
.L529:
	cmpl	%ebp, %edi
	jle	.L526
	movq	24(%r8), %rdx
	leaq	0(,%rbp,8), %rcx
	movq	%rcx, 24(%rsp)
	movq	(%rdx,%rbp,8), %rsi
	cmpb	$2, (%rsi)
	jne	.L530
	cmpl	$37, 16(%rsi)
	je	.L531
.L530:
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
	jne	.L689
	cmpb	$-79, 35(%rsp)
	jne	.L533
	testl	%r15d, %r15d
	je	.L690
.L533:
	cmpl	%r15d, 16(%r13)
	jg	.L534
	jmp	.L535
	.p2align 4,,10
	.p2align 3
.L536:
	addl	$1, %r15d
	addq	$16, %r12
	cmpl	%r15d, 16(%r13)
	jle	.L535
.L534:
	movq	8(%r13), %rax
	movq	(%rax,%r12), %rdi
	movq	8(%rax,%r12), %rsi
	call	neo_is_void@PLT
	testb	%al, %al
	je	.L536
	movq	24(%r14), %rdx
	movl	16(%r13), %eax
	movq	8(%rdx), %r8
	movl	32(%r8), %edi
	jmp	.L531
	.p2align 4,,10
	.p2align 3
.L535:
	movq	24(%r14), %rax
	movq	8(%rax), %rax
	movl	32(%rax), %edi
.L526:
	movl	16(%rbx), %eax
	cmpl	%edi, %eax
	jg	.L691
.L538:
	xorl	%ecx, %ecx
	cmpb	$0, 20(%rbx)
	movl	$55, %r8d
	je	.L542
	movl	16(%r13), %ebp
	movl	36(%rsp), %edx
	movl	%ebp, %ecx
	subl	%edx, %ecx
	movl	%eax, %edx
	subl	24(%rbx), %edx
	cmpl	%edx, %ecx
	jg	.L543
	xorl	%edi, %edi
	call	nelist_create@PLT
	movq	%rax, %r15
.L544:
	movq	%r15, %rdi
	call	neo_list_convert@PLT
	movq	%rax, %rcx
	movq	%rdx, %r8
.L542:
	pxor	%xmm0, %xmm0
	cmpb	$-79, 35(%rsp)
	movups	%xmm0, 40(%r14)
	movb	$55, 48(%r14)
	je	.L692
	movq	%rcx, %rdx
	movq	%rbx, %rdi
	movq	%r8, %rcx
	movq	%r13, %rsi
	call	callUserFunc
	movq	%rax, %rbp
	movq	%rdx, %rbx
.L548:
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
	jmp	.L638
.L694:
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
	jne	.L693
	testq	%rbp, %rbp
	jne	.L539
.L657:
	movl	$110, CODE_ERROR(%rip)
	movq	%r13, %rdi
	call	nelist_destroy@PLT
	jmp	.L656
.L670:
	movq	24(%r14), %rax
	movq	8(%rax), %rsi
	movq	(%rax), %rdi
	leaq	OPFONC(%rip), %rax
	movq	(%rax,%rcx,8), %rax
	jmp	.L654
.L678:
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
	jne	.L644
	cmpb	$-78, 48(%rbp)
	je	.L451
	movq	40(%rbp), %rdi
	movq	48(%rbp), %rsi
	movl	$100, CODE_ERROR(%rip)
	call	neobject_destroy@PLT
.L644:
	movq	%rbp, %rdi
	call	free@PLT
	xorl	%eax, %eax
	movl	$55, %edx
	jmp	.L638
.L665:
	movq	8(%r14), %rsi
	movq	%rax, %rdi
	call	get_field_index@PLT
	movq	8(%r14), %rdi
	movl	%eax, 16(%r14)
	call	free@PLT
	movq	$0, 8(%r14)
	movl	16(%r14), %esi
	jmp	.L568
.L690:
	movq	24(%r14), %rax
	movq	24(%rsp), %rcx
	movq	8(%rax), %rax
	movq	24(%rax), %rax
	movq	(%rax,%rcx), %rdi
	call	get_address
	movq	%rax, 40(%rsp)
	jmp	.L533
.L683:
	movl	$14, CODE_ERROR(%rip)
	movq	%r12, %rdi
	call	nelist_destroy@PLT
	jmp	.L656
.L558:
	movl	$18, CODE_ERROR(%rip)
	xorl	%eax, %eax
	movl	$55, %edx
	jmp	.L638
.L688:
	leal	1(%rbp), %r15d
	jmp	.L527
.L671:
	movl	$95, CODE_ERROR(%rip)
	xorl	%eax, %eax
	movl	$55, %edx
	jmp	.L638
.L673:
	movq	%rsi, %rdi
	call	get_address
	movq	%rax, %rbx
	movl	CODE_ERROR(%rip), %eax
	testl	%eax, %eax
	jne	.L653
	movq	24(%r14), %rax
	leaq	64(%rsp), %rdi
	movq	8(%rax), %rsi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %r15d
	testl	%r15d, %r15d
	jne	.L653
	movslq	16(%r14), %rcx
	leaq	OPFONC(%rip), %rax
	movq	64(%rsp), %rsi
	movq	%rbx, %rdi
	movq	72(%rsp), %rdx
	call	*(%rax,%rcx,8)
	jmp	.L646
.L684:
	movq	%rbp, %rdi
	movq	%rbx, %rsi
	call	neobject_destroy@PLT
	xorl	%eax, %eax
	movl	$55, %edx
	jmp	.L638
.L672:
	movl	$92, CODE_ERROR(%rip)
	xorl	%eax, %eax
	movl	$55, %edx
	jmp	.L638
.L691:
	testl	%eax, %eax
	jle	.L538
	xorl	%ebp, %ebp
.L541:
	movq	8(%r13), %rax
	movq	%rbp, %r15
	salq	$4, %r15
	movq	(%rax,%r15), %rdi
	movq	8(%rax,%r15), %rsi
	call	neo_is_void@PLT
	testb	%al, %al
	jne	.L694
.L539:
	movl	16(%rbx), %eax
	addq	$1, %rbp
	cmpl	%ebp, %eax
	jg	.L541
	jmp	.L538
	.p2align 4,,10
	.p2align 3
.L469:
	leaq	48(%rsp), %rdi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %ebx
	testl	%ebx, %ebx
	jne	.L695
	movq	24(%r14), %rax
	leaq	64(%rsp), %rdi
	movq	8(%rax), %rsi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %r11d
	testl	%r11d, %r11d
	jne	.L696
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
	jmp	.L647
.L676:
	movb	$8, %dl
	movq	%rdx, %rsi
	call	neo_to_bool@PLT
.L573:
	movq	64(%rsp), %rdi
	movq	72(%rsp), %rsi
	testb	%al, %al
	je	.L575
	call	neobject_destroy@PLT
	call	neo_none_create@PLT
	jmp	.L638
.L556:
	movq	%r12, %rdi
	movq	%rbp, %rsi
	call	neo_list_len@PLT
	cltq
	cmpq	%r13, %rax
	jle	.L558
	testq	%r13, %r13
	js	.L558
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
	jmp	.L638
.L674:
	leaq	64(%rsp), %rdi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %r13d
	testl	%r13d, %r13d
	jne	.L653
	movq	24(%r14), %rax
	movq	8(%rax), %rdi
	call	get_address
	cmpl	$0, CODE_ERROR(%rip)
	movq	%rax, %rdx
	jne	.L653
	movslq	16(%r14), %rcx
	leaq	OPFONC(%rip), %rax
	movq	64(%rsp), %rdi
	movq	72(%rsp), %rsi
	call	*(%rax,%rcx,8)
	jmp	.L646
.L686:
	movl	$6, CODE_ERROR(%rip)
	movq	16(%rsp), %rdi
	movq	(%rsp), %rsi
	jmp	.L652
.L451:
	movq	%rbp, %rdi
	movl	$1, %esi
	call	create_new_process
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
.L523:
	.cfi_restore_state
	movq	24(%rsi), %rax
	movq	(%rax,%r15), %rax
	movq	24(%rax), %rax
	movq	8(%rax), %rdi
	call	get_address
	movq	24(%r14), %rdi
	movq	%rax, 40(%rsp)
	movq	8(%rdi), %rsi
	jmp	.L642
.L689:
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
	jmp	.L638
.L677:
	movb	$4, %dl
	movq	%rdx, %rsi
	call	neo_to_integer@PLT
	testq	%rax, %rax
	setne	%al
	jmp	.L573
.L692:
	movq	40(%rsp), %rsi
	movq	%rbx, %rdi
	movq	%r13, %rdx
	call	callUserMethod
	movq	%rax, %rbp
	movq	%rdx, %rbx
	jmp	.L548
.L543:
	subl	%eax, %ebp
	movl	%ebp, %edi
	call	nelist_create@PLT
	movq	%rax, %r15
	testl	%ebp, %ebp
	jle	.L544
	movslq	%ebp, %r12
	xorl	%ebp, %ebp
	.p2align 4,,10
	.p2align 3
.L546:
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
	jne	.L546
	jmp	.L544
.L581:
	xorl	%r12d, %r12d
	jmp	.L515
.L517:
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
	jmp	.L638
.L695:
	movq	48(%rsp), %rdi
	movq	56(%rsp), %rsi
	call	neobject_destroy@PLT
	xorl	%eax, %eax
	movl	$55, %edx
	jmp	.L638
.L696:
	movq	48(%rsp), %rdi
	movq	56(%rsp), %rsi
	call	neobject_destroy@PLT
	movq	64(%rsp), %rdi
	movq	72(%rsp), %rsi
	call	neobject_destroy@PLT
	xorl	%eax, %eax
	movl	$55, %edx
	jmp	.L638
.L687:
	movq	8(%rdi), %rax
	movl	32(%rax), %edi
	jmp	.L526
.L685:
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
	jmp	.L638
.L521:
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
	jmp	.L638
.L693:
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
	jmp	.L638
	.cfi_endproc
.LFE36:
	.size	eval_aux, .-eval_aux
	.section	.rodata.str1.1
.LC2:
	.string	"switch registers"
.LC3:
	.string	"Appel eval %p   %d\n"
.LC4:
	.string	"Retour eval %p\n"
	.text
	.p2align 4
	.globl	eval_prolog
	.type	eval_prolog, @function
eval_prolog:
.LFB32:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movq	%rdi, %rbx
	movq	process_cycle(%rip), %rdi
	movq	(%rdi), %rax
	testq	%rsi, %rsi
	je	.L708
	movq	%rsi, 64(%rax)
	movq	%rbx, 72(%rax)
.L702:
	movl	atomic_counter(%rip), %edx
	testl	%edx, %edx
	jne	.L700
	cmpq	%rdi, 8(%rdi)
	je	.L703
	call	nextProcess_noinline
	leaq	.LC2(%rip), %rdi
	movq	%rax, process_cycle(%rip)
	call	puts@PLT
	movq	process_cycle(%rip), %rax
	xorl	%ecx, %ecx
	movq	(%rax), %rdi
	movq	16(%rax), %rax
	movq	(%rax), %rsi
	cmpq	$0, 88(%rdi)
	sete	%cl
	xorl	%edx, %edx
	cmpq	$0, 88(%rsi)
	sete	%dl
.L707:
	call	switch_registers@PLT
	movl	ATOMIC_TIME(%rip), %edx
	movl	CODE_ERROR(%rip), %eax
	movl	%edx, atomic_counter(%rip)
	testl	%eax, %eax
	jne	.L699
	movq	process_cycle(%rip), %rdi
.L700:
	movq	(%rdi), %rax
	subl	$1, %edx
	leaq	.LC3(%rip), %rdi
	movl	%edx, atomic_counter(%rip)
	movq	64(%rax), %rsi
	xorl	%eax, %eax
	call	printf@PLT
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	64(%rax), %rdi
	call	eval_aux
	leaq	.LC4(%rip), %rdi
	movq	%rax, (%rbx)
	movq	process_cycle(%rip), %rax
	movq	%rdx, 8(%rbx)
	movq	(%rax), %rax
	movq	64(%rax), %rsi
	xorl	%eax, %eax
	call	printf@PLT
	cmpb	$55, 8(%rbx)
	je	.L709
.L701:
	movq	%rbx, %rax
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L703:
	.cfi_restore_state
	movl	ATOMIC_TIME(%rip), %edx
	jmp	.L700
	.p2align 4,,10
	.p2align 3
.L708:
	xorl	%ecx, %ecx
	cmpq	$0, 88(%rax)
	movl	$1, %edx
	movq	%rax, %rdi
	sete	%cl
	jmp	.L707
	.p2align 4,,10
	.p2align 3
.L699:
	pxor	%xmm0, %xmm0
	movq	%rbx, %rax
	movups	%xmm0, (%rbx)
	movb	$55, 8(%rbx)
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L709:
	.cfi_restore_state
	cmpq	$-1, (%rbx)
	movq	process_cycle(%rip), %rdi
	jne	.L701
	jmp	.L702
	.cfi_endproc
.LFE32:
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
	jne	.L711
	movzbl	8(%rsp), %eax
	movq	%rsi, %rdx
	cmpb	$8, %al
	je	.L716
	cmpb	$4, %al
	je	.L717
	movl	$20, CODE_ERROR(%rip)
.L711:
	xorl	%ebx, %ebx
.L713:
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
.L716:
	.cfi_restore_state
	movb	$8, %dl
	movq	%rdx, %rsi
	call	neo_to_bool@PLT
	movq	(%rsp), %rdi
	movq	8(%rsp), %rsi
	movl	%eax, %ebx
	jmp	.L713
	.p2align 4,,10
	.p2align 3
.L717:
	movb	$4, %dl
	movq	%rdx, %rsi
	call	neo_to_integer@PLT
	movq	(%rsp), %rdi
	movq	8(%rsp), %rsi
	testq	%rax, %rax
	setne	%bl
	jmp	.L713
	.cfi_endproc
.LFE14:
	.size	isTrue, .-isTrue
	.section	.rodata.str1.1
.LC5:
	.string	"Lancement du processus"
	.text
	.p2align 4
	.globl	launch_process
	.type	launch_process, @function
launch_process:
.LFB31:
	.cfi_startproc
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	leaq	.LC5(%rip), %rdi
	call	puts@PLT
	movq	process_cycle(%rip), %rax
	leaq	result.0(%rip), %rdi
	movq	(%rax), %rax
	movq	64(%rax), %rsi
	call	eval_prolog
	movq	process_cycle(%rip), %rax
	movq	PROMISES_CNT(%rip), %rdx
	movq	(%rax), %rax
	movslq	104(%rax), %rax
	movq	(%rdx,%rax,8), %rdx
	movl	(%rdx), %edx
	testl	%edx, %edx
	je	.L719
	movq	PROMISES(%rip), %rcx
	movdqa	result.0(%rip), %xmm1
	movq	%rax, %rdx
	salq	$4, %rdx
	addq	8(%rcx), %rdx
	movups	%xmm1, (%rdx)
.L720:
	movq	PROCESS_FINISH(%rip), %rdx
	movl	$1, (%rdx,%rax,4)
	call	reset_stack_and_registers@PLT
	movq	process_cycle(%rip), %rdi
	cmpq	8(%rdi), %rdi
	je	.L732
.L721:
	call	processCycle_remove_noinline
	movq	%rax, process_cycle(%rip)
#APP
# 493 "runtime.c" 1
	addq   $8,     %rsp
	mov    $0,     %rsi
	jmp    eval_prolog
	
# 0 "" 2
#NO_APP
	movq	process_cycle(%rip), %rax
.L722:
	movq	(%rax), %rax
	movl	$1, %edx
	movl	$1, %esi
	movq	112(%rax), %rdi
	call	ptrlist_destroy_noinline
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	88(%rax), %rdi
	testq	%rdi, %rdi
	je	.L723
	call	free@PLT
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
.L723:
	movq	96(%rax), %rdi
	call	free@PLT
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rdi
	movq	80(%rdi), %rax
	testq	%rax, %rax
	je	.L724
	movq	40(%rax), %rdi
	movq	48(%rax), %rsi
	call	neobject_destroy_noinline
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	80(%rax), %rdi
	call	free@PLT
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rdi
.L724:
	call	free@PLT
	movq	process_cycle(%rip), %rax
	pxor	%xmm0, %xmm0
	movq	$0, 16(%rax)
	movups	%xmm0, (%rax)
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L732:
	.cfi_restore_state
	movq	16(%rdi), %rax
	cmpq	%rax, %rdi
	jne	.L721
	jmp	.L722
	.p2align 4,,10
	.p2align 3
.L719:
	movq	result.0(%rip), %rdi
	movq	8+result.0(%rip), %rsi
	call	neobject_destroy_noinline
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movslq	104(%rax), %rax
	jmp	.L720
	.cfi_endproc
.LFE31:
	.size	launch_process, .-launch_process
	.p2align 4
	.globl	treeToList
	.type	treeToList, @function
treeToList:
.LFB33:
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
	jle	.L733
	xorl	%ebx, %ebx
	jmp	.L738
	.p2align 4,,10
	.p2align 3
.L742:
	call	eval_prolog
	movl	CODE_ERROR(%rip), %eax
	testl	%eax, %eax
	jne	.L741
	addq	$1, %rbx
	cmpl	%ebx, 32(%rbp)
	jle	.L733
.L738:
	movq	24(%rbp), %rax
	movq	%rbx, %rdi
	salq	$4, %rdi
	addq	8(%r12), %rdi
	movq	(%rax,%rbx,8), %rsi
	testq	%rsi, %rsi
	jne	.L742
	pxor	%xmm0, %xmm0
	addq	$1, %rbx
	movups	%xmm0, (%rdi)
	movb	$55, 8(%rdi)
	cmpl	%ebx, 32(%rbp)
	jg	.L738
.L733:
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
.L741:
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
.LFE33:
	.size	treeToList, .-treeToList
	.p2align 4
	.globl	get_address
	.type	get_address, @function
get_address:
.LFB37:
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
	je	.L771
	cmpb	$1, %al
	je	.L772
	cmpb	$52, %al
	je	.L773
	movl	$89, CODE_ERROR(%rip)
.L770:
	xorl	%eax, %eax
.L743:
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
.L773:
	.cfi_restore_state
	movq	24(%rdi), %rax
	leaq	16(%rsp), %rdi
	movq	(%rax), %rsi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %eax
	testl	%eax, %eax
	jne	.L770
	cmpb	$-77, 24(%rsp)
	movq	16(%rsp), %rdi
	movq	24(%rsp), %rsi
	je	.L759
	call	neobject_destroy@PLT
	movl	$80, CODE_ERROR(%rip)
	jmp	.L770
	.p2align 4,,10
	.p2align 3
.L772:
	movq	24(%rdi), %rax
	movq	(%rax), %rdi
	call	get_address
	movl	CODE_ERROR(%rip), %ecx
	movq	(%rax), %r13
	movq	8(%rax), %r12
	movzbl	8(%rax), %ebp
	testl	%ecx, %ecx
	jne	.L770
	movq	24(%rbx), %rax
	leaq	16(%rsp), %rdi
	movq	8(%rax), %rsi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %edx
	testl	%edx, %edx
	jne	.L770
	cmpb	$-122, %bpl
	je	.L749
	cmpb	$-125, %bpl
	je	.L749
	movl	$15, CODE_ERROR(%rip)
	jmp	.L770
	.p2align 4,,10
	.p2align 3
.L771:
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
.L759:
	.cfi_restore_state
	call	neo_to_container@PLT
	movl	16(%rbx), %esi
	movq	%rax, %rbp
	cmpl	$-1, %esi
	je	.L774
.L761:
	movq	%rbp, %rdi
	call	get_container_field_addr@PLT
	movq	16(%rsp), %rdi
	movq	24(%rsp), %rsi
	movq	%rax, 8(%rsp)
	call	neobject_destroy@PLT
	movq	8(%rsp), %rax
	jmp	.L743
	.p2align 4,,10
	.p2align 3
.L749:
	cmpb	$4, 24(%rsp)
	movq	16(%rsp), %rdi
	movq	24(%rsp), %rsi
	je	.L750
	call	neobject_destroy@PLT
	movl	$16, CODE_ERROR(%rip)
	jmp	.L770
	.p2align 4,,10
	.p2align 3
.L774:
	movq	8(%rbx), %rsi
	movq	%rax, %rdi
	call	get_field_index@PLT
	movq	8(%rbx), %rdi
	movl	%eax, 16(%rbx)
	call	free@PLT
	movq	$0, 8(%rbx)
	movl	16(%rbx), %esi
	jmp	.L761
	.p2align 4,,10
	.p2align 3
.L750:
	call	neo_to_integer@PLT
	movq	16(%rsp), %rdi
	movq	24(%rsp), %rsi
	movq	%rax, %rbx
	call	neobject_destroy@PLT
	cmpb	$-122, %bpl
	je	.L751
	testq	%rbx, %rbx
	js	.L753
	movb	$-125, %r12b
	movq	%r13, %rdi
	movq	%r12, %rsi
	call	neo_to_string@PLT
	movq	%rax, %rdi
	call	strlen@PLT
	cmpq	%rax, %rbx
	jnb	.L753
	movl	$105, CODE_ERROR(%rip)
	jmp	.L770
.L753:
	movl	$18, CODE_ERROR(%rip)
	jmp	.L770
.L751:
	movq	%r13, %rdi
	movq	%r12, %rsi
	call	neo_list_len@PLT
	testq	%rbx, %rbx
	js	.L753
	cltq
	cmpq	%rbx, %rax
	jle	.L753
	movb	$-122, %r12b
	movq	%r13, %rdi
	movq	%r12, %rsi
	call	neo_to_list@PLT
	movl	%ebx, %esi
	movq	%rax, %rdi
	call	nelist_nth_addr@PLT
	jmp	.L743
	.cfi_endproc
.LFE37:
	.size	get_address, .-get_address
	.p2align 4
	.globl	execConditionBlock
	.type	execConditionBlock, @function
execConditionBlock:
.LFB38:
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
	subq	$40, %rsp
	.cfi_def_cfa_offset 96
	movq	process_cycle(%rip), %rax
	movl	32(%rdi), %r14d
	movq	(%rax), %rax
	movq	112(%rax), %rax
	movq	(%rax), %rax
	movq	%rax, 8(%rsp)
	testl	%r14d, %r14d
	jle	.L778
	movq	%rdi, %r12
	xorl	%ebx, %ebx
	leaq	16(%rsp), %r14
.L776:
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
	jne	.L778
	movzbl	24(%rsp), %eax
	movq	16(%rsp), %rdi
	movq	24(%rsp), %rsi
	cmpb	$8, %al
	je	.L813
	cmpb	$4, %al
	je	.L814
	movl	$20, CODE_ERROR(%rip)
	call	neobject_destroy@PLT
	movl	CODE_ERROR(%rip), %r11d
	testl	%r11d, %r11d
	jne	.L778
.L782:
	movl	32(%r12), %edx
	addl	$1, %ebx
	cmpl	%edx, %ebx
	jge	.L788
	addq	$8, %rbp
	xorl	%r15d, %r15d
	jmp	.L789
	.p2align 4,,10
	.p2align 3
.L795:
	movl	32(%r12), %edx
	addl	$1, %ebx
	addq	$8, %rbp
	cmpl	%ebx, %edx
	jle	.L786
.L789:
	movq	24(%r12), %rcx
	movq	(%rcx,%rbp), %rax
	cmpb	$31, (%rax)
	jne	.L815
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
	jne	.L790
	cmpb	$8, %al
	je	.L816
	cmpb	$4, %al
	je	.L817
	movl	$20, CODE_ERROR(%rip)
.L790:
	call	neobject_destroy@PLT
	movl	CODE_ERROR(%rip), %eax
	testl	%eax, %eax
	je	.L795
.L778:
	xorl	%r15d, %r15d
.L775:
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
.L816:
	.cfi_restore_state
	movb	$8, %dl
	movq	%rdx, %rsi
	call	neo_to_bool@PLT
.L792:
	movq	24(%rsp), %rsi
	movq	16(%rsp), %rdi
	movb	%al, 7(%rsp)
	call	neobject_destroy@PLT
	movl	CODE_ERROR(%rip), %esi
	testl	%esi, %esi
	jne	.L778
	movl	%r15d, %r13d
	movzbl	7(%rsp), %eax
	xorl	$1, %r13d
	andb	%al, %r13b
	je	.L795
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	96(%rax), %r15
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
	movq	96(%rax), %rdi
	call	deleteContext
	movl	CODE_ERROR(%rip), %ecx
	testl	%ecx, %ecx
	jne	.L778
	testl	%r15d, %r15d
	jne	.L775
	movl	32(%r12), %edx
	addl	$1, %ebx
	movl	%r13d, %r15d
	addq	$8, %rbp
	cmpl	%ebx, %edx
	jg	.L789
	.p2align 4,,10
	.p2align 3
.L786:
	cmpl	%edx, %ebx
	jl	.L776
	.p2align 4,,10
	.p2align 3
.L788:
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	112(%rax), %rbp
	movq	8(%rsp), %rax
	cmpq	%rax, 0(%rbp)
	je	.L778
	.p2align 4,,10
	.p2align 3
.L787:
	movq	%rbp, %rdi
	call	ptrlist_pop@PLT
	movl	16(%rax), %edi
	movq	(%rax), %rsi
	movq	%rax, %rbx
	movq	8(%rax), %rdx
	call	set_var@PLT
	movq	%rbx, %rdi
	call	free@PLT
	movq	8(%rsp), %rax
	cmpq	0(%rbp), %rax
	jne	.L787
	jmp	.L778
	.p2align 4,,10
	.p2align 3
.L817:
	movb	$4, %dl
	movq	%rdx, %rsi
	call	neo_to_integer@PLT
	testq	%rax, %rax
	setne	%al
	jmp	.L792
	.p2align 4,,10
	.p2align 3
.L815:
	cmpl	%ebx, %edx
	jle	.L786
	movslq	%ebx, %rbp
	movq	(%rcx,%rbp,8), %rax
	cmpb	$32, (%rax)
	jne	.L786
	testb	%r15b, %r15b
	je	.L800
.L801:
	addl	$1, %ebx
	cmpl	%edx, %ebx
	jl	.L776
	jmp	.L788
	.p2align 4,,10
	.p2align 3
.L813:
	movb	$8, %sil
	call	neo_to_bool@PLT
	movl	%eax, %r13d
.L780:
	movq	16(%rsp), %rdi
	movq	24(%rsp), %rsi
	call	neobject_destroy@PLT
	movl	CODE_ERROR(%rip), %r10d
	testl	%r10d, %r10d
	jne	.L778
	testb	%r13b, %r13b
	je	.L782
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rbp
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
	movq	96(%rax), %rdi
	call	deleteContext
	movl	CODE_ERROR(%rip), %r9d
	testl	%r9d, %r9d
	jne	.L778
	testl	%r15d, %r15d
	jne	.L775
	movl	32(%r12), %edx
	addl	$1, %ebx
	cmpl	%edx, %ebx
	jge	.L783
	movq	24(%r12), %rcx
	movslq	%ebx, %rbx
	jmp	.L784
	.p2align 4,,10
	.p2align 3
.L785:
	addq	$1, %rbx
	cmpl	%ebx, %edx
	jle	.L818
.L784:
	movq	(%rcx,%rbx,8), %rax
	movl	%ebx, %esi
	cmpb	$30, (%rax)
	jne	.L785
.L783:
	cmpl	%edx, %ebx
	je	.L788
.L812:
	cmpl	%edx, %ebx
	jl	.L776
	jmp	.L788
	.p2align 4,,10
	.p2align 3
.L818:
	leal	1(%rsi), %ebx
	cmpl	%edx, %ebx
	je	.L788
	jmp	.L812
.L814:
	movb	$4, %sil
	call	neo_to_integer@PLT
	testq	%rax, %rax
	setne	%r13b
	jmp	.L780
.L800:
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	96(%rax), %r13
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
	movq	96(%rax), %rdi
	call	deleteContext
	movl	CODE_ERROR(%rip), %edx
	testl	%edx, %edx
	jne	.L778
	testl	%r15d, %r15d
	jne	.L775
	movl	32(%r12), %edx
	jmp	.L801
	.cfi_endproc
.LFE38:
	.size	execConditionBlock, .-execConditionBlock
	.p2align 4
	.globl	execStatementFor
	.type	execStatementFor, @function
execStatementFor:
.LFB39:
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
	subq	$72, %rsp
	.cfi_def_cfa_offset 128
	movq	24(%rdi), %rax
	movq	(%rax), %rdx
	movl	32(%rdx), %eax
	cmpl	$4, %eax
	je	.L861
	cmpl	$3, %eax
	je	.L862
	cmpl	$2, %eax
	je	.L863
	movl	$108, CODE_ERROR(%rip)
.L825:
	xorl	%eax, %eax
.L819:
	addq	$72, %rsp
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
.L863:
	.cfi_restore_state
	xorl	%edi, %edi
	call	neo_integer_create@PLT
	movl	CODE_ERROR(%rip), %ecx
	movq	%rax, 16(%rsp)
	movq	%rdx, 24(%rsp)
	testl	%ecx, %ecx
	jne	.L825
	movq	24(%rbx), %rax
	leaq	32(%rsp), %rdi
	movq	(%rax), %rax
	movq	24(%rax), %rax
	movq	8(%rax), %rsi
	call	eval_prolog
.L829:
	movl	$1, %r12d
.L824:
	movl	CODE_ERROR(%rip), %edx
	movq	16(%rsp), %rdi
	movq	24(%rsp), %rsi
	testl	%edx, %edx
	jne	.L860
	cmpb	$4, 24(%rsp)
	jne	.L833
	cmpb	$4, 40(%rsp)
	je	.L834
.L833:
	call	neobject_destroy@PLT
	movq	32(%rsp), %rdi
	movq	40(%rsp), %rsi
	call	neobject_destroy@PLT
	movl	$10, CODE_ERROR(%rip)
	jmp	.L825
	.p2align 4,,10
	.p2align 3
.L861:
	movq	24(%rdx), %rax
	leaq	48(%rsp), %rdi
	movq	24(%rax), %rsi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %r8d
	testl	%r8d, %r8d
	jne	.L825
	cmpb	$4, 56(%rsp)
	movq	48(%rsp), %rdi
	movq	56(%rsp), %rsi
	je	.L822
	call	neobject_destroy@PLT
	movl	$108, CODE_ERROR(%rip)
	jmp	.L825
	.p2align 4,,10
	.p2align 3
.L862:
	movq	24(%rdx), %rax
	leaq	16(%rsp), %rdi
	movq	8(%rax), %rsi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %esi
	testl	%esi, %esi
	jne	.L825
	movq	24(%rbx), %rax
	leaq	32(%rsp), %rdi
	movq	(%rax), %rax
	movq	24(%rax), %rax
	movq	16(%rax), %rsi
	call	eval_prolog
	jmp	.L829
	.p2align 4,,10
	.p2align 3
.L822:
	call	neo_to_integer@PLT
	movq	48(%rsp), %rdi
	movq	56(%rsp), %rsi
	movq	%rax, %r12
	call	neobject_destroy@PLT
	movq	24(%rbx), %rax
	leaq	16(%rsp), %rdi
	movq	(%rax), %rax
	movq	24(%rax), %rax
	movq	8(%rax), %rsi
	call	eval_prolog
	movl	CODE_ERROR(%rip), %edi
	testl	%edi, %edi
	jne	.L825
	movq	24(%rbx), %rax
	leaq	32(%rsp), %rdi
	movq	(%rax), %rax
	movq	24(%rax), %rax
	movq	16(%rax), %rsi
	call	eval_prolog
	jmp	.L824
	.p2align 4,,10
	.p2align 3
.L834:
	call	neo_to_integer@PLT
	movq	16(%rsp), %rdi
	movq	24(%rsp), %rsi
	movq	%rax, %rbp
	call	neobject_destroy@PLT
	movq	24(%rbx), %rax
	movq	(%rax), %rax
	movq	24(%rax), %rax
	movq	(%rax), %rax
	cmpb	$10, (%rax)
	je	.L835
	movl	$111, CODE_ERROR(%rip)
	movq	32(%rsp), %rdi
	movq	40(%rsp), %rsi
.L860:
	call	neobject_destroy@PLT
	jmp	.L825
	.p2align 4,,10
	.p2align 3
.L835:
	movl	56(%rax), %r14d
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	96(%rax), %r13
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%r13, %rdi
	call	ptrlist_append@PLT
	movq	process_cycle(%rip), %rax
	xorl	%ecx, %ecx
	movq	(%rax), %rsi
	movq	112(%rsi), %r15
	movq	(%r15), %rax
	movq	%rax, (%rsp)
	movq	%rax, %rdx
	movq	%r15, %rax
	jmp	.L836
	.p2align 4,,10
	.p2align 3
.L864:
	testb	%cl, %cl
	jne	.L838
	movq	8(%rax), %rax
	cmpl	%r14d, 16(%rdx)
	sete	%cl
	testq	%rax, %rax
	je	.L837
	movq	(%rax), %rdx
.L836:
	testq	%rdx, %rdx
	jne	.L864
.L837:
	testb	%cl, %cl
	jne	.L838
	movl	$24, %edi
	call	malloc@PLT
	movl	%r14d, %edi
	movq	%rax, %r13
	call	get_var_value@PLT
	movl	%r14d, 16(%r13)
	movq	%r13, %rsi
	movq	%r15, %rdi
	movq	%rax, 0(%r13)
	movq	%rdx, 8(%r13)
	call	ptrlist_append@PLT
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rsi
.L838:
	movq	96(%rsi), %rsi
	movl	$24, %edi
	movl	%ebp, %r13d
	movq	%rsi, 8(%rsp)
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
	movq	8(%rsp), %rsi
	movq	(%rsi), %rdi
	movq	%r15, %rsi
	call	ptrlist_append@PLT
	movq	32(%rsp), %rdi
	movq	40(%rsp), %rsi
	call	neo_to_integer@PLT
	movq	32(%rsp), %rdi
	movq	40(%rsp), %rsi
	movq	%rax, 8(%rsp)
	movl	%eax, %r15d
	call	neobject_destroy@PLT
	movq	8(%rsp), %rax
	cmpl	%eax, %ebp
	jl	.L843
	jmp	.L847
	.p2align 4,,10
	.p2align 3
.L841:
	cmpq	$1, %rbp
	ja	.L840
	addl	%r12d, %r13d
	cmpl	%r13d, %r15d
	jle	.L840
.L843:
	movslq	%r13d, %rdi
	call	neo_integer_create@PLT
	movl	%r14d, %edi
	movq	%rax, %rsi
	call	replace_var@PLT
	movq	24(%rbx), %rax
	movq	8(%rax), %rdi
	call	exec_aux
	movslq	%eax, %rbp
	movl	CODE_ERROR(%rip), %eax
	testl	%eax, %eax
	je	.L841
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	call	deleteContext
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	112(%rax), %rbp
	movq	(%rsp), %rax
	cmpq	0(%rbp), %rax
	je	.L825
	.p2align 4,,10
	.p2align 3
.L842:
	movq	%rbp, %rdi
	call	ptrlist_pop@PLT
	movl	16(%rax), %edi
	movq	(%rax), %rsi
	movq	%rax, %rbx
	movq	8(%rax), %rdx
	call	set_var@PLT
	movq	%rbx, %rdi
	call	free@PLT
	movq	(%rsp), %rax
	cmpq	0(%rbp), %rax
	jne	.L842
	jmp	.L825
.L847:
	xorl	%ebp, %ebp
	.p2align 4,,10
	.p2align 3
.L840:
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	call	deleteContext
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	112(%rax), %r12
	movq	(%rsp), %rax
	cmpq	(%r12), %rax
	je	.L846
	.p2align 4,,10
	.p2align 3
.L844:
	movq	%r12, %rdi
	call	ptrlist_pop@PLT
	movl	16(%rax), %edi
	movq	(%rax), %rsi
	movq	%rax, %rbx
	movq	8(%rax), %rdx
	call	set_var@PLT
	movq	%rbx, %rdi
	call	free@PLT
	movq	(%rsp), %rax
	cmpq	(%r12), %rax
	jne	.L844
.L846:
	movl	%ebp, %eax
	cmpq	$2, %rbp
	jne	.L819
	jmp	.L825
	.cfi_endproc
.LFE39:
	.size	execStatementFor, .-execStatementFor
	.p2align 4
	.globl	execStatementForeachList
	.type	execStatementForeachList, @function
execStatementForeachList:
.LFB40:
	.cfi_startproc
	pushq	%r15
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	pushq	%r14
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	movq	%rdi, %r14
	movq	%rsi, %rdi
	movq	%rdx, %rsi
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
	subq	$8, %rsp
	.cfi_def_cfa_offset 64
	call	neo_to_list@PLT
	movq	%rax, %rbx
	movq	24(%r14), %rax
	movq	(%rax), %rax
	movq	24(%rax), %rax
	movq	(%rax), %rax
	cmpb	$10, (%rax)
	je	.L866
	movl	$111, CODE_ERROR(%rip)
.L867:
	xorl	%eax, %eax
.L865:
	addq	$8, %rsp
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
.L866:
	.cfi_restore_state
	movl	56(%rax), %r13d
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rbp
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%rbp, %rdi
	call	ptrlist_append@PLT
	movq	process_cycle(%rip), %rax
	xorl	%ecx, %ecx
	movq	(%rax), %rsi
	movq	112(%rsi), %rbp
	movq	0(%rbp), %r12
	movq	%rbp, %rax
	movq	%r12, %rdx
	jmp	.L868
	.p2align 4,,10
	.p2align 3
.L896:
	testb	%cl, %cl
	jne	.L870
	movq	8(%rax), %rax
	cmpl	%r13d, 16(%rdx)
	sete	%cl
	testq	%rax, %rax
	je	.L869
	movq	(%rax), %rdx
.L868:
	testq	%rdx, %rdx
	jne	.L896
.L869:
	testb	%cl, %cl
	je	.L897
.L870:
	movl	$24, %edi
	movq	96(%rsi), %r15
	call	malloc@PLT
	movl	%r13d, %edi
	movq	%rax, %rbp
	call	get_var_value@PLT
	movl	%r13d, 16(%rbp)
	movl	%r13d, %edi
	movq	%rdx, 8(%rbp)
	movq	%rax, 0(%rbp)
	call	get_absolute_address@PLT
	movq	%rax, %rdi
	call	var_reset@PLT
	movq	(%r15), %rdi
	movq	%rbp, %rsi
	call	ptrlist_append@PLT
	movl	16(%rbx), %edx
	testl	%edx, %edx
	jle	.L881
	xorl	%r15d, %r15d
	jmp	.L877
	.p2align 4,,10
	.p2align 3
.L873:
	cmpq	$1, %rbp
	ja	.L872
	movslq	16(%rbx), %rax
	addq	$1, %r15
	cmpq	%r15, %rax
	jle	.L872
.L877:
	movl	%r15d, %esi
	movq	%rbx, %rdi
	call	nelist_nth@PLT
	movq	%rax, %rdi
	movq	%rdx, %rsi
	call	neo_copy@PLT
	movl	%r13d, %edi
	movq	%rax, %rsi
	call	replace_var@PLT
	movq	24(%r14), %rax
	movq	8(%rax), %rdi
	call	exec_aux
	movslq	%eax, %rbp
	movl	CODE_ERROR(%rip), %eax
	testl	%eax, %eax
	je	.L873
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	call	deleteContext
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	112(%rax), %rbp
	cmpq	0(%rbp), %r12
	je	.L867
	.p2align 4,,10
	.p2align 3
.L874:
	movq	%rbp, %rdi
	call	ptrlist_pop@PLT
	movl	16(%rax), %edi
	movq	(%rax), %rsi
	movq	%rax, %rbx
	movq	8(%rax), %rdx
	call	set_var@PLT
	movq	%rbx, %rdi
	call	free@PLT
	cmpq	0(%rbp), %r12
	jne	.L874
	xorl	%eax, %eax
	jmp	.L865
.L881:
	xorl	%ebp, %ebp
	.p2align 4,,10
	.p2align 3
.L872:
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	call	deleteContext
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	112(%rax), %r13
	cmpq	0(%r13), %r12
	je	.L880
	.p2align 4,,10
	.p2align 3
.L878:
	movq	%r13, %rdi
	call	ptrlist_pop@PLT
	movl	16(%rax), %edi
	movq	(%rax), %rsi
	movq	%rax, %rbx
	movq	8(%rax), %rdx
	call	set_var@PLT
	movq	%rbx, %rdi
	call	free@PLT
	cmpq	0(%r13), %r12
	jne	.L878
.L880:
	movl	%ebp, %eax
	cmpq	$2, %rbp
	je	.L867
	addq	$8, %rsp
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
.L897:
	.cfi_restore_state
	movl	$24, %edi
	call	malloc@PLT
	movl	%r13d, %edi
	movq	%rax, %r15
	call	get_var_value@PLT
	movl	%r13d, 16(%r15)
	movq	%r15, %rsi
	movq	%rbp, %rdi
	movq	%rax, (%r15)
	movq	%rdx, 8(%r15)
	call	ptrlist_append@PLT
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rsi
	jmp	.L870
	.cfi_endproc
.LFE40:
	.size	execStatementForeachList, .-execStatementForeachList
	.p2align 4
	.globl	execStatementForeachString
	.type	execStatementForeachString, @function
execStatementForeachString:
.LFB41:
	.cfi_startproc
	pushq	%r15
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	movq	%rdi, %r15
	movq	%rsi, %rdi
	movq	%rdx, %rsi
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
	call	neo_to_string@PLT
	movq	%rax, %rdi
	movq	%rax, %rbx
	call	strlen@PLT
	movq	%rax, %r12
	movq	24(%r15), %rax
	movq	(%rax), %rax
	movq	24(%rax), %rax
	movq	(%rax), %rax
	cmpb	$10, (%rax)
	je	.L899
	movl	$111, CODE_ERROR(%rip)
.L900:
	xorl	%eax, %eax
.L898:
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
.L899:
	.cfi_restore_state
	movl	56(%rax), %r14d
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rbp
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%rbp, %rdi
	call	ptrlist_append@PLT
	movq	process_cycle(%rip), %rax
	xorl	%ecx, %ecx
	movq	(%rax), %rsi
	movq	112(%rsi), %rbp
	movq	0(%rbp), %r13
	movq	%rbp, %rax
	movq	%r13, %rdx
	jmp	.L901
	.p2align 4,,10
	.p2align 3
.L929:
	testb	%cl, %cl
	jne	.L903
	movq	8(%rax), %rax
	cmpl	%r14d, 16(%rdx)
	sete	%cl
	testq	%rax, %rax
	je	.L902
	movq	(%rax), %rdx
.L901:
	testq	%rdx, %rdx
	jne	.L929
.L902:
	testb	%cl, %cl
	je	.L930
.L903:
	movq	96(%rsi), %rcx
	movl	$24, %edi
	movq	%rcx, 8(%rsp)
	call	malloc@PLT
	movl	%r14d, %edi
	movq	%rax, %rbp
	call	get_var_value@PLT
	movl	%r14d, 16(%rbp)
	movl	%r14d, %edi
	movq	%rdx, 8(%rbp)
	movq	%rax, 0(%rbp)
	call	get_absolute_address@PLT
	movq	%rax, %rdi
	call	var_reset@PLT
	movq	8(%rsp), %rcx
	movq	%rbp, %rsi
	movq	(%rcx), %rdi
	call	ptrlist_append@PLT
	testl	%r12d, %r12d
	jle	.L914
	movslq	%r12d, %r12
	addq	%rbx, %r12
	jmp	.L910
	.p2align 4,,10
	.p2align 3
.L906:
	cmpq	$1, %rbp
	ja	.L905
	addq	$1, %rbx
	cmpq	%rbx, %r12
	je	.L905
.L910:
	movsbl	(%rbx), %edi
	call	charToString@PLT
	movq	%rax, %rdi
	call	neo_str_create@PLT
	movl	%r14d, %edi
	movq	%rax, %rsi
	call	replace_var@PLT
	movq	24(%r15), %rax
	movq	8(%rax), %rdi
	call	exec_aux
	movslq	%eax, %rbp
	movl	CODE_ERROR(%rip), %eax
	testl	%eax, %eax
	je	.L906
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	call	deleteContext
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	112(%rax), %rbp
	cmpq	0(%rbp), %r13
	je	.L900
	.p2align 4,,10
	.p2align 3
.L907:
	movq	%rbp, %rdi
	call	ptrlist_pop@PLT
	movl	16(%rax), %edi
	movq	(%rax), %rsi
	movq	%rax, %rbx
	movq	8(%rax), %rdx
	call	set_var@PLT
	movq	%rbx, %rdi
	call	free@PLT
	cmpq	0(%rbp), %r13
	jne	.L907
	xorl	%eax, %eax
	jmp	.L898
.L914:
	xorl	%ebp, %ebp
	.p2align 4,,10
	.p2align 3
.L905:
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	call	deleteContext
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	112(%rax), %r12
	cmpq	(%r12), %r13
	je	.L913
	.p2align 4,,10
	.p2align 3
.L911:
	movq	%r12, %rdi
	call	ptrlist_pop@PLT
	movl	16(%rax), %edi
	movq	(%rax), %rsi
	movq	%rax, %rbx
	movq	8(%rax), %rdx
	call	set_var@PLT
	movq	%rbx, %rdi
	call	free@PLT
	cmpq	(%r12), %r13
	jne	.L911
.L913:
	movl	%ebp, %eax
	cmpq	$2, %rbp
	je	.L900
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
.L930:
	.cfi_restore_state
	movl	$24, %edi
	call	malloc@PLT
	movl	%r14d, %edi
	movq	%rax, 8(%rsp)
	call	get_var_value@PLT
	movq	8(%rsp), %rsi
	movq	%rbp, %rdi
	movq	%rax, (%rsi)
	movq	%rdx, 8(%rsi)
	movl	%r14d, 16(%rsi)
	call	ptrlist_append@PLT
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rsi
	jmp	.L903
	.cfi_endproc
.LFE41:
	.size	execStatementForeachString, .-execStatementForeachString
	.p2align 4
	.globl	initRuntime
	.type	initRuntime, @function
initRuntime:
.LFB43:
	.cfi_startproc
	xorl	%esi, %esi
	xorl	%edi, %edi
	jmp	create_new_process
	.cfi_endproc
.LFE43:
	.size	initRuntime, .-initRuntime
	.p2align 4
	.globl	exitRuntime
	.type	exitRuntime, @function
exitRuntime:
.LFB44:
	.cfi_startproc
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movq	process_cycle(%rip), %rax
	movq	PROCESS_FINISH(%rip), %rdx
	movl	$1, %esi
	movq	(%rax), %rax
	movslq	104(%rax), %rcx
	movq	112(%rax), %rdi
	movl	$1, (%rdx,%rcx,4)
	movl	$1, %edx
	call	ptrlist_destroy@PLT
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	88(%rax), %rdi
	testq	%rdi, %rdi
	je	.L933
	call	free@PLT
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
.L933:
	movq	96(%rax), %rdi
	call	free@PLT
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rdi
	movq	80(%rdi), %rax
	testq	%rax, %rax
	je	.L934
	movq	40(%rax), %rdi
	movq	48(%rax), %rsi
	call	neobject_destroy@PLT
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	80(%rax), %rdi
	call	free@PLT
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rdi
.L934:
	call	free@PLT
	movq	process_cycle(%rip), %rax
	pxor	%xmm0, %xmm0
	movups	%xmm0, (%rax)
	movq	$0, 16(%rax)
	movq	PROMISES_CNT(%rip), %rax
	movq	(%rax), %rax
	movl	$0, (%rax)
	movq	PROCESS_FINISH(%rip), %rax
	movl	$1, (%rax)
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE44:
	.size	exitRuntime, .-exitRuntime
	.p2align 4
	.globl	exec
	.type	exec, @function
exec:
.LFB45:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	xorl	%esi, %esi
	movq	%rdi, %rbx
	xorl	%edi, %edi
	call	create_new_process
	movq	%rbx, %rdi
	call	exec_aux
	popq	%rbx
	.cfi_def_cfa_offset 8
	jmp	exitRuntime
	.cfi_endproc
.LFE45:
	.size	exec, .-exec
	.p2align 4
	.globl	eval
	.type	eval, @function
eval:
.LFB46:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	xorl	%esi, %esi
	movq	%rdi, %rbx
	xorl	%edi, %edi
	subq	$16, %rsp
	.cfi_def_cfa_offset 32
	call	create_new_process
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
.LFE46:
	.size	eval, .-eval
	.local	result.0
	.comm	result.0,16,16
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
