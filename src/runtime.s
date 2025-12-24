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
	movq	168(%rax), %rax
	movq	8(%rax), %rdx
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
	cmpq	%rax, %rdx
	je	.L3
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	unloadCurrentProcess@PLT
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
	movq	global_env(%rip), %rbx
	movq	%rax, %rdi
	call	loadNextLivingProcess@PLT
	movq	%rax, 168(%rbx)
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
	movq	16(%rax), %rax
	movq	(%rax), %rdx
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
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
	movq	168(%rax), %rax
	movq	global_env(%rip), %rbx
	movq	%rax, %rdi
	call	loadNextLivingProcess@PLT
	movq	%rax, 168(%rbx)
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
	movq	16(%rax), %rax
	movq	(%rax), %rdx
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	switch_registers@PLT
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
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
	movq	168(%rax), %rax
	movq	%rax, %rdi
	call	ProcessCycle_clean@PLT
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movb	$1, 56(%rax)
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	global_env(%rip), %rdx
	movq	168(%rdx), %rdx
	movq	(%rdx), %rdx
	movq	64(%rdx), %rdx
	movq	16(%rdx), %rcx
	movq	global_env(%rip), %rdx
	movq	168(%rdx), %rdx
	movq	(%rdx), %rdx
	movl	72(%rdx), %edx
	movl	%edx, %edx
	leaq	(%rcx,%rdx), %rbx
	movq	80(%rax), %rdx
	movq	88(%rax), %rax
	movq	%rdx, %rdi
	movq	%rax, %rsi
	call	neo_copy@PLT
	movq	%rax, 16(%rbx)
	movq	%rdx, 24(%rbx)
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movl	72(%rax), %edx
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	64(%rax), %rax
	movl	%edx, %esi
	movq	%rax, %rdi
	call	eval_aux
	movq	%rax, -32(%rbp)
	movq	%rdx, -24(%rbp)
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	process_preRemove@PLT
	movq	global_env(%rip), %rax
	movq	24(%rax), %rdx
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movl	112(%rax), %eax
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
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movl	112(%rax), %eax
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
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movl	112(%rax), %eax
	cltq
	salq	$2, %rax
	addq	%rdx, %rax
	movl	$1, (%rax)
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
	movq	%rax, %rdi
	call	ProcessCycle_isActive@PLT
	testb	%al, %al
	je	.L9
	call	neon_interp_next_process
.L9:
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	reset_stack_and_registers@PLT
	jmp	.L10
.L11:
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
	movq	global_env(%rip), %rbx
	movq	%rax, %rdi
	call	ProcessCycle_remove@PLT
	movq	%rax, 168(%rbx)
.L10:
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
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
	.section	.rodata
.LC0:
	.string	"Retour au code"
	.text
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
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movl	112(%rax), %eax
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
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	process_preRemove@PLT
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
	movq	%rax, %rdi
	call	ProcessCycle_isActive@PLT
	testb	%al, %al
	je	.L15
	leaq	.LC0(%rip), %rax
	movq	%rax, %rdi
	call	puts@PLT
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	save_stack_and_registers@PLT
	call	neon_interp_next_process
	jmp	.L15
.L16:
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
	movq	global_env(%rip), %rbx
	movq	%rax, %rdi
	call	ProcessCycle_remove@PLT
	movq	%rax, 168(%rbx)
.L15:
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
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
.LC1:
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
	movq	144(%rax), %rdi
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
	leaq	.LC1(%rip), %rsi
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
	movq	168(%rcx), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rsi
	cmpq	$0, (%rsi)
	je	.L187
	xorl	%r15d, %r15d
	jmp	.L88
	.p2align 4,,10
	.p2align 3
.L188:
	movq	168(%rcx), %rax
	movq	(%rax), %rax
.L88:
	leaq	0(,%r15,4), %rcx
	movl	12(%r12), %esi
	addq	$1, %r15
	leaq	(%rdx,%rcx), %rdi
	movq	%rcx, 8(%rsp)
	movl	(%rdi,%rsi), %esi
	movq	120(%rax), %rdi
	movl	8(%rdx,%rsi), %esi
	call	save_later@PLT
	movq	8(%rsp), %rcx
	movl	$24, %edi
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	104(%rax), %r8
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
.LC2:
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
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	104(%rax), %r13
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%r13, %rdi
	call	ptrlist_append@PLT
	movq	global_env(%rip), %rax
	cmpb	$0, 20(%rbp)
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	120(%rax), %rax
	movq	(%rax), %rax
	movq	%rax, 24(%rsp)
	jne	.L193
	movl	16(%rbp), %eax
.L194:
	xorl	%ebx, %ebx
	testl	%eax, %eax
	jg	.L197
	jmp	.L204
	.p2align 4,,10
	.p2align 3
.L200:
	addq	$1, %rbx
	cmpl	%ebx, 16(%rbp)
	jle	.L204
.L197:
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
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	call	save_later@PLT
	movq	global_env(%rip), %rax
	movl	$24, %edi
	movq	168(%rax), %rax
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
	movq	global_env(%rip), %rax
	movl	(%rax), %edx
	testl	%edx, %edx
	je	.L200
	cmpl	$1, %ebx
	jle	.L201
	movl	$4, %ebx
	.p2align 4
	.p2align 3
.L202:
	movq	8(%rbp), %rax
	movl	(%rax,%rbx), %edi
	addq	$4, %rbx
	call	free_var@PLT
	cmpq	%rbx, 8(%rsp)
	jne	.L202
	movq	global_env(%rip), %rax
.L201:
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rdi
	call	deleteContext
	movq	global_env(%rip), %rax
	movq	24(%rsp), %rsi
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	120(%rax), %rdi
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
	.p2align 4,,10
	.p2align 3
.L204:
	.cfi_restore_state
	movl	40(%rbp), %esi
	movq	32(%rbp), %rdi
	call	exec_aux
	movl	%eax, %ebx
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rdi
	call	deleteContext
	movq	global_env(%rip), %rax
	movq	24(%rsp), %rsi
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	call	partialRestore@PLT
	movq	global_env(%rip), %rcx
	movl	(%rcx), %esi
	testl	%esi, %esi
	jne	.L212
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
	leaq	.LC2(%rip), %rdi
	movl	%eax, %r15d
	call	get_var@PLT
	movl	%eax, %r13d
	movq	global_env(%rip), %rax
	movl	%r13d, %esi
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	call	save_later@PLT
	movq	global_env(%rip), %rax
	movl	$24, %edi
	movq	168(%rax), %rax
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
	jle	.L194
	movq	16(%rsp), %rcx
	movl	%eax, 16(%rcx)
	jmp	.L194
	.p2align 4,,10
	.p2align 3
.L212:
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
	movq	168(%rax), %rax
	movq	%rdx, 24(%rsp)
	movq	(%rax), %rax
	movq	104(%rax), %r12
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%r12, %rdi
	call	ptrlist_append@PLT
	movq	global_env(%rip), %rax
	cmpb	$0, 20(%rbp)
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	120(%rax), %rax
	movq	(%rax), %rax
	movq	%rax, 32(%rsp)
	jne	.L214
	movl	16(%rbp), %eax
.L215:
	xorl	%ebx, %ebx
	testl	%eax, %eax
	jg	.L218
	jmp	.L225
	.p2align 4,,10
	.p2align 3
.L221:
	addq	$1, %rbx
	cmpl	%ebx, 16(%rbp)
	jle	.L225
.L218:
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
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	call	save_later@PLT
	movq	global_env(%rip), %rax
	movl	$24, %edi
	movq	168(%rax), %rax
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
	movq	global_env(%rip), %rax
	movl	(%rax), %edx
	testl	%edx, %edx
	je	.L221
	cmpl	$1, %ebx
	jle	.L219
	movl	$4, %ebx
	.p2align 4
	.p2align 3
.L223:
	movq	8(%rbp), %rax
	movl	(%rax,%rbx), %edi
	addq	$4, %rbx
	call	free_var@PLT
	cmpq	%rbx, 16(%rsp)
	jne	.L223
	movq	global_env(%rip), %rax
.L219:
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rdi
	call	deleteContext
	movq	global_env(%rip), %rax
	movq	32(%rsp), %rsi
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	120(%rax), %rdi
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
	.p2align 4,,10
	.p2align 3
