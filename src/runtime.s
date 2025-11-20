	.file	"runtime.c"
	.text
	.p2align 4
	.globl	neon_interp_yield
	.type	neon_interp_yield, @function
neon_interp_yield:
.LFB28:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$8, %rsp
	.cfi_offset 3, -24
	movq	global_env(%rip), %rax
	movq	200(%rax), %rax
	movq	8(%rax), %rdx
	movq	global_env(%rip), %rax
	movq	200(%rax), %rax
	cmpq	%rax, %rdx
	je	.L3
	movq	global_env(%rip), %rax
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	unloadCurrentProcess@PLT
	movq	global_env(%rip), %rax
	movq	200(%rax), %rax
	movq	global_env(%rip), %rbx
	movq	%rax, %rdi
	call	loadNextLivingProcess@PLT
	movq	%rax, 200(%rbx)
	movq	global_env(%rip), %rax
	movq	200(%rax), %rax
	movq	16(%rax), %rax
	movq	(%rax), %rdx
	movq	global_env(%rip), %rax
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	switch_registers@PLT
.L3:
	nop
	movq	-8(%rbp), %rbx
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE28:
	.size	neon_interp_yield, .-neon_interp_yield
	.p2align 4
	.globl	neon_interp_next_process
	.type	neon_interp_next_process, @function
neon_interp_next_process:
.LFB29:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$8, %rsp
	.cfi_offset 3, -24
	movq	global_env(%rip), %rdx
	movq	global_env(%rip), %rax
	movl	4(%rdx), %edx
	movl	%edx, 8(%rax)
	movq	global_env(%rip), %rax
	movq	200(%rax), %rax
	movq	global_env(%rip), %rbx
	movq	%rax, %rdi
	call	loadNextLivingProcess@PLT
	movq	%rax, 200(%rbx)
	movq	global_env(%rip), %rax
	movq	200(%rax), %rax
	movq	16(%rax), %rax
	movq	(%rax), %rdx
	movq	global_env(%rip), %rax
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	switch_registers@PLT
	movq	global_env(%rip), %rax
	movq	200(%rax), %rax
	movq	%rax, %rdi
	call	ProcessCycle_clean@PLT
	nop
	movq	-8(%rbp), %rbx
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE29:
	.size	neon_interp_next_process, .-neon_interp_next_process
	.p2align 4
	.globl	launch_process
	.type	launch_process, @function
launch_process:
.LFB31:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$24, %rsp
	.cfi_offset 3, -24
	movq	global_env(%rip), %rax
	movq	200(%rax), %rax
	movq	%rax, %rdi
	call	ProcessCycle_clean@PLT
	movq	global_env(%rip), %rax
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movb	$1, 56(%rax)
	movq	global_env(%rip), %rax
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movl	60(%rax), %edx
	movq	global_env(%rip), %rax
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	64(%rax), %rax
	movl	%edx, %esi
	movq	%rax, %rdi
	call	eval_aux
	movq	%rax, -32(%rbp)
	movq	%rdx, -24(%rbp)
	movq	global_env(%rip), %rax
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	process_preRemove@PLT
	movq	global_env(%rip), %rax
	movq	24(%rax), %rdx
	movq	global_env(%rip), %rax
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movl	88(%rax), %eax
	cltq
	salq	$3, %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	movl	(%rax), %eax
	testl	%eax, %eax
	je	.L7
	movq	global_env(%rip), %rax
	movq	16(%rax), %rax
	movq	8(%rax), %rdx
	movq	global_env(%rip), %rax
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movl	88(%rax), %eax
	cltq
	salq	$4, %rax
	leaq	(%rdx,%rax), %rcx
	movq	-32(%rbp), %rax
	movq	-24(%rbp), %rdx
	movq	%rax, (%rcx)
	movq	%rdx, 8(%rcx)
	jmp	.L8
.L7:
	movq	-32(%rbp), %rcx
	movq	-24(%rbp), %rax
	movl	$0, %edx
	movq	%rcx, %rdi
	movq	%rax, %rsi
	call	general_neobject_destroy@PLT
.L8:
	movq	global_env(%rip), %rax
	movq	40(%rax), %rdx
	movq	global_env(%rip), %rax
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movl	88(%rax), %eax
	cltq
	salq	$2, %rax
	addq	%rdx, %rax
	movl	$1, (%rax)
	movq	global_env(%rip), %rax
	movq	200(%rax), %rax
	movq	%rax, %rdi
	call	ProcessCycle_isActive@PLT
	testb	%al, %al
	je	.L9
	call	neon_interp_next_process
.L9:
	movq	global_env(%rip), %rax
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	reset_stack_and_registers@PLT
	jmp	.L10
.L11:
	movq	global_env(%rip), %rax
	movq	200(%rax), %rax
	movq	global_env(%rip), %rbx
	movq	%rax, %rdi
	call	ProcessCycle_remove@PLT
	movq	%rax, 200(%rbx)
.L10:
	movq	global_env(%rip), %rax
	movq	200(%rax), %rax
	movq	%rax, %rdi
	call	ProcessCycle_isEmpty@PLT
	xorl	$1, %eax
	testb	%al, %al
	jne	.L11
	nop
	movq	-8(%rbp), %rbx
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE31:
	.size	launch_process, .-launch_process
	.p2align 4
	.globl	exitRuntime
	.type	exitRuntime, @function
exitRuntime:
.LFB43:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$24, %rsp
	.cfi_offset 3, -24
	movq	$0, -32(%rbp)
	movq	$0, -24(%rbp)
	movq	global_env(%rip), %rax
	movq	40(%rax), %rdx
	movq	global_env(%rip), %rax
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movl	88(%rax), %eax
	cltq
	salq	$2, %rax
	addq	%rdx, %rax
	movl	$1, (%rax)
	movq	global_env(%rip), %rax
	movq	24(%rax), %rax
	movq	(%rax), %rax
	movl	$0, (%rax)
	movq	global_env(%rip), %rax
	movq	40(%rax), %rax
	movl	$1, (%rax)
	movq	global_env(%rip), %rax
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	process_preRemove@PLT
	movq	global_env(%rip), %rax
	movq	200(%rax), %rax
	movq	%rax, %rdi
	call	ProcessCycle_isActive@PLT
	testb	%al, %al
	je	.L15
	movq	global_env(%rip), %rax
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	save_stack_and_registers@PLT
	call	neon_interp_next_process
	jmp	.L15
.L16:
	movq	global_env(%rip), %rax
	movq	200(%rax), %rax
	movq	global_env(%rip), %rbx
	movq	%rax, %rdi
	call	ProcessCycle_remove@PLT
	movq	%rax, 200(%rbx)
.L15:
	movq	global_env(%rip), %rax
	movq	200(%rax), %rax
	movq	%rax, %rdi
	call	ProcessCycle_isEmpty@PLT
	xorl	$1, %eax
	testb	%al, %al
	jne	.L16
	nop
	movq	-8(%rbp), %rbx
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE43:
	.size	exitRuntime, .-exitRuntime
	.p2align 4
	.globl	update__name__
	.type	update__name__, @function
update__name__:
.LFB22:
	.cfi_startproc
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	call	neo_str_create@PLT
	movq	%rax, %rsi
	movq	global_env(%rip), %rax
	movl	72(%rax), %edi
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	jmp	replace_var@PLT
	.cfi_endproc
.LFE22:
	.size	update__name__, .-update__name__
	.p2align 4
	.globl	newContext
	.type	newContext, @function
newContext:
.LFB23:
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
.LFE23:
	.size	newContext, .-newContext
	.p2align 4
	.globl	deleteContext
	.type	deleteContext, @function
deleteContext:
.LFB24:
	.cfi_startproc
	pushq	%r13
	.cfi_def_cfa_offset 16
	.cfi_offset 13, -16
	pushq	%r12
	.cfi_def_cfa_offset 24
	.cfi_offset 12, -24
	movq	%rdi, %r12
	pushq	%rbp
	.cfi_def_cfa_offset 32
	.cfi_offset 6, -32
	pushq	%rbx
	.cfi_def_cfa_offset 40
	.cfi_offset 3, -40
	subq	$8, %rsp
	.cfi_def_cfa_offset 48
	movq	(%rdi), %r13
	movq	0(%r13), %rbx
	testq	%rbx, %rbx
	je	.L31
	cmpq	$0, 8(%r13)
	movq	%r13, %rbp
	je	.L32
	.p2align 4
	.p2align 3
.L26:
	movq	(%rbx), %rsi
	movq	8(%rbx), %rdx
	movl	16(%rbx), %edi
	call	replace_var@PLT
	movq	%rbx, %rdi
	call	free@PLT
	movq	%rbp, %rdi
	movq	8(%rbp), %rbp
	call	free@PLT
	cmpq	$0, 8(%rbp)
	movq	0(%rbp), %rbx
	jne	.L26
	movq	(%rbx), %rsi
	movq	8(%rbx), %rdx
	movl	16(%rbx), %edi
	call	replace_var@PLT
	movq	%rbx, %rdi
	call	free@PLT
	movq	%rbp, %rdi
	call	free@PLT
.L25:
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 40
	movq	%r13, %rsi
	movq	%r12, %rdi
	movl	$1, %edx
	popq	%rbx
	.cfi_def_cfa_offset 32
	popq	%rbp
	.cfi_def_cfa_offset 24
	popq	%r12
	.cfi_def_cfa_offset 16
	popq	%r13
	.cfi_def_cfa_offset 8
	jmp	ptrlist_remove@PLT
	.p2align 4,,10
	.p2align 3
.L32:
	.cfi_restore_state
	movl	16(%rbx), %edi
	movq	(%rbx), %rsi
	movq	8(%rbx), %rdx
	call	replace_var@PLT
	movq	%rbx, %rdi
	call	free@PLT
.L31:
	movq	%r13, %rdi
	call	free@PLT
	jmp	.L25
	.cfi_endproc
.LFE24:
	.size	deleteContext, .-deleteContext
	.p2align 4
	.globl	neoIsTrue
	.type	neoIsTrue, @function
neoIsTrue:
.LFB26:
	.cfi_startproc
	cmpb	$8, %sil
	je	.L41
	cmpb	$4, %sil
	je	.L42
	movq	global_env(%rip), %rax
	movl	$20, (%rax)
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L41:
	jmp	neo_to_bool@PLT
	.p2align 4,,10
	.p2align 3
.L42:
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	call	neo_to_integer@PLT
	testq	%rax, %rax
	setne	%al
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE26:
	.size	neoIsTrue, .-neoIsTrue
	.p2align 4
	.globl	local
	.type	local, @function
local:
.LFB27:
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
.LFE27:
	.size	local, .-local
	.p2align 4
	.globl	interrupt
	.type	interrupt, @function
interrupt:
.LFB30:
	.cfi_startproc
	jmp	neon_interp_yield
	.cfi_endproc
.LFE30:
	.size	interrupt, .-interrupt
	.section	.rodata
.LC0:
	.string	".ne"
	.text
	.p2align 4
	.globl	exec_aux
	.type	exec_aux, @function
exec_aux:
.LFB41:
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
	movl	%esi, %ebp
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	movq	%rdi, %rbx
	subq	$72, %rsp
	.cfi_def_cfa_offset 128
	movq	global_env(%rip), %rcx
	movl	8(%rcx), %eax
	testl	%eax, %eax
	je	.L170
.L47:
	movl	(%rcx), %edi
	subl	$1, %eax
	movl	%eax, 8(%rcx)
	testl	%edi, %edi
	jne	.L112
	movq	16(%rbx), %rdx
	movl	%ebp, %esi
	addq	%rdx, %rsi
	movzwl	12(%rsi), %eax
	movl	8(%rsi), %ebp
	testw	%ax, %ax
	je	.L112
	subl	$1, %eax
	leaq	.L51(%rip), %r13
	leaq	4(%rbp,%rax,4), %r14
	.p2align 4
	.p2align 3
.L121:
	leaq	(%rdx,%rbp), %rsi
	movl	(%rsi), %eax
	movzwl	4(%rdx,%rax), %eax
	movl	%eax, 88(%rcx)
	movl	(%rsi), %r8d
	leaq	(%rdx,%r8), %r12
	movq	%r8, %rdi
	movl	(%r12), %eax
	subl	$4, %eax
	cmpl	$16, %eax
	ja	.L49
	movslq	0(%r13,%rax,4), %rax
	addq	%r13, %rax
	jmp	*%rax
	.section	.rodata
	.align 4
	.align 4
.L51:
	.long	.L59-.L51
	.long	.L58-.L51
	.long	.L57-.L51
	.long	.L49-.L51
	.long	.L49-.L51
	.long	.L49-.L51
	.long	.L56-.L51
	.long	.L55-.L51
	.long	.L49-.L51
	.long	.L49-.L51
	.long	.L49-.L51
	.long	.L49-.L51
	.long	.L54-.L51
	.long	.L53-.L51
	.long	.L49-.L51
	.long	.L52-.L51
	.long	.L50-.L51
	.text
	.p2align 4,,10
	.p2align 3
.L178:
	cmpw	$1, 16(%r12)
	jbe	.L91
	jmp	.L171
	.p2align 4,,10
	.p2align 3
.L174:
	movq	%rdx, %rsi
	cmpb	$8, %dl
	je	.L172
	cmpb	$4, %dl
	je	.L173
	movl	$20, (%rax)
	xorl	%edx, %edx
	call	general_neobject_destroy@PLT
.L96:
	movq	global_env(%rip), %rax
	movl	(%rax), %r10d
	testl	%r10d, %r10d
	jne	.L112
	movl	8(%rax), %r9d
	testl	%r9d, %r9d
	js	.L159
	call	neon_interp_yield
.L159:
	movq	16(%rbx), %rdx
	movl	(%rdx,%rbp), %r8d
.L91:
	movl	12(%rdx,%r8), %eax
	movq	%rbx, %rdi
	movl	(%rdx,%rax), %esi
	call	eval_aux
	movq	%rax, %rdi
	movq	global_env(%rip), %rax
	movq	%rdx, %r12
	movl	(%rax), %r8d
	testl	%r8d, %r8d
	je	.L174
.L112:
	xorl	%r12d, %r12d
.L46:
	addq	$72, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	movl	%r12d, %eax
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
.L49:
	.cfi_restore_state
	movl	%edi, %esi
	movq	%rbx, %rdi
	call	eval_aux
	movq	%rdx, %rsi
	movq	%rax, %rdi
	xorl	%edx, %edx
	call	general_neobject_destroy@PLT
	movq	global_env(%rip), %rcx
	movl	(%rcx), %edx
	testl	%edx, %edx
	jne	.L112
	.p2align 4
	.p2align 3
.L119:
	addq	$4, %rbp
	cmpq	%rbp, %r14
	je	.L112
	movq	16(%rbx), %rdx
	jmp	.L121
	.p2align 4,,10
	.p2align 3
.L50:
	movl	8(%r12), %eax
	cmpl	$1, %eax
	je	.L175
	cmpl	$2, %eax
	je	.L176
	cmpl	$3, %eax
	je	.L177
	cmpl	$4, %eax
	je	.L178
	.p2align 4
	.p2align 3
.L85:
	movl	(%rcx), %eax
	testl	%eax, %eax
	je	.L119
	jmp	.L112
	.p2align 4,,10
	.p2align 3
.L52:
	movl	8(%r12), %r12d
	cmpl	$3, %r12d
	je	.L85
	jmp	.L46
	.p2align 4,,10
	.p2align 3
.L53:
	movl	%r8d, %esi
	movq	%rbx, %rdi
	call	execConditionBlock
	movl	%eax, %r12d
	testl	%eax, %eax
	jne	.L46