.L225:
	.cfi_restore_state
	movl	40(%rbp), %esi
	movq	32(%rbp), %rdi
	call	exec_aux
	movl	%eax, %ebx
	movq	global_env(%rip), %rax
	movl	(%rax), %ecx
	testl	%ecx, %ecx
	jne	.L219
	movq	8(%rbp), %rax
	movl	(%rax), %edi
	call	get_var_value@PLT
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	neo_copy@PLT
	movq	%rax, %r12
	movq	global_env(%rip), %rax
	movq	%rdx, %r13
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rdi
	call	deleteContext
	movq	global_env(%rip), %rax
	movq	32(%rsp), %rsi
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	call	partialRestore@PLT
	movq	40(%rsp), %r15
	xorl	%edx, %edx
	movq	(%r15), %rdi
	movq	8(%r15), %rsi
	call	general_neobject_destroy@PLT
	movq	%r12, (%r15)
	movq	%r13, 8(%r15)
	cmpl	$4, %ebx
	jne	.L226
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
.L214:
	.cfi_restore_state
	movq	%r13, %rsi
	movq	%rbx, %rdi
	call	neo_list_len@PLT
	leaq	.LC2(%rip), %rdi
	movl	%eax, %r15d
	call	get_var@PLT
	movl	%eax, %r12d
	movq	global_env(%rip), %rax
	movl	%r12d, %esi
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	call	save_later@PLT
	movq	global_env(%rip), %rax
	movl	$24, %edi
	movq	168(%rax), %rax
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
	jle	.L215
	movq	24(%rsp), %rcx
	movl	%eax, 16(%rcx)
	jmp	.L215
	.p2align 4,,10
	.p2align 3
.L226:
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
	movl	(%rax), %ecx
	testl	%ecx, %ecx
	jne	.L441
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
	cmpl	$25, (%rsi)
	movl	%edx, 88(%rax)
	ja	.L236
	movl	(%rsi), %edx
	leaq	.L238(%rip), %rdi
	movslq	(%rdi,%rdx,4), %rdx
	addq	%rdi, %rdx
	jmp	*%rdx
	.section	.rodata
	.align 4
	.align 4
.L238:
	.long	.L236-.L238
	.long	.L247-.L238
	.long	.L246-.L238
	.long	.L245-.L238
	.long	.L236-.L238
	.long	.L236-.L238
	.long	.L236-.L238
	.long	.L236-.L238
	.long	.L236-.L238
	.long	.L236-.L238
	.long	.L236-.L238
	.long	.L236-.L238
	.long	.L244-.L238
	.long	.L243-.L238
	.long	.L242-.L238
	.long	.L241-.L238
	.long	.L236-.L238
	.long	.L236-.L238
	.long	.L240-.L238
	.long	.L236-.L238
	.long	.L236-.L238
	.long	.L236-.L238
	.long	.L239-.L238
	.long	.L236-.L238
	.long	.L236-.L238
	.long	.L237-.L238
	.text
	.p2align 4,,10
	.p2align 3
.L236:
	movl	$19, (%rax)
.L436:
	xorl	%eax, %eax
	movl	$54, %edx
.L411:
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
.L237:
	.cfi_restore_state
	movl	8(%rsi), %edx
	addq	%rdx, %rcx
	movq	%rdx, %rbx
	cmpl	$14, (%rcx)
	je	.L274
	movl	$100, (%rax)
	movl	$54, %edx
	xorl	%eax, %eax
	jmp	.L411
	.p2align 4,,10
	.p2align 3
.L239:
	movzwl	16(%rsi), %edi
	call	nelist_create@PLT
	movq	%rax, %r13
	movq	16(%rbp), %rax
	leaq	(%rax,%rbx), %rdx
	cmpw	$0, 16(%rdx)
	je	.L357
	xorl	%r12d, %r12d
.L360:
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
	jne	.L442
	movq	16(%rbp), %rax
	addq	$1, %r12
	leaq	(%rax,%rbx), %rdx
	movzwl	16(%rdx), %ecx
	cmpl	%r12d, %ecx
	jg	.L360
.L357:
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
.L240:
	.cfi_restore_state
	addq	$8, %rsi
	movq	%rbp, %rdi
	call	treeToList
	movq	global_env(%rip), %rdx
	movl	(%rdx), %esi
	testl	%esi, %esi
	jne	.L436
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
.L241:
	.cfi_restore_state
	movl	8(%rsi), %esi
	movq	%rbp, %rdi
	call	eval_aux
	movq	%rax, %r15
	movq	global_env(%rip), %rax
	movq	%rdx, %r14
	movl	(%rax), %edx
	testl	%edx, %edx
	jne	.L436
	cmpb	$-77, %r14b
	je	.L363
	xorl	%edx, %edx
	movq	%r15, %rdi
	movq	%r14, %rsi
	call	general_neobject_destroy@PLT
	movq	global_env(%rip), %rax
	movl	$54, %edx
	movl	$80, (%rax)
	xorl	%eax, %eax
	jmp	.L411
	.p2align 4,,10
	.p2align 3
.L242:
	movq	16(%rsi), %rdi
	movq	24(%rsi), %rsi
	call	neo_is_void@PLT
	testb	%al, %al
	jne	.L443
.L279:
	movq	16(%rbp), %rax
	addq	%rbx, %rax
	movzbl	24(%rax), %edx
	cmpb	$-128, %dl
	je	.L444
	leal	79(%rdx), %ecx
	cmpb	$1, %cl
	jbe	.L445
	movq	global_env(%rip), %rcx
	cmpb	$54, %dl
	je	.L446
	movl	$9, (%rcx)
.L432:
	movq	16(%rax), %rdi
	movq	24(%rax), %rsi
.L439:
	xorl	%edx, %edx
.L435:
	call	general_neobject_destroy@PLT
	addq	16(%rbp), %rbx
	pxor	%xmm0, %xmm0
	movups	%xmm0, 16(%rbx)
	movb	$54, 24(%rbx)
	jmp	.L436
	.p2align 4,,10
	.p2align 3
.L243:
	movl	8(%rsi), %esi
	movq	%rbp, %rdi
	call	eval_aux
	movq	%rax, %r14
	movq	global_env(%rip), %rax
	movq	%rdx, %r13
	movl	(%rax), %edi
	testl	%edi, %edi
	jne	.L436
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
	jne	.L436
	cmpb	$-122, %r13b
	je	.L351
	cmpb	$-125, %r13b
	je	.L351
	movl	$15, (%rax)
	movl	$54, %edx
	xorl	%eax, %eax
	jmp	.L411
	.p2align 4,,10
	.p2align 3
.L244:
	movl	8(%rsi), %edi
	call	get_var_value@PLT
	movq	%rax, %rdi
	movq	%rdx, %rsi
	cmpb	$54, %dl
	je	.L447
.L438:
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
.L245:
	.cfi_restore_state
	movq	8(%rsi), %rdi
	movq	16(%rsi), %rsi
	jmp	.L438
	.p2align 4,,10
	.p2align 3
.L246:
	movl	8(%rsi), %edi
	movl	$256, %esi
	call	operatorIs@PLT
	movl	%eax, %edx
	movq	16(%rbp), %rax
	addq	%rbx, %rax
	movslq	8(%rax), %rdi
	testb	%dl, %dl
	je	.L248
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
.L247:
	.cfi_restore_state
	movl	8(%rsi), %edi
	movl	$256, %esi
	call	operatorIs@PLT
	movl	%eax, %edx
	movq	16(%rbp), %rax
	addq	%rbx, %rax
	testb	%dl, %dl
	jne	.L448
	movl	8(%rax), %edi
	cmpl	$37, %edi
	je	.L449
	cmpl	$35, %edi
	je	.L450
	movl	$8, %esi
	call	operatorIs@PLT
	movl	%eax, %edx
	movq	16(%rbp), %rax
	addq	%rbx, %rax
	testb	%dl, %dl
	je	.L259
	movl	12(%rax), %esi
	movq	%rbp, %rdi
	call	get_address
	movq	%rax, %r12
	movq	global_env(%rip), %rax
	movl	(%rax), %r14d
	testl	%r14d, %r14d
	jne	.L436
	movq	16(%rbp), %rax
	movq	%rbp, %rdi
	movl	16(%rax,%rbx), %esi
	call	eval_aux
	movq	%rax, %r15
	movq	global_env(%rip), %rax
	movq	%rdx, %r14
	movl	(%rax), %r13d
	testl	%r13d, %r13d
	jne	.L436
	movq	16(%rbp), %rax
	movq	%r15, %rsi
	movq	%r12, %rdi
	movslq	8(%rax,%rbx), %rcx
	leaq	operators_functions(%rip), %rax
	call	*(%rax,%rcx,8)
	.p2align 4
	.p2align 3
.L425:
	movq	%rdx, %r12
	movq	%r15, %rdi
	xorl	%edx, %edx
	movq	%r14, %rsi
	movq	%rax, %r13
	call	general_neobject_destroy@PLT
.L428:
	movq	%r13, %rax
	movq	%r12, %rdx
	jmp	.L411
	.p2align 4,,10
	.p2align 3
.L441:
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
.L248:
	.cfi_def_cfa_offset 128
	.cfi_offset 3, -56
	.cfi_offset 6, -48
	.cfi_offset 12, -40
	.cfi_offset 13, -32
	.cfi_offset 14, -24
	.cfi_offset 15, -16
	movl	$32, %esi
	call	operatorIs@PLT
	testb	%al, %al
	jne	.L252
	movq	16(%rbp), %rax
	movl	$1, %esi
	movl	8(%rax,%rbx), %edi
	call	operatorIs@PLT
	testb	%al, %al
	je	.L451
.L252:
	movq	16(%rbp), %rax
	movq	%rbp, %rdi
	movl	12(%rax,%rbx), %esi
	call	get_address
	movq	global_env(%rip), %rdx
	movl	(%rdx), %edx
	testl	%edx, %edx
	jne	.L436
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
.L447:
	.cfi_restore_state
	movq	global_env(%rip), %rax
	movl	$5, (%rax)
	jmp	.L436
	.p2align 4,,10
	.p2align 3
.L448:
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
.L274:
	.cfi_restore_state
	movl	8(%rcx), %esi
	movq	%rbp, %rdi
	call	eval_aux
	movq	%rax, %r13
	movq	global_env(%rip), %rax
	movq	%rdx, %r12
	movl	(%rax), %ecx
	testl	%ecx, %ecx
	jne	.L436
	cmpb	$-78, %dl
	je	.L277
	movl	$100, (%rax)
.L426:
	xorl	%edx, %edx
	movq	%r13, %rdi
	movq	%r12, %rsi
	call	general_neobject_destroy@PLT
	jmp	.L436
	.p2align 4,,10
	.p2align 3
.L443:
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
	je	.L279
	jmp	.L436
	.p2align 4,,10
	.p2align 3
.L444:
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
	jne	.L452
	movq	%r14, %rsi
	movq	%r12, %rdi
	call	funcArgsCheck@PLT
	testb	%al, %al
	je	.L453
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
	pxor	%xmm4, %xmm4
	movups	%xmm4, 16(%rbx)
	movb	$54, 24(%rbx)
	movl	(%rax), %r14d
	testl	%r14d, %r14d
	jne	.L437
	movzwl	4(%rbx), %edx
	movl	%edx, 88(%rax)
	movq	%r13, %rax
	movq	%r12, %rdx
	jmp	.L411
	.p2align 4,,10
	.p2align 3
.L442:
	leal	-1(%r12), %esi
	movq	%r13, %rdi
	call	nelist_destroy_until@PLT
	xorl	%eax, %eax
	movl	$54, %edx
	jmp	.L411
	.p2align 4,,10
	.p2align 3
.L363:
	movq	%r14, %rsi
	movq	%r15, %rdi
	call	neo_to_container@PLT
	movq	16(%rbp), %r13
	movq	%rax, %r12
	addq	%rbx, %r13
	movl	12(%r13), %esi
	cmpl	$-1, %esi
	je	.L364
	movl	4(%rax), %eax
	cmpl	%eax, 16(%r13)
	je	.L365
.L364:
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
.L365:
	movq	%r12, %rdi
	call	get_container_field@PLT
	movq	%rax, %rdi
	movq	%rdx, %rsi
	call	neo_copy@PLT
	jmp	.L425
	.p2align 4,,10
	.p2align 3
.L351:
	cmpb	$4, %bl
	je	.L352
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
	jmp	.L411
	.p2align 4,,10
	.p2align 3
.L450:
	movq	global_env(%rip), %rax
	movl	$54, %edx
	movl	$92, (%rax)
	xorl	%eax, %eax
	jmp	.L411
	.p2align 4,,10
	.p2align 3
.L446:
	movl	$8, (%rcx)
	jmp	.L432
	.p2align 4,,10
	.p2align 3
.L445:
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
	jle	.L286
	cmpb	$0, 20(%r9)
	je	.L454
	testl	%r8d, %r8d
	je	.L455
.L295:
	movl	32(%rax), %eax
	leal	-1(%r8), %esi
	movl	$0, 48(%rsp)
	leaq	(%rcx,%rax), %rdx
	leaq	4(%rcx,%rax), %rax
	leaq	(%rax,%rsi,4), %rsi
	jmp	.L293
	.p2align 5
	.p2align 4,,10
	.p2align 3
.L289:
	addq	$4, %rdx
	cmpq	%rdx, %rsi
	je	.L294
.L293:
	movl	(%rdx), %eax
	addq	%rcx, %rax
	cmpl	$1, (%rax)
	jne	.L289
	cmpl	$37, 8(%rax)
	jne	.L289
	testl	%edi, %edi
	jle	.L369
	movl	12(%rax), %eax
	movq	8(%r9), %r10
	movl	8(%rcx,%rax), %r11d
	xorl	%eax, %eax
	jmp	.L291
	.p2align 4
	.p2align 4,,10
	.p2align 3
.L292:
	addl	$1, %eax
	addq	$4, %r10
	cmpl	%eax, %edi
	je	.L290
.L291:
	cmpl	%r11d, (%r10)
	jne	.L292
.L290:
	movl	%edi, %r10d
	subl	24(%r9), %r10d
	cmpl	%eax, %r10d
	setle	%r10b
	cmpl	%eax, %edi
	setg	%al
	movzbl	%al, %eax
	andl	%r10d, %eax
	addl	%eax, 48(%rsp)
	jmp	.L289
	.p2align 4,,10
	.p2align 3
.L259:
	movl	8(%rax), %edi
	movl	$16, %esi
	call	operatorIs@PLT
	movl	%eax, %edx
	movq	16(%rbp), %rax
	addq	%rbx, %rax
	testb	%dl, %dl
	je	.L263
	movl	12(%rax), %esi
	movq	%rbp, %rdi
	call	eval_aux
	movq	%rax, %r15
	movq	global_env(%rip), %rax
	movq	%rdx, %r14
	movl	(%rax), %r12d
	testl	%r12d, %r12d
	jne	.L436
	movq	16(%rbp), %rax
	movq	%rbp, %rdi
	movl	16(%rax,%rbx), %esi
	call	get_address
	movq	global_env(%rip), %rdx
	movl	(%rdx), %r11d
	testl	%r11d, %r11d
	jne	.L436
	movq	16(%rbp), %rdx
	movq	%r15, %rdi
	movq	%r14, %rsi
	movslq	8(%rdx,%rbx), %rcx
	movq	%rax, %rdx
	leaq	operators_functions(%rip), %rax
	call	*(%rax,%rcx,8)
	jmp	.L425
.L286:
	testl	%r8d, %r8d
	jne	.L295
	xorl	%r12d, %r12d
	movl	%r12d, 48(%rsp)
.L294:
	cmpb	$0, 20(%r9)
	jne	.L368
.L296:
	movq	%r9, (%rsp)
	call	nelist_create@PLT
	movq	(%rsp), %r9
	movslq	16(%rax), %rdx
	movq	%rax, %r15
	testl	%edx, %edx
	jle	.L301
	movq	8(%rax), %rax
	salq	$4, %rdx
	addq	%rax, %rdx
	.p2align 5
	.p2align 4
	.p2align 3
.L300:
	pxor	%xmm0, %xmm0
	addq	$16, %rax
	movups	%xmm0, -16(%rax)
	movb	$54, -8(%rax)
	cmpq	%rax, %rdx
	jne	.L300
.L301:
	movl	16(%r9), %eax
	movq	%r9, %r14
	movl	%eax, %r12d
	subl	24(%r9), %r12d
	movslq	%r12d, %r13
	salq	$4, %r13
	cmpl	%r12d, %eax
	jle	.L299
	.p2align 4
	.p2align 3
.L305:
	testl	%r12d, %r12d
	jne	.L304
	movq	16(%rbp), %rax
	cmpb	$-79, 24(%rax,%rbx)
	je	.L440
.L304:
	movq	48(%r14), %rdi
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
	jg	.L305
	movq	%r14, %r9