.L155:
	movq	global_env(%rip), %rcx
	jmp	.L85
	.p2align 4,,10
	.p2align 3
.L54:
	movl	8(%rcx), %r15d
	movl	$-1, 8(%rcx)
	movq	%rbx, %rdi
	movl	(%rsi), %esi
	call	exec_aux
	movq	global_env(%rip), %rcx
	xorl	%edx, %edx
	movl	%eax, %r12d
	addl	%r15d, 8(%rcx)
	movl	8(%rcx), %eax
	testl	%eax, %eax
	cmovs	%edx, %eax
	movl	%eax, 8(%rcx)
	movl	%r12d, %eax
	orl	(%rcx), %eax
	je	.L119
	jmp	.L46
	.p2align 4,,10
	.p2align 3
.L55:
	leaq	32(%r12), %rsi
	movq	%rbx, %rdi
	call	treeToList
	movq	24(%r12), %rsi
	movq	16(%r12), %rdi
	movq	%rax, %rdx
	call	userFuncDefine@PLT
	movq	8(%r12), %rdi
	movq	%rdx, 8(%rsp)
	movq	%rax, %r15
	call	get_var@PLT
	movq	8(%rsp), %rdx
	movq	%r15, %rsi
	movl	%eax, %edi
	call	replace_var@PLT
	movq	global_env(%rip), %rcx
	jmp	.L85
	.p2align 4,,10
	.p2align 3
.L56:
	movl	8(%r12), %esi
	movq	%rbx, %rdi
	movq	%r8, 16(%rsp)
	call	exec_aux
	movl	%eax, %r12d
	testl	%eax, %eax
	jne	.L46
	movq	global_env(%rip), %rcx
	movq	16(%rsp), %r8
	movl	(%rcx), %eax
	movq	%rcx, %rdx
	testl	%eax, %eax
	movl	%eax, 8(%rsp)
	je	.L119
	movq	16(%rbx), %rax
	movl	$0, (%rcx)
	addq	%rax, %r8
	movzwl	16(%r8), %edi
	movl	12(%r8), %esi
	movl	%edi, 52(%rsp)
	testw	%di, %di
	je	.L62
	movl	%esi, %ecx
	movl	8(%rsp), %esi
	movq	%rcx, 56(%rsp)
	testl	%esi, %esi
	js	.L124
	movq	%rcx, 16(%rsp)
	movq	$0, 24(%rsp)
.L68:
	movq	24(%rsp), %rcx
	salq	$2, %rcx
	movq	%rcx, 40(%rsp)
	movq	16(%rsp), %rcx
	movl	(%rax,%rcx), %edx
	addq	%rax, %rdx
	cmpw	$0, 12(%rdx)
	je	.L74
	xorl	%r12d, %r12d
	jmp	.L67
	.p2align 4,,10
	.p2align 3
.L180:
	call	get_exception_code@PLT
	movl	8(%rsp), %edi
	movl	%eax, %r15d
	call	get_exception_from_code_error@PLT
	cmpl	%r15d, %eax
	movq	16(%rbx), %rax
	je	.L74
	movq	16(%rsp), %rcx
	addq	$1, %r12
	movl	(%rax,%rcx), %edx
	addq	%rax, %rdx
	movzwl	12(%rdx), %ecx
	cmpl	%r12d, %ecx
	jle	.L179
.L67:
	movl	8(%rdx), %edx
	leaq	(%rax,%r12,4), %rax
	movq	%rbx, %rdi
	movl	(%rax,%rdx), %esi
	call	eval_aux
	movq	%rax, %rdi
	movq	%rdx, %rsi
	cmpb	$20, %dl
	je	.L180
.L65:
	movq	global_env(%rip), %rax
	movl	$78, (%rax)
	jmp	.L112
	.p2align 4,,10
	.p2align 3
.L57:
	movl	8(%r12), %esi
	.p2align 4
	.p2align 3
.L169:
	movq	%rbx, %rdi
	call	eval_aux
	movq	%rax, %rdi
	movq	global_env(%rip), %rax
	movq	%rdx, %r12
	movl	(%rax), %esi
	testl	%esi, %esi
	jne	.L112
	movq	%rdx, %rsi
	cmpb	$8, %dl
	je	.L181
	cmpb	$4, %dl
	je	.L182
	movl	$20, (%rax)
	xorl	%edx, %edx
	call	general_neobject_destroy@PLT
	movq	global_env(%rip), %rcx
	jmp	.L85
	.p2align 4,,10
	.p2align 3
.L58:
	movl	12(%r12), %eax
	movl	(%rdx,%rax), %esi
	cmpl	$12, (%rdx,%rsi)
	jne	.L162
	cmpw	$2, 16(%r12)
	jne	.L183
	movl	4(%rdx,%rax), %esi
	movq	%rbx, %rdi
	call	eval_aux
	movq	%rax, %r8
	movq	global_env(%rip), %rax
	movq	%rdx, %r15
	movl	(%rax), %ecx
	testl	%ecx, %ecx
	jne	.L112
	cmpb	$-122, %dl
	je	.L184
	cmpb	$-125, %dl
	jne	.L117
	movq	16(%rbx), %rax
	movq	%r8, %rdx
	movq	%r15, %rcx
	movq	%rbx, %rdi
	movq	%r8, 8(%rsp)
	movl	(%rax,%rbp), %esi
	call	execStatementForeachString
	movq	8(%rsp), %r8
	movl	%eax, %r12d
.L116:
	xorl	%edx, %edx
	movq	%r8, %rdi
	movq	%r15, %rsi
	call	general_neobject_destroy@PLT
	movq	global_env(%rip), %rcx
	movl	%r12d, %eax
	orl	(%rcx), %eax
	je	.L119
	jmp	.L46
	.p2align 4,,10
	.p2align 3
.L59:
	movl	12(%r12), %eax
	movl	(%rdx,%rax), %eax
	cmpl	$12, (%rdx,%rax)
	jne	.L162
	movl	%r8d, %esi
	movq	%rbx, %rdi
	call	execStatementFor
.L161:
	movq	global_env(%rip), %rcx
	movl	%eax, %r12d
	orl	(%rcx), %eax
	je	.L119
	jmp	.L46
	.p2align 4,,10
	.p2align 3
.L170:
	movl	4(%rcx), %eax
	movl	%eax, 8(%rcx)
	call	neon_interp_yield
	movq	global_env(%rip), %rcx
	movl	8(%rcx), %eax
	jmp	.L47
	.p2align 4,,10
	.p2align 3
.L181:
	movb	$8, %sil
	movq	%rdi, 8(%rsp)
	call	neo_to_bool@PLT
	movq	8(%rsp), %rdi
	movl	%eax, %r15d
.L106:
	xorl	%edx, %edx
	movq	%r12, %rsi
	call	general_neobject_destroy@PLT
	testb	%r15b, %r15b
	je	.L155
	movq	16(%rbx), %rax
	movq	%rbx, %rdi
	movl	(%rax,%rbp), %edx
	movl	12(%rax,%rdx), %esi
	call	exec_aux
	movq	global_env(%rip), %rcx
	movl	%eax, %r12d
	movl	(%rcx), %edi
	testl	%edi, %edi
	jne	.L112
	cmpl	$1, %eax
	ja	.L104
	movq	16(%rbx), %rax
	movl	(%rax,%rbp), %edx
	movl	8(%rax,%rdx), %esi
	jmp	.L169
	.p2align 4,,10
	.p2align 3
.L182:
	movb	$4, %sil
	movq	%rdi, 8(%rsp)
	call	neo_to_integer@PLT
	movq	8(%rsp), %rdi
	testq	%rax, %rax
	setne	%r15b
	jmp	.L106
	.p2align 4,,10
	.p2align 3
.L179:
	addq	$1, 24(%rsp)
	movq	24(%rsp), %rcx
	addq	$4, 16(%rsp)
	cmpl	%ecx, 52(%rsp)
	jg	.L68
	jmp	.L69
	.p2align 4,,10
	.p2align 3
.L104:
	cmpl	$4, %eax
	je	.L46
	jmp	.L85
	.p2align 4,,10
	.p2align 3
.L184:
	movq	16(%rbx), %rax
	movq	%r8, %rdx
	movq	%r15, %rcx
	movq	%rbx, %rdi
	movq	%r8, 8(%rsp)
	movl	(%rax,%rbp), %esi
	call	execStatementForeachList
	movq	8(%rsp), %r8
	movl	%eax, %r12d
	jmp	.L116
	.p2align 4,,10
	.p2align 3
.L176:
	xorl	%r15d, %r15d
	cmpw	$0, 16(%r12)
	jne	.L83
	jmp	.L85
	.p2align 4,,10
	.p2align 3
.L185:
	movl	72(%rax), %esi
	movq	176(%rax), %rdi
	call	nelist_nth@PLT
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	neo_to_string@PLT
	movq	%rax, %rdi
	call	strdup@PLT
	movq	8(%rsp), %rsi
	movq	%r12, %rdi
	movq	%rax, 16(%rsp)
	call	neo_to_string@PLT
	movq	%rax, %rdi
	call	strdup@PLT
	movq	%rax, %rdi
	call	neo_str_create@PLT
	movq	%rax, %rsi
	movq	global_env(%rip), %rax
	movl	72(%rax), %edi
	call	replace_var@PLT
	movq	8(%rsp), %rsi
	movq	%r12, %rdi
	call	neo_to_string@PLT
	leaq	.LC0(%rip), %rsi
	movq	%rax, %rdi
	call	addStr@PLT
	movq	%rax, %rdi
	movq	%rax, 24(%rsp)
	call	importFile@PLT
	movq	24(%rsp), %rdi
	call	free@PLT
	movq	16(%rsp), %rdi
	call	neo_str_create@PLT
	movq	%rax, %rsi
	movq	global_env(%rip), %rax
	movl	72(%rax), %edi
	call	replace_var@PLT
	movq	8(%rsp), %rsi
	xorl	%edx, %edx
	movq	%r12, %rdi
	call	general_neobject_destroy@PLT
	movq	global_env(%rip), %rax
	movl	(%rax), %r11d
	testl	%r11d, %r11d
	jne	.L112
	call	gc_mark_and_sweep@PLT
	movq	16(%rbx), %rdx
	addq	$1, %r15
	movl	(%rdx,%rbp), %r12d
	addq	%rdx, %r12
	movzwl	16(%r12), %eax
	cmpl	%r15d, %eax
	jle	.L155
.L83:
	movl	12(%r12), %eax
	leaq	(%rdx,%r15,4), %rdx
	movq	%rbx, %rdi
	movl	(%rdx,%rax), %esi
	call	eval_aux
	movq	%rax, %r12
	movq	global_env(%rip), %rax
	movq	%rdx, 8(%rsp)
	movl	(%rax), %edx
	testl	%edx, %edx
	je	.L185
	jmp	.L112
	.p2align 4,,10
	.p2align 3
.L177:
	cmpw	$0, 16(%r12)
	je	.L186
	movq	200(%rcx), %rax
	movq	(%rax), %rax
	movq	80(%rax), %rsi
	cmpq	$0, (%rsi)
	je	.L187
	xorl	%r15d, %r15d
	jmp	.L88
	.p2align 4,,10
	.p2align 3
.L188:
	movq	200(%rcx), %rax
	movq	(%rax), %rax
.L88:
	leaq	0(,%r15,4), %rcx
	movl	12(%r12), %esi
	addq	$1, %r15
	leaq	(%rdx,%rcx), %rdi
	movq	%rcx, 8(%rsp)
	movl	(%rdi,%rsi), %esi
	movq	96(%rax), %rdi
	movl	8(%rdx,%rsi), %esi
	call	save_later@PLT
	movq	8(%rsp), %rcx
	movl	$24, %edi
	movq	global_env(%rip), %rax
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	80(%rax), %r8
	movq	16(%rbx), %rax
	movl	(%rax,%rbp), %edx
	movq	%r8, 16(%rsp)
	movl	12(%rax,%rdx), %esi
	leaq	(%rax,%rcx), %rdx
	movl	(%rdx,%rsi), %edx
	movl	8(%rax,%rdx), %eax
	movl	%eax, 8(%rsp)
	call	malloc@PLT
	movl	8(%rsp), %edi
	movq	%rax, %r12
	call	get_var_value@PLT
	movq	%rax, (%r12)
	movl	8(%rsp), %eax
	movq	%rdx, 8(%r12)
	movl	%eax, 16(%r12)
	movl	%eax, %edi
	call	get_absolute_address@PLT
	movq	%rax, %rdi
	call	var_reset@PLT
	movq	16(%rsp), %r8
	movq	%r12, %rsi
	movq	(%r8), %rdi
	call	ptrlist_append@PLT
	movq	16(%rbx), %rdx
	movq	global_env(%rip), %rcx
	movl	(%rdx,%rbp), %r12d
	addq	%rdx, %r12
	movzwl	16(%r12), %eax
	cmpl	%r15d, %eax
	jg	.L188
	jmp	.L85
	.p2align 4,,10
	.p2align 3
.L124:
	movq	$0, 32(%rsp)
	movq	%rcx, %r15
	movl	8(%rsp), %ecx
	negl	%ecx
	movl	%ecx, 24(%rsp)
.L63:
	movl	(%rax,%r15), %edx
	movq	32(%rsp), %rcx
	addq	%rax, %rdx
	salq	$2, %rcx
	cmpw	$0, 12(%rdx)
	movq	%rcx, 40(%rsp)
	je	.L74
	xorl	%r12d, %r12d
	jmp	.L75
	.p2align 4,,10
	.p2align 3
.L70:
	call	get_exception_code@PLT
	movl	8(%rsp), %edi
	movl	%eax, 16(%rsp)
	call	get_exception_from_code_error@PLT
	movl	16(%rsp), %edx
	cmpl	%edx, %eax
	je	.L72
	cmpl	%edx, 24(%rsp)
	je	.L72
	movq	16(%rbx), %rax
	addq	$1, %r12
	movl	(%rax,%r15), %edx
	addq	%rax, %rdx
	movzwl	12(%rdx), %ecx
	cmpl	%r12d, %ecx
	jle	.L189
.L75:
	movl	8(%rdx), %edx
	leaq	(%rax,%r12,4), %rax
	movq	%rbx, %rdi
	movl	(%rax,%rdx), %esi
	call	eval_aux
	movq	%rax, %rdi
	movq	%rdx, %rsi
	cmpb	$20, %dl
	je	.L70
	jmp	.L65
.L189:
	movq	%r15, %rdi
	addq	$1, 32(%rsp)
	movq	32(%rsp), %rcx
	addq	$4, %rdi
	movq	%rdi, %r15
	cmpl	%ecx, 52(%rsp)
	jg	.L63
.L69:
	movq	global_env(%rip), %rdx
.L62:
	movl	8(%rsp), %eax
	movl	%eax, (%rdx)
	jmp	.L112
	.p2align 4,,10
	.p2align 3
.L72:
	movq	global_env(%rip), %rax
	movq	$0, 56(%rax)
	movq	16(%rbx), %rax
.L74:
	movq	40(%rsp), %rdx
	movq	56(%rsp), %rcx
	movq	%rbx, %rdi
	addq	%rax, %rdx
	movl	(%rdx,%rcx), %edx
	movl	16(%rax,%rdx), %esi
	call	exec_aux
	jmp	.L161
.L172:
	movb	$8, %sil
	movq	%rdi, 8(%rsp)
	call	neo_to_bool@PLT
	movq	8(%rsp), %rdi
	movl	%eax, %r15d