.L299:
	movq	$0, 56(%rsp)
	movq	16(%rbp), %rdi
	xorl	%r13d, %r13d
	leaq	(%rdi,%rbx), %rdx
	movq	%rdi, %r10
	movzwl	36(%rdx), %r8d
	testw	%r8w, %r8w
	je	.L303
	movq	%rbx, %rcx
	movq	%r9, %rbx
	movq	%r15, %r9
	movq	24(%rsp), %r15
	jmp	.L302
	.p2align 6
	.p2align 4,,10
	.p2align 3
.L307:
	addq	$1, %r13
	movzwl	%r8w, %eax
	cmpl	%r13d, %eax
	jle	.L456
.L302:
	movl	32(%rdx), %eax
	leaq	0(,%r13,4), %r12
	leaq	(%rdi,%r12), %rsi
	movl	(%rsi,%rax), %eax
	addq	%rdi, %rax
	cmpl	$1, (%rax)
	jne	.L307
	cmpl	$37, 8(%rax)
	jne	.L307
	movl	12(%rax), %eax
	addq	%rdi, %rax
	cmpl	$12, (%rax)
	jne	.L417
	movl	16(%rbx), %edi
	testl	%edi, %edi
	jle	.L372
	movl	8(%rax), %r11d
	xorl	%r8d, %r8d
	movq	8(%rbx), %rax
	jmp	.L311
	.p2align 5
	.p2align 4,,10
	.p2align 3
.L313:
	addl	$1, %r8d
	addq	$4, %rax
	cmpl	%edi, %r8d
	je	.L417
.L311:
	cmpl	%r11d, (%rax)
	jne	.L313
.L310:
	cmpl	%r8d, %edi
	je	.L417
	movslq	%r8d, %r14
	subl	24(%rbx), %edi
	salq	$4, %r14
	cmpl	%r8d, %edi
	jle	.L315
	movq	8(%r9), %rax
	movq	%rcx, 40(%rsp)
	movq	%r9, (%rsp)
	movq	(%rax,%r14), %rdi
	movq	8(%rax,%r14), %rsi
	movl	%r8d, 24(%rsp)
	call	neo_is_void@PLT
	movq	(%rsp), %r9
	movq	40(%rsp), %rcx
	testb	%al, %al
	je	.L316
	movq	16(%rbp), %r10
	movl	24(%rsp), %r8d
	leaq	(%r10,%rcx), %rdx
	leaq	(%r10,%r12), %rsi
.L315:
	movl	32(%rdx), %eax
	pxor	%xmm3, %xmm3
	movq	%rbp, %rdi
	movq	%r9, 40(%rsp)
	movups	%xmm3, 16(%rdx)
	movb	$54, 24(%rdx)
	movl	(%rsi,%rax), %eax
	movq	%rcx, 24(%rsp)
	movl	16(%r10,%rax), %esi
	movl	%r8d, (%rsp)
	call	eval_aux
	movq	40(%rsp), %r9
	movq	16(%rbp), %r10
	movq	24(%rsp), %rcx
	movb	54(%rsp), %r15b
	movq	8(%r9), %rsi
	movl	(%rsp), %r8d
	movq	%r10, %rdi
	movq	%rax, (%rsi,%r14)
	movq	32(%rsp), %rax
	movq	%rdx, 8(%rsi,%r14)
	leaq	(%r10,%rcx), %rdx
	movq	%rax, 16(%rdx)
	movq	global_env(%rip), %rax
	movq	%r15, 24(%rdx)
	movl	(%rax), %r11d
	testl	%r11d, %r11d
	jne	.L457
	testl	%r8d, %r8d
	jne	.L374
	cmpb	$-79, 54(%rsp)
	je	.L318
.L374:
	movzwl	36(%rdx), %r8d
	jmp	.L307
.L449:
	movq	global_env(%rip), %rax
	movl	$54, %edx
	movl	$95, (%rax)
	xorl	%eax, %eax
	jmp	.L411
.L277:
	movq	%r13, %rdx
	xorl	%r8d, %r8d
	movq	%r12, %rcx
	movl	%ebx, %esi
	movq	%rbp, %rdi
	call	create_new_process@PLT
	movq	global_env(%rip), %rdx
	movl	(%rdx), %edx
	testl	%edx, %edx
	jne	.L437
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
.L352:
	.cfi_restore_state
	movq	%rbp, %rdi
	movq	%rbx, %rsi
	call	neo_to_integer@PLT
	xorl	%edx, %edx
	movq	%rbp, %rdi
	movq	%rbx, %rsi
	movq	%rax, %r12
	call	general_neobject_destroy@PLT
	cmpb	$-122, %r15b
	je	.L353
	movb	$-125, %r13b
	movq	%r14, %rdi
	movq	%r13, %rsi
	call	neo_to_string@PLT
	movq	%rax, %rdi
	call	strlen@PLT
	cmpq	%rax, %r12
	jnb	.L354
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
.L356:
	movb	%r15b, %r13b
	xorl	%edx, %edx
	movq	%r14, %rdi
	movb	%bl, %r12b
	movq	%r13, %rsi
	call	general_neobject_destroy@PLT
	movq	%rbp, %rax
	movq	%r12, %rdx
	jmp	.L411
.L263:
	movl	8(%rax), %edi
	movl	$64, %esi
	call	operatorIs@PLT
	movq	16(%rbp), %rdx
	movq	%rbp, %rdi
	movl	12(%rdx,%rbx), %esi
	testb	%al, %al
	je	.L267
	call	get_address
	movq	%rax, %r12
	movq	global_env(%rip), %rax
	movl	(%rax), %r10d
	testl	%r10d, %r10d
	jne	.L436
	movq	16(%rbp), %rax
	movq	%rbp, %rdi
	movl	16(%rax,%rbx), %esi
	call	get_address
	movq	%rax, %rsi
	movq	global_env(%rip), %rax
	movl	(%rax), %r9d
	testl	%r9d, %r9d
	jne	.L436
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
.L456:
	.cfi_restore_state
	movq	%r15, 24(%rsp)
	movq	%r9, %r15
	movq	%rbx, %r9
	movq	%rcx, %rbx
.L303:
	movl	16(%r15), %r10d
	xorl	%r12d, %r12d
	movq	%r9, %r14
	testl	%r10d, %r10d
	jg	.L320
	jmp	.L458
	.p2align 4,,10
	.p2align 3
.L323:
	addq	$1, %r12
	cmpl	%r12d, %edx
	jle	.L322
.L320:
	movq	%r12, %rax
	movl	%r12d, %r13d
	salq	$4, %rax
	addq	8(%r15), %rax
	movq	(%rax), %rdi
	movq	8(%rax), %rsi
	call	neo_is_void@PLT
	movl	16(%r15), %edx
	testb	%al, %al
	je	.L323
	movq	%r14, %r9
	movl	%edx, %eax
.L324:
	movq	16(%rbp), %r10
	movzwl	36(%r10,%rbx), %edx
	cmpl	%eax, %r12d
	jge	.L459
	cmpb	$-79, 54(%rsp)
	movq	%rbp, (%rsp)
	movq	%rbx, %r14
	sete	55(%rsp)
	xorl	%ecx, %ecx
	movq	24(%rsp), %rbp
	movq	%r9, 40(%rsp)
	movq	%rcx, %rbx
	jmp	.L332
	.p2align 4,,10
	.p2align 3
.L326:
	addq	$1, %rbx
	cmpl	%eax, %r12d
	jge	.L423
.L332:
	leaq	(%r10,%r14), %r11
	movl	%edx, %r13d
	cmpl	%ebx, %edx
	jle	.L423
	movl	32(%r11), %esi
	leaq	0(,%rbx,4), %rcx
	leaq	(%r10,%rcx), %rdi
	movl	(%rdi,%rsi), %esi
	movq	%rsi, %rdi
	addq	%r10, %rsi
	cmpl	$1, (%rsi)
	jne	.L325
	cmpl	$37, 8(%rsi)
	je	.L326
.L325:
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
	jne	.L460
	testl	%r12d, %r12d
	jne	.L328
	cmpb	$0, 55(%rsp)
	jne	.L461
.L328:
	cmpl	%r12d, 16(%r15)
	jg	.L329
	jmp	.L419
	.p2align 4,,10
	.p2align 3
.L331:
	addl	$1, %r12d
	addq	$16, %r13
	cmpl	%edx, %r12d
	jge	.L419