.L94:
	xorl	%edx, %edx
	movq	%r12, %rsi
	call	general_neobject_destroy@PLT
	testb	%r15b, %r15b
	jne	.L155
	jmp	.L96
.L173:
	movb	$4, %sil
	movq	%rdi, 8(%rsp)
	call	neo_to_integer@PLT
	movq	8(%rsp), %rdi
	testq	%rax, %rax
	setne	%r15b
	jmp	.L94
.L162:
	movl	$22, (%rcx)
	jmp	.L112
.L175:
	cmpw	$1, 16(%r12)
	jbe	.L77
	movl	$21, (%rcx)
	jmp	.L112
.L183:
	movl	$109, (%rcx)
	jmp	.L112
.L117:
	xorl	%edx, %edx
	movq	%r8, %rdi
	movq	%r15, %rsi
	call	general_neobject_destroy@PLT
	movq	global_env(%rip), %rax
	movl	$109, (%rax)
	jmp	.L112
.L77:
	movq	96(%rcx), %rdi
	movq	104(%rcx), %rsi
	call	neo_is_void@PLT
	movq	global_env(%rip), %r12
	testb	%al, %al
	je	.L190
	movq	16(%rbx), %rdx
	movl	(%rdx,%rbp), %eax
	addq	%rdx, %rax
	cmpw	$0, 16(%rax)
	je	.L191
	movl	12(%rax), %eax
	movq	%rbx, %rdi
	movl	(%rdx,%rax), %esi
	call	eval_aux
	movq	%rax, 96(%r12)
	movq	global_env(%rip), %rax
	movq	%rdx, 104(%r12)
	xorl	%r12d, %r12d
	movl	(%rax), %ecx
	testl	%ecx, %ecx
	sete	%r12b
	sall	$2, %r12d
	jmp	.L46
.L187:
	movl	$70, (%rcx)
	jmp	.L112
.L186:
	movl	$69, (%rcx)
	jmp	.L112
.L190:
	movl	$99, (%r12)
	jmp	.L112
.L171:
	movl	$101, (%rcx)
	jmp	.L112
.L191:
	call	neo_none_create@PLT
	movq	%rax, 96(%r12)
	movq	%rdx, 104(%r12)
	movl	$4, %r12d
	jmp	.L46
	.cfi_endproc
.LFE41:
	.size	exec_aux, .-exec_aux
	.section	.rodata
.LC1:
	.string	"__local_args__"
	.text
	.p2align 4
	.globl	callUserFunc
	.type	callUserFunc, @function
callUserFunc:
.LFB33:
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
	movq	global_env(%rip), %rax
	movq	%rsi, 16(%rsp)
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	80(%rax), %r13
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%r13, %rdi
	call	ptrlist_append@PLT
	movq	global_env(%rip), %rax
	cmpb	$0, 20(%rbp)
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rax
	movq	(%rax), %rax
	movq	%rax, 24(%rsp)
	jne	.L193
	movl	16(%rbp), %eax
.L194:
	testl	%eax, %eax
	jle	.L197
	xorl	%ebx, %ebx
	jmp	.L202
	.p2align 4,,10
	.p2align 3
.L198:
	addq	$1, %rbx
	cmpl	%ebx, 16(%rbp)
	jle	.L203
.L202:
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
	movq	global_env(%rip), %rax
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	call	save_later@PLT
	movq	global_env(%rip), %rax
	movl	$24, %edi
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	80(%rax), %r12
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
	movq	global_env(%rip), %rdi
	movl	(%rdi), %edx
	testl	%edx, %edx
	je	.L198
	cmpl	$1, %ebx
	jle	.L199
	movl	$4, %ebx
	.p2align 4
	.p2align 3
.L200:
	movq	8(%rbp), %rax
	movl	(%rax,%rbx), %edi
	addq	$4, %rbx
	call	free_var@PLT
	cmpq	%rbx, 8(%rsp)
	jne	.L200
	movq	global_env(%rip), %rdi
.L199:
	movq	200(%rdi), %rax
	movq	(%rax), %rax
	movq	80(%rax), %rdi
	call	deleteContext
	movq	global_env(%rip), %rax
	movq	24(%rsp), %rsi
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	call	partialRestore@PLT
	addq	$40, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	xorl	%eax, %eax
	movl	$54, %edx
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
.L197:
	.cfi_restore_state
	movq	global_env(%rip), %rdi
	.p2align 4
	.p2align 3
.L203:
	movl	28(%rbp), %esi
	subq	$-128, %rdi
	call	exec_aux
	movl	%eax, %ebx
	movq	global_env(%rip), %rax
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	80(%rax), %rdi
	call	deleteContext
	movq	global_env(%rip), %rax
	movq	24(%rsp), %rsi
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	call	partialRestore@PLT
	movq	global_env(%rip), %rcx
	movl	(%rcx), %eax
	testl	%eax, %eax
	jne	.L209
	cmpl	$4, %ebx
	jne	.L205
	pxor	%xmm0, %xmm0
	movq	104(%rcx), %rdx
	movq	96(%rcx), %rax
	movups	%xmm0, 96(%rcx)
	movb	$54, 104(%rcx)
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
.L193:
	.cfi_restore_state
	movq	%rbx, %rsi
	movq	%r12, %rdi
	call	neo_list_len@PLT
	leaq	.LC1(%rip), %rdi
	movl	%eax, %r15d
	call	get_var@PLT
	movl	%eax, %r13d
	movq	global_env(%rip), %rax
	movl	%r13d, %esi
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	call	save_later@PLT
	movq	global_env(%rip), %rax
	movl	$24, %edi
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	80(%rax), %rcx
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
	jle	.L194
	movq	16(%rsp), %rcx
	movl	%eax, 16(%rcx)
	jmp	.L194
	.p2align 4,,10
	.p2align 3
.L209:
	addq	$40, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	xorl	%eax, %eax
	movl	$54, %edx
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
.L205:
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
.LFE33:
	.size	callUserFunc, .-callUserFunc
	.p2align 4
	.globl	callUserMethod
	.type	callUserMethod, @function
callUserMethod:
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
	movq	global_env(%rip), %rax
	movq	%rsi, 40(%rsp)
	movq	200(%rax), %rax
	movq	%rdx, 24(%rsp)
	movq	(%rax), %rax
	movq	80(%rax), %r12
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%r12, %rdi
	call	ptrlist_append@PLT
	movq	global_env(%rip), %rax
	cmpb	$0, 20(%rbp)
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rax
	movq	(%rax), %rax
	movq	%rax, 32(%rsp)
	jne	.L211
	movl	16(%rbp), %eax
.L212:
	testl	%eax, %eax
	jle	.L215
	xorl	%ebx, %ebx
	jmp	.L220
	.p2align 4,,10
	.p2align 3
.L216:
	addq	$1, %rbx
	cmpl	%ebx, 16(%rbp)
	jle	.L221
.L220:
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
	movq	global_env(%rip), %rax
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	call	save_later@PLT
	movq	global_env(%rip), %rax
	movl	$24, %edi
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	80(%rax), %r12
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
	movq	global_env(%rip), %rdi
	movl	(%rdi), %ecx
	testl	%ecx, %ecx
	je	.L216
	cmpl	$1, %ebx
	jle	.L217
	movl	$4, %ebx
	.p2align 4
	.p2align 3
.L218:
	movq	8(%rbp), %rax
	movl	(%rax,%rbx), %edi
	addq	$4, %rbx
	call	free_var@PLT
	cmpq	%rbx, 16(%rsp)
	jne	.L218
	movq	global_env(%rip), %rdi
.L217:
	movq	200(%rdi), %rax
.L227:
	movq	(%rax), %rax
	movq	80(%rax), %rdi
	call	deleteContext
	movq	global_env(%rip), %rax
	movq	32(%rsp), %rsi
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	call	partialRestore@PLT
	addq	$56, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	xorl	%eax, %eax
	movl	$54, %edx
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
.L215:
	.cfi_restore_state
	movq	global_env(%rip), %rdi
	.p2align 4
	.p2align 3
.L221:
	movl	28(%rbp), %esi
	subq	$-128, %rdi
	call	exec_aux
	movl	%eax, %ebx
	movq	global_env(%rip), %rax
	movl	(%rax), %edx
	testl	%edx, %edx
	jne	.L228
	movq	8(%rbp), %rax
	movl	(%rax), %edi
	call	get_var_value@PLT
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	neo_copy@PLT
	movq	%rax, %r12
	movq	global_env(%rip), %rax
	movq	%rdx, %r13
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	80(%rax), %rdi
	call	deleteContext
	movq	global_env(%rip), %rax
	movq	32(%rsp), %rsi
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	call	partialRestore@PLT
	movq	40(%rsp), %r15
	xorl	%edx, %edx
	movq	(%r15), %rdi
	movq	8(%r15), %rsi
	call	general_neobject_destroy@PLT
	movq	%r12, (%r15)
	movq	%r13, 8(%r15)
	cmpl	$4, %ebx
	jne	.L223
	movq	global_env(%rip), %rcx
	pxor	%xmm0, %xmm0
	movq	104(%rcx), %rdx
	movq	96(%rcx), %rax
	movups	%xmm0, 96(%rcx)
	movb	$54, 104(%rcx)
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
.L211:
	.cfi_restore_state
	movq	%r13, %rsi
	movq	%rbx, %rdi
	call	neo_list_len@PLT
	leaq	.LC1(%rip), %rdi
	movl	%eax, %r15d
	call	get_var@PLT
	movl	%eax, %r12d
	movq	global_env(%rip), %rax
	movl	%r12d, %esi
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	call	save_later@PLT
	movq	global_env(%rip), %rax
	movl	$24, %edi
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	80(%rax), %rcx
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
	jle	.L212
	movq	24(%rsp), %rcx
	movl	%eax, 16(%rcx)
	jmp	.L212
	.p2align 4,,10
	.p2align 3
.L228:
	movq	200(%rax), %rax
	jmp	.L227
	.p2align 4,,10
	.p2align 3
.L223:
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
.LFE34:
	.size	callUserMethod, .-callUserMethod
	.p2align 4
	.globl	eval_aux
	.type	eval_aux, @function
eval_aux:
.LFB35:
	.cfi_startproc
	movq	global_env(%rip), %rax
	movl	(%rax), %r8d
	testl	%r8d, %r8d
	jne	.L438
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
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	movl	%esi, %ebx
	subq	$72, %rsp
	.cfi_def_cfa_offset 128
	movq	16(%rdi), %rcx
	leaq	(%rcx,%rbx), %rsi
	movzwl	4(%rsi), %edx
	cmpl	$22, (%rsi)
	movl	%edx, 88(%rax)
	ja	.L232
	movl	(%rsi), %edx
	leaq	.L234(%rip), %rdi
	movslq	(%rdi,%rdx,4), %rdx
	addq	%rdi, %rdx
	jmp	*%rdx
	.section	.rodata
	.align 4
	.align 4
.L234:
	.long	.L232-.L234
	.long	.L242-.L234
	.long	.L241-.L234
	.long	.L240-.L234
	.long	.L232-.L234
	.long	.L232-.L234
	.long	.L232-.L234
	.long	.L232-.L234
	.long	.L232-.L234
	.long	.L232-.L234
	.long	.L232-.L234
	.long	.L232-.L234
	.long	.L239-.L234
	.long	.L238-.L234
	.long	.L237-.L234
	.long	.L236-.L234
	.long	.L232-.L234
	.long	.L232-.L234
	.long	.L235-.L234
	.long	.L232-.L234
	.long	.L232-.L234
	.long	.L232-.L234
	.long	.L233-.L234
	.text
	.p2align 4,,10
	.p2align 3
.L232:
	movl	$19, (%rax)
.L433:
	xorl	%eax, %eax
	movl	$54, %edx
.L407:
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
.L233:
	.cfi_restore_state
	movzwl	16(%rsi), %edi
	call	nelist_create@PLT
	movq	%rax, %r13
	movq	16(%rbp), %rax
	leaq	(%rax,%rbx), %rdx
	cmpw	$0, 16(%rdx)
	je	.L353
	xorl	%r12d, %r12d
.L356:
	movl	12(%rdx), %edx
	leaq	(%rax,%r12,4), %rcx
	movq	%rbp, %rdi
	movl	(%rcx,%rdx), %edx
	movl	16(%rax,%rdx), %esi
	call	eval_aux
	movq	%rax, %rsi
	movq	global_env(%rip), %rax
	movq	%rdx, %rdi
	movq	%r12, %rdx
	salq	$4, %rdx
	addq	8(%r13), %rdx
	movl	(%rax), %ecx
	movq	%rsi, (%rdx)
	movq	%rdi, 8(%rdx)
	testl	%ecx, %ecx
	jne	.L439
	movq	16(%rbp), %rax
	addq	$1, %r12
	leaq	(%rax,%rbx), %rdx
	movzwl	16(%rdx), %ecx
	cmpl	%r12d, %ecx
	jg	.L356
.L353:
	movl	8(%rdx), %edi
	addq	$72, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	movq	%r13, %rsi
	popq	%rbx
	.cfi_restore 3
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_restore 6
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_restore 12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_restore 13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_restore 14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_restore 15
	.cfi_def_cfa_offset 8
	jmp	neo_container_create@PLT
	.p2align 4,,10
	.p2align 3
.L235:
	.cfi_restore_state
	addq	$8, %rsi
	movq	%rbp, %rdi
	call	treeToList
	movq	global_env(%rip), %rdx
	movl	(%rdx), %edx
	testl	%edx, %edx
	jne	.L433
	addq	$72, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	movq	%rax, %rdi
	popq	%rbx
	.cfi_restore 3
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_restore 6
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_restore 12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_restore 13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_restore 14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_restore 15
	.cfi_def_cfa_offset 8
	jmp	neo_list_convert@PLT
	.p2align 4,,10
	.p2align 3
.L236:
	.cfi_restore_state
	movl	8(%rsi), %esi
	movq	%rbp, %rdi
	call	eval_aux
	movq	%rax, %r15
	movq	global_env(%rip), %rax
	movq	%rdx, %r14
	movl	(%rax), %edx
	testl	%edx, %edx
	jne	.L433
	cmpb	$-77, %r14b
	je	.L359
	xorl	%edx, %edx
	movq	%r15, %rdi
	movq	%r14, %rsi
	call	general_neobject_destroy@PLT
	movq	global_env(%rip), %rax
	movl	$54, %edx
	movl	$80, (%rax)
	xorl	%eax, %eax
	jmp	.L407
	.p2align 4,,10
	.p2align 3
.L237:
	movq	16(%rsi), %rdi
	movq	24(%rsi), %rsi
	call	neo_is_void@PLT
	testb	%al, %al
	jne	.L440
.L275:
	movq	16(%rbp), %rax
	addq	%rbx, %rax
	movzbl	24(%rax), %edx
	cmpb	$-128, %dl
	je	.L441
	leal	79(%rdx), %ecx
	cmpb	$1, %cl
	jbe	.L442
	movq	global_env(%rip), %rcx
	cmpb	$54, %dl
	je	.L443
	movl	$9, (%rcx)
.L424:
	movq	16(%rax), %rdi
	movq	24(%rax), %rsi
.L436:
	xorl	%edx, %edx
.L432:
	call	general_neobject_destroy@PLT
	addq	16(%rbp), %rbx
	pxor	%xmm0, %xmm0
	movups	%xmm0, 16(%rbx)
	movb	$54, 24(%rbx)
	jmp	.L433
	.p2align 4,,10
	.p2align 3
.L238:
	movl	8(%rsi), %esi
	movq	%rbp, %rdi
	call	eval_aux
	movq	%rax, %r14
	movq	global_env(%rip), %rax
	movq	%rdx, %r13
	movl	(%rax), %edi
	testl	%edi, %edi
	jne	.L433
	movq	16(%rbp), %rax
	movq	%rbp, %rdi
	movl	%edx, %r15d
	movl	12(%rax,%rbx), %esi
	call	eval_aux
	movq	%rax, %rbp
	movq	global_env(%rip), %rax
	movq	%rdx, %rbx
	movl	(%rax), %esi
	testl	%esi, %esi
	jne	.L433
	cmpb	$-122, %r13b
	je	.L347
	cmpb	$-125, %r13b
	je	.L347
	movl	$15, (%rax)
	movl	$54, %edx
	xorl	%eax, %eax
	jmp	.L407
	.p2align 4,,10
	.p2align 3
.L239:
	movl	8(%rsi), %edi
	call	get_var_value@PLT
	movq	%rax, %rdi
	movq	%rdx, %rsi
	cmpb	$54, %dl
	je	.L444
.L435:
	addq	$72, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	popq	%rbx
	.cfi_restore 3
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_restore 6
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_restore 12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_restore 13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_restore 14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_restore 15
	.cfi_def_cfa_offset 8
	jmp	neo_copy@PLT
	.p2align 4,,10
	.p2align 3
.L240:
	.cfi_restore_state
	movq	8(%rsi), %rdi
	movq	16(%rsi), %rsi
	jmp	.L435
	.p2align 4,,10
	.p2align 3
.L241:
	movl	8(%rsi), %edi
	cmpl	$39, %edi
	jne	.L243
	movl	12(%rsi), %ebx
	addq	%rbx, %rcx
	cmpl	$14, (%rcx)
	je	.L244
	movl	$100, (%rax)
	movl	$54, %edx
	xorl	%eax, %eax
	jmp	.L407
	.p2align 4,,10
	.p2align 3
.L242:
	movl	8(%rsi), %edi
	movl	$256, %esi
	call	operatorIs@PLT
	movl	%eax, %edx
	movq	16(%rbp), %rax
	addq	%rbx, %rax
	testb	%dl, %dl
	jne	.L445
	movl	8(%rax), %edi
	cmpl	$37, %edi
	je	.L446
	cmpl	$35, %edi
	je	.L447
	movl	$8, %esi
	call	operatorIs@PLT
	movl	%eax, %edx
	movq	16(%rbp), %rax
	addq	%rbx, %rax
	testb	%dl, %dl
	je	.L260
	movl	12(%rax), %esi
	movq	%rbp, %rdi
	call	get_address
	movq	%rax, %r12
	movq	global_env(%rip), %rax
	movl	(%rax), %r13d
	testl	%r13d, %r13d
	jne	.L433
	movq	16(%rbp), %rax
	movq	%rbp, %rdi
	movl	16(%rax,%rbx), %esi
	call	eval_aux
	movq	%rax, %r15
	movq	global_env(%rip), %rax
	movq	%rdx, %r14
	movl	(%rax), %r11d
	testl	%r11d, %r11d
	jne	.L433
	movq	16(%rbp), %rax
	movq	%r15, %rsi
	movq	%r12, %rdi
	movslq	8(%rax,%rbx), %rcx
	leaq	operators_functions(%rip), %rax
	call	*(%rax,%rcx,8)
	.p2align 4
	.p2align 3
.L423:
	movq	%rdx, %r12
	movq	%r15, %rdi
	xorl	%edx, %edx
	movq	%r14, %rsi
	movq	%rax, %r13
	call	general_neobject_destroy@PLT
.L428:
	movq	%r13, %rax
	movq	%r12, %rdx
	jmp	.L407
	.p2align 4,,10
	.p2align 3
.L438:
	.cfi_def_cfa_offset 8
	.cfi_restore 3
	.cfi_restore 6
	.cfi_restore 12
	.cfi_restore 13
	.cfi_restore 14
	.cfi_restore 15
	xorl	%eax, %eax
	movl	$54, %edx
	ret
	.p2align 4,,10
	.p2align 3
.L243:
	.cfi_def_cfa_offset 128
	.cfi_offset 3, -56
	.cfi_offset 6, -48
	.cfi_offset 12, -40
	.cfi_offset 13, -32
	.cfi_offset 14, -24
	.cfi_offset 15, -16
	movl	$256, %esi
	call	operatorIs@PLT
	movl	%eax, %edx
	movq	16(%rbp), %rax
	addq	%rbx, %rax
	movslq	8(%rax), %rdi
	testb	%dl, %dl
	je	.L249
	movl	12(%rax), %esi
	leaq	operators_functions(%rip), %rax
	movq	(%rax,%rdi,8), %rax
	addq	$72, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	movq	%rbp, %rdi
	popq	%rbx
	.cfi_restore 3
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_restore 6
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_restore 12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_restore 13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_restore 14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_restore 15
	.cfi_def_cfa_offset 8
	jmp	*%rax
	.p2align 4,,10
	.p2align 3
.L444:
	.cfi_restore_state
	movq	global_env(%rip), %rax
	movl	$5, (%rax)
	jmp	.L433
	.p2align 4,,10
	.p2align 3
.L445:
	movslq	8(%rax), %rcx
	movl	16(%rax), %edx
	movq	%rbp, %rdi
	movl	12(%rax), %esi
	leaq	operators_functions(%rip), %rax
	movq	(%rax,%rcx,8), %rax
	addq	$72, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	popq	%rbx
	.cfi_restore 3
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_restore 6
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_restore 12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_restore 13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_restore 14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_restore 15
	.cfi_def_cfa_offset 8
	jmp	*%rax
	.p2align 4,,10
	.p2align 3
.L440:
	.cfi_restore_state
	movq	16(%rbp), %r12
	movq	%rbp, %rdi
	addq	%rbx, %r12
	movl	8(%r12), %esi
	call	eval_aux
	movq	%rax, 16(%r12)
	movq	global_env(%rip), %rax
	movq	%rdx, 24(%r12)
	movl	(%rax), %eax
	testl	%eax, %eax
	je	.L275
	jmp	.L433
	.p2align 4,,10
	.p2align 3
.L441:
	movdqu	16(%rax), %xmm0
	movq	16(%rax), %rdi
	movq	24(%rax), %rsi
	movaps	%xmm0, (%rsp)
	call	neo_to_function@PLT
	pxor	%xmm2, %xmm2
	movq	%rbp, %rdi
	movq	%rax, %r12
	movq	16(%rbp), %rax
	addq	%rbx, %rax
	movups	%xmm2, 16(%rax)
	leaq	32(%rax), %rsi
	movb	$54, 24(%rax)
	call	treeToList
	movq	global_env(%rip), %rdx
	movdqa	(%rsp), %xmm0
	movq	%rax, %r14
	movq	16(%rbp), %rax
	movl	(%rdx), %r15d
	addq	%rbx, %rax
	movups	%xmm0, 16(%rax)
	testl	%r15d, %r15d
	jne	.L448
	movq	%r14, %rsi
	movq	%r12, %rdi
	call	funcArgsCheck@PLT
	testb	%al, %al
	je	.L449
	movq	16(%rbp), %rax
	movq	%r14, %rdx
	movq	16(%rax,%rbx), %rdi
	movq	24(%rax,%rbx), %rsi
	call	functionCall@PLT
	movq	%r14, %rdi
	xorl	%esi, %esi
	movq	%rax, %r13
	movq	%rdx, %r12
	call	general_nelist_destroy@PLT
	movq	16(%rbp), %rax
	xorl	%edx, %edx
	movq	16(%rax,%rbx), %rdi
	movq	24(%rax,%rbx), %rsi
	call	general_neobject_destroy@PLT
	movq	global_env(%rip), %rax
	addq	16(%rbp), %rbx
	pxor	%xmm3, %xmm3
	movups	%xmm3, 16(%rbx)
	movb	$54, 24(%rbx)
	movl	(%rax), %r14d
	testl	%r14d, %r14d
	jne	.L434
	movzwl	4(%rbx), %edx
	movl	%edx, 88(%rax)
	movq	%r13, %rax
	movq	%r12, %rdx
	jmp	.L407
	.p2align 4,,10
	.p2align 3
.L439:
	leal	-1(%r12), %esi
	movq	%r13, %rdi
	call	nelist_destroy_until@PLT
	xorl	%eax, %eax
	movl	$54, %edx
	jmp	.L407
	.p2align 4,,10
	.p2align 3
.L359:
	movq	%r14, %rsi
	movq	%r15, %rdi
	call	neo_to_container@PLT
	movq	16(%rbp), %r13
	movq	%rax, %r12
	addq	%rbx, %r13
	movl	12(%r13), %esi
	cmpl	$-1, %esi
	je	.L360
	movl	4(%rax), %eax
	cmpl	%eax, 16(%r13)
	je	.L361
.L360:
	movq	24(%r13), %rsi
	movq	%r12, %rdi
	call	get_field_index@PLT
	movl	%eax, 12(%r13)
	movq	16(%rbp), %rax
	movq	24(%rax,%rbx), %rdi
	call	free@PLT
	movl	4(%r12), %eax
	addq	16(%rbp), %rbx
	movq	$0, 24(%rbx)
	movl	12(%rbx), %esi
	movl	%eax, 16(%rbx)
.L361:
	movq	%r12, %rdi
	call	get_container_field@PLT
	movq	%rax, %rdi
	movq	%rdx, %rsi
	call	neo_copy@PLT
	jmp	.L423
	.p2align 4,,10
	.p2align 3
.L249:
	movl	$32, %esi
	call	operatorIs@PLT
	testb	%al, %al
	jne	.L253
	movq	16(%rbp), %rax
	movl	$1, %esi
	movl	8(%rax,%rbx), %edi
	call	operatorIs@PLT
	testb	%al, %al
	je	.L450
.L253:
	movq	16(%rbp), %rax
	movq	%rbp, %rdi
	movl	12(%rax,%rbx), %esi
	call	get_address
	movq	global_env(%rip), %rdx
	movl	(%rdx), %edx
	testl	%edx, %edx
	jne	.L433
	movq	16(%rbp), %rdx
	movq	%rax, %rdi
	movslq	8(%rdx,%rbx), %rcx
	leaq	operators_functions(%rip), %rdx
	movq	(%rdx,%rcx,8), %rdx
	addq	$72, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	popq	%rbx
	.cfi_restore 3
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_restore 6
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_restore 12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_restore 13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_restore 14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_restore 15
	.cfi_def_cfa_offset 8
	jmp	*%rdx
	.p2align 4,,10
	.p2align 3
.L347:
	.cfi_restore_state
	cmpb	$4, %bl
	je	.L348
	xorl	%edx, %edx
	movq	%rbp, %rdi
	movq	%rbx, %rsi
	call	general_neobject_destroy@PLT
	xorl	%edx, %edx
	movq	%r14, %rdi
	movq	%r13, %rsi
	call	general_neobject_destroy@PLT
	movq	global_env(%rip), %rax
	movl	$54, %edx
	movl	$16, (%rax)
	xorl	%eax, %eax
	jmp	.L407
	.p2align 4,,10
	.p2align 3
.L447:
	movq	global_env(%rip), %rax
	movl	$54, %edx
	movl	$92, (%rax)
	xorl	%eax, %eax
	jmp	.L407
	.p2align 4,,10
	.p2align 3
.L442:
	movq	16(%rax), %rdi
	movq	24(%rax), %rsi
	call	neo_to_userfunc@PLT
	movq	16(%rbp), %rcx
	movq	%rax, %r9
	leaq	(%rcx,%rbx), %rax
	movq	16(%rax), %rdi
	movzwl	36(%rax), %r8d
	movq	%rdi, 32(%rsp)
	movq	24(%rax), %rdi
	movq	%rdi, 24(%rsp)
	movzbl	24(%rax), %edi
	movb	%dil, 54(%rsp)
	movl	16(%r9), %edi
	cmpl	%edi, %r8d
	jle	.L282
	cmpb	$0, 20(%r9)
	je	.L451
	testl	%r8d, %r8d
	je	.L452
.L291:
	movl	32(%rax), %eax
	leal	-1(%r8), %esi
	movl	$0, 48(%rsp)
	leaq	(%rcx,%rax), %rdx
	leaq	4(%rcx,%rax), %rax
	leaq	(%rax,%rsi,4), %rsi
	jmp	.L289
	.p2align 5
	.p2align 4,,10
	.p2align 3
.L285:
	addq	$4, %rdx
	cmpq	%rdx, %rsi
	je	.L290
.L289:
	movl	(%rdx), %eax
	addq	%rcx, %rax
	cmpl	$1, (%rax)
	jne	.L285
	cmpl	$37, 8(%rax)
	jne	.L285
	testl	%edi, %edi
	jle	.L365
	movl	12(%rax), %eax
	movq	8(%r9), %r10
	movl	8(%rcx,%rax), %r11d
	xorl	%eax, %eax
	jmp	.L287
	.p2align 4
	.p2align 4,,10
	.p2align 3
.L288:
	addl	$1, %eax
	addq	$4, %r10
	cmpl	%eax, %edi
	je	.L286
.L287:
	cmpl	%r11d, (%r10)
	jne	.L288
.L286:
	movl	%edi, %r10d
	subl	24(%r9), %r10d
	cmpl	%eax, %r10d
	setle	%r10b
	cmpl	%eax, %edi
	setg	%al
	movzbl	%al, %eax
	andl	%r10d, %eax
	addl	%eax, 48(%rsp)
	jmp	.L285
	.p2align 4,,10
	.p2align 3
.L443:
	movl	$8, (%rcx)
	jmp	.L424
	.p2align 4,,10
	.p2align 3
.L260:
	movl	8(%rax), %edi
	movl	$16, %esi
	call	operatorIs@PLT
	movq	16(%rbp), %rdx
	addq	%rbx, %rdx
	testb	%al, %al
	je	.L264
	movl	12(%rdx), %esi
	movq	%rbp, %rdi
	call	eval_aux
	movq	%rax, %r15
	movq	global_env(%rip), %rax
	movq	%rdx, %r14
	movl	(%rax), %r10d
	testl	%r10d, %r10d
	jne	.L433
	movq	16(%rbp), %rax
	movq	%rbp, %rdi
	movl	16(%rax,%rbx), %esi
	call	get_address
	movq	global_env(%rip), %rdx
	movl	(%rdx), %r9d
	testl	%r9d, %r9d
	jne	.L433
	movq	16(%rbp), %rdx
	movq	%r15, %rdi
	movq	%r14, %rsi
	movslq	8(%rdx,%rbx), %rcx
	movq	%rax, %rdx
	leaq	operators_functions(%rip), %rax
	call	*(%rax,%rcx,8)
	jmp	.L423
	.p2align 4,,10
	.p2align 3
.L244:
	movl	8(%rcx), %esi
	movq	%rbp, %rdi
	call	eval_aux
	movq	%rax, %r13
	movq	global_env(%rip), %rax
	movq	%rdx, %r12
	movl	(%rax), %esi
	testl	%esi, %esi
	jne	.L433
	cmpb	$-78, %dl
	je	.L247
	movl	$100, (%rax)
.L434:
	xorl	%edx, %edx
	movq	%r13, %rdi
	movq	%r12, %rsi
	call	general_neobject_destroy@PLT
	xorl	%eax, %eax
	movl	$54, %edx
	jmp	.L407
.L282:
	testl	%r8d, %r8d
	jne	.L291
	xorl	%r12d, %r12d
	movl	%r12d, 48(%rsp)
	.p2align 4
	.p2align 3
.L290:
	cmpb	$0, 20(%r9)
	jne	.L364