.L329:
	movq	8(%r15), %rax
	movq	(%rax,%r13), %rdi
	movq	8(%rax,%r13), %rsi
	call	neo_is_void@PLT
	movl	16(%r15), %edx
	testb	%al, %al
	je	.L331
	movq	(%rsp), %rcx
	movl	%edx, %eax
	movq	16(%rcx), %r10
	movzwl	36(%r10,%r14), %edx
	movl	%edx, %r13d
	jmp	.L326
.L419:
	movq	%rbp, 24(%rsp)
	movq	(%rsp), %rbp
	movq	%r14, %rbx
	movq	40(%rsp), %r9
	movq	16(%rbp), %rax
	movzwl	36(%rax,%r14), %edi
.L321:
	movl	16(%r9), %eax
	cmpl	%edi, %eax
	jg	.L462
.L335:
	xorl	%ecx, %ecx
	cmpb	$0, 20(%r9)
	movl	$54, %r8d
	je	.L339
	movl	16(%r15), %r12d
	movl	48(%rsp), %edi
	movl	%eax, %edx
	movq	%r9, (%rsp)
	subl	24(%r9), %edx
	movl	%r12d, %ecx
	subl	%edi, %ecx
	cmpl	%edx, %ecx
	jg	.L340
	xorl	%edi, %edi
	call	nelist_create@PLT
	movq	(%rsp), %r9
.L341:
	movq	%rax, %rdi
	movq	%r9, (%rsp)
	call	neo_list_convert@PLT
	movq	(%rsp), %r9
	movq	%rax, %rcx
	movq	%rdx, %r8
.L339:
	movq	16(%rbp), %rax
	pxor	%xmm0, %xmm0
	addq	%rbx, %rax
	cmpb	$-79, 54(%rsp)
	movups	%xmm0, 16(%rax)
	movb	$54, 24(%rax)
	je	.L463
	movq	%rcx, %rdx
	movq	%r15, %rsi
	movq	%r8, %rcx
	movq	%r9, %rdi
	call	callUserFunc
	movq	%rax, %r13
	movq	%rdx, %r12
.L345:
	movq	24(%rsp), %rax
	movb	54(%rsp), %al
	xorl	%edx, %edx
	movq	32(%rsp), %rdi
	movq	%rax, %rsi
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
	jmp	.L436
.L455:
	xorl	%eax, %eax
	movl	%eax, 48(%rsp)
.L368:
	movl	24(%r9), %eax
	movl	%edi, %edx
	movl	48(%rsp), %ecx
	subl	%eax, %edx
	addl	%r8d, %eax
	subl	%ecx, %eax
	cmpl	%edx, %r8d
	cmovg	%eax, %edi
	jmp	.L296
.L466:
	movq	48(%r14), %rdi
	movl	%r12d, %esi
	call	nelist_nth@PLT
	movq	%rax, %rdi
	movq	%rdx, %rsi
	call	neo_copy@PLT
	movq	8(%r15), %rcx
	movq	%rax, (%rcx,%r13)
	movq	8(%r15), %rax
	movq	%rdx, 8(%rcx,%r13)
	movq	(%rax,%r13), %rdi
	movq	8(%rax,%r13), %rsi
	call	neo_is_void@PLT
	testb	%al, %al
	jne	.L464
	testq	%r12, %r12
	jne	.L336
.L440:
	movq	global_env(%rip), %rax
	xorl	%esi, %esi
	movq	%r15, %rdi
	movl	$110, (%rax)
	call	general_nelist_destroy@PLT
	movq	16(%rbp), %rax
	movq	16(%rax,%rbx), %rdi
	movq	24(%rax,%rbx), %rsi
	jmp	.L439
.L451:
	movq	16(%rbp), %rax
	movq	%rbp, %rdi
	movl	12(%rax,%rbx), %esi
	call	eval_aux
	movq	%rax, %r15
	movq	global_env(%rip), %rax
	movq	%rdx, %r14
	movl	(%rax), %eax
	testl	%eax, %eax
	jne	.L436
	movq	16(%rbp), %rax
	movq	%r15, %rdi
	movq	%r14, %rsi
	movslq	8(%rax,%rbx), %rdx
	leaq	operators_functions(%rip), %rax
	call	*(%rax,%rdx,8)
	jmp	.L425
.L437:
	xorl	%edx, %edx
	movq	%r13, %rdi
	movq	%r12, %rsi
	call	general_neobject_destroy@PLT
	xorl	%eax, %eax
	movl	$54, %edx
	jmp	.L411
.L452:
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
	jmp	.L411
.L417:
	movq	%r9, %rdi
	xorl	%esi, %esi
	movq	%rcx, %rbx
	call	general_nelist_destroy@PLT
	movq	16(%rbp), %rax
	xorl	%edx, %edx
	movq	16(%rax,%rbx), %rdi
	movq	24(%rax,%rbx), %rsi
	call	general_neobject_destroy@PLT
	addq	16(%rbp), %rbx
	pxor	%xmm0, %xmm0
	movl	$54, %edx
	movups	%xmm0, 16(%rbx)
	movb	$54, 24(%rbx)
	movq	global_env(%rip), %rax
	movl	$93, (%rax)
	xorl	%eax, %eax
	jmp	.L411
.L267:
	call	eval_aux
	movq	%rax, %r13
	movq	global_env(%rip), %rax
	movq	%rdx, %r12
	movl	(%rax), %r8d
	testl	%r8d, %r8d
	jne	.L426
	movq	16(%rbp), %rax
	movq	%rbp, %rdi
	movl	16(%rax,%rbx), %esi
	call	eval_aux
	movq	%rax, %r15
	movq	global_env(%rip), %rax
	movq	%rdx, %r14
	movl	(%rax), %edi
	testl	%edi, %edi
	jne	.L465
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
	jmp	.L411
.L453:
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
	jmp	.L411
.L322:
	movq	%r14, %r9
	movl	%edx, %eax
	leal	1(%r13), %r12d
	jmp	.L324
.L354:
	movq	global_env(%rip), %rax
	movb	%r15b, %r13b
	xorl	%edx, %edx
	movq	%r14, %rdi
	movq	%r13, %rsi
	movl	$18, (%rax)
	call	general_neobject_destroy@PLT
	xorl	%eax, %eax
	movl	$54, %edx
	jmp	.L411
.L423:
	movq	%rbp, 24(%rsp)
	movq	40(%rsp), %r9
	movq	%r14, %rbx
	movl	%r13d, %edi
	movq	(%rsp), %rbp
	jmp	.L321
.L353:
	movb	$-122, %r13b
	movq	%r14, %rdi
	movq	%r13, %rsi
	call	neo_list_len@PLT
	cltq
	cmpq	%rax, %r12
	jnb	.L354
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
	jmp	.L356
.L461:
	movl	32(%rax), %eax
	addq	%rcx, %rdx
	movq	(%rsp), %rdi
	movl	(%rdx,%rax), %esi
	call	get_address
	movq	%rax, 56(%rsp)
	jmp	.L328
.L462:
	xorl	%r12d, %r12d
	movq	%r9, %r14
.L338:
	movq	8(%r15), %rax
	movq	%r12, %r13
	salq	$4, %r13
	movq	(%rax,%r13), %rdi
	movq	8(%rax,%r13), %rsi
	call	neo_is_void@PLT
	testb	%al, %al
	jne	.L466
.L336:
	movl	16(%r14), %eax
	addq	$1, %r12
	cmpl	%r12d, %eax
	jg	.L338
	movq	%r14, %r9
	jmp	.L335
	.p2align 4,,10
	.p2align 3
.L454:
	movq	global_env(%rip), %rax
	movq	32(%rsp), %rdi
	xorl	%edx, %edx
	movq	24(%rsp), %rsi
	movl	$6, (%rax)
	jmp	.L435
.L463:
	movq	56(%rsp), %rsi
	movq	%r15, %rdx
	movq	%r9, %rdi
	call	callUserMethod
	movq	%rax, %r13
	movq	%rdx, %r12
	jmp	.L345
.L460:
	movq	(%rsp), %rbp
	movq	%r14, %rbx
.L433:
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
	xorl	%eax, %eax
	movups	%xmm7, 16(%rbx)
	movl	$54, %edx
	movb	$54, 24(%rbx)
	jmp	.L411
.L340:
	subl	%eax, %r12d
	movl	%r12d, %edi
	call	nelist_create@PLT
	testl	%r12d, %r12d
	movq	(%rsp), %r9
	jle	.L341
	xorl	%r13d, %r13d
	movq	%rbx, (%rsp)
	movslq	%r12d, %r12
	movq	%r9, %r14
	movq	%rax, %rbx
	xchgq	%r13, %rbp
	.p2align 4
	.p2align 3