.L292:
	movq	%r9, (%rsp)
	call	nelist_create@PLT
	movq	(%rsp), %r9
	movslq	16(%rax), %rdx
	movq	%rax, %r15
	testl	%edx, %edx
	jle	.L297
	movq	8(%rax), %rax
	salq	$4, %rdx
	addq	%rax, %rdx
	.p2align 5
	.p2align 4
	.p2align 3
.L296:
	pxor	%xmm0, %xmm0
	addq	$16, %rax
	movups	%xmm0, -16(%rax)
	movb	$54, -8(%rax)
	cmpq	%rax, %rdx
	jne	.L296
.L297:
	movl	16(%r9), %eax
	movq	%r9, %r14
	movl	%eax, %r12d
	subl	24(%r9), %r12d
	movslq	%r12d, %r13
	salq	$4, %r13
	cmpl	%r12d, %eax
	jle	.L295
	.p2align 4
	.p2align 3
.L301:
	testl	%r12d, %r12d
	jne	.L300
	movq	16(%rbp), %rax
	cmpb	$-79, 24(%rax,%rbx)
	je	.L437
.L300:
	movq	32(%r14), %rdi
	movl	%r12d, %esi
	addl	$1, %r12d
	call	nelist_nth@PLT
	movq	%rax, %rdi
	movq	%rdx, %rsi
	call	neo_copy@PLT
	movq	8(%r15), %rcx
	movq	%rax, (%rcx,%r13)
	movq	%rdx, 8(%rcx,%r13)
	addq	$16, %r13
	cmpl	%r12d, 16(%r14)
	jg	.L301
	movq	%r14, %r9
.L295:
	movq	$0, 56(%rsp)
	movq	16(%rbp), %rsi
	xorl	%r12d, %r12d
	leaq	(%rsi,%rbx), %rdx
	movq	%rsi, %r10
	movzwl	36(%rdx), %edi
	testw	%di, %di
	je	.L299
	movq	%r15, %r13
	movq	%rbx, %r15
	movq	%r9, %rbx
	jmp	.L298
	.p2align 6
	.p2align 4,,10
	.p2align 3
.L303:
	addq	$1, %r12
	movzwl	%di, %eax
	cmpl	%r12d, %eax
	jle	.L453
.L298:
	movl	32(%rdx), %eax
	leaq	0(,%r12,4), %r14
	leaq	(%rsi,%r14), %rcx
	movl	(%rcx,%rax), %eax
	addq	%rsi, %rax
	cmpl	$1, (%rax)
	jne	.L303
	cmpl	$37, 8(%rax)
	jne	.L303
	movl	12(%rax), %eax
	addq	%rsi, %rax
	cmpl	$12, (%rax)
	jne	.L413
	movl	16(%rbx), %esi
	testl	%esi, %esi
	jle	.L368
	movl	8(%rax), %edi
	xorl	%r9d, %r9d
	movq	8(%rbx), %rax
	jmp	.L307
	.p2align 5
	.p2align 4,,10
	.p2align 3
.L309:
	addl	$1, %r9d
	addq	$4, %rax
	cmpl	%esi, %r9d
	je	.L413
.L307:
	cmpl	%edi, (%rax)
	jne	.L309
.L306:
	cmpl	%r9d, %esi
	je	.L413
	movslq	%r9d, %r11
	subl	24(%rbx), %esi
	salq	$4, %r11
	cmpl	%r9d, %esi
	jle	.L311
	movq	8(%r13), %rax
	movl	%r9d, 40(%rsp)
	movq	%r11, (%rsp)
	movq	(%rax,%r11), %rdi
	movq	8(%rax,%r11), %rsi
	call	neo_is_void@PLT
	testb	%al, %al
	je	.L312
	movq	16(%rbp), %r10
	movq	(%rsp), %r11
	movl	40(%rsp), %r9d
	leaq	(%r10,%r15), %rdx
	leaq	(%r10,%r14), %rcx
.L311:
	movl	32(%rdx), %eax
	pxor	%xmm4, %xmm4
	movq	%rbp, %rdi
	movq	%r11, 40(%rsp)
	movups	%xmm4, 16(%rdx)
	movb	$54, 24(%rdx)
	movl	(%rcx,%rax), %eax
	movl	%r9d, (%rsp)
	movl	16(%r10,%rax), %esi
	call	eval_aux
	movq	8(%r13), %rcx
	movq	40(%rsp), %r11
	movq	16(%rbp), %r10
	movl	(%rsp), %r9d
	movq	%rax, (%rcx,%r11)
	movq	24(%rsp), %rax
	movb	54(%rsp), %al
	movq	%rdx, 8(%rcx,%r11)
	leaq	(%r10,%r15), %rdx
	movq	%r10, %rsi
	movq	32(%rsp), %rcx
	movq	%rax, 24(%rsp)
	movq	%rax, 24(%rdx)
	movq	global_env(%rip), %rax
	movq	%rcx, 16(%rdx)
	movl	(%rax), %r11d
	testl	%r11d, %r11d
	jne	.L454
	testl	%r9d, %r9d
	jne	.L419
	cmpb	$-79, 54(%rsp)
	je	.L314
.L419:
	movzwl	36(%rdx), %edi
	jmp	.L303
.L446:
	movq	global_env(%rip), %rax
	movl	$54, %edx
	movl	$95, (%rax)
	xorl	%eax, %eax
	jmp	.L407
.L348:
	movq	%rbp, %rdi
	movq	%rbx, %rsi
	call	neo_to_integer@PLT
	xorl	%edx, %edx
	movq	%rbp, %rdi
	movq	%rbx, %rsi
	movq	%rax, %r12
	call	general_neobject_destroy@PLT
	cmpb	$-122, %r15b
	je	.L349
	movb	$-125, %r13b
	movq	%r14, %rdi
	movq	%r13, %rsi
	call	neo_to_string@PLT
	movq	%rax, %rdi
	call	strlen@PLT
	cmpq	%rax, %r12
	jnb	.L350
	movq	%r13, %rsi
	movq	%r14, %rdi
	call	neo_to_string@PLT
	movsbl	(%rax,%r12), %edi
	call	charToString@PLT
	movq	%rax, %rdi
	call	neo_str_create@PLT
	movq	%rax, %rbp
	movq	%rdx, %r12
	movl	%edx, %ebx
.L352:
	movb	%r15b, %r13b
	xorl	%edx, %edx
	movq	%r14, %rdi
	movb	%bl, %r12b
	movq	%r13, %rsi
	call	general_neobject_destroy@PLT
	movq	%rbp, %rax
	movq	%r12, %rdx
	jmp	.L407
.L453:
	movq	%rbx, %r9
	movq	%r15, %rbx
	movq	%r13, %r15
.L299:
	movl	16(%r15), %r10d
	xorl	%r12d, %r12d
	movq	%r9, %r14
	testl	%r10d, %r10d
	jg	.L316
	jmp	.L455
	.p2align 4,,10
	.p2align 3
.L319:
	addq	$1, %r12
	cmpl	%r12d, %edx
	jle	.L318
.L316:
	movq	%r12, %rax
	movl	%r12d, %r13d
	salq	$4, %rax
	addq	8(%r15), %rax
	movq	(%rax), %rdi
	movq	8(%rax), %rsi
	call	neo_is_void@PLT
	movl	16(%r15), %edx
	testb	%al, %al
	je	.L319
	movq	%r14, %r9
	movl	%edx, %eax
.L320:
	movq	16(%rbp), %r10
	movzwl	36(%r10,%rbx), %edx
	cmpl	%eax, %r12d
	jge	.L456
	cmpb	$-79, 54(%rsp)
	movq	%rbp, (%rsp)
	movq	%rbx, %r14
	sete	55(%rsp)
	xorl	%ecx, %ecx
	movq	24(%rsp), %rbp
	movq	%r9, 40(%rsp)
	movq	%rcx, %rbx
	jmp	.L328
	.p2align 4,,10
	.p2align 3
.L322:
	addq	$1, %rbx
	cmpl	%eax, %r12d
	jge	.L420
.L328:
	leaq	(%r10,%r14), %r11
	movl	%edx, %r13d
	cmpl	%ebx, %edx
	jle	.L420
	movl	32(%r11), %esi
	leaq	0(,%rbx,4), %rcx
	leaq	(%r10,%rcx), %rdi
	movl	(%rdi,%rsi), %esi
	movq	%rsi, %rdi
	addq	%r10, %rsi
	cmpl	$1, (%rsi)
	jne	.L321
	cmpl	$37, 8(%rsi)
	je	.L322
.L321:
	pxor	%xmm1, %xmm1
	movl	%edi, %esi
	movq	(%rsp), %rdi
	movq	%rcx, 24(%rsp)
	movups	%xmm1, 16(%r11)
	movslq	%r12d, %r13
	movb	$54, 24(%r11)
	salq	$4, %r13
	call	eval_aux
	movq	8(%r15), %rsi
	movq	32(%rsp), %rcx
	movb	54(%rsp), %bpl
	movq	%rax, (%rsi,%r13)
	movq	(%rsp), %rax
	movq	%rdx, 8(%rsi,%r13)
	movq	global_env(%rip), %rsi
	movq	16(%rax), %rdx
	movl	(%rsi), %r9d
	leaq	(%rdx,%r14), %rax
	testl	%r9d, %r9d
	movq	%rcx, 16(%rax)
	movq	24(%rsp), %rcx
	movq	%rbp, 24(%rax)
	jne	.L457
	testl	%r12d, %r12d
	jne	.L324
	cmpb	$0, 55(%rsp)
	jne	.L458
.L324:
	cmpl	%r12d, 16(%r15)
	jg	.L325
	jmp	.L415
	.p2align 4,,10
	.p2align 3
.L327:
	addl	$1, %r12d
	addq	$16, %r13
	cmpl	%edx, %r12d
	jge	.L415
.L325:
	movq	8(%r15), %rax
	movq	(%rax,%r13), %rdi
	movq	8(%rax,%r13), %rsi
	call	neo_is_void@PLT
	movl	16(%r15), %edx
	testb	%al, %al
	je	.L327
	movq	(%rsp), %rcx
	movl	%edx, %eax
	movq	16(%rcx), %r10
	movzwl	36(%r10,%r14), %edx
	movl	%edx, %r13d
	jmp	.L322
.L415:
	movq	%rbp, 24(%rsp)
	movq	(%rsp), %rbp
	movq	%r14, %rbx
	movq	40(%rsp), %r9
	movq	16(%rbp), %rax
	movzwl	36(%rax,%r14), %edi
.L317:
	movl	16(%r9), %eax
	cmpl	%edi, %eax
	jg	.L459
.L331:
	xorl	%ecx, %ecx
	cmpb	$0, 20(%r9)
	movl	$54, %r8d
	je	.L335
	movl	16(%r15), %r12d
	movl	48(%rsp), %edi
	movl	%eax, %edx
	movq	%r9, (%rsp)
	subl	24(%r9), %edx
	movl	%r12d, %ecx
	subl	%edi, %ecx
	cmpl	%edx, %ecx
	jg	.L336
	xorl	%edi, %edi
	call	nelist_create@PLT
	movq	(%rsp), %r9
.L337:
	movq	%rax, %rdi
	movq	%r9, (%rsp)
	call	neo_list_convert@PLT
	movq	(%rsp), %r9
	movq	%rax, %rcx
	movq	%rdx, %r8
.L335:
	movq	16(%rbp), %rax
	pxor	%xmm0, %xmm0
	addq	%rbx, %rax
	cmpb	$-79, 54(%rsp)
	movups	%xmm0, 16(%rax)
	movb	$54, 24(%rax)
	je	.L460
	movq	%rcx, %rdx
	movq	%r15, %rsi
	movq	%r8, %rcx
	movq	%r9, %rdi
	call	callUserFunc
	movq	%rax, %r13
	movq	%rdx, %r12
.L341:
	movq	16(%rbp), %rax
	movq	32(%rsp), %rdi
	xorl	%edx, %edx
	movq	24(%rsp), %rcx
	movb	54(%rsp), %cl
	addq	%rbx, %rax
	movq	%rdi, 16(%rax)
	movq	%rcx, %rsi
	movq	%rcx, 24(%rax)
	call	general_neobject_destroy@PLT
	addq	16(%rbp), %rbx
	pxor	%xmm0, %xmm0
	xorl	%esi, %esi
	movups	%xmm0, 16(%rbx)
	movq	%r15, %rdi
	movb	$54, 24(%rbx)
	call	general_nelist_destroy@PLT
	movq	global_env(%rip), %rax
	movl	(%rax), %r8d
	testl	%r8d, %r8d
	je	.L428
	jmp	.L433
.L264:
	movl	8(%rdx), %edi
	movl	$64, %esi
	call	operatorIs@PLT
	movq	16(%rbp), %rdx
	movq	%rbp, %rdi
	movl	12(%rdx,%rbx), %esi
	testb	%al, %al
	je	.L268
	call	get_address
	movq	%rax, %r12
	movq	global_env(%rip), %rax
	movl	(%rax), %r8d
	testl	%r8d, %r8d
	jne	.L433
	movq	16(%rbp), %rax
	movq	%rbp, %rdi
	movl	16(%rax,%rbx), %esi
	call	get_address
	movq	%rax, %rsi
	movq	global_env(%rip), %rax
	movl	(%rax), %edi
	testl	%edi, %edi
	jne	.L433
	movq	16(%rbp), %rax
	movq	%r12, %rdi
	movslq	8(%rax,%rbx), %rdx
	leaq	operators_functions(%rip), %rax
	movq	(%rax,%rdx,8), %rax
	addq	$72, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	popq	%rbx
	.cfi_restore 3
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_restore 6
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_restore 12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_restore 13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_restore 14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_restore 15
	.cfi_def_cfa_offset 8
	jmp	*%rax
.L452:
	.cfi_restore_state
	xorl	%eax, %eax
	movl	%eax, 48(%rsp)
.L364:
	movl	24(%r9), %eax
	movl	%edi, %edx
	movl	48(%rsp), %ecx
	subl	%eax, %edx
	addl	%r8d, %eax
	subl	%ecx, %eax
	cmpl	%edx, %r8d
	cmovg	%eax, %edi
	jmp	.L292
.L463:
	movq	32(%r14), %rdi
	movl	%r13d, %esi
	call	nelist_nth@PLT
	movq	%rax, %rdi
	movq	%rdx, %rsi
	call	neo_copy@PLT
	movq	8(%r15), %rcx
	movq	%rax, (%rcx,%r12)
	movq	8(%r15), %rax
	movq	%rdx, 8(%rcx,%r12)
	movq	(%rax,%r12), %rdi
	movq	8(%rax,%r12), %rsi
	call	neo_is_void@PLT
	testb	%al, %al
	jne	.L461
	testq	%r13, %r13
	jne	.L332
.L437:
	movq	global_env(%rip), %rax
	xorl	%esi, %esi
	movq	%r15, %rdi
	movl	$110, (%rax)
	call	general_nelist_destroy@PLT
	movq	16(%rbp), %rax
	movq	16(%rax,%rbx), %rdi
	movq	24(%rax,%rbx), %rsi
	jmp	.L436
.L448:
	movq	16(%rax), %rdi
	movq	24(%rax), %rsi
	xorl	%edx, %edx
	call	general_neobject_destroy@PLT
	addq	16(%rbp), %rbx
	pxor	%xmm5, %xmm5
	xorl	%eax, %eax
	movups	%xmm5, 16(%rbx)
	movl	$54, %edx
	movb	$54, 24(%rbx)
	jmp	.L407