.L343:
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
	jne	.L343
	movq	%rbx, %rax
	movq	%r14, %r9
	movq	(%rsp), %rbx
	movq	%r13, %rbp
	jmp	.L341
.L465:
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
	jmp	.L411
.L318:
	movl	32(%rdx), %eax
	addq	%r10, %r12
	movq	%rbp, %rdi
	movq	%r9, 24(%rsp)
	movq	%rcx, (%rsp)
	movl	(%r12,%rax), %eax
	movl	16(%r10,%rax), %esi
	call	get_address
	movq	16(%rbp), %r10
	movq	(%rsp), %rcx
	movq	%rax, 56(%rsp)
	movq	24(%rsp), %r9
	leaq	(%r10,%rcx), %rdx
	movq	%r10, %rdi
	movzwl	36(%rdx), %r8d
	jmp	.L307
.L372:
	xorl	%r8d, %r8d
	jmp	.L310
.L458:
	movzwl	36(%rdi,%rbx), %edi
	jmp	.L321
.L369:
	xorl	%eax, %eax
	jmp	.L290
.L457:
	movq	%rcx, %rbx
	movq	%r9, %r15
	jmp	.L433
.L459:
	movzwl	%dx, %edi
	jmp	.L321
.L316:
	movq	%r9, %rdi
	xorl	%esi, %esi
	movq	%rcx, %rbx
	call	general_nelist_destroy@PLT
	movq	16(%rbp), %rax
	xorl	%edx, %edx
	movq	16(%rax,%rbx), %rdi
	movq	24(%rax,%rbx), %rsi
	call	general_neobject_destroy@PLT
	movq	global_env(%rip), %rax
	addq	16(%rbp), %rbx
	pxor	%xmm0, %xmm0
	movups	%xmm0, 16(%rbx)
	movl	$54, %edx
	movb	$54, 24(%rbx)
	movl	$94, (%rax)
	xorl	%eax, %eax
	jmp	.L411
.L464:
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
	jmp	.L436
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
	je	.L474
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
.L474:
	.cfi_restore_state
	movq	%rax, %rdi
	movq	%rbx, %rsi
	cmpb	$8, %bl
	je	.L475
	cmpb	$4, %bl
	je	.L476
	movl	$20, (%rdx)
.L470:
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
.L475:
	.cfi_restore_state
	movb	$8, %sil
	movq	%rax, 8(%rsp)
	call	neo_to_bool@PLT
	movq	8(%rsp), %rdi
	movl	%eax, %ebp
	jmp	.L470
	.p2align 4,,10
	.p2align 3
.L476:
	movb	$4, %sil
	movq	%rax, 8(%rsp)
	call	neo_to_integer@PLT
	movq	8(%rsp), %rdi
	testq	%rax, %rax
	setne	%bpl
	jmp	.L470
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
	je	.L477
	xorl	%ebx, %ebx
	jmp	.L483
	.p2align 4,,10
	.p2align 3
.L489:
	movq	%r14, %rdi
	call	eval_aux
	movq	%rax, 0(%rbp)
	movq	global_env(%rip), %rax
	movq	%rdx, 8(%rbp)
	movl	(%rax), %eax
	testl	%eax, %eax
	jne	.L480
	movzwl	4(%r12), %ecx
	addq	$1, %rbx
	movzwl	%cx, %eax
	cmpl	%ebx, %eax
	jle	.L477
.L483:
	movl	(%r12), %edx
	leaq	0(,%rbx,4), %rax
	addq	16(%r14), %rax
	movq	%rbx, %rbp
	salq	$4, %rbp
	addq	8(%r13), %rbp
	movl	(%rax,%rdx), %esi
	cmpl	$-1, %esi
	jne	.L489
	pxor	%xmm0, %xmm0
	addq	$1, %rbx
	movzwl	%cx, %eax
	movups	%xmm0, 0(%rbp)
	movb	$54, 8(%rbp)
	cmpl	%ebx, %eax
	jg	.L483
.L477:
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
.L480:
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
	je	.L491
	cmpl	$15, %eax
	je	.L492
	cmpl	$12, %eax
	jne	.L493
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
.L492:
	.cfi_restore_state
	movl	8(%rdx), %esi
	call	eval_aux
	movq	%rax, %r12
	movq	global_env(%rip), %rax
	movq	%rdx, %r13
	movl	(%rax), %eax
	testl	%eax, %eax
	jne	.L522
	cmpb	$-77, %dl
	je	.L505
	xorl	%edx, %edx
	movq	%r12, %rdi
	movq	%r13, %rsi
	call	general_neobject_destroy@PLT
	movq	global_env(%rip), %rax
	movl	$80, (%rax)
	.p2align 4
	.p2align 3
.L522:
	xorl	%eax, %eax
.L490:
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
.L491:
	.cfi_restore_state
	movl	8(%rdx), %esi
	call	get_address
	movq	global_env(%rip), %rdx
	movl	(%rdx), %ecx
	testl	%ecx, %ecx
	jne	.L522
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
	jne	.L522
	cmpb	$-122, %r14b
	je	.L497
	cmpb	$-125, %r14b
	je	.L497
	movl	$15, (%rax)
	jmp	.L522
	.p2align 4,,10
	.p2align 3
.L493:
	movq	global_env(%rip), %rax
	movl	$89, (%rax)
	jmp	.L522
	.p2align 4,,10
	.p2align 3
.L497:
	cmpb	$4, %bl
	jne	.L523
	movq	%rbp, %rdi
	movq	%rbx, %rsi
	call	neo_to_integer@PLT
	xorl	%edx, %edx
	movq	%rbp, %rdi
	movq	%rbx, %rsi
	movq	%rax, %r15
	call	general_neobject_destroy@PLT
	cmpb	$-122, %r14b
	je	.L500
	movb	$-125, %r12b
	movq	%r13, %rdi
	movq	%r12, %rsi
	call	neo_to_string@PLT
	movq	%rax, %rdi
	call	strlen@PLT
	movq	%rax, %rdx
	movq	global_env(%rip), %rax
	cmpq	%rdx, %r15
	jnb	.L501
	movl	$105, (%rax)
	jmp	.L522
	.p2align 4,,10
	.p2align 3
.L505:
	movq	%rdx, %rsi
	movq	%r12, %rdi
	call	neo_to_container@PLT
	movq	16(%rbx), %r15
	movq	%rax, %r14
	addq	%rbp, %r15
	movl	12(%r15), %esi
	cmpl	$-1, %esi
	je	.L507
	movl	4(%rax), %eax
	cmpl	%eax, 16(%r15)
	je	.L508
.L507:
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
.L508:
	movq	%r14, %rdi
	call	get_container_field_addr@PLT
	xorl	%edx, %edx
	movq	%r12, %rdi
	movq	%r13, %rsi
	movq	%rax, 8(%rsp)
	call	general_neobject_destroy@PLT
	movq	8(%rsp), %rax
	jmp	.L490
	.p2align 4,,10
	.p2align 3
.L523:
	xorl	%edx, %edx
	movq	%rbp, %rdi
	movq	%rbx, %rsi
	call	general_neobject_destroy@PLT
	movq	global_env(%rip), %rax
	movl	$16, (%rax)
	jmp	.L522
.L524:
	movq	global_env(%rip), %rax
.L501:
	movl	$18, (%rax)
	jmp	.L522
.L500:
	movq	%r13, %rdi
	movq	%r12, %rsi
	call	neo_list_len@PLT
	cltq
	cmpq	%rax, %r15
	jnb	.L524
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
	movq	168(%rax), %rax
	leaq	(%rdx,%r13), %r8
	cmpw	$0, 12(%r8)
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	movq	(%rdi), %rax
	movq	%rax, 24(%rsp)
	je	.L526
	xorl	%ebx, %ebx
.L552:
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
	jne	.L528
	movq	%rdx, %rsi
	cmpb	$8, %dl
	je	.L563
	cmpb	$4, %dl
	je	.L564
	movl	$20, (%rax)
	xorl	%edx, %edx
	call	general_neobject_destroy@PLT
	movq	global_env(%rip), %rdi
	movl	(%rdi), %r10d
	testl	%r10d, %r10d
	jne	.L528
.L533:
	movq	16(%r12), %rdx
	addl	$1, %ebx
	leaq	(%rdx,%r13), %rax
	movzwl	12(%rax), %ecx
	cmpl	%ecx, %ebx
	jge	.L540
	addq	$4, %rbp
	xorl	%r15d, %r15d
	jmp	.L541
	.p2align 4,,10
	.p2align 3
.L545:
	movq	16(%r12), %rdx
	addl	$1, %ebx
	addq	$4, %rbp
	leaq	(%rdx,%r13), %rax
	movzwl	12(%rax), %ecx
	cmpl	%ebx, %ecx
	jle	.L562
.L541:
	movl	8(%rax), %esi
	leaq	(%rdx,%rbp), %rax
	movl	(%rax,%rsi), %eax
	addq	%rdx, %rax
	cmpl	$8, (%rax)
	jne	.L565
	movl	8(%rax), %esi
	movq	%r12, %rdi
	call	eval_aux
	movq	%rax, %rdi
	movq	%rdx, %rsi
	cmpb	$8, %dl
	je	.L566
	cmpb	$4, %dl
	je	.L567
	movq	global_env(%rip), %rax
	xorl	%edx, %edx
	movl	$20, (%rax)
	call	general_neobject_destroy@PLT
	movq	global_env(%rip), %rdi
	movl	(%rdi), %esi
	testl	%esi, %esi
	je	.L545
.L528:
	xorl	%r15d, %r15d
.L553:
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
.L566:
	.cfi_restore_state
	movb	$8, %sil
	movq	%rdx, 16(%rsp)
	movq	%rax, 8(%rsp)
	call	neo_to_bool@PLT
	movq	8(%rsp), %rdi
	movq	16(%rsp), %rcx
	movl	%eax, %r14d
.L543:
	movq	%rcx, %rsi
	xorl	%edx, %edx
	call	general_neobject_destroy@PLT
	movq	global_env(%rip), %rdi
	movl	(%rdi), %ecx
	testl	%ecx, %ecx
	jne	.L528
	movl	%r15d, %eax
	xorl	$1, %eax
	andb	%al, %r14b
	je	.L545
	movq	168(%rdi), %rax
	movq	(%rax), %rax
	movq	104(%rax), %r15
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
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rdi
	call	deleteContext
	movq	global_env(%rip), %rdi
	movl	(%rdi), %edx
	testl	%edx, %edx
	jne	.L528
	testl	%r15d, %r15d
	jne	.L553
	movl	%r14d, %r15d
	jmp	.L545
	.p2align 4,,10
	.p2align 3
.L567:
	movb	$4, %sil
	movq	%rdx, 16(%rsp)
	movq	%rax, 8(%rsp)
	call	neo_to_integer@PLT
	movq	8(%rsp), %rdi
	movq	16(%rsp), %rcx
	testq	%rax, %rax
	setne	%r14b
	jmp	.L543
	.p2align 4,,10
	.p2align 3
.L565:
	cmpl	%ebx, %ecx
	jle	.L562
	movslq	%ebx, %rbp
	salq	$2, %rbp
	leaq	(%rdx,%rbp), %rax
	movl	(%rax,%rsi), %eax
	cmpl	$9, (%rdx,%rax)
	je	.L549
.L562:
	leaq	(%rdx,%r13), %r8
	movzwl	12(%r8), %esi
.L539:
	cmpl	%esi, %ebx
	jl	.L552
	.p2align 4
	.p2align 3
.L540:
	movq	168(%rdi), %rax
	movq	(%rax), %rax
	movq	120(%rax), %rdi
.L526:
	movq	24(%rsp), %rsi
	call	partialRestore@PLT
	jmp	.L528
	.p2align 4,,10
	.p2align 3
.L563:
	movb	$8, %sil
	movq	%rdi, 8(%rsp)
	movq	%rdx, 16(%rsp)
	call	neo_to_bool@PLT
	movq	8(%rsp), %rdi
	movq	16(%rsp), %rcx
	movl	%eax, %r14d
.L530:
	xorl	%edx, %edx
	movq	%rcx, %rsi
	call	general_neobject_destroy@PLT
	movq	global_env(%rip), %rdi
	movl	(%rdi), %r9d
	testl	%r9d, %r9d
	jne	.L528
	testb	%r14b, %r14b
	je	.L533
	movq	168(%rdi), %rax
	movq	(%rax), %rax
	movq	104(%rax), %r14
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
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rdi
	call	deleteContext
	movq	global_env(%rip), %rdi
	movl	(%rdi), %r8d
	testl	%r8d, %r8d
	jne	.L528
	testl	%r15d, %r15d
	jne	.L553
	movq	16(%r12), %rdx
	addl	$1, %ebx
	leaq	(%rdx,%r13), %r8
	movzwl	12(%r8), %esi
	cmpl	%esi, %ebx
	jge	.L540
	movl	8(%r8), %eax
	leaq	4(%rbp,%rax), %rax
	addq	%rdx, %rax
	jmp	.L536
	.p2align 5
	.p2align 4,,10
	.p2align 3
.L538:
	addl	$1, %ebx
	addq	$4, %rax
	cmpl	%esi, %ebx
	je	.L540
.L536:
	movl	(%rax), %ecx
	cmpl	$7, (%rdx,%rcx)
	jne	.L538
	cmpl	%ebx, %esi
	jne	.L539
	jmp	.L540
	.p2align 4,,10
	.p2align 3
.L549:
	testb	%r15b, %r15b
	je	.L550
.L551:
	addl	$1, %ebx
	jmp	.L562
	.p2align 4,,10
	.p2align 3
.L564:
	movb	$4, %sil
	movq	%rdi, 8(%rsp)
	movq	%rdx, 16(%rsp)
	call	neo_to_integer@PLT
	movq	8(%rsp), %rdi
	movq	16(%rsp), %rcx
	testq	%rax, %rax
	setne	%r14b
	jmp	.L530
.L550:
	movq	168(%rdi), %rax
	movq	(%rax), %rax
	movq	104(%rax), %r14
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
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rdi
	call	deleteContext
	movq	global_env(%rip), %rdi
	movl	(%rdi), %eax
	testl	%eax, %eax
	jne	.L528
	testl	%r15d, %r15d
	jne	.L553
	movq	16(%r12), %rdx
	jmp	.L551
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
	je	.L599
	cmpw	$3, %ax
	je	.L600
	cmpw	$2, %ax
	je	.L601
	movq	global_env(%rip), %rax
	movl	$108, (%rax)
.L574:
	xorl	%r15d, %r15d
.L596:
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
.L601:
	.cfi_restore_state
	xorl	%edi, %edi
	call	neo_integer_create@PLT
	movq	%rax, %r14
	movq	global_env(%rip), %rax
	movq	%rdx, %r13
	movl	(%rax), %ecx
	testl	%ecx, %ecx
	jne	.L574
	movq	16(%r12), %rax
	movq	%r12, %rdi
	movl	12(%rax,%rbx), %edx
	movl	4(%rax,%rdx), %esi
	call	eval_aux
	movq	%rax, %rbp
	movq	%rdx, %rcx
.L578:
	movl	$1, (%rsp)
.L573:
	movq	global_env(%rip), %rax
	movl	(%rax), %r15d
	testl	%r15d, %r15d
	jne	.L602
	cmpb	$4, %r13b
	jne	.L582
	cmpb	$4, %cl
	je	.L583
.L582:
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
	movl	$10, (%rax)
	jmp	.L574
	.p2align 4,,10
	.p2align 3
.L599:
	movl	12(%rsi), %eax
	movl	12(%rdx,%rax), %esi
	call	eval_aux
	movq	%rax, %r13
	movq	global_env(%rip), %rax
	movq	%rdx, %rbp
	movl	(%rax), %r8d
	testl	%r8d, %r8d
	jne	.L574
	cmpb	$4, %dl
	je	.L571
	xorl	%edx, %edx
	movq	%r13, %rdi
	movq	%rbp, %rsi
	call	general_neobject_destroy@PLT
	movq	global_env(%rip), %rax
	movl	$108, (%rax)
	jmp	.L574
	.p2align 4,,10
	.p2align 3
.L600:
	movl	12(%rsi), %eax
	movl	4(%rdx,%rax), %esi
	call	eval_aux
	movq	%rax, %r14
	movq	global_env(%rip), %rax
	movq	%rdx, %r13
	movl	(%rax), %esi
	testl	%esi, %esi
	jne	.L574
	movq	16(%r12), %rax
	movq	%r12, %rdi
	movl	12(%rax,%rbx), %edx
	movl	8(%rax,%rdx), %esi
	call	eval_aux
	movq	%rax, %rbp
	movq	%rdx, %rcx
	jmp	.L578
	.p2align 4,,10
	.p2align 3