.L247:
	movq	16(%rbp), %rax
	movl	$14, %esi
	movq	%rbp, %rdi
	movzwl	4(%rax,%rbx), %edx
	call	NeTree_create@PLT
	movq	16(%rbp), %rdx
	movq	%rbp, %rdi
	movl	%eax, %r14d
	addq	%rdx, %rbx
	addq	%r14, %rdx
	movq	%r14, %rsi
	movq	32(%rbx), %rax
	movq	%rax, 32(%rdx)
	movl	8(%rbx), %eax
	movq	%r13, 16(%rdx)
	movl	%eax, 8(%rdx)
	movq	%r12, 24(%rdx)
	xorl	%edx, %edx
	call	create_new_process@PLT
	movq	global_env(%rip), %rdx
	movl	(%rdx), %ecx
	testl	%ecx, %ecx
	je	.L248
	movq	16(%rbp), %rax
	xorl	%edx, %edx
	movq	16(%rax,%r14), %rdi
	movq	24(%rax,%r14), %rsi
	call	general_neobject_destroy@PLT
	xorl	%eax, %eax
	movl	$54, %edx
	jmp	.L407
.L413:
	xorl	%esi, %esi
	movq	%r13, %rdi
	movq	%r15, %rbx
	call	general_nelist_destroy@PLT
	movq	16(%rbp), %rax
	xorl	%edx, %edx
	movq	16(%rax,%r15), %rdi
	movq	24(%rax,%r15), %rsi
	call	general_neobject_destroy@PLT
	addq	16(%rbp), %rbx
	pxor	%xmm0, %xmm0
	movl	$54, %edx
	movups	%xmm0, 16(%rbx)
	movb	$54, 24(%rbx)
	movq	global_env(%rip), %rax
	movl	$93, (%rax)
	xorl	%eax, %eax
	jmp	.L407
.L268:
	call	eval_aux
	movq	%rax, %r13
	movq	global_env(%rip), %rax
	movq	%rdx, %r12
	movl	(%rax), %esi
	testl	%esi, %esi
	jne	.L434
	movq	16(%rbp), %rax
	movq	%rbp, %rdi
	movl	16(%rax,%rbx), %esi
	call	eval_aux
	movq	%rax, %r15
	movq	global_env(%rip), %rax
	movq	%rdx, %r14
	movl	(%rax), %ecx
	testl	%ecx, %ecx
	jne	.L462
	movq	16(%rbp), %rax
	movq	%r14, %rcx
	movq	%r13, %rdi
	movq	%r12, %rsi
	movq	%r15, %rdx
	movslq	8(%rax,%rbx), %r10
	leaq	operators_functions(%rip), %rax
	call	*(%rax,%r10,8)
	movq	%r13, %rdi
	movq	%r12, %rsi
	movq	%rdx, %rbx
	xorl	%edx, %edx
	movq	%rax, %rbp
	call	general_neobject_destroy@PLT
	xorl	%edx, %edx
	movq	%r15, %rdi
	movq	%r14, %rsi
	call	general_neobject_destroy@PLT
	movq	%rbp, %rax
	movq	%rbx, %rdx
	jmp	.L407
.L449:
	movq	global_env(%rip), %rax
	xorl	%esi, %esi
	movq	%r14, %rdi
	movl	$14, (%rax)
	call	general_nelist_destroy@PLT
	movq	16(%rbp), %rax
	xorl	%edx, %edx
	movq	16(%rax,%rbx), %rdi
	movq	24(%rax,%rbx), %rsi
	call	general_neobject_destroy@PLT
	addq	16(%rbp), %rbx
	pxor	%xmm6, %xmm6
	xorl	%eax, %eax
	movups	%xmm6, 16(%rbx)
	movl	$54, %edx
	movb	$54, 24(%rbx)
	jmp	.L407
.L450:
	movq	16(%rbp), %rax
	movq	%rbp, %rdi
	movl	12(%rax,%rbx), %esi
	call	eval_aux
	movq	%rax, %r15
	movq	global_env(%rip), %rax
	movq	%rdx, %r14
	movl	(%rax), %eax
	testl	%eax, %eax
	jne	.L433
	movq	16(%rbp), %rax
	movq	%r15, %rdi
	movq	%r14, %rsi
	movslq	8(%rax,%rbx), %rdx
	leaq	operators_functions(%rip), %rax
	call	*(%rax,%rdx,8)
	jmp	.L423
.L318:
	movq	%r14, %r9
	movl	%edx, %eax
	leal	1(%r13), %r12d
	jmp	.L320
.L350:
	movq	global_env(%rip), %rax
	movb	%r15b, %r13b
	xorl	%edx, %edx
	movq	%r14, %rdi
	movq	%r13, %rsi
	movl	$18, (%rax)
	call	general_neobject_destroy@PLT
	xorl	%eax, %eax
	movl	$54, %edx
	jmp	.L407
.L420:
	movq	%rbp, 24(%rsp)
	movq	40(%rsp), %r9
	movq	%r14, %rbx
	movl	%r13d, %edi
	movq	(%rsp), %rbp
	jmp	.L317
.L248:
	addq	$72, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	movl	%eax, %edi
	popq	%rbx
	.cfi_restore 3
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_restore 6
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_restore 12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_restore 13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_restore 14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_restore 15
	.cfi_def_cfa_offset 8
	jmp	neo_promise_create@PLT
.L459:
	.cfi_restore_state
	xorl	%r13d, %r13d
	movq	%r9, %r14
.L334:
	movq	8(%r15), %rax
	movq	%r13, %r12
	salq	$4, %r12
	movq	(%rax,%r12), %rdi
	movq	8(%rax,%r12), %rsi
	call	neo_is_void@PLT
	testb	%al, %al
	jne	.L463
.L332:
	movl	16(%r14), %eax
	addq	$1, %r13
	cmpl	%r13d, %eax
	jg	.L334
	movq	%r14, %r9
	jmp	.L331
	.p2align 4,,10
	.p2align 3
.L458:
	movl	32(%rax), %eax
	addq	%rcx, %rdx
	movq	(%rsp), %rdi
	movl	(%rdx,%rax), %esi
	call	get_address
	movq	%rax, 56(%rsp)
	jmp	.L324
.L349:
	movb	$-122, %r13b
	movq	%r14, %rdi
	movq	%r13, %rsi
	call	neo_list_len@PLT
	cltq
	cmpq	%rax, %r12
	jnb	.L350
	movl	%r12d, %edx
	movq	%r14, %rdi
	movq	%r13, %rsi
	call	neo_list_nth@PLT
	movq	%rax, %rdi
	movq	%rdx, %rsi
	call	neo_copy@PLT
	movq	%rax, %rbp
	movq	%rdx, %r12
	movl	%edx, %ebx
	jmp	.L352
.L451:
	movq	global_env(%rip), %rax
	movq	32(%rsp), %rdi
	xorl	%edx, %edx
	movq	24(%rsp), %rsi
	movl	$6, (%rax)
	jmp	.L432
.L457:
	movq	(%rsp), %rbp
	movq	%r14, %rbx
.L429:
	xorl	%esi, %esi
	movq	%r15, %rdi
	call	general_nelist_destroy@PLT
	movq	16(%rbp), %rax
	xorl	%edx, %edx
	movq	16(%rax,%rbx), %rdi
	movq	24(%rax,%rbx), %rsi
	call	general_neobject_destroy@PLT
	addq	16(%rbp), %rbx
	pxor	%xmm7, %xmm7
	movups	%xmm7, 16(%rbx)
	movb	$54, 24(%rbx)
	jmp	.L433
.L460:
	movq	56(%rsp), %rsi
	movq	%r15, %rdx
	movq	%r9, %rdi
	call	callUserMethod
	movq	%rax, %r13
	movq	%rdx, %r12
	jmp	.L341
.L336:
	subl	%eax, %r12d
	movl	%r12d, %edi
	call	nelist_create@PLT
	testl	%r12d, %r12d
	movq	(%rsp), %r9
	jle	.L337
	xorl	%r13d, %r13d
	movq	%rbx, (%rsp)
	movslq	%r12d, %r12
	movq	%r9, %r14
	movq	%rax, %rbx
	xchgq	%r13, %rbp
	.p2align 4
	.p2align 3
.L339:
	movl	16(%r14), %esi
	movq	%r15, %rdi
	addl	%ebp, %esi
	call	nelist_nth@PLT
	movq	%rdx, %rdi
	movq	%rbp, %rdx
	movq	%rax, %rsi
	addq	$1, %rbp
	salq	$4, %rdx
	addq	8(%rbx), %rdx
	movq	%rsi, (%rdx)
	movq	%rdi, 8(%rdx)
	cmpq	%r12, %rbp
	jne	.L339
	movq	%rbx, %rax
	movq	%r14, %r9
	movq	(%rsp), %rbx
	movq	%r13, %rbp
	jmp	.L337
.L462:
	xorl	%edx, %edx
	movq	%r13, %rdi
	movq	%r12, %rsi
	call	general_neobject_destroy@PLT
	xorl	%edx, %edx
	movq	%r15, %rdi
	movq	%r14, %rsi
	call	general_neobject_destroy@PLT
	xorl	%eax, %eax
	movl	$54, %edx
	jmp	.L407
.L314:
	movl	32(%rdx), %eax
	addq	%r10, %r14
	movq	%rbp, %rdi
	movl	(%r14,%rax), %eax
	movl	16(%r10,%rax), %esi
	call	get_address
	movq	16(%rbp), %r10
	movq	%rax, 56(%rsp)
	movq	%r10, %rsi
	leaq	(%r10,%r15), %rdx
	jmp	.L419
.L365:
	xorl	%eax, %eax
	jmp	.L286
.L455:
	movzwl	36(%rsi,%rbx), %edi
	jmp	.L317
.L454:
	movq	%r15, %rbx
	movq	%r13, %r15
	jmp	.L429
.L368:
	xorl	%r9d, %r9d
	jmp	.L306
.L312:
	xorl	%esi, %esi
	movq	%r13, %rdi
	movq	%r15, %rbx
	call	general_nelist_destroy@PLT
	movq	16(%rbp), %rax
	xorl	%edx, %edx
	movq	16(%rax,%r15), %rdi
	movq	24(%rax,%r15), %rsi
	call	general_neobject_destroy@PLT
	movq	global_env(%rip), %rax
	addq	16(%rbp), %rbx
	pxor	%xmm0, %xmm0
	movups	%xmm0, 16(%rbx)
	movl	$54, %edx
	movb	$54, 24(%rbx)
	movl	$94, (%rax)
	xorl	%eax, %eax
	jmp	.L407
.L456:
	movzwl	%dx, %edi
	jmp	.L317
.L461:
	movq	global_env(%rip), %rax
	xorl	%edx, %edx
	movl	$7, (%rax)
	movq	16(%rbp), %rax
	movq	16(%rax,%rbx), %rdi
	movq	24(%rax,%rbx), %rsi
	call	general_neobject_destroy@PLT
	addq	16(%rbp), %rbx
	pxor	%xmm0, %xmm0
	xorl	%esi, %esi
	movups	%xmm0, 16(%rbx)
	movq	%r15, %rdi
	movb	$54, 24(%rbx)
	call	general_nelist_destroy@PLT
	jmp	.L433
	.cfi_endproc
.LFE35:
	.size	eval_aux, .-eval_aux
	.p2align 4
	.globl	isTrue
	.type	isTrue, @function
isTrue:
.LFB25:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	xorl	%ebp, %ebp
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	subq	$24, %rsp
	.cfi_def_cfa_offset 48
	call	eval_aux
	movq	%rdx, %rbx
	movq	global_env(%rip), %rdx
	movl	(%rdx), %ecx
	testl	%ecx, %ecx
	je	.L471
	addq	$24, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	movl	%ebp, %eax
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L471:
	.cfi_restore_state
	movq	%rax, %rdi
	movq	%rbx, %rsi
	cmpb	$8, %bl
	je	.L472
	cmpb	$4, %bl
	je	.L473
	movl	$20, (%rdx)
.L467:
	movq	%rbx, %rsi
	xorl	%edx, %edx
	call	general_neobject_destroy@PLT
	addq	$24, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	movl	%ebp, %eax
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L472:
	.cfi_restore_state
	movb	$8, %sil
	movq	%rax, 8(%rsp)
	call	neo_to_bool@PLT
	movq	8(%rsp), %rdi
	movl	%eax, %ebp
	jmp	.L467
	.p2align 4,,10
	.p2align 3
.L473:
	movb	$4, %sil
	movq	%rax, 8(%rsp)
	call	neo_to_integer@PLT
	movq	8(%rsp), %rdi
	testq	%rax, %rax
	setne	%bpl
	jmp	.L467
	.cfi_endproc
.LFE25:
	.size	isTrue, .-isTrue
	.p2align 4
	.globl	treeToList
	.type	treeToList, @function
treeToList:
.LFB32:
	.cfi_startproc
	pushq	%r14
	.cfi_def_cfa_offset 16
	.cfi_offset 14, -16
	movq	%rdi, %r14
	pushq	%r13
	.cfi_def_cfa_offset 24
	.cfi_offset 13, -24
	pushq	%r12
	.cfi_def_cfa_offset 32
	.cfi_offset 12, -32
	movq	%rsi, %r12
	pushq	%rbp
	.cfi_def_cfa_offset 40
	.cfi_offset 6, -40
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	movzwl	4(%rsi), %edi
	call	nelist_create@PLT
	movzwl	4(%r12), %ecx
	movq	%rax, %r13
	testw	%cx, %cx
	je	.L474
	xorl	%ebx, %ebx
	jmp	.L480
	.p2align 4,,10
	.p2align 3
.L486:
	movq	%r14, %rdi
	call	eval_aux
	movq	%rax, 0(%rbp)
	movq	global_env(%rip), %rax
	movq	%rdx, 8(%rbp)
	movl	(%rax), %eax
	testl	%eax, %eax
	jne	.L477
	movzwl	4(%r12), %ecx
	addq	$1, %rbx
	movzwl	%cx, %eax
	cmpl	%ebx, %eax
	jle	.L474
.L480:
	movl	(%r12), %edx
	leaq	0(,%rbx,4), %rax
	addq	16(%r14), %rax
	movq	%rbx, %rbp
	salq	$4, %rbp
	addq	8(%r13), %rbp
	movl	(%rax,%rdx), %esi
	cmpl	$-1, %esi
	jne	.L486
	pxor	%xmm0, %xmm0
	addq	$1, %rbx
	movzwl	%cx, %eax
	movups	%xmm0, 0(%rbp)
	movb	$54, 8(%rbp)
	cmpl	%ebx, %eax
	jg	.L480
.L474:
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
.L477:
	.cfi_restore_state
	leal	-1(%rbx), %esi
	movq	%r13, %rdi
	xorl	%r13d, %r13d
	call	nelist_destroy_until@PLT
	popq	%rbx
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
	.cfi_endproc
.LFE32:
	.size	treeToList, .-treeToList
	.p2align 4
	.globl	get_address
	.type	get_address, @function
get_address:
.LFB36:
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
	movl	%esi, %ebp
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	movq	%rdi, %rbx
	subq	$24, %rsp
	.cfi_def_cfa_offset 80
	movq	16(%rdi), %rdx
	addq	%rbp, %rdx
	movl	(%rdx), %eax
	cmpl	$13, %eax
	je	.L488
	cmpl	$15, %eax
	je	.L489
	cmpl	$12, %eax
	jne	.L490
	movl	8(%rdx), %edi
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
	jmp	get_absolute_address@PLT
	.p2align 4,,10
	.p2align 3