.L571:
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
	jne	.L574
	movq	16(%r12), %rax
	movq	%r12, %rdi
	movl	%r15d, (%rsp)
	movl	12(%rax,%rbx), %edx
	movl	8(%rax,%rdx), %esi
	call	eval_aux
	movq	%rax, %rbp
	movq	%rdx, %rcx
	jmp	.L573
	.p2align 4,,10
	.p2align 3
.L602:
	xorl	%edx, %edx
	movq	%r14, %rdi
	movq	%r13, %rsi
	call	general_neobject_destroy@PLT
	jmp	.L574
	.p2align 4,,10
	.p2align 3
.L583:
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
	je	.L584
	movl	$111, (%rdx)
	movq	%rbp, %rdi
	xorl	%edx, %edx
	movq	%rcx, %rsi
	call	general_neobject_destroy@PLT
	jmp	.L574
	.p2align 4,,10
	.p2align 3
.L584:
	movl	8(%rax), %eax
	movl	16(%rsp), %r13d
	movq	%rcx, 40(%rsp)
	movl	%eax, 8(%rsp)
	movq	168(%rdx), %rax
	movq	(%rax), %rax
	movq	104(%rax), %r14
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%r14, %rdi
	call	ptrlist_append@PLT
	movq	global_env(%rip), %rax
	movl	8(%rsp), %esi
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	movq	(%rdi), %rcx
	movq	%rcx, 24(%rsp)
	call	save_later@PLT
	movq	global_env(%rip), %rax
	movl	$24, %edi
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rsi
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
	jl	.L588
	jmp	.L585
	.p2align 4,,10
	.p2align 3
.L586:
	movslq	%r15d, %rdx
	cmpq	$1, %rdx
	ja	.L587
	movl	(%rsp), %ecx
	addl	%ecx, %r13d
	cmpl	%r13d, %r14d
	jle	.L597
.L588:
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
	je	.L586
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rdi
	call	deleteContext
	movq	global_env(%rip), %rax
	movq	24(%rsp), %rsi
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	call	partialRestore@PLT
	jmp	.L574
.L585:
	movq	global_env(%rip), %rax
.L597:
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rdi
	call	deleteContext
	movq	global_env(%rip), %rax
	movq	24(%rsp), %rsi
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	call	partialRestore@PLT
	jmp	.L596
.L587:
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rdi
	call	deleteContext
	movq	global_env(%rip), %rax
	movq	24(%rsp), %rsi
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	call	partialRestore@PLT
	cmpl	$2, %r15d
	je	.L574
	jmp	.L596
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
	je	.L604
	movl	$111, (%rdx)
.L605:
	xorl	%eax, %eax
.L603:
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
.L604:
	.cfi_restore_state
	movl	8(%rax), %r13d
	movq	168(%rdx), %rax
	movq	(%rax), %rax
	movq	104(%rax), %r14
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%r14, %rdi
	call	ptrlist_append@PLT
	movq	global_env(%rip), %rax
	movl	%r13d, %esi
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	movq	(%rdi), %rax
	movq	%rax, (%rsp)
	call	save_later@PLT
	movq	global_env(%rip), %rax
	movl	$24, %edi
	movq	168(%rax), %rax
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
	movl	16(%rbp), %esi
	testl	%esi, %esi
	jle	.L606
	xorl	%r15d, %r15d
	jmp	.L610
	.p2align 4,,10
	.p2align 3
.L607:
	movslq	%eax, %rcx
	cmpq	$1, %rcx
	ja	.L609
	movslq	16(%rbp), %rcx
	addq	$1, %r15
	cmpq	%r15, %rcx
	jle	.L621
.L610:
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
	je	.L607
	movq	168(%rdx), %rax
.L620:
	movq	(%rax), %rax
	movq	104(%rax), %rdi
	call	deleteContext
	movq	global_env(%rip), %rax
	movq	(%rsp), %rsi
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	call	partialRestore@PLT
	jmp	.L605
	.p2align 4,,10
	.p2align 3
.L609:
	movq	168(%rdx), %rdx
	movl	%eax, 12(%rsp)
	movq	(%rdx), %rdx
	movq	104(%rdx), %rdi
	call	deleteContext
	movq	global_env(%rip), %rdx
	movq	(%rsp), %rsi
	movq	168(%rdx), %rdx
	movq	(%rdx), %rdx
	movq	120(%rdx), %rdi
	call	partialRestore@PLT
	movl	12(%rsp), %eax
	cmpl	$2, %eax
	je	.L605
	jmp	.L603
	.p2align 4,,10
	.p2align 3
.L621:
	movq	168(%rdx), %rdx
	movl	%eax, 12(%rsp)
	movq	(%rdx), %rdx
	movq	104(%rdx), %rdi
	call	deleteContext
	movq	global_env(%rip), %rdx
	movq	(%rsp), %rsi
	movq	168(%rdx), %rdx
	movq	(%rdx), %rdx
	movq	120(%rdx), %rdi
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
.L606:
	.cfi_restore_state
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
	jmp	.L620
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
	je	.L623
	movl	$111, (%r15)
.L624:
	xorl	%eax, %eax
.L622:
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
.L623:
	.cfi_restore_state
	movq	%rbx, %rdi
	call	strlen@PLT
	movl	8(%r14), %r14d
	movq	%rax, %r13
	movq	168(%r15), %rax
	movq	(%rax), %rax
	movq	104(%rax), %r15
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%r15, %rdi
	call	ptrlist_append@PLT
	movq	global_env(%rip), %rax
	movl	%r14d, %esi
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	movq	(%rdi), %rax
	movq	%rax, 8(%rsp)
	call	save_later@PLT
	movq	global_env(%rip), %rax
	movl	$24, %edi
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rcx
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
	jle	.L625
	movslq	%r13d, %r13
	addq	%rbx, %r13
	jmp	.L629
	.p2align 4,,10
	.p2align 3
.L626:
	movslq	%eax, %rcx
	cmpq	$1, %rcx
	ja	.L628
	addq	$1, %rbx
	cmpq	%r13, %rbx
	je	.L640
.L629:
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
	je	.L626
	movq	168(%rdx), %rax
.L639:
	movq	(%rax), %rax
	movq	104(%rax), %rdi
	call	deleteContext
	movq	global_env(%rip), %rax
	movq	8(%rsp), %rsi
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	call	partialRestore@PLT
	jmp	.L624
	.p2align 4,,10
	.p2align 3
.L628:
	movq	168(%rdx), %rdx
	movl	%eax, (%rsp)
	movq	(%rdx), %rdx
	movq	104(%rdx), %rdi
	call	deleteContext
	movq	global_env(%rip), %rdx
	movq	8(%rsp), %rsi
	movq	168(%rdx), %rdx
	movq	(%rdx), %rdx
	movq	120(%rdx), %rdi
	call	partialRestore@PLT
	movl	(%rsp), %eax
	cmpl	$2, %eax
	je	.L624
	jmp	.L622
	.p2align 4,,10
	.p2align 3
.L640:
	movq	168(%rdx), %rdx
	movl	%eax, (%rsp)
	movq	(%rdx), %rdx
	movq	104(%rdx), %rdi
	call	deleteContext
	movq	global_env(%rip), %rdx
	movq	8(%rsp), %rsi
	movq	168(%rdx), %rdx
	movq	(%rdx), %rdx
	movq	120(%rdx), %rdi
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
.L625:
	.cfi_restore_state
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
	jmp	.L639
	.cfi_endproc
.LFE40:
	.size	execStatementForeachString, .-execStatementForeachString
	.p2align 4
	.globl	initRuntime
	.type	initRuntime, @function
initRuntime:
.LFB42:
	.cfi_startproc
	movl	$1, %r8d
	xorl	%edx, %edx
	movl	$54, %ecx
	xorl	%edi, %edi
	movl	$-1, %esi
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
	movl	$1, %r8d
	movl	%esi, %ebp
	xorl	%edx, %edx
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	movl	$54, %ecx
	movq	%rdi, %rbx
	movl	$-1, %esi
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
	movl	$1, %r8d
	movl	%esi, %r13d
	movl	$54, %ecx
	pushq	%r12
	.cfi_def_cfa_offset 24
	.cfi_offset 12, -24
	xorl	%edx, %edx
	movq	%rdi, %r12
	movl	$-1, %esi
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