.L489:
	.cfi_restore_state
	movl	8(%rdx), %esi
	call	eval_aux
	movq	%rax, %r12
	movq	global_env(%rip), %rax
	movq	%rdx, %r13
	movl	(%rax), %eax
	testl	%eax, %eax
	jne	.L519
	cmpb	$-77, %dl
	je	.L502
	xorl	%edx, %edx
	movq	%r12, %rdi
	movq	%r13, %rsi
	call	general_neobject_destroy@PLT
	movq	global_env(%rip), %rax
	movl	$80, (%rax)
	.p2align 4
	.p2align 3
.L519:
	xorl	%eax, %eax
.L487:
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
.L488:
	.cfi_restore_state
	movl	8(%rdx), %esi
	call	get_address
	movq	global_env(%rip), %rdx
	movl	(%rdx), %ecx
	testl	%ecx, %ecx
	jne	.L519
	movq	(%rax), %r13
	movq	8(%rax), %r12
	movq	%rbx, %rdi
	movzbl	8(%rax), %r14d
	movq	16(%rbx), %rax
	movl	12(%rax,%rbp), %esi
	call	eval_aux
	movq	%rax, %rbp
	movq	global_env(%rip), %rax
	movq	%rdx, %rbx
	movl	(%rax), %edx
	testl	%edx, %edx
	jne	.L519
	cmpb	$-122, %r14b
	je	.L494
	cmpb	$-125, %r14b
	je	.L494
	movl	$15, (%rax)
	jmp	.L519
	.p2align 4,,10
	.p2align 3
.L490:
	movq	global_env(%rip), %rax
	movl	$89, (%rax)
	jmp	.L519
	.p2align 4,,10
	.p2align 3
.L494:
	cmpb	$4, %bl
	jne	.L520
	movq	%rbp, %rdi
	movq	%rbx, %rsi
	call	neo_to_integer@PLT
	xorl	%edx, %edx
	movq	%rbp, %rdi
	movq	%rbx, %rsi
	movq	%rax, %r15
	call	general_neobject_destroy@PLT
	cmpb	$-122, %r14b
	je	.L497
	movb	$-125, %r12b
	movq	%r13, %rdi
	movq	%r12, %rsi
	call	neo_to_string@PLT
	movq	%rax, %rdi
	call	strlen@PLT
	movq	%rax, %rdx
	movq	global_env(%rip), %rax
	cmpq	%rdx, %r15
	jnb	.L498
	movl	$105, (%rax)
	jmp	.L519
	.p2align 4,,10
	.p2align 3
.L502:
	movq	%rdx, %rsi
	movq	%r12, %rdi
	call	neo_to_container@PLT
	movq	16(%rbx), %r15
	movq	%rax, %r14
	addq	%rbp, %r15
	movl	12(%r15), %esi
	cmpl	$-1, %esi
	je	.L504
	movl	4(%rax), %eax
	cmpl	%eax, 16(%r15)
	je	.L505
.L504:
	movq	24(%r15), %rsi
	movq	%r14, %rdi
	call	get_field_index@PLT
	movl	%eax, 12(%r15)
	movq	16(%rbx), %rax
	movq	24(%rax,%rbp), %rdi
	call	free@PLT
	movl	4(%r14), %eax
	addq	16(%rbx), %rbp
	movq	$0, 24(%rbp)
	movl	12(%rbp), %esi
	movl	%eax, 16(%rbp)
.L505:
	movq	%r14, %rdi
	call	get_container_field_addr@PLT
	xorl	%edx, %edx
	movq	%r12, %rdi
	movq	%r13, %rsi
	movq	%rax, 8(%rsp)
	call	general_neobject_destroy@PLT
	movq	8(%rsp), %rax
	jmp	.L487
	.p2align 4,,10
	.p2align 3
.L520:
	xorl	%edx, %edx
	movq	%rbp, %rdi
	movq	%rbx, %rsi
	call	general_neobject_destroy@PLT
	movq	global_env(%rip), %rax
	movl	$16, (%rax)
	jmp	.L519
.L521:
	movq	global_env(%rip), %rax
.L498:
	movl	$18, (%rax)
	jmp	.L519
.L497:
	movq	%r13, %rdi
	movq	%r12, %rsi
	call	neo_list_len@PLT
	cltq
	cmpq	%rax, %r15
	jnb	.L521
	movb	$-122, %r12b
	movq	%r13, %rdi
	movq	%r12, %rsi
	call	neo_to_list@PLT
	addq	$24, %rsp
	.cfi_def_cfa_offset 56
	movl	%r15d, %esi
	popq	%rbx
	.cfi_def_cfa_offset 48
	movq	%rax, %rdi
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
	jmp	nelist_nth_addr@PLT
	.cfi_endproc
.LFE36:
	.size	get_address, .-get_address
	.p2align 4
	.globl	execConditionBlock
	.type	execConditionBlock, @function
execConditionBlock:
.LFB37:
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
	movl	%esi, %r13d
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
	movq	global_env(%rip), %rax
	movq	16(%r12), %rdx
	movq	200(%rax), %rax
	leaq	(%rdx,%r13), %r8
	cmpw	$0, 12(%r8)
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	movq	(%rdi), %rax
	movq	%rax, 24(%rsp)
	je	.L523
	xorl	%ebx, %ebx
.L549:
	movslq	%ebx, %rbp
	movl	8(%r8), %eax
	movq	%r12, %rdi
	salq	$2, %rbp
	leaq	(%rdx,%rbp), %rcx
	movl	(%rcx,%rax), %eax
	movl	8(%rdx,%rax), %esi
	call	eval_aux
	movq	%rax, %rdi
	movq	global_env(%rip), %rax
	movl	(%rax), %r11d
	testl	%r11d, %r11d
	jne	.L525
	movq	%rdx, %rsi
	cmpb	$8, %dl
	je	.L560
	cmpb	$4, %dl
	je	.L561
	movl	$20, (%rax)
	xorl	%edx, %edx
	call	general_neobject_destroy@PLT
	movq	global_env(%rip), %rdi
	movl	(%rdi), %r10d
	testl	%r10d, %r10d
	jne	.L525
.L530:
	movq	16(%r12), %rdx
	addl	$1, %ebx
	leaq	(%rdx,%r13), %rax
	movzwl	12(%rax), %ecx
	cmpl	%ecx, %ebx
	jge	.L537
	addq	$4, %rbp
	xorl	%r15d, %r15d
	jmp	.L538
	.p2align 4,,10
	.p2align 3
.L542:
	movq	16(%r12), %rdx
	addl	$1, %ebx
	addq	$4, %rbp
	leaq	(%rdx,%r13), %rax
	movzwl	12(%rax), %ecx
	cmpl	%ebx, %ecx
	jle	.L559
.L538:
	movl	8(%rax), %esi
	leaq	(%rdx,%rbp), %rax
	movl	(%rax,%rsi), %eax
	addq	%rdx, %rax
	cmpl	$8, (%rax)
	jne	.L562
	movl	8(%rax), %esi
	movq	%r12, %rdi
	call	eval_aux
	movq	%rax, %rdi
	movq	%rdx, %rsi
	cmpb	$8, %dl
	je	.L563
	cmpb	$4, %dl
	je	.L564
	movq	global_env(%rip), %rax
	xorl	%edx, %edx
	movl	$20, (%rax)
	call	general_neobject_destroy@PLT
	movq	global_env(%rip), %rdi
	movl	(%rdi), %esi
	testl	%esi, %esi
	je	.L542
.L525:
	xorl	%r15d, %r15d
.L550:
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
.L563:
	.cfi_restore_state
	movb	$8, %sil
	movq	%rdx, 16(%rsp)
	movq	%rax, 8(%rsp)
	call	neo_to_bool@PLT
	movq	8(%rsp), %rdi
	movq	16(%rsp), %rcx
	movl	%eax, %r14d
.L540:
	movq	%rcx, %rsi
	xorl	%edx, %edx
	call	general_neobject_destroy@PLT
	movq	global_env(%rip), %rdi
	movl	(%rdi), %ecx
	testl	%ecx, %ecx
	jne	.L525
	movl	%r15d, %eax
	xorl	$1, %eax
	andb	%al, %r14b
	je	.L542
	movq	200(%rdi), %rax
	movq	(%rax), %rax
	movq	80(%rax), %r15
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%r15, %rdi
	call	ptrlist_append@PLT
	movq	16(%r12), %rax
	movq	%r12, %rdi
	movl	8(%rax,%r13), %edx
	leaq	(%rax,%rbp), %rcx
	movl	(%rcx,%rdx), %edx
	movl	12(%rax,%rdx), %esi
	call	exec_aux
	movl	%eax, %r15d
	movq	global_env(%rip), %rax
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	80(%rax), %rdi
	call	deleteContext
	movq	global_env(%rip), %rdi
	movl	(%rdi), %edx
	testl	%edx, %edx
	jne	.L525
	testl	%r15d, %r15d
	jne	.L550
	movl	%r14d, %r15d
	jmp	.L542
	.p2align 4,,10
	.p2align 3
.L564:
	movb	$4, %sil
	movq	%rdx, 16(%rsp)
	movq	%rax, 8(%rsp)
	call	neo_to_integer@PLT
	movq	8(%rsp), %rdi
	movq	16(%rsp), %rcx
	testq	%rax, %rax
	setne	%r14b
	jmp	.L540
	.p2align 4,,10
	.p2align 3
.L562:
	cmpl	%ebx, %ecx
	jle	.L559
	movslq	%ebx, %rbp
	salq	$2, %rbp
	leaq	(%rdx,%rbp), %rax
	movl	(%rax,%rsi), %eax
	cmpl	$9, (%rdx,%rax)
	je	.L546
.L559:
	leaq	(%rdx,%r13), %r8
	movzwl	12(%r8), %esi
.L536:
	cmpl	%esi, %ebx
	jl	.L549
	.p2align 4
	.p2align 3
.L537:
	movq	200(%rdi), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
.L523:
	movq	24(%rsp), %rsi
	call	partialRestore@PLT
	jmp	.L525
	.p2align 4,,10
	.p2align 3
.L560:
	movb	$8, %sil
	movq	%rdi, 8(%rsp)
	movq	%rdx, 16(%rsp)
	call	neo_to_bool@PLT
	movq	8(%rsp), %rdi
	movq	16(%rsp), %rcx
	movl	%eax, %r14d
.L527:
	xorl	%edx, %edx
	movq	%rcx, %rsi
	call	general_neobject_destroy@PLT
	movq	global_env(%rip), %rdi
	movl	(%rdi), %r9d
	testl	%r9d, %r9d
	jne	.L525
	testb	%r14b, %r14b
	je	.L530
	movq	200(%rdi), %rax
	movq	(%rax), %rax
	movq	80(%rax), %r14
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%r14, %rdi
	call	ptrlist_append@PLT
	movq	16(%r12), %rax
	movq	%r12, %rdi
	movl	8(%rax,%r13), %edx
	leaq	(%rax,%rbp), %rcx
	movl	(%rcx,%rdx), %edx
	movl	12(%rax,%rdx), %esi
	call	exec_aux
	movl	%eax, %r15d
	movq	global_env(%rip), %rax
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	80(%rax), %rdi
	call	deleteContext
	movq	global_env(%rip), %rdi
	movl	(%rdi), %r8d
	testl	%r8d, %r8d
	jne	.L525
	testl	%r15d, %r15d
	jne	.L550
	movq	16(%r12), %rdx
	addl	$1, %ebx
	leaq	(%rdx,%r13), %r8
	movzwl	12(%r8), %esi
	cmpl	%esi, %ebx
	jge	.L537
	movl	8(%r8), %eax
	leaq	4(%rbp,%rax), %rax
	addq	%rdx, %rax
	jmp	.L533
	.p2align 5
	.p2align 4,,10
	.p2align 3
.L535:
	addl	$1, %ebx
	addq	$4, %rax
	cmpl	%esi, %ebx
	je	.L537
.L533:
	movl	(%rax), %ecx
	cmpl	$7, (%rdx,%rcx)
	jne	.L535
	cmpl	%ebx, %esi
	jne	.L536
	jmp	.L537
	.p2align 4,,10
	.p2align 3
.L546:
	testb	%r15b, %r15b
	je	.L547
.L548:
	addl	$1, %ebx
	jmp	.L559
	.p2align 4,,10
	.p2align 3
.L561:
	movb	$4, %sil
	movq	%rdi, 8(%rsp)
	movq	%rdx, 16(%rsp)
	call	neo_to_integer@PLT
	movq	8(%rsp), %rdi
	movq	16(%rsp), %rcx
	testq	%rax, %rax
	setne	%r14b
	jmp	.L527
.L547:
	movq	200(%rdi), %rax
	movq	(%rax), %rax
	movq	80(%rax), %r14
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%r14, %rdi
	call	ptrlist_append@PLT
	movq	16(%r12), %rax
	movq	%r12, %rdi
	movl	8(%rax,%r13), %edx
	addq	%rbp, %rax
	movl	(%rax,%rdx), %esi
	call	exec_aux
	movl	%eax, %r15d
	movq	global_env(%rip), %rax
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	80(%rax), %rdi
	call	deleteContext
	movq	global_env(%rip), %rdi
	movl	(%rdi), %eax
	testl	%eax, %eax
	jne	.L525
	testl	%r15d, %r15d
	jne	.L550
	movq	16(%r12), %rdx
	jmp	.L548
	.cfi_endproc
.LFE37:
	.size	execConditionBlock, .-execConditionBlock
	.p2align 4
	.globl	execStatementFor
	.type	execStatementFor, @function
execStatementFor:
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
	movq	%rdi, %r12
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	movl	%esi, %ebx
	subq	$56, %rsp
	.cfi_def_cfa_offset 112
	movq	16(%rdi), %rdx
	leaq	(%rdx,%rbx), %rsi
	movzwl	16(%rsi), %eax
	cmpw	$4, %ax
	je	.L596
	cmpw	$3, %ax
	je	.L597
	cmpw	$2, %ax
	je	.L598
	movq	global_env(%rip), %rax
	movl	$108, (%rax)
.L571:
	xorl	%r15d, %r15d
.L593:
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
.L598:
	.cfi_restore_state
	xorl	%edi, %edi
	call	neo_integer_create@PLT
	movq	%rax, %r14
	movq	global_env(%rip), %rax
	movq	%rdx, %r13
	movl	(%rax), %ecx
	testl	%ecx, %ecx
	jne	.L571
	movq	16(%r12), %rax
	movq	%r12, %rdi
	movl	12(%rax,%rbx), %edx
	movl	4(%rax,%rdx), %esi
	call	eval_aux
	movq	%rax, %rbp
	movq	%rdx, %rcx
.L575:
	movl	$1, (%rsp)
.L570:
	movq	global_env(%rip), %rax
	movl	(%rax), %r15d
	testl	%r15d, %r15d
	jne	.L599
	cmpb	$4, %r13b
	jne	.L579
	cmpb	$4, %cl
	je	.L580
.L579:
	xorl	%edx, %edx
	movq	%r14, %rdi
	movq	%r13, %rsi
	movq	%rcx, (%rsp)
	call	general_neobject_destroy@PLT
	movq	(%rsp), %rsi
	xorl	%edx, %edx
	movq	%rbp, %rdi
	call	general_neobject_destroy@PLT
	movq	global_env(%rip), %rax
	movq	80(%rax), %rdi
	call	puts@PLT
	movq	global_env(%rip), %rax
	movl	$10, (%rax)
	jmp	.L571
	.p2align 4,,10
	.p2align 3
.L596:
	movl	12(%rsi), %eax
	movl	12(%rdx,%rax), %esi
	call	eval_aux
	movq	%rax, %r13
	movq	global_env(%rip), %rax
	movq	%rdx, %rbp
	movl	(%rax), %r8d
	testl	%r8d, %r8d
	jne	.L571
	cmpb	$4, %dl
	je	.L568
	xorl	%edx, %edx
	movq	%r13, %rdi
	movq	%rbp, %rsi
	call	general_neobject_destroy@PLT
	movq	global_env(%rip), %rax
	movl	$108, (%rax)
	jmp	.L571
	.p2align 4,,10
	.p2align 3
.L597:
	movl	12(%rsi), %eax
	movl	4(%rdx,%rax), %esi
	call	eval_aux
	movq	%rax, %r14
	movq	global_env(%rip), %rax
	movq	%rdx, %r13
	movl	(%rax), %esi
	testl	%esi, %esi
	jne	.L571
	movq	16(%r12), %rax
	movq	%r12, %rdi
	movl	12(%rax,%rbx), %edx
	movl	8(%rax,%rdx), %esi
	call	eval_aux
	movq	%rax, %rbp
	movq	%rdx, %rcx
	jmp	.L575
	.p2align 4,,10
	.p2align 3
.L568:
	movq	%r13, %rdi
	movq	%rdx, %rsi
	call	neo_to_integer@PLT
	movq	%r13, %rdi
	xorl	%edx, %edx
	movq	%rbp, %rsi
	movq	%rax, %r15
	call	general_neobject_destroy@PLT
	movq	16(%r12), %rax
	movq	%r12, %rdi
	movl	12(%rax,%rbx), %edx
	movl	4(%rax,%rdx), %esi
	call	eval_aux
	movq	%rax, %r14
	movq	global_env(%rip), %rax
	movq	%rdx, %r13
	movl	(%rax), %edi
	testl	%edi, %edi
	jne	.L571
	movq	16(%r12), %rax
	movq	%r12, %rdi
	movl	%r15d, (%rsp)
	movl	12(%rax,%rbx), %edx
	movl	8(%rax,%rdx), %esi
	call	eval_aux
	movq	%rax, %rbp
	movq	%rdx, %rcx
	jmp	.L570
	.p2align 4,,10
	.p2align 3
.L599:
	xorl	%edx, %edx
	movq	%r14, %rdi
	movq	%r13, %rsi
	call	general_neobject_destroy@PLT
	jmp	.L571
	.p2align 4,,10
	.p2align 3
.L580:
	movq	%r14, %rdi
	movq	%r13, %rsi
	movq	%rcx, 8(%rsp)
	call	neo_to_integer@PLT
	xorl	%edx, %edx
	movq	%r14, %rdi
	movq	%r13, %rsi
	movq	%rax, 16(%rsp)
	call	general_neobject_destroy@PLT
	movq	16(%r12), %rax
	movq	8(%rsp), %rcx
	movl	12(%rax,%rbx), %edx
	movl	(%rax,%rdx), %edx
	addq	%rdx, %rax
	movq	global_env(%rip), %rdx
	cmpl	$12, (%rax)
	je	.L581
	movl	$111, (%rdx)
	movq	%rbp, %rdi
	xorl	%edx, %edx
	movq	%rcx, %rsi
	call	general_neobject_destroy@PLT
	jmp	.L571
	.p2align 4,,10
	.p2align 3
.L581:
	movl	8(%rax), %eax
	movl	16(%rsp), %r13d
	movq	%rcx, 40(%rsp)
	movl	%eax, 8(%rsp)
	movq	200(%rdx), %rax
	movq	(%rax), %rax
	movq	80(%rax), %r14
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%r14, %rdi
	call	ptrlist_append@PLT
	movq	global_env(%rip), %rax
	movl	8(%rsp), %esi
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	movq	(%rdi), %rcx
	movq	%rcx, 24(%rsp)
	call	save_later@PLT
	movq	global_env(%rip), %rax
	movl	$24, %edi
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	80(%rax), %rsi
	movq	%rsi, 32(%rsp)
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
	movq	32(%rsp), %rsi
	movq	(%rsi), %rdi
	movq	%r14, %rsi
	call	ptrlist_append@PLT
	movq	40(%rsp), %rsi
	movq	%rbp, %rdi
	call	neo_to_integer@PLT
	movq	40(%rsp), %rsi
	xorl	%edx, %edx
	movq	%rbp, %rdi
	movq	%rax, 32(%rsp)
	movl	%eax, %r14d
	call	general_neobject_destroy@PLT
	movq	32(%rsp), %rax
	cmpl	%eax, 16(%rsp)
	jl	.L585
	jmp	.L582
	.p2align 4,,10
	.p2align 3
.L583:
	movslq	%r15d, %rdx
	cmpq	$1, %rdx
	ja	.L584
	movl	(%rsp), %ecx
	addl	%ecx, %r13d
	cmpl	%r13d, %r14d
	jle	.L594
.L585:
	movslq	%r13d, %rdi
	call	neo_integer_create@PLT
	movl	8(%rsp), %edi
	movq	%rax, %rsi
	call	replace_var@PLT
	movq	16(%r12), %rax
	movq	%r12, %rdi
	movl	8(%rax,%rbx), %esi
	call	exec_aux
	movl	%eax, %r15d
	movq	global_env(%rip), %rax
	movl	(%rax), %edx
	testl	%edx, %edx
	je	.L583
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	80(%rax), %rdi
	call	deleteContext
	movq	global_env(%rip), %rax
	movq	24(%rsp), %rsi
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	call	partialRestore@PLT
	jmp	.L571
.L582:
	movq	global_env(%rip), %rax
.L594:
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	80(%rax), %rdi
	call	deleteContext
	movq	global_env(%rip), %rax
	movq	24(%rsp), %rsi
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	call	partialRestore@PLT
	jmp	.L593
.L584:
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	80(%rax), %rdi
	call	deleteContext
	movq	global_env(%rip), %rax
	movq	24(%rsp), %rsi
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	call	partialRestore@PLT
	cmpl	$2, %r15d
	je	.L571
	jmp	.L593
	.cfi_endproc
.LFE38:
	.size	execStatementFor, .-execStatementFor
	.p2align 4
	.globl	execStatementForeachList
	.type	execStatementForeachList, @function
execStatementForeachList:
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
	movl	%esi, %r12d
	movq	%rcx, %rsi
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	movq	%rdi, %rbx
	movq	%rdx, %rdi
	subq	$24, %rsp
	.cfi_def_cfa_offset 80
	call	neo_to_list@PLT
	movq	%rax, %rbp
	movq	16(%rbx), %rax
	movl	12(%rax,%r12), %edx
	movl	(%rax,%rdx), %edx
	addq	%rdx, %rax
	movq	global_env(%rip), %rdx
	cmpl	$12, (%rax)
	je	.L601
	movl	$111, (%rdx)
.L602:
	xorl	%eax, %eax
.L600:
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
.L601:
	.cfi_restore_state
	movl	8(%rax), %r13d
	movq	200(%rdx), %rax
	movq	(%rax), %rax
	movq	80(%rax), %r14
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%r14, %rdi
	call	ptrlist_append@PLT
	movq	global_env(%rip), %rax
	movl	%r13d, %esi
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	movq	(%rdi), %rax
	movq	%rax, (%rsp)
	call	save_later@PLT
	movq	global_env(%rip), %rax
	movl	$24, %edi
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	80(%rax), %r14
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
	movl	16(%rbp), %esi
	testl	%esi, %esi
	jle	.L603
	xorl	%r15d, %r15d
	jmp	.L607
	.p2align 4,,10
	.p2align 3
.L604:
	movslq	%eax, %rcx
	cmpq	$1, %rcx
	ja	.L606
	movslq	16(%rbp), %rcx
	addq	$1, %r15
	cmpq	%r15, %rcx
	jle	.L618
.L607:
	movl	%r15d, %esi
	movq	%rbp, %rdi
	call	nelist_nth@PLT
	movq	%rax, %rdi
	movq	%rdx, %rsi
	call	neo_copy@PLT
	movl	%r13d, %edi
	movq	%rax, %rsi
	call	replace_var@PLT
	movq	16(%rbx), %rax
	movq	%rbx, %rdi
	movl	8(%rax,%r12), %esi
	call	exec_aux
	movq	global_env(%rip), %rdx
	movl	(%rdx), %ecx
	testl	%ecx, %ecx
	je	.L604
	movq	200(%rdx), %rax
.L617:
	movq	(%rax), %rax
	movq	80(%rax), %rdi
	call	deleteContext
	movq	global_env(%rip), %rax
	movq	(%rsp), %rsi
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	call	partialRestore@PLT
	jmp	.L602
	.p2align 4,,10
	.p2align 3
.L606:
	movq	200(%rdx), %rdx
	movl	%eax, 12(%rsp)
	movq	(%rdx), %rdx
	movq	80(%rdx), %rdi
	call	deleteContext
	movq	global_env(%rip), %rdx
	movq	(%rsp), %rsi
	movq	200(%rdx), %rdx
	movq	(%rdx), %rdx
	movq	96(%rdx), %rdi
	call	partialRestore@PLT
	movl	12(%rsp), %eax
	cmpl	$2, %eax
	je	.L602
	jmp	.L600
	.p2align 4,,10
	.p2align 3
.L618:
	movq	200(%rdx), %rdx
	movl	%eax, 12(%rsp)
	movq	(%rdx), %rdx
	movq	80(%rdx), %rdi
	call	deleteContext
	movq	global_env(%rip), %rdx
	movq	(%rsp), %rsi
	movq	200(%rdx), %rdx
	movq	(%rdx), %rdx
	movq	96(%rdx), %rdi
	call	partialRestore@PLT
	movl	12(%rsp), %eax
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
.L603:
	.cfi_restore_state
	movq	global_env(%rip), %rax
	movq	200(%rax), %rax
	jmp	.L617
	.cfi_endproc
.LFE39:
	.size	execStatementForeachList, .-execStatementForeachList
	.p2align 4
	.globl	execStatementForeachString
	.type	execStatementForeachString, @function
execStatementForeachString:
.LFB40:
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
	movl	%esi, %r12d
	movq	%rcx, %rsi
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	movq	%rdi, %rbp
	movq	%rdx, %rdi
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	subq	$24, %rsp
	.cfi_def_cfa_offset 80
	call	neo_to_string@PLT
	movq	global_env(%rip), %r15
	movq	%rax, %rbx
	movq	16(%rbp), %rax
	movl	12(%rax,%r12), %edx
	movl	(%rax,%rdx), %edx
	leaq	(%rax,%rdx), %r14
	cmpl	$12, (%r14)
	je	.L620
	movl	$111, (%r15)
.L621:
	xorl	%eax, %eax
.L619:
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
.L620:
	.cfi_restore_state
	movq	%rbx, %rdi
	call	strlen@PLT
	movl	8(%r14), %r14d
	movq	%rax, %r13
	movq	200(%r15), %rax
	movq	(%rax), %rax
	movq	80(%rax), %r15
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%r15, %rdi
	call	ptrlist_append@PLT
	movq	global_env(%rip), %rax
	movl	%r14d, %esi
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	movq	(%rdi), %rax
	movq	%rax, 8(%rsp)
	call	save_later@PLT
	movq	global_env(%rip), %rax
	movl	$24, %edi
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	80(%rax), %rcx
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
	testl	%r13d, %r13d
	jle	.L622
	movslq	%r13d, %r13
	addq	%rbx, %r13
	jmp	.L626
	.p2align 4,,10
	.p2align 3
.L623:
	movslq	%eax, %rcx
	cmpq	$1, %rcx
	ja	.L625
	addq	$1, %rbx
	cmpq	%r13, %rbx
	je	.L637
.L626:
	movsbl	(%rbx), %edi
	call	charToString@PLT
	movq	%rax, %rdi
	call	neo_str_create@PLT
	movl	%r14d, %edi
	movq	%rax, %rsi
	call	replace_var@PLT
	movq	16(%rbp), %rax
	movq	%rbp, %rdi
	movl	8(%rax,%r12), %esi
	call	exec_aux
	movq	global_env(%rip), %rdx
	movl	(%rdx), %ecx
	testl	%ecx, %ecx
	je	.L623
	movq	200(%rdx), %rax
.L636:
	movq	(%rax), %rax
	movq	80(%rax), %rdi
	call	deleteContext
	movq	global_env(%rip), %rax
	movq	8(%rsp), %rsi
	movq	200(%rax), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	call	partialRestore@PLT
	jmp	.L621
	.p2align 4,,10
	.p2align 3
.L625:
	movq	200(%rdx), %rdx
	movl	%eax, (%rsp)
	movq	(%rdx), %rdx
	movq	80(%rdx), %rdi
	call	deleteContext
	movq	global_env(%rip), %rdx
	movq	8(%rsp), %rsi
	movq	200(%rdx), %rdx
	movq	(%rdx), %rdx
	movq	96(%rdx), %rdi
	call	partialRestore@PLT
	movl	(%rsp), %eax
	cmpl	$2, %eax
	je	.L621
	jmp	.L619
	.p2align 4,,10
	.p2align 3
.L637:
	movq	200(%rdx), %rdx
	movl	%eax, (%rsp)
	movq	(%rdx), %rdx
	movq	80(%rdx), %rdi
	call	deleteContext
	movq	global_env(%rip), %rdx
	movq	8(%rsp), %rsi
	movq	200(%rdx), %rdx
	movq	(%rdx), %rdx
	movq	96(%rdx), %rdi
	call	partialRestore@PLT
	movl	(%rsp), %eax
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
.L622:
	.cfi_restore_state
	movq	global_env(%rip), %rax
	movq	200(%rax), %rax
	jmp	.L636
	.cfi_endproc
.LFE40:
	.size	execStatementForeachString, .-execStatementForeachString
	.p2align 4
	.globl	initRuntime
	.type	initRuntime, @function
initRuntime:
.LFB42:
	.cfi_startproc
	movl	$1, %edx
	movl	$-1, %esi
	xorl	%edi, %edi
	jmp	create_new_process@PLT
	.cfi_endproc
.LFE42:
	.size	initRuntime, .-initRuntime
	.p2align 4
	.globl	exec
	.type	exec, @function
exec:
.LFB44:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movl	$1, %edx
	movl	%esi, %ebp
	movl	$-1, %esi
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	movq	%rdi, %rbx
	xorl	%edi, %edi
	subq	$8, %rsp
	.cfi_def_cfa_offset 32
	call	create_new_process@PLT
	movl	%ebp, %esi
	movq	%rbx, %rdi
	call	exec_aux
	addq	$8, %rsp
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	jmp	exitRuntime
	.cfi_endproc
.LFE44:
	.size	exec, .-exec
	.p2align 4
	.globl	eval
	.type	eval, @function
eval:
.LFB45:
	.cfi_startproc
	pushq	%r13
	.cfi_def_cfa_offset 16
	.cfi_offset 13, -16
	movl	$1, %edx
	movl	%esi, %r13d
	movl	$-1, %esi
	pushq	%r12
	.cfi_def_cfa_offset 24
	.cfi_offset 12, -24
	movq	%rdi, %r12
	xorl	%edi, %edi
	subq	$8, %rsp
	.cfi_def_cfa_offset 32
	call	create_new_process@PLT
	movl	%r13d, %esi
	movq	%r12, %rdi
	call	eval_aux
	movq	%rax, %r12
	movq	%rdx, %r13
	call	exitRuntime
	addq	$8, %rsp
	.cfi_def_cfa_offset 24
	movq	%r12, %rax
	movq	%r13, %rdx
	popq	%r12
	.cfi_def_cfa_offset 16
	popq	%r13
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE45:
	.size	eval, .-eval
	.ident	"GCC: (Debian 14.2.0-19) 14.2.0"
	.section	.note.GNU-stack,"",@progbits
