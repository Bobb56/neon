	.file	"runtime.c"
	.text
	.p2align 4
	.globl	neon_interp_yield
	.type	neon_interp_yield, @function
neon_interp_yield:
.LFB17:
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
	movl	8(%rax), %eax
	testl	%eax, %eax
	jne	.L2
	movq	global_env(%rip), %rdx
	movq	global_env(%rip), %rax
	movl	4(%rdx), %edx
	movl	%edx, 8(%rax)
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
	movq	8(%rax), %rdx
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
	cmpq	%rax, %rdx
	je	.L2
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
.L2:
	movq	global_env(%rip), %rax
	movl	8(%rax), %edx
	subl	$1, %edx
	movl	%edx, 8(%rax)
	nop
	movq	-8(%rbp), %rbx
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE17:
	.size	neon_interp_yield, .-neon_interp_yield
	.p2align 4
	.globl	neon_interp_next_process
	.type	neon_interp_next_process, @function
neon_interp_next_process:
.LFB18:
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
.LFE18:
	.size	neon_interp_next_process, .-neon_interp_next_process
	.p2align 4
	.globl	launch_process
	.type	launch_process, @function
launch_process:
.LFB19:
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
	movq	64(%rax), %rax
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
	movq	168(%rax), %rax
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
	movq	-32(%rbp), %rdx
	movq	-24(%rbp), %rax
	movq	%rdx, %rdi
	movq	%rax, %rsi
	call	neobject_destroy@PLT
.L8:
	movq	global_env(%rip), %rax
	movq	40(%rax), %rdx
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movl	88(%rax), %eax
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
.LFE19:
	.size	launch_process, .-launch_process
	.p2align 4
	.globl	exitRuntime
	.type	exitRuntime, @function
exitRuntime:
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
	movq	$0, -32(%rbp)
	movq	$0, -24(%rbp)
	movq	global_env(%rip), %rax
	movq	40(%rax), %rdx
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
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
.LFE31:
	.size	exitRuntime, .-exitRuntime
	.p2align 4
	.globl	update__name__
	.type	update__name__, @function
update__name__:
.LFB11:
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
.LFE11:
	.size	update__name__, .-update__name__
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
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	movq	(%rdi), %r15
	movq	(%r15), %rbx
	testq	%rbx, %rbx
	je	.L31
	cmpq	$0, 8(%r15)
	movl	16(%rbx), %edi
	movq	%r15, %r13
	jne	.L26
	jmp	.L32
	.p2align 4,,10
	.p2align 3
.L27:
	movq	%r12, %r13
.L26:
	movq	(%rbx), %rsi
	movq	8(%rbx), %rdx
	movl	16(%rbx), %edi
	call	replace_var@PLT
	movq	%rbx, %rdi
	call	free@PLT
	movq	8(%r13), %r12
	movq	%r13, %rdi
	call	free@PLT
	cmpq	$0, 8(%r12)
	movq	(%r12), %rbx
	jne	.L27
	movq	(%rbx), %rsi
	movq	8(%rbx), %rdx
	movl	16(%rbx), %edi
	call	replace_var@PLT
	movq	%rbx, %rdi
	call	free@PLT
	movq	%r12, %rdi
	call	free@PLT
.L25:
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 40
	movq	%r15, %rsi
	popq	%r12
	.cfi_def_cfa_offset 32
	movq	%r14, %rdi
	popq	%r13
	.cfi_def_cfa_offset 24
	movl	$1, %edx
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
	jmp	ptrlist_remove@PLT
	.p2align 4,,10
	.p2align 3
.L32:
	.cfi_restore_state
	movq	(%rbx), %rsi
	movq	8(%rbx), %rdx
	call	replace_var@PLT
	movq	%rbx, %rdi
	call	free@PLT
.L31:
	movq	%r15, %rdi
	call	free@PLT
	jmp	.L25
	.cfi_endproc
.LFE13:
	.size	deleteContext, .-deleteContext
	.p2align 4
	.globl	neoIsTrue
	.type	neoIsTrue, @function
neoIsTrue:
.LFB15:
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
.LFE15:
	.size	neoIsTrue, .-neoIsTrue
	.p2align 4
	.globl	local
	.type	local, @function
local:
.LFB16:
	.cfi_startproc
	pushq	%r13
	.cfi_def_cfa_offset 16
	.cfi_offset 13, -16
	movq	%rsi, %r13
	pushq	%r12
	.cfi_def_cfa_offset 24
	.cfi_offset 12, -24
	movl	%edi, %r12d
	movl	$24, %edi
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	call	malloc@PLT
	movl	%r12d, %edi
	movq	%rax, %rbx
	call	get_var_value@PLT
	movl	%r12d, 16(%rbx)
	movl	%r12d, %edi
	movq	%rdx, 8(%rbx)
	movq	%rax, (%rbx)
	call	get_absolute_address@PLT
	movq	%rax, %rdi
	call	var_reset@PLT
	movq	0(%r13), %rdi
	movq	%rbx, %rsi
	popq	%rbx
	.cfi_def_cfa_offset 24
	popq	%r12
	.cfi_def_cfa_offset 16
	popq	%r13
	.cfi_def_cfa_offset 8
	jmp	ptrlist_append@PLT
	.cfi_endproc
.LFE16:
	.size	local, .-local
	.section	.rodata
.LC0:
	.string	".ne"
	.text
	.p2align 4
	.globl	exec_aux
	.type	exec_aux, @function
exec_aux:
.LFB29:
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
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	subq	$32, %rsp
	.cfi_def_cfa_offset 80
	call	neon_interp_yield
	movq	global_env(%rip), %rax
	movl	(%rax), %r11d
	testl	%r11d, %r11d
	jne	.L110
	movl	32(%r12), %r10d
	testl	%r10d, %r10d
	jle	.L110
	xorl	%ebx, %ebx
	.p2align 4,,10
	.p2align 3
.L124:
	movq	24(%r12), %rdx
	leaq	0(,%rbx,8), %r13
	movq	(%rdx,%rbx,8), %rdi
	movl	4(%rdi), %edx
	movl	%edx, 88(%rax)
	movzbl	(%rdi), %ecx
	leal	-16(%rcx), %edx
	cmpb	$40, %dl
	ja	.L47
	leaq	.L49(%rip), %rsi
	movzbl	%dl, %edx
	movslq	(%rsi,%rdx,4), %rdx
	addq	%rsi, %rdx
	jmp	*%rdx
	.section	.rodata
	.align 4
	.align 4
.L49:
	.long	.L57-.L49
	.long	.L47-.L49
	.long	.L56-.L49
	.long	.L47-.L49
	.long	.L47-.L49
	.long	.L47-.L49
	.long	.L47-.L49
	.long	.L47-.L49
	.long	.L47-.L49
	.long	.L47-.L49
	.long	.L47-.L49
	.long	.L55-.L49
	.long	.L54-.L49
	.long	.L53-.L49
	.long	.L47-.L49
	.long	.L47-.L49
	.long	.L47-.L49
	.long	.L47-.L49
	.long	.L47-.L49
	.long	.L52-.L49
	.long	.L47-.L49
	.long	.L47-.L49
	.long	.L47-.L49
	.long	.L47-.L49
	.long	.L47-.L49
	.long	.L47-.L49
	.long	.L47-.L49
	.long	.L47-.L49
	.long	.L51-.L49
	.long	.L47-.L49
	.long	.L50-.L49
	.long	.L47-.L49
	.long	.L47-.L49
	.long	.L47-.L49
	.long	.L47-.L49
	.long	.L47-.L49
	.long	.L47-.L49
	.long	.L47-.L49
	.long	.L47-.L49
	.long	.L47-.L49
	.long	.L48-.L49
	.text
	.p2align 4,,10
	.p2align 3
.L47:
	call	eval_aux
	movq	%rax, %rdi
	movq	%rdx, %rsi
	call	neobject_destroy@PLT
.L147:
	movq	global_env(%rip), %rax
	movl	(%rax), %edx
	testl	%edx, %edx
	jne	.L110
.L117:
	addq	$1, %rbx
	cmpl	%ebx, 32(%r12)
	jg	.L124
	.p2align 4,,10
	.p2align 3
.L110:
	xorl	%r15d, %r15d
.L45:
	addq	$32, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 48
	movl	%r15d, %eax
	popq	%rbx
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
.L48:
	.cfi_restore_state
	movq	24(%rdi), %rdx
	movq	(%rdx), %rdx
	movq	24(%rdx), %rcx
	movq	(%rcx), %rsi
	cmpb	$10, (%rsi)
	jne	.L159
	cmpl	$2, 32(%rdx)
	jne	.L165
	movq	8(%rcx), %rdi
	call	eval_aux
	movq	%rax, %rcx
	movq	global_env(%rip), %rax
	movl	(%rax), %edi
	testl	%edi, %edi
	jne	.L110
	cmpb	$-122, %dl
	je	.L166
	cmpb	$-125, %dl
	jne	.L115
	movq	24(%r12), %rax
	movq	%rcx, %rsi
	movq	%rcx, 16(%rsp)
	movq	%rdx, 8(%rsp)
	movq	(%rax,%r13), %rdi
	call	execStatementForeachString
	movq	8(%rsp), %rdx
	movq	16(%rsp), %rcx
	movl	%eax, %r15d
.L114:
	movq	%rcx, %rdi
	movq	%rdx, %rsi
	call	neobject_destroy@PLT
	movq	global_env(%rip), %rax
	movl	%r15d, %ecx
	orl	(%rax), %ecx
	je	.L117
	jmp	.L45
	.p2align 4,,10
	.p2align 3
.L50:
	movl	8(%rax), %r13d
	movl	$-1, 8(%rax)
	call	exec_aux
	xorl	%ecx, %ecx
	movl	%eax, %r15d
	movq	global_env(%rip), %rax
	addl	%r13d, 8(%rax)
	movl	8(%rax), %edx
	testl	%edx, %edx
	cmovs	%ecx, %edx
	movl	%r15d, %ecx
	orl	(%rax), %ecx
	movl	%edx, 8(%rax)
	je	.L117
	jmp	.L45
	.p2align 4,,10
	.p2align 3
.L54:
	movq	24(%rdi), %rdx
	movq	(%rdx), %rdx
	movq	24(%rdx), %rdx
	movq	(%rdx), %rdx
	cmpb	$10, (%rdx)
	jne	.L159
	call	execStatementFor
.L156:
	movl	%eax, %r15d
	movq	global_env(%rip), %rax
	movl	%r15d, %ecx
	orl	(%rax), %ecx
	je	.L117
	jmp	.L45
	.p2align 4,,10
	.p2align 3
.L55:
	call	execConditionBlock
	movl	%eax, %r15d
	testl	%eax, %eax
	je	.L147
	jmp	.L45
	.p2align 4,,10
	.p2align 3
.L56:
	movl	16(%rdi), %edx
	cmpl	$1, %edx
	je	.L167
	cmpl	$2, %edx
	je	.L168
	cmpl	$3, %edx
	je	.L169
	cmpl	$4, %edx
	je	.L86
	.p2align 4,,10
	.p2align 3
.L153:
	movl	(%rax), %edx
.L59:
	testl	%edx, %edx
	je	.L117
	jmp	.L110
	.p2align 4,,10
	.p2align 3
.L57:
	movl	16(%rdi), %r15d
	cmpl	$3, %r15d
	je	.L153
	jmp	.L45
	.p2align 4,,10
	.p2align 3
.L52:
	movq	24(%rdi), %rax
	movq	(%rax), %r14
	movl	32(%r14), %edi
	call	nelist_create@PLT
	movl	32(%r14), %esi
	movq	%rax, %r8
	testl	%esi, %esi
	jle	.L121
	xorl	%r15d, %r15d
	jmp	.L122
	.p2align 4,,10
	.p2align 3
.L171:
	movq	%rcx, 16(%rsp)
	movq	%r8, 8(%rsp)
	call	eval_aux
	movq	16(%rsp), %rcx
	movq	8(%rsp), %r8
	movq	%rax, (%rcx)
	movq	global_env(%rip), %rax
	movq	%rdx, 8(%rcx)
	movl	(%rax), %ecx
	testl	%ecx, %ecx
	jne	.L170
	addq	$1, %r15
	cmpl	%r15d, 32(%r14)
	jle	.L121
.L122:
	movq	24(%r14), %rax
	movq	%r15, %rcx
	salq	$4, %rcx
	addq	8(%r8), %rcx
	movq	(%rax,%r15,8), %rdi
	testq	%rdi, %rdi
	jne	.L171
	pxor	%xmm0, %xmm0
	addq	$1, %r15
	movups	%xmm0, (%rcx)
	movb	$54, 8(%rcx)
	cmpl	%r15d, 32(%r14)
	jg	.L122
.L121:
	movq	24(%r12), %rax
	movq	%r8, %rdx
	movq	(%rax,%r13), %rax
	movq	48(%rax), %rsi
	movq	40(%rax), %rdi
	call	userFuncDefine@PLT
	movq	%rax, %r15
	movq	24(%r12), %rax
	movq	%rdx, %r14
	movq	(%rax,%r13), %rax
	movq	8(%rax), %rdi
	call	get_var@PLT
	movq	%r15, %rsi
	movq	%r14, %rdx
	movl	%eax, %edi
	call	replace_var@PLT
	jmp	.L154
	.p2align 4,,10
	.p2align 3
.L53:
	movq	24(%rdi), %rax
	.p2align 4,,10
	.p2align 3
.L164:
	movq	(%rax), %rdi
	call	eval_aux
	movq	%rax, %rdi
	movq	global_env(%rip), %rax
	movq	%rdx, %r14
	movl	(%rax), %r8d
	testl	%r8d, %r8d
	jne	.L110
	movq	%rdx, %rsi
	cmpb	$8, %dl
	je	.L172
	cmpb	$4, %r14b
	je	.L173
	movl	$20, (%rax)
	movq	%r14, %rsi
	call	neobject_destroy@PLT
.L154:
	movq	global_env(%rip), %rax
	jmp	.L153
	.p2align 4,,10
	.p2align 3
.L51:
	movq	24(%rdi), %rax
	movq	(%rax), %rdi
	call	exec_aux
	movl	%eax, %r15d
	testl	%eax, %eax
	jne	.L45
	movq	24(%r12), %rax
	movq	(%rax,%r13), %rax
	movq	%rax, 8(%rsp)
	movq	global_env(%rip), %rax
	movl	(%rax), %edx
	cmpl	$1, %edx
	jbe	.L59
	movq	8(%rsp), %rsi
	movl	32(%rsi), %ecx
	cmpl	$1, %ecx
	jle	.L110
	movl	$1, 16(%rsp)
	movq	24(%rsi), %rsi
	movl	$8, %r14d
.L69:
	movq	(%rsi,%r14), %rdx
	movq	24(%rdx), %rdx
	movq	(%rdx), %rdx
	movl	32(%rdx), %r9d
	testl	%r9d, %r9d
	jne	.L174
	movq	%rax, %rdi
.L68:
	movslq	16(%rsp), %r15
	movl	$0, (%rdi)
	movq	(%rsi,%r15,8), %rax
	movq	24(%rax), %rax
	movq	8(%rax), %rdi
	call	exec_aux
	jmp	.L156
	.p2align 4,,10
	.p2align 3
.L172:
	movb	$8, %sil
	movq	%rdi, 8(%rsp)
	call	neo_to_bool@PLT
	movq	8(%rsp), %rdi
	movl	%eax, %r15d
.L104:
	movq	%r14, %rsi
	call	neobject_destroy@PLT
	testb	%r15b, %r15b
	je	.L154
	movq	24(%r12), %rax
	movq	(%rax,%r13), %rax
	movq	24(%rax), %rax
	movq	8(%rax), %rdi
	call	exec_aux
	movl	%eax, %r15d
	movq	global_env(%rip), %rax
	movl	(%rax), %r9d
	testl	%r9d, %r9d
	jne	.L110
	cmpl	$1, %r15d
	ja	.L102
	movq	24(%r12), %rax
	movq	(%rax,%r13), %rax
	movq	24(%rax), %rax
	jmp	.L164
	.p2align 4,,10
	.p2align 3
.L173:
	movb	$4, %sil
	movq	%rdi, 8(%rsp)
	call	neo_to_integer@PLT
	movq	8(%rsp), %rdi
	testq	%rax, %rax
	setne	%r15b
	jmp	.L104
	.p2align 4,,10
	.p2align 3
.L174:
	jle	.L61
	xorl	%r15d, %r15d
	jmp	.L67
	.p2align 4,,10
	.p2align 3
.L66:
	movq	8(%rsp), %rcx
	addq	$1, %r15
	movq	24(%rcx), %rsi
	movq	(%rsi,%r14), %rdx
	movq	24(%rdx), %rdx
	movq	(%rdx), %rdx
	cmpl	%r15d, 32(%rdx)
	jle	.L175
.L67:
	movq	24(%rdx), %rdx
	movq	(%rdx,%r15,8), %rdx
	cmpb	$20, 48(%rdx)
	jne	.L176
	movq	40(%rdx), %rdi
	movq	48(%rdx), %rsi
	call	get_exception_code@PLT
	movl	%eax, %r13d
	movq	global_env(%rip), %rax
	movl	(%rax), %edi
	call	get_exception_from_code_error@PLT
	cmpl	%r13d, %eax
	je	.L177
	movq	global_env(%rip), %rax
	movl	(%rax), %esi
	movq	%rax, %rdi
	testl	%esi, %esi
	jns	.L66
	addl	%r13d, %esi
	jne	.L66
.L64:
	movq	$0, 56(%rdi)
.L149:
	movq	8(%rsp), %rax
	movq	24(%rax), %rsi
	jmp	.L68
	.p2align 4,,10
	.p2align 3
.L175:
	movl	32(%rcx), %ecx
.L61:
	addl	$1, 16(%rsp)
	movl	16(%rsp), %edi
	addq	$8, %r14
	cmpl	%ecx, %edi
	jl	.L69
	jmp	.L153
	.p2align 4,,10
	.p2align 3
.L170:
	movq	%r8, %rdi
	leal	-1(%r15), %esi
	call	nelist_destroy_until@PLT
	xorl	%r8d, %r8d
	jmp	.L121
	.p2align 4,,10
	.p2align 3
.L176:
	movl	$78, (%rax)
	jmp	.L110
	.p2align 4,,10
	.p2align 3
.L177:
	movq	global_env(%rip), %rdi
	movl	(%rdi), %r8d
	testl	%r8d, %r8d
	jns	.L149
	jmp	.L64
	.p2align 4,,10
	.p2align 3
.L102:
	cmpl	$4, %r15d
	je	.L45
	jmp	.L153
	.p2align 4,,10
	.p2align 3
.L166:
	movq	24(%r12), %rax
	movq	%rcx, %rsi
	movq	%rcx, 16(%rsp)
	movq	%rdx, 8(%rsp)
	movq	(%rax,%r13), %rdi
	call	execStatementForeachList
	movq	8(%rsp), %rdx
	movq	16(%rsp), %rcx
	movl	%eax, %r15d
	jmp	.L114
	.p2align 4,,10
	.p2align 3
.L168:
	movl	32(%rdi), %edx
	xorl	%r15d, %r15d
	testl	%edx, %edx
	jg	.L80
	jmp	.L153
	.p2align 4,,10
	.p2align 3
.L178:
	movl	72(%rax), %esi
	movq	144(%rax), %rdi
	call	nelist_nth@PLT
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	neo_to_string@PLT
	movq	%rax, %rdi
	call	strdup@PLT
	movq	8(%rsp), %rcx
	movq	%r14, %rsi
	movq	%rax, 24(%rsp)
	movq	%rcx, %rdi
	movq	%rcx, 16(%rsp)
	call	neo_to_string@PLT
	leaq	.LC0(%rip), %rsi
	movq	%rax, %rdi
	call	addStr@PLT
	movq	16(%rsp), %rdi
	movq	%r14, %rsi
	movq	%rax, 8(%rsp)
	call	neo_to_string@PLT
	movq	%rax, %rdi
	call	strdup@PLT
	movq	%rax, %rdi
	call	neo_str_create@PLT
	movq	%rax, %rsi
	movq	global_env(%rip), %rax
	movl	72(%rax), %edi
	call	replace_var@PLT
	movq	8(%rsp), %rdi
	call	importFile@PLT
	movq	8(%rsp), %rdi
	call	free@PLT
	movq	24(%rsp), %rdi
	call	neo_str_create@PLT
	movq	%rax, %rsi
	movq	global_env(%rip), %rax
	movl	72(%rax), %edi
	call	replace_var@PLT
	movq	16(%rsp), %rdi
	movq	%r14, %rsi
	call	neobject_destroy@PLT
	movq	global_env(%rip), %rax
	movl	(%rax), %ecx
	testl	%ecx, %ecx
	jne	.L110
	call	gc_mark_and_sweep@PLT
	movq	24(%r12), %rax
	addq	$1, %r15
	movq	(%rax,%r13), %rdi
	cmpl	%r15d, 32(%rdi)
	jle	.L154
.L80:
	movq	24(%rdi), %rax
	movq	(%rax,%r15,8), %rdi
	call	eval_aux
	movq	%rax, 8(%rsp)
	movq	global_env(%rip), %rax
	movq	%rdx, %r14
	movl	(%rax), %esi
	testl	%esi, %esi
	je	.L178
	jmp	.L110
	.p2align 4,,10
	.p2align 3
.L169:
	movl	32(%rdi), %esi
	testl	%esi, %esi
	je	.L179
	movq	168(%rax), %rdx
	movq	(%rdx), %rdx
	movq	80(%rdx), %rcx
	cmpq	$0, (%rcx)
	je	.L83
	xorl	%r14d, %r14d
	testl	%esi, %esi
	jg	.L84
	jmp	.L153
	.p2align 4,,10
	.p2align 3
.L85:
	movq	168(%rax), %rax
	movq	(%rax), %rdx
.L84:
	movq	24(%rdi), %rax
	movq	96(%rdx), %rdi
	movq	(%rax,%r14,8), %rax
	movl	56(%rax), %esi
	call	save_later@PLT
	movq	global_env(%rip), %rax
	movl	$24, %edi
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	80(%rax), %r8
	movq	24(%r12), %rax
	movq	(%rax,%r13), %rax
	movq	%r8, 16(%rsp)
	movq	24(%rax), %rax
	movq	(%rax,%r14,8), %rax
	addq	$1, %r14
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
	movq	24(%r12), %rax
	movq	(%rax,%r13), %rdi
	movq	global_env(%rip), %rax
	cmpl	%r14d, 32(%rdi)
	jg	.L85
	jmp	.L153
.L86:
	cmpl	$1, 32(%rdi)
	jle	.L89
	jmp	.L180
	.p2align 4,,10
	.p2align 3
.L183:
	movq	%rdx, %rsi
	cmpb	$8, %dl
	je	.L181
	cmpb	$4, %dl
	je	.L182
	movl	$20, (%rax)
	call	neobject_destroy@PLT
.L94:
	movq	global_env(%rip), %rax
	movl	(%rax), %r14d
	testl	%r14d, %r14d
	jne	.L110
	movl	8(%rax), %r11d
	testl	%r11d, %r11d
	js	.L150
	movl	$0, 8(%rax)
	call	neon_interp_yield
.L150:
	movq	24(%r12), %rax
	movq	(%rax,%r13), %rdi
.L89:
	movq	24(%rdi), %rax
	movq	(%rax), %rdi
	call	eval_aux
	movq	%rax, %rdi
	movq	global_env(%rip), %rax
	movl	(%rax), %r10d
	testl	%r10d, %r10d
	jne	.L110
	jmp	.L183
.L181:
	movb	$8, %sil
	movq	%rdx, 16(%rsp)
	movq	%rdi, 8(%rsp)
	call	neo_to_bool@PLT
	movq	8(%rsp), %rdi
	movq	16(%rsp), %rdx
	movl	%eax, %r15d
.L92:
	movq	%rdx, %rsi
	call	neobject_destroy@PLT
	testb	%r15b, %r15b
	je	.L94
	jmp	.L147
.L182:
	movb	$4, %sil
	movq	%rdx, 16(%rsp)
	movq	%rdi, 8(%rsp)
	call	neo_to_integer@PLT
	movq	8(%rsp), %rdi
	movq	16(%rsp), %rdx
	testq	%rax, %rax
	setne	%r15b
	jmp	.L92
.L159:
	movl	$22, (%rax)
	jmp	.L110
.L167:
	cmpl	$1, 32(%rdi)
	jle	.L72
	movl	$21, (%rax)
	jmp	.L110
.L165:
	movl	$109, (%rax)
	jmp	.L110
.L115:
	movq	%rcx, %rdi
	movq	%rdx, %rsi
	call	neobject_destroy@PLT
	movq	global_env(%rip), %rax
	movl	$109, (%rax)
	jmp	.L110
.L72:
	movq	96(%rax), %rdi
	movq	104(%rax), %rsi
	call	neo_is_void@PLT
	testb	%al, %al
	jne	.L74
	movq	global_env(%rip), %rax
	movl	$99, (%rax)
	jmp	.L110
.L74:
	movq	24(%r12), %rax
	movq	global_env(%rip), %rbx
	movq	(%rax,%r13), %rax
	movl	32(%rax), %edi
	testl	%edi, %edi
	jne	.L75
	call	neo_none_create@PLT
	movq	%rax, 96(%rbx)
	movq	%rdx, 104(%rbx)
.L76:
	movl	$4, %r15d
	jmp	.L45
.L180:
	movl	$101, (%rax)
	jmp	.L110
.L75:
	movq	24(%rax), %rax
	movq	(%rax), %rdi
	call	eval_aux
	movq	%rax, 96(%rbx)
	movq	%rdx, 104(%rbx)
	jmp	.L76
.L83:
	movl	$70, (%rax)
	jmp	.L110
.L179:
	movl	$69, (%rax)
	jmp	.L110
	.cfi_endproc
.LFE29:
	.size	exec_aux, .-exec_aux
	.section	.rodata
.LC1:
	.string	"__local_args__"
	.text
	.p2align 4
	.globl	callUserFunc
	.type	callUserFunc, @function
callUserFunc:
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
	movq	%rdx, %r13
	pushq	%r12
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	movq	%rdi, %r12
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	movq	%rcx, %rbx
	subq	$48, %rsp
	.cfi_def_cfa_offset 96
	movq	global_env(%rip), %rax
	movq	%rsi, 32(%rsp)
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	80(%rax), %r14
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%r14, %rdi
	call	ptrlist_append@PLT
	movq	global_env(%rip), %rax
	cmpb	$0, 20(%r12)
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rax
	movq	(%rax), %rax
	movq	%rax, 40(%rsp)
	jne	.L185
	movl	16(%r12), %eax
.L186:
	xorl	%ebx, %ebx
	testl	%eax, %eax
	jg	.L188
	jmp	.L195
	.p2align 4,,10
	.p2align 3
.L191:
	addq	$1, %rbx
	cmpl	%ebx, 16(%r12)
	jle	.L195
.L188:
	movq	32(%rsp), %rdi
	movl	%ebx, %esi
	call	nelist_nth@PLT
	movq	%rax, %rdi
	movq	%rdx, %rsi
	call	neo_copy@PLT
	movq	%rax, 8(%rsp)
	leaq	0(,%rbx,4), %rax
	movq	%rax, 24(%rsp)
	movq	8(%r12), %rax
	movq	%rdx, 16(%rsp)
	movl	(%rax,%rbx,4), %esi
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	call	save_later@PLT
	movq	global_env(%rip), %rax
	movl	$24, %edi
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	80(%rax), %r14
	movq	8(%r12), %rax
	movl	(%rax,%rbx,4), %r15d
	call	malloc@PLT
	movl	%r15d, %edi
	movq	%rax, %r13
	call	get_var_value@PLT
	movl	%r15d, 16(%r13)
	movl	%r15d, %edi
	movq	%rdx, 8(%r13)
	movq	%rax, 0(%r13)
	call	get_absolute_address@PLT
	movq	%rax, %rdi
	call	var_reset@PLT
	movq	(%r14), %rdi
	movq	%r13, %rsi
	call	ptrlist_append@PLT
	movq	8(%r12), %rax
	movq	16(%rsp), %rdx
	movq	8(%rsp), %rsi
	movl	(%rax,%rbx,4), %edi
	call	replace_var@PLT
	movq	global_env(%rip), %rax
	movl	(%rax), %edx
	testl	%edx, %edx
	je	.L191
	cmpl	$1, %ebx
	jle	.L192
	movl	$4, %ebx
	.p2align 4,,10
	.p2align 3
.L193:
	movq	8(%r12), %rax
	movl	(%rax,%rbx), %edi
	addq	$4, %rbx
	call	free_var@PLT
	cmpq	%rbx, 24(%rsp)
	jne	.L193
	movq	global_env(%rip), %rax
.L192:
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	80(%rax), %rdi
	call	deleteContext
	movq	global_env(%rip), %rax
	movq	40(%rsp), %rsi
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	call	partialRestore@PLT
	addq	$48, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 48
	xorl	%eax, %eax
	movl	$54, %edx
	popq	%rbx
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
.L195:
	.cfi_restore_state
	movq	32(%r12), %rdi
	call	exec_aux
	movl	%eax, %ebx
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	80(%rax), %rdi
	call	deleteContext
	movq	global_env(%rip), %rax
	movq	40(%rsp), %rsi
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	call	partialRestore@PLT
	movq	global_env(%rip), %rcx
	movl	(%rcx), %esi
	testl	%esi, %esi
	jne	.L203
	cmpl	$4, %ebx
	jne	.L196
	pxor	%xmm0, %xmm0
	movq	104(%rcx), %rdx
	movq	96(%rcx), %rax
	movups	%xmm0, 96(%rcx)
	movb	$54, 104(%rcx)
	addq	$48, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 48
	popq	%rbx
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
.L185:
	.cfi_restore_state
	movq	%r13, %rcx
	movq	%rbx, %rsi
	movq	%rbx, %r13
	movq	%rcx, %rdi
	movq	%rcx, 16(%rsp)
	call	neo_list_len@PLT
	leaq	.LC1(%rip), %rdi
	movl	%eax, %ebx
	call	get_var@PLT
	movl	%eax, %r14d
	movq	global_env(%rip), %rax
	movl	%r14d, %esi
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	call	save_later@PLT
	movq	global_env(%rip), %rax
	movl	$24, %edi
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	80(%rax), %rsi
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
	movq	16(%rsp), %rsi
	movq	%r13, %rdx
	movl	%r14d, %edi
	call	replace_var@PLT
	movl	16(%r12), %eax
	testl	%ebx, %ebx
	jle	.L186
	movq	32(%rsp), %rcx
	movl	%eax, 16(%rcx)
	jmp	.L186
	.p2align 4,,10
	.p2align 3
.L203:
	addq	$48, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 48
	xorl	%eax, %eax
	movl	$54, %edx
	popq	%rbx
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
.L196:
	.cfi_restore_state
	addq	$48, %rsp
	.cfi_def_cfa_offset 48
	popq	%rbx
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
	.size	callUserFunc, .-callUserFunc
	.p2align 4
	.globl	callUserMethod
	.type	callUserMethod, @function
callUserMethod:
.LFB22:
	.cfi_startproc
	pushq	%r15
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	pushq	%r14
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	movq	%r8, %r14
	pushq	%r13
	.cfi_def_cfa_offset 32
	.cfi_offset 13, -32
	movq	%rcx, %r13
	pushq	%r12
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	movq	%rdi, %r12
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	subq	$48, %rsp
	.cfi_def_cfa_offset 96
	movq	global_env(%rip), %rax
	movq	%rsi, 40(%rsp)
	movq	168(%rax), %rax
	movq	%rdx, 24(%rsp)
	movq	(%rax), %rax
	movq	80(%rax), %rbx
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%rbx, %rdi
	call	ptrlist_append@PLT
	movq	global_env(%rip), %rax
	cmpb	$0, 20(%r12)
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rax
	movq	(%rax), %rax
	movq	%rax, 32(%rsp)
	jne	.L205
	movl	16(%r12), %eax
.L206:
	xorl	%ebx, %ebx
	testl	%eax, %eax
	jg	.L208
	jmp	.L215
	.p2align 4,,10
	.p2align 3
.L211:
	addq	$1, %rbx
	cmpl	%ebx, 16(%r12)
	jle	.L215
.L208:
	movq	24(%rsp), %rdi
	movl	%ebx, %esi
	call	nelist_nth@PLT
	movq	%rax, %rdi
	movq	%rdx, %rsi
	call	neo_copy@PLT
	movq	%rax, (%rsp)
	leaq	0(,%rbx,4), %rax
	movq	%rax, 16(%rsp)
	movq	8(%r12), %rax
	movq	%rdx, 8(%rsp)
	movl	(%rax,%rbx,4), %esi
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	call	save_later@PLT
	movq	global_env(%rip), %rax
	movl	$24, %edi
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	80(%rax), %r14
	movq	8(%r12), %rax
	movl	(%rax,%rbx,4), %r15d
	call	malloc@PLT
	movl	%r15d, %edi
	movq	%rax, %r13
	call	get_var_value@PLT
	movl	%r15d, 16(%r13)
	movl	%r15d, %edi
	movq	%rdx, 8(%r13)
	movq	%rax, 0(%r13)
	call	get_absolute_address@PLT
	movq	%rax, %rdi
	call	var_reset@PLT
	movq	(%r14), %rdi
	movq	%r13, %rsi
	call	ptrlist_append@PLT
	movq	8(%r12), %rax
	movq	8(%rsp), %rdx
	movq	(%rsp), %rsi
	movl	(%rax,%rbx,4), %edi
	call	replace_var@PLT
	movq	global_env(%rip), %rax
	movl	(%rax), %edx
	testl	%edx, %edx
	je	.L211
	cmpl	$1, %ebx
	jle	.L209
	movl	$4, %ebx
	.p2align 4,,10
	.p2align 3
.L213:
	movq	8(%r12), %rax
	movl	(%rax,%rbx), %edi
	addq	$4, %rbx
	call	free_var@PLT
	cmpq	%rbx, 16(%rsp)
	jne	.L213
	movq	global_env(%rip), %rax
.L209:
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	80(%rax), %rdi
	call	deleteContext
	movq	global_env(%rip), %rax
	movq	32(%rsp), %rsi
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	call	partialRestore@PLT
	addq	$48, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 48
	xorl	%eax, %eax
	movl	$54, %edx
	popq	%rbx
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
.L215:
	.cfi_restore_state
	movq	32(%r12), %rdi
	call	exec_aux
	movl	%eax, %ebx
	movq	global_env(%rip), %rax
	movl	(%rax), %ecx
	testl	%ecx, %ecx
	jne	.L209
	movq	8(%r12), %rax
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
	movq	80(%rax), %rdi
	call	deleteContext
	movq	global_env(%rip), %rax
	movq	32(%rsp), %rsi
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	call	partialRestore@PLT
	movq	40(%rsp), %r14
	movq	(%r14), %rdi
	movq	8(%r14), %rsi
	call	neobject_destroy@PLT
	movq	%r12, (%r14)
	movq	%r13, 8(%r14)
	cmpl	$4, %ebx
	jne	.L216
	movq	global_env(%rip), %rcx
	pxor	%xmm0, %xmm0
	movq	104(%rcx), %rdx
	movq	96(%rcx), %rax
	movups	%xmm0, 96(%rcx)
	movb	$54, 104(%rcx)
	addq	$48, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 48
	popq	%rbx
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
	movq	%r14, %rsi
	movq	%r13, %rdi
	call	neo_list_len@PLT
	leaq	.LC1(%rip), %rdi
	movl	%eax, 8(%rsp)
	call	get_var@PLT
	movl	%eax, %ebx
	movq	global_env(%rip), %rax
	movl	%ebx, %esi
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	call	save_later@PLT
	movq	global_env(%rip), %rax
	movl	$24, %edi
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	80(%rax), %rsi
	movq	%rsi, (%rsp)
	call	malloc@PLT
	movl	%ebx, %edi
	movq	%rax, %r15
	call	get_var_value@PLT
	movl	%ebx, 16(%r15)
	movl	%ebx, %edi
	movq	%rdx, 8(%r15)
	movq	%rax, (%r15)
	call	get_absolute_address@PLT
	movq	%rax, %rdi
	call	var_reset@PLT
	movq	(%rsp), %rsi
	movq	(%rsi), %rdi
	movq	%r15, %rsi
	call	ptrlist_append@PLT
	movq	%r13, %rsi
	movq	%r14, %rdx
	movl	%ebx, %edi
	call	replace_var@PLT
	movl	8(%rsp), %ecx
	movl	16(%r12), %eax
	testl	%ecx, %ecx
	jle	.L206
	movq	24(%rsp), %rcx
	movl	%eax, 16(%rcx)
	jmp	.L206
	.p2align 4,,10
	.p2align 3
.L216:
	addq	$48, %rsp
	.cfi_def_cfa_offset 48
	popq	%rbx
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
.LFE22:
	.size	callUserMethod, .-callUserMethod
	.p2align 4
	.globl	eval_aux
	.type	eval_aux, @function
eval_aux:
.LFB23:
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
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	subq	$64, %rsp
	.cfi_def_cfa_offset 112
	call	neon_interp_yield
	movq	global_env(%rip), %rax
	movl	(%rax), %esi
	testl	%esi, %esi
	jne	.L438
	movl	4(%r14), %edx
	movl	%edx, 88(%rax)
	movzbl	(%r14), %ecx
	cmpb	$52, %cl
	ja	.L226
	cmpb	$3, %cl
	ja	.L227
	cmpb	$1, %cl
	je	.L228
	cmpb	$2, %cl
	jne	.L230
	movl	32(%r14), %edi
	cmpl	$1, %edi
	je	.L442
	cmpl	$2, %edi
	jne	.L250
	movl	16(%r14), %edi
	movl	$256, %esi
	call	operatorIs@PLT
	testb	%al, %al
	jne	.L443
	movl	16(%r14), %edi
	cmpl	$37, %edi
	je	.L444
	cmpl	$35, %edi
	je	.L445
	movl	$8, %esi
	call	operatorIs@PLT
	testb	%al, %al
	je	.L254
	movq	24(%r14), %rax
	movq	(%rax), %rdi
	call	get_address
	movq	%rax, %rbx
	movq	global_env(%rip), %rax
	movl	(%rax), %r12d
	testl	%r12d, %r12d
	jne	.L438
	movq	24(%r14), %rax
	movq	8(%rax), %rdi
	call	eval_aux
	movq	%rax, %rcx
	movq	global_env(%rip), %rax
	movq	%rdx, %r13
	cmpl	$0, (%rax)
	jne	.L438
	movq	%rcx, %r15
	movslq	16(%r14), %rcx
	leaq	operators_functions(%rip), %rax
	movq	%rbx, %rdi
	movq	%r15, %rsi
	call	*(%rax,%rcx,8)
	.p2align 4,,10
	.p2align 3
.L434:
	movq	%rax, %r12
	movq	%rdx, %rbx
.L436:
	movq	%r15, %rdi
	movq	%r13, %rsi
	call	neobject_destroy@PLT
	movq	%r12, %rax
	movq	%rbx, %rdx
.L417:
	addq	$64, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 48
	popq	%rbx
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
.L226:
	.cfi_restore_state
	cmpb	$-122, %cl
	je	.L234
	jbe	.L446
	cmpb	$-77, %cl
	jne	.L230
	movl	32(%r14), %edi
	xorl	%ebx, %ebx
	call	nelist_create@PLT
	movl	32(%r14), %esi
	movq	%rax, %r12
	testl	%esi, %esi
	jle	.L356
.L353:
	movq	24(%r14), %rax
	movq	(%rax,%rbx,8), %rax
	movq	24(%rax), %rax
	movq	(%rax), %rdi
	call	eval_aux
	movq	%rax, %rsi
	movq	global_env(%rip), %rax
	movq	%rdx, %rdi
	movq	%rbx, %rdx
	salq	$4, %rdx
	addq	8(%r12), %rdx
	movl	(%rax), %ecx
	movq	%rsi, (%rdx)
	movq	%rdi, 8(%rdx)
	testl	%ecx, %ecx
	jne	.L447
	addq	$1, %rbx
	cmpl	%ebx, 32(%r14)
	jg	.L353
.L356:
	movl	16(%r14), %edi
	addq	$64, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 48
	movq	%r12, %rsi
	popq	%rbx
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
.L227:
	.cfi_restore_state
	movl	$1, %edx
	salq	%cl, %rdx
	testl	$1179952, %edx
	jne	.L231
	cmpb	$10, %cl
	je	.L232
	cmpb	$52, %cl
	je	.L448
.L230:
	movl	$19, (%rax)
	movl	$54, %edx
	xorl	%eax, %eax
	jmp	.L417
.L446:
	cmpb	$-128, %cl
	jne	.L449
	movq	40(%r14), %rdi
	movq	48(%r14), %rsi
	call	neo_is_void@PLT
	testb	%al, %al
	jne	.L450
.L274:
	movzbl	48(%r14), %eax
	cmpb	$-128, %al
	je	.L451
	leal	79(%rax), %edx
	cmpb	$1, %dl
	jbe	.L452
	movq	global_env(%rip), %rdx
	cmpb	$54, %al
	je	.L453
	movl	$9, (%rdx)
.L429:
	movq	40(%r14), %rdi
	movq	48(%r14), %rsi
.L437:
	call	neobject_destroy@PLT
	pxor	%xmm0, %xmm0
	movups	%xmm0, 40(%r14)
	movb	$54, 48(%r14)
.L438:
	addq	$64, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 48
	xorl	%eax, %eax
	movl	$54, %edx
	popq	%rbx
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
.L449:
	.cfi_restore_state
	cmpb	$-125, %cl
	jne	.L230
.L231:
	movq	40(%r14), %rdi
	movq	48(%r14), %rsi
.L440:
	addq	$64, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 48
	popq	%rbx
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
.L228:
	.cfi_restore_state
	movq	24(%r14), %rax
	movq	(%rax), %rdi
	call	get_address
	movq	(%rax), %r12
	movq	8(%rax), %rbx
	movzbl	8(%rax), %r15d
	movq	global_env(%rip), %rax
	movl	(%rax), %r8d
	testl	%r8d, %r8d
	jne	.L438
	movq	24(%r14), %rax
	movq	8(%rax), %rdi
	call	eval_aux
	movq	%rax, %r14
	movq	global_env(%rip), %rax
	movq	%rdx, %r13
	movl	(%rax), %edi
	testl	%edi, %edi
	jne	.L438
	cmpb	$-122, %r15b
	je	.L346
	cmpb	$-125, %r15b
	je	.L346
	movl	$15, (%rax)
	movl	$54, %edx
	xorl	%eax, %eax
	jmp	.L417
	.p2align 4,,10
	.p2align 3
.L234:
	movl	32(%r14), %edi
.L250:
	call	nelist_create@PLT
	movl	32(%r14), %r11d
	movq	%rax, %r13
	testl	%r11d, %r11d
	jle	.L272
	xorl	%ebx, %ebx
	jmp	.L273
	.p2align 4,,10
	.p2align 3
.L455:
	call	eval_aux
	movq	%rax, (%r12)
	movq	global_env(%rip), %rax
	movq	%rdx, 8(%r12)
	movl	(%rax), %r10d
	testl	%r10d, %r10d
	jne	.L454
.L271:
	addq	$1, %rbx
	cmpl	%ebx, 32(%r14)
	jle	.L272
.L273:
	movq	24(%r14), %rax
	movq	%rbx, %r12
	salq	$4, %r12
	addq	8(%r13), %r12
	movq	(%rax,%rbx,8), %rdi
	testq	%rdi, %rdi
	jne	.L455
	pxor	%xmm0, %xmm0
	movups	%xmm0, (%r12)
	movb	$54, 8(%r12)
	jmp	.L271
	.p2align 4,,10
	.p2align 3
.L454:
	movq	%r13, %rdi
	leal	-1(%rbx), %esi
	xorl	%r13d, %r13d
	call	nelist_destroy_until@PLT
.L272:
	movq	global_env(%rip), %rax
	movl	(%rax), %r9d
	testl	%r9d, %r9d
	jne	.L438
	addq	$64, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 48
	movq	%r13, %rdi
	popq	%rbx
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
.L232:
	.cfi_restore_state
	movl	56(%r14), %edi
	call	get_var_value@PLT
	movq	%rax, %rdi
	movq	%rdx, %rsi
	cmpb	$54, %dl
	jne	.L440
	movq	global_env(%rip), %rax
	movl	$5, (%rax)
	jmp	.L438
.L448:
	movq	24(%r14), %rax
	movq	(%rax), %rdi
	call	eval_aux
	movq	%rax, %r15
	movq	global_env(%rip), %rax
	movq	%rdx, %r13
	movl	(%rax), %edx
	testl	%edx, %edx
	jne	.L438
	movq	%r15, %rdi
	movq	%r13, %rsi
	cmpb	$-77, %r13b
	je	.L359
	call	neobject_destroy@PLT
	movq	global_env(%rip), %rax
	movl	$54, %edx
	movl	$80, (%rax)
	xorl	%eax, %eax
	jmp	.L417
	.p2align 4,,10
	.p2align 3
.L442:
	movl	16(%r14), %edi
	cmpl	$39, %edi
	je	.L456
	movl	$256, %esi
	call	operatorIs@PLT
	testb	%al, %al
	je	.L242
	movq	24(%r14), %rax
	movslq	16(%r14), %rdx
	movq	(%rax), %rdi
	leaq	operators_functions(%rip), %rax
	movq	(%rax,%rdx,8), %rax
	addq	$64, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 48
	popq	%rbx
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
.L450:
	.cfi_restore_state
	movq	24(%r14), %rax
	movq	(%rax), %rdi
	call	eval_aux
	movq	%rax, 40(%r14)
	movq	global_env(%rip), %rax
	movq	%rdx, 48(%r14)
	movl	(%rax), %r8d
	testl	%r8d, %r8d
	je	.L274
	jmp	.L438
.L452:
	movq	48(%r14), %rsi
	movq	40(%r14), %rdi
	call	neo_to_userfunc@PLT
	movq	%rax, %r12
	movq	40(%r14), %rax
	movl	16(%r12), %r8d
	movzbl	20(%r12), %esi
	movq	%rax, 24(%rsp)
	movq	48(%r14), %rax
	movq	%rax, 16(%rsp)
	movzbl	48(%r14), %eax
	movb	%al, (%rsp)
	movq	24(%r14), %rax
	movq	8(%rax), %rax
	movl	32(%rax), %ecx
	cmpl	%r8d, %ecx
	jg	.L286
	testl	%ecx, %ecx
	jle	.L457
.L287:
	movq	24(%rax), %rdi
	movl	$0, 44(%rsp)
	xorl	%eax, %eax
	jmp	.L294
	.p2align 4,,10
	.p2align 3
.L291:
	addq	$1, %rax
	cmpl	%eax, %ecx
	jle	.L288
.L294:
	movq	(%rdi,%rax,8), %rdx
	cmpb	$2, (%rdx)
	jne	.L291
	cmpl	$37, 16(%rdx)
	jne	.L291
	testl	%r8d, %r8d
	jle	.L291
	movq	24(%rdx), %rdx
	movq	8(%r12), %r9
	movq	(%rdx), %rdx
	movl	56(%rdx), %r10d
	xorl	%edx, %edx
	jmp	.L292
	.p2align 4,,10
	.p2align 3
.L293:
	addl	$1, %edx
	addq	$4, %r9
	cmpl	%edx, %r8d
	je	.L291
.L292:
	cmpl	%r10d, (%r9)
	jne	.L293
	movl	%r8d, %r9d
	subl	24(%r12), %r9d
	cmpl	%edx, %r9d
	jg	.L291
	cmpl	%edx, %r8d
	setg	%dl
	cmpb	$1, %dl
	sbbl	$-1, 44(%rsp)
	jmp	.L291
	.p2align 4,,10
	.p2align 3
.L447:
	leal	-1(%rbx), %esi
	movq	%r12, %rdi
	call	nelist_destroy_until@PLT
	xorl	%eax, %eax
	movl	$54, %edx
	jmp	.L417
	.p2align 4,,10
	.p2align 3
.L359:
	call	neo_to_container@PLT
	movl	16(%r14), %esi
	movq	%rax, %rbx
	cmpl	$-1, %esi
	je	.L458
.L360:
	movq	%rbx, %rdi
	call	get_container_field@PLT
	movq	%rax, %rdi
	movq	%rdx, %rsi
	call	neo_copy@PLT
	jmp	.L434
	.p2align 4,,10
	.p2align 3
.L346:
	movq	%r14, %rdi
	movq	%r13, %rsi
	cmpb	$4, %r13b
	je	.L347
	call	neobject_destroy@PLT
	movq	global_env(%rip), %rax
	movl	$54, %edx
	movl	$16, (%rax)
	xorl	%eax, %eax
	jmp	.L417
.L451:
	movdqu	40(%r14), %xmm2
	movq	40(%r14), %rdi
	movq	48(%r14), %rsi
	movaps	%xmm2, (%rsp)
	call	neo_to_function@PLT
	pxor	%xmm0, %xmm0
	movq	%rax, %r12
	movq	24(%r14), %rax
	movups	%xmm0, 40(%r14)
	movb	$54, 48(%r14)
	movq	8(%rax), %r15
	movl	32(%r15), %edi
	call	nelist_create@PLT
	movl	32(%r15), %edi
	movq	%rax, %r13
	testl	%edi, %edi
	jle	.L279
	movq	%r12, 16(%rsp)
	xorl	%ebx, %ebx
	movq	%rax, %r12
	movq	%r15, %r13
	jmp	.L280
	.p2align 4,,10
	.p2align 3
.L461:
	call	eval_aux
	movq	%rax, (%r15)
	movq	global_env(%rip), %rax
	movq	%rdx, 8(%r15)
	movl	(%rax), %esi
	testl	%esi, %esi
	jne	.L459
.L278:
	addq	$1, %rbx
	cmpl	%ebx, 32(%r13)
	jle	.L460
.L280:
	movq	24(%r13), %rax
	movq	%rbx, %r15
	salq	$4, %r15
	addq	8(%r12), %r15
	movq	(%rax,%rbx,8), %rdi
	testq	%rdi, %rdi
	jne	.L461
	pxor	%xmm0, %xmm0
	movups	%xmm0, (%r15)
	movb	$54, 8(%r15)
	jmp	.L278
.L460:
	movq	%r12, %r13
	movq	16(%rsp), %r12
.L279:
	movq	global_env(%rip), %rax
	movdqa	(%rsp), %xmm3
	movl	(%rax), %ecx
	movups	%xmm3, 40(%r14)
	testl	%ecx, %ecx
	jne	.L429
	movq	%r13, %rsi
	movq	%r12, %rdi
	call	funcArgsCheck@PLT
	testb	%al, %al
	je	.L462
	movq	48(%r14), %rsi
	movq	40(%r14), %rdi
	movq	%r13, %rdx
	call	functionCall@PLT
	movq	%r13, %rdi
	movq	%rax, %r12
	movq	%rdx, %rbx
	call	nelist_destroy@PLT
	movq	40(%r14), %rdi
	movq	48(%r14), %rsi
	call	neobject_destroy@PLT
	movq	global_env(%rip), %rax
	pxor	%xmm0, %xmm0
	movups	%xmm0, 40(%r14)
	movb	$54, 48(%r14)
	movl	(%rax), %edx
	testl	%edx, %edx
	jne	.L435
	movl	4(%r14), %edx
	movl	%edx, 88(%rax)
	movq	%r12, %rax
	movq	%rbx, %rdx
	jmp	.L417
.L453:
	movl	$8, (%rdx)
	jmp	.L429
.L457:
	xorl	%eax, %eax
	movl	%eax, 44(%rsp)
.L288:
	testb	%sil, %sil
	jne	.L363
.L295:
	movl	%r8d, %edi
	call	nelist_create@PLT
	movslq	16(%rax), %rdx
	movq	%rax, %r15
	testl	%edx, %edx
	jle	.L300
	movq	8(%rax), %rax
	salq	$4, %rdx
	addq	%rax, %rdx
	.p2align 4,,10
	.p2align 3
.L299:
	pxor	%xmm0, %xmm0
	addq	$16, %rax
	movups	%xmm0, -16(%rax)
	movb	$54, -8(%rax)
	cmpq	%rax, %rdx
	jne	.L299
.L300:
	movl	16(%r12), %eax
	movl	%eax, %ebx
	subl	24(%r12), %ebx
	movslq	%ebx, %rcx
	salq	$4, %rcx
	movq	%rcx, %r13
	cmpl	%ebx, %eax
	jle	.L298
	.p2align 4,,10
	.p2align 3
.L304:
	testl	%ebx, %ebx
	jne	.L303
	cmpb	$-79, 48(%r14)
	je	.L441
.L303:
	movq	40(%r12), %rdi
	movl	%ebx, %esi
	addl	$1, %ebx
	call	nelist_nth@PLT
	movq	%rax, %rdi
	movq	%rdx, %rsi
	call	neo_copy@PLT
	movq	8(%r15), %rcx
	movq	%rax, (%rcx,%r13)
	movq	%rdx, 8(%rcx,%r13)
	addq	$16, %r13
	cmpl	%ebx, 16(%r12)
	jg	.L304
.L298:
	movq	$0, 56(%rsp)
	movq	24(%r14), %rsi
	xorl	%ebx, %ebx
	movq	8(%rsi), %rdx
	movl	32(%rdx), %ecx
	testl	%ecx, %ecx
	jle	.L302
	movq	%r15, %r9
	movq	%r14, %r15
	jmp	.L301
	.p2align 4,,10
	.p2align 3
.L305:
	addq	$1, %rbx
	cmpl	%ebx, %ecx
	jle	.L463
.L301:
	movq	24(%rdx), %rax
	leaq	0(,%rbx,8), %r14
	movq	(%rax,%rbx,8), %rax
	cmpb	$2, (%rax)
	jne	.L305
	cmpl	$37, 16(%rax)
	jne	.L305
	movq	24(%rax), %rax
	movq	(%rax), %rax
	cmpb	$10, (%rax)
	jne	.L423
	movl	16(%r12), %ecx
	testl	%ecx, %ecx
	jle	.L365
	movl	56(%rax), %esi
	xorl	%r13d, %r13d
	movq	8(%r12), %rax
	jmp	.L308
	.p2align 4,,10
	.p2align 3
.L310:
	addl	$1, %r13d
	addq	$4, %rax
	cmpl	%ecx, %r13d
	je	.L423
.L308:
	cmpl	%esi, (%rax)
	jne	.L310
.L307:
	cmpl	%r13d, %ecx
	je	.L423
	movslq	%r13d, %r10
	subl	24(%r12), %ecx
	movq	8(%r9), %rax
	salq	$4, %r10
	cmpl	%r13d, %ecx
	jle	.L312
	movq	(%rax,%r10), %rdi
	movq	8(%rax,%r10), %rsi
	movq	%r9, 48(%rsp)
	movq	%r10, 32(%rsp)
	call	neo_is_void@PLT
	movq	48(%rsp), %r9
	testb	%al, %al
	je	.L313
	movq	24(%r15), %rax
	movq	32(%rsp), %r10
	movq	8(%rax), %rdx
.L312:
	pxor	%xmm4, %xmm4
	movq	%r9, 48(%rsp)
	movups	%xmm4, 40(%r15)
	movb	$54, 48(%r15)
	movq	24(%rdx), %rax
	movq	%r10, 32(%rsp)
	movq	(%rax,%r14), %rax
	movq	24(%rax), %rax
	movq	8(%rax), %rdi
	call	eval_aux
	movq	48(%rsp), %r9
	movq	32(%rsp), %r10
	movq	24(%rsp), %rsi
	movq	8(%r9), %rcx
	movq	%rax, (%rcx,%r10)
	movq	16(%rsp), %rax
	movb	(%rsp), %al
	movq	%rdx, 8(%rcx,%r10)
	movq	%rsi, 40(%r15)
	movq	%rax, 16(%rsp)
	movq	%rax, 48(%r15)
	movq	global_env(%rip), %rax
	movl	(%rax), %eax
	testl	%eax, %eax
	jne	.L464
	movq	24(%r15), %rsi
	movq	8(%rsi), %rdx
	testl	%r13d, %r13d
	jne	.L369
	cmpb	$-79, (%rsp)
	je	.L315
.L369:
	movl	32(%rdx), %ecx
	jmp	.L305
.L456:
	movq	24(%r14), %rax
	movl	$64, %edi
	movq	(%rax), %r12
	call	malloc@PLT
	movq	%rax, %rbx
	movzbl	(%r12), %eax
	movb	%al, (%rbx)
	movq	8(%r12), %rax
	movq	%rax, 8(%rbx)
	movl	16(%r12), %eax
	movl	%eax, 16(%rbx)
	movq	24(%r12), %rax
	movq	32(%r12), %rdx
	movq	%rax, 24(%rbx)
	movq	(%rax), %rdi
	movq	%rdx, 32(%rbx)
	call	eval_aux
	movq	%rax, 40(%rbx)
	movq	global_env(%rip), %rax
	movq	%rdx, 48(%rbx)
	movl	(%rax), %ecx
	testl	%ecx, %ecx
	jne	.L431
	cmpb	$-78, 48(%rbx)
	je	.L241
	movq	40(%rbx), %rdi
	movq	48(%rbx), %rsi
	movl	$100, (%rax)
	call	neobject_destroy@PLT
.L431:
	movq	%rbx, %rdi
	call	free@PLT
	jmp	.L438
.L242:
	movl	16(%r14), %edi
	movl	$32, %esi
	call	operatorIs@PLT
	testb	%al, %al
	jne	.L246
	movl	16(%r14), %edi
	movl	$1, %esi
	call	operatorIs@PLT
	testb	%al, %al
	je	.L465
.L246:
	movq	24(%r14), %rax
	movq	(%rax), %rdi
	call	get_address
	movq	global_env(%rip), %rdx
	movl	(%rdx), %edx
	testl	%edx, %edx
	jne	.L438
	movslq	16(%r14), %rcx
	leaq	operators_functions(%rip), %rdx
	movq	%rax, %rdi
	movq	(%rdx,%rcx,8), %rdx
	addq	$64, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 48
	popq	%rbx
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
.L347:
	.cfi_restore_state
	call	neo_to_integer@PLT
	movq	%r14, %rdi
	movq	%r13, %rsi
	movq	%rax, (%rsp)
	call	neobject_destroy@PLT
	cmpb	$-122, %r15b
	je	.L348
	cmpq	$0, (%rsp)
	js	.L350
	movb	$-125, %bl
	movq	%r12, %rdi
	movq	%rbx, %rsi
	call	neo_to_string@PLT
	movq	%rax, %rdi
	call	strlen@PLT
	movq	(%rsp), %r15
	cmpq	%rax, %r15
	jnb	.L350
	movq	%rbx, %rsi
	movq	%r12, %rdi
	call	neo_to_string@PLT
	movsbl	(%rax,%r15), %edi
	call	charToString@PLT
	movq	%rax, %rdi
	call	neo_str_create@PLT
	movl	%edx, %ecx
	movb	%cl, %dl
	jmp	.L417
.L286:
	testb	%sil, %sil
	je	.L466
	testl	%ecx, %ecx
	jg	.L287
	xorl	%eax, %eax
	movl	%eax, 44(%rsp)
.L363:
	movl	24(%r12), %eax
	movl	%r8d, %edx
	movl	44(%rsp), %esi
	subl	%eax, %edx
	addl	%ecx, %eax
	subl	%esi, %eax
	cmpl	%edx, %ecx
	cmovg	%eax, %r8d
	jmp	.L295
.L463:
	movq	%r15, %r14
	movq	%r9, %r15
.L302:
	movl	16(%r15), %r13d
	xorl	%ebx, %ebx
	testl	%r13d, %r13d
	jg	.L317
	jmp	.L467
	.p2align 4,,10
	.p2align 3
.L320:
	addq	$1, %rbx
	cmpl	%ebx, %eax
	jle	.L468
.L317:
	movq	%rbx, %rax
	movl	%ebx, %r13d
	salq	$4, %rax
	addq	8(%r15), %rax
	movq	(%rax), %rdi
	movq	8(%rax), %rsi
	call	neo_is_void@PLT
	testb	%al, %al
	movl	16(%r15), %eax
	je	.L320
.L319:
	movq	24(%r14), %rdx
	movq	8(%rdx), %rcx
	movl	32(%rcx), %edx
	cmpl	%eax, %ebx
	jge	.L318
	movq	%r15, %rdi
	xorl	%esi, %esi
	movq	%r12, 48(%rsp)
	movl	%ebx, %r15d
	movq	%r14, %r12
	movq	16(%rsp), %r10
	movq	%rsi, %r14
	movq	%rdi, %rbx
	jmp	.L321
	.p2align 4,,10
	.p2align 3
.L323:
	addq	$1, %r14
	cmpl	%eax, %r15d
	jge	.L428
.L321:
	cmpl	%r14d, %edx
	jle	.L428
	movq	24(%rcx), %rdi
	leaq	0(,%r14,8), %rsi
	movq	%rsi, 32(%rsp)
	movq	(%rdi,%r14,8), %rdi
	cmpb	$2, (%rdi)
	jne	.L322
	cmpl	$37, 16(%rdi)
	je	.L323
.L322:
	pxor	%xmm1, %xmm1
	movq	%r10, 16(%rsp)
	movslq	%r15d, %r13
	movups	%xmm1, 40(%r12)
	salq	$4, %r13
	movb	$54, 48(%r12)
	call	eval_aux
	movq	8(%rbx), %rcx
	movq	16(%rsp), %r10
	movb	(%rsp), %r10b
	movq	%rax, (%rcx,%r13)
	movq	24(%rsp), %rax
	movq	%rdx, 8(%rcx,%r13)
	movq	%rax, 40(%r12)
	movq	global_env(%rip), %rax
	movq	%r10, 48(%r12)
	movl	(%rax), %r11d
	testl	%r11d, %r11d
	jne	.L469
	cmpb	$-79, (%rsp)
	jne	.L325
	testl	%r15d, %r15d
	je	.L470
.L325:
	cmpl	16(%rbx), %r15d
	jge	.L425
	movq	%r14, 16(%rsp)
	movq	%rbx, %r14
	movl	%r15d, %ebx
	movq	%r10, %r15
	jmp	.L326
	.p2align 4,,10
	.p2align 3
.L328:
	addl	$1, %ebx
	addq	$16, %r13
	cmpl	%ebx, 16(%r14)
	jle	.L471
.L326:
	movq	8(%r14), %rax
	movq	(%rax,%r13), %rdi
	movq	8(%rax,%r13), %rsi
	call	neo_is_void@PLT
	testb	%al, %al
	je	.L328
	movq	24(%r12), %rdx
	movq	%r15, %r10
	movl	%ebx, %r15d
	movq	%r14, %rbx
	movl	16(%rbx), %eax
	movq	16(%rsp), %r14
	movq	8(%rdx), %rcx
	movl	32(%rcx), %edx
	jmp	.L323
.L471:
	movq	%r15, 16(%rsp)
	movq	%r14, %r15
	movq	%r12, %r14
	movq	48(%rsp), %r12
.L327:
	movq	24(%r14), %rax
	movq	8(%rax), %rax
	movl	32(%rax), %edx
.L318:
	movl	16(%r12), %eax
	cmpl	%edx, %eax
	jg	.L472
.L330:
	xorl	%ecx, %ecx
	cmpb	$0, 20(%r12)
	movl	$54, %r8d
	je	.L334
	movl	16(%r15), %ebx
	movl	44(%rsp), %esi
	movl	%eax, %edx
	subl	24(%r12), %edx
	movl	%ebx, %ecx
	subl	%esi, %ecx
	cmpl	%edx, %ecx
	jg	.L335
	xorl	%edi, %edi
	call	nelist_create@PLT
.L336:
	movq	%rax, %rdi
	call	neo_list_convert@PLT
	movq	%rax, %rcx
	movq	%rdx, %r8
.L334:
	pxor	%xmm0, %xmm0
	cmpb	$-79, (%rsp)
	movups	%xmm0, 40(%r14)
	movb	$54, 48(%r14)
	je	.L473
	movq	%rcx, %rdx
	movq	%r12, %rdi
	movq	%r8, %rcx
	movq	%r15, %rsi
	call	callUserFunc
	movq	%rax, %r12
	movq	%rdx, %rbx
.L340:
	movq	24(%rsp), %rdi
	movq	16(%rsp), %rax
	movb	(%rsp), %al
	movq	%rdi, 40(%r14)
	movq	%rax, 48(%r14)
	movq	%rax, %rsi
	call	neobject_destroy@PLT
	pxor	%xmm0, %xmm0
	movq	%r15, %rdi
	movups	%xmm0, 40(%r14)
	movb	$54, 48(%r14)
	call	nelist_destroy@PLT
	movq	global_env(%rip), %rdx
	xorl	%eax, %eax
	movl	(%rdx), %r9d
	movl	$54, %edx
	testl	%r9d, %r9d
	cmove	%r12, %rax
	cmove	%rbx, %rdx
	jmp	.L417
.L475:
	movq	40(%r12), %rdi
	movl	%ebx, %esi
	call	nelist_nth@PLT
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	neo_copy@PLT
	movq	8(%r15), %rcx
	movq	%rax, (%rcx,%r13)
	movq	8(%r15), %rax
	movq	%rdx, 8(%rcx,%r13)
	movq	(%rax,%r13), %rdi
	movq	8(%rax,%r13), %rsi
	call	neo_is_void@PLT
	testb	%al, %al
	jne	.L474
	testq	%rbx, %rbx
	jne	.L331
.L441:
	movq	global_env(%rip), %rax
	movq	%r15, %rdi
	movl	$110, (%rax)
	call	nelist_destroy@PLT
	jmp	.L429
.L458:
	movq	8(%r14), %rsi
	movq	%rax, %rdi
	call	get_field_index@PLT
	movq	8(%r14), %rdi
	movl	%eax, 16(%r14)
	call	free@PLT
	movq	$0, 8(%r14)
	movl	16(%r14), %esi
	jmp	.L360
.L459:
	movq	%r12, %r13
	leal	-1(%rbx), %esi
	movq	16(%rsp), %r12
	movq	%r13, %rdi
	xorl	%r13d, %r13d
	call	nelist_destroy_until@PLT
	jmp	.L279
.L423:
	movq	%r9, %rdi
	call	nelist_destroy@PLT
	movq	40(%r15), %rdi
	movq	48(%r15), %rsi
	call	neobject_destroy@PLT
	movq	global_env(%rip), %rax
	pxor	%xmm0, %xmm0
	movl	$54, %edx
	movups	%xmm0, 40(%r15)
	movb	$54, 48(%r15)
	movl	$93, (%rax)
	xorl	%eax, %eax
	jmp	.L417
.L443:
	movq	24(%r14), %rax
	movslq	16(%r14), %rdx
	movq	8(%rax), %rsi
	movq	(%rax), %rdi
	leaq	operators_functions(%rip), %rax
	movq	(%rax,%rdx,8), %rax
.L439:
	addq	$64, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 48
	popq	%rbx
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
.L462:
	.cfi_restore_state
	movq	global_env(%rip), %rax
	movq	%r13, %rdi
	movl	$14, (%rax)
	call	nelist_destroy@PLT
	jmp	.L429
.L350:
	movq	global_env(%rip), %rax
	movl	$18, (%rax)
	jmp	.L438
.L468:
	leal	1(%r13), %ebx
	jmp	.L319
.L428:
	movq	%r12, %r14
	movq	%r10, 16(%rsp)
	movq	48(%rsp), %r12
	movq	%rbx, %r15
	jmp	.L318
.L241:
	movq	%rbx, %rdi
	xorl	%esi, %esi
	call	create_new_process@PLT
	addq	$64, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 48
	popq	%rbx
	.cfi_def_cfa_offset 40
	movl	%eax, %edi
	popq	%r12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
	jmp	neo_promise_create@PLT
.L435:
	.cfi_restore_state
	movq	%r12, %rdi
	movq	%rbx, %rsi
	call	neobject_destroy@PLT
	xorl	%eax, %eax
	movl	$54, %edx
	jmp	.L417
.L445:
	movq	global_env(%rip), %rax
	movl	$54, %edx
	movl	$92, (%rax)
	xorl	%eax, %eax
	jmp	.L417
.L254:
	movl	16(%r14), %edi
	movl	$16, %esi
	call	operatorIs@PLT
	testb	%al, %al
	je	.L258
	movq	24(%r14), %rax
	movq	(%rax), %rdi
	call	eval_aux
	movq	%rax, %rbx
	movq	global_env(%rip), %rax
	movq	%rdx, %r13
	cmpl	$0, (%rax)
	jne	.L438
	movq	24(%r14), %rax
	movq	8(%rax), %rdi
	call	get_address
	movq	%rax, %rdx
	movq	global_env(%rip), %rax
	cmpl	$0, (%rax)
	jne	.L438
	movslq	16(%r14), %rcx
	movq	%rbx, %r15
	movq	%rbx, %rdi
	movq	%r13, %rsi
	leaq	operators_functions(%rip), %rax
	call	*(%rax,%rcx,8)
	jmp	.L434
.L470:
	movq	24(%r12), %rax
	movq	32(%rsp), %rsi
	movq	%r10, 16(%rsp)
	movq	8(%rax), %rax
	movq	24(%rax), %rax
	movq	(%rax,%rsi), %rdi
	call	get_address
	movq	16(%rsp), %r10
	movq	%rax, 56(%rsp)
	jmp	.L325
.L348:
	movq	%rbx, %rsi
	movq	%r12, %rdi
	call	neo_list_len@PLT
	movq	(%rsp), %rsi
	cltq
	cmpq	%rsi, %rax
	jle	.L350
	testq	%rsi, %rsi
	js	.L350
	movl	(%rsp), %edx
	movb	$-122, %bl
	movq	%r12, %rdi
	movq	%rbx, %rsi
	call	neo_list_nth@PLT
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	neo_copy@PLT
	movl	%edx, %ecx
	movb	%cl, %dl
	jmp	.L417
.L472:
	testl	%eax, %eax
	jle	.L330
	xorl	%ebx, %ebx
.L333:
	movq	8(%r15), %rax
	movq	%rbx, %r13
	salq	$4, %r13
	movq	(%rax,%r13), %rdi
	movq	8(%rax,%r13), %rsi
	call	neo_is_void@PLT
	testb	%al, %al
	jne	.L475
.L331:
	movl	16(%r12), %eax
	addq	$1, %rbx
	cmpl	%ebx, %eax
	jg	.L333
	jmp	.L330
	.p2align 4,,10
	.p2align 3
.L465:
	movq	24(%r14), %rax
	movq	(%rax), %rdi
	call	eval_aux
	movq	%rax, %rcx
	movq	global_env(%rip), %rax
	movq	%rdx, %r13
	movl	(%rax), %r15d
	testl	%r15d, %r15d
	jne	.L438
	movslq	16(%r14), %rdx
	movq	%rcx, %r15
	movq	%rcx, %rdi
	movq	%r13, %rsi
	leaq	operators_functions(%rip), %rax
	call	*(%rax,%rdx,8)
	jmp	.L434
.L466:
	movq	global_env(%rip), %rax
	movq	24(%rsp), %rdi
	movq	16(%rsp), %rsi
	movl	$6, (%rax)
	jmp	.L437
.L444:
	movq	global_env(%rip), %rax
	movl	$54, %edx
	movl	$95, (%rax)
	xorl	%eax, %eax
	jmp	.L417
.L473:
	movq	56(%rsp), %rsi
	movq	%r12, %rdi
	movq	%r15, %rdx
	call	callUserMethod
	movq	%rax, %r12
	movq	%rdx, %rbx
	jmp	.L340
.L469:
	movq	%rbx, %rdi
	call	nelist_destroy@PLT
	movq	40(%r12), %rdi
	movq	48(%r12), %rsi
	call	neobject_destroy@PLT
	pxor	%xmm6, %xmm6
	xorl	%eax, %eax
	movl	$54, %edx
	movups	%xmm6, 40(%r12)
	movb	$54, 48(%r12)
	jmp	.L417
.L335:
	subl	%eax, %ebx
	movl	%ebx, %edi
	call	nelist_create@PLT
	testl	%ebx, %ebx
	jle	.L336
	movslq	%ebx, %r13
	xorl	%ebx, %ebx
	movq	%r14, 32(%rsp)
	movq	%rbx, %r14
	movq	%rax, %rbx
.L338:
	movl	16(%r12), %esi
	movq	%r15, %rdi
	addl	%r14d, %esi
	call	nelist_nth@PLT
	movq	%rdx, %rdi
	movq	%r14, %rdx
	movq	%rax, %rsi
	addq	$1, %r14
	salq	$4, %rdx
	addq	8(%rbx), %rdx
	movq	%rsi, (%rdx)
	movq	%rdi, 8(%rdx)
	cmpq	%r13, %r14
	jne	.L338
	movq	32(%rsp), %r14
	movq	%rbx, %rax
	jmp	.L336
.L258:
	movl	16(%r14), %edi
	movl	$64, %esi
	call	operatorIs@PLT
	testb	%al, %al
	movq	24(%r14), %rax
	movq	(%rax), %rdi
	je	.L262
	call	get_address
	movq	%rax, %rbx
	movq	global_env(%rip), %rax
	cmpl	$0, (%rax)
	jne	.L438
	movq	24(%r14), %rax
	movq	8(%rax), %rdi
	call	get_address
	movq	%rax, %rsi
	movq	global_env(%rip), %rax
	cmpl	$0, (%rax)
	jne	.L438
	movslq	16(%r14), %rdx
	leaq	operators_functions(%rip), %rax
	movq	%rbx, %rdi
	movq	(%rax,%rdx,8), %rax
	jmp	.L439
.L425:
	movq	%r12, %r14
	movq	%r10, 16(%rsp)
	movq	48(%rsp), %r12
	movq	%rbx, %r15
	jmp	.L327
.L315:
	movq	24(%rdx), %rax
	movq	%r9, 32(%rsp)
	movq	(%rax,%r14), %rax
	movq	24(%rax), %rax
	movq	8(%rax), %rdi
	call	get_address
	movq	24(%r15), %rsi
	movq	32(%rsp), %r9
	movq	%rax, 56(%rsp)
	movq	8(%rsi), %rdx
	movl	32(%rdx), %ecx
	jmp	.L305
.L365:
	xorl	%r13d, %r13d
	jmp	.L307
.L467:
	movq	8(%rsi), %rax
	movl	32(%rax), %edx
	jmp	.L318
.L262:
	call	eval_aux
	movq	%rax, %r12
	movq	global_env(%rip), %rax
	movq	%rdx, %rbx
	cmpl	$0, (%rax)
	jne	.L435
	movq	24(%r14), %rax
	movq	8(%rax), %rdi
	call	eval_aux
	movq	%r12, %rdi
	movq	%rbx, %rsi
	movq	%rax, %r15
	movq	global_env(%rip), %rax
	movq	%rdx, %r13
	cmpl	$0, (%rax)
	jne	.L476
	movslq	16(%r14), %r8
	movq	%r12, 16(%rsp)
	movq	%r15, %rdx
	movq	%r13, %rcx
	leaq	operators_functions(%rip), %rax
	movq	%rbx, (%rsp)
	call	*(%rax,%r8,8)
	movq	16(%rsp), %rdi
	movq	(%rsp), %rsi
	movq	%rax, %r12
	movq	%rdx, %rbx
	call	neobject_destroy@PLT
	jmp	.L436
.L464:
	movq	%r9, %rdi
	call	nelist_destroy@PLT
	movq	40(%r15), %rdi
	movq	48(%r15), %rsi
	call	neobject_destroy@PLT
	pxor	%xmm5, %xmm5
	xorl	%eax, %eax
	movl	$54, %edx
	movups	%xmm5, 40(%r15)
	movb	$54, 48(%r15)
	jmp	.L417
.L313:
	movq	%r9, %rdi
	call	nelist_destroy@PLT
	movq	40(%r15), %rdi
	movq	48(%r15), %rsi
	call	neobject_destroy@PLT
	movq	global_env(%rip), %rax
	pxor	%xmm0, %xmm0
	movl	$54, %edx
	movups	%xmm0, 40(%r15)
	movb	$54, 48(%r15)
	movl	$94, (%rax)
	xorl	%eax, %eax
	jmp	.L417
.L476:
	call	neobject_destroy@PLT
	movq	%r15, %rdi
	movq	%r13, %rsi
	call	neobject_destroy@PLT
	xorl	%eax, %eax
	movl	$54, %edx
	jmp	.L417
.L474:
	movq	global_env(%rip), %rax
	movq	40(%r14), %rdi
	movq	48(%r14), %rsi
	movl	$7, (%rax)
	call	neobject_destroy@PLT
	pxor	%xmm0, %xmm0
	movq	%r15, %rdi
	movups	%xmm0, 40(%r14)
	movb	$54, 48(%r14)
	call	nelist_destroy@PLT
	xorl	%eax, %eax
	movl	$54, %edx
	jmp	.L417
	.cfi_endproc
.LFE23:
	.size	eval_aux, .-eval_aux
	.p2align 4
	.globl	isTrue
	.type	isTrue, @function
isTrue:
.LFB14:
	.cfi_startproc
	pushq	%r12
	.cfi_def_cfa_offset 16
	.cfi_offset 12, -16
	xorl	%r12d, %r12d
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
	je	.L484
	addq	$24, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	movl	%r12d, %eax
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%r12
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L484:
	.cfi_restore_state
	movq	%rax, %rdi
	movq	%rbx, %rsi
	cmpb	$8, %bl
	je	.L485
	cmpb	$4, %bl
	je	.L486
	movl	$20, (%rdx)
.L480:
	movq	%rbx, %rsi
	call	neobject_destroy@PLT
	addq	$24, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	movl	%r12d, %eax
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%r12
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L485:
	.cfi_restore_state
	movb	$8, %sil
	movq	%rax, 8(%rsp)
	call	neo_to_bool@PLT
	movq	8(%rsp), %rdi
	movl	%eax, %r12d
	jmp	.L480
	.p2align 4,,10
	.p2align 3
.L486:
	movb	$4, %sil
	movq	%rax, 8(%rsp)
	call	neo_to_integer@PLT
	movq	8(%rsp), %rdi
	testq	%rax, %rax
	setne	%r12b
	jmp	.L480
	.cfi_endproc
.LFE14:
	.size	isTrue, .-isTrue
	.p2align 4
	.globl	treeToList
	.type	treeToList, @function
treeToList:
.LFB20:
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
	pushq	%rbx
	.cfi_def_cfa_offset 40
	.cfi_offset 3, -40
	subq	$8, %rsp
	.cfi_def_cfa_offset 48
	movl	32(%rdi), %edi
	call	nelist_create@PLT
	movl	32(%r13), %edx
	movq	%rax, %r14
	testl	%edx, %edx
	jle	.L487
	xorl	%ebx, %ebx
	jmp	.L492
	.p2align 4,,10
	.p2align 3
.L496:
	call	eval_aux
	movq	%rax, (%r12)
	movq	global_env(%rip), %rax
	movq	%rdx, 8(%r12)
	movl	(%rax), %eax
	testl	%eax, %eax
	jne	.L495
	addq	$1, %rbx
	cmpl	%ebx, 32(%r13)
	jle	.L487
.L492:
	movq	24(%r13), %rax
	movq	%rbx, %r12
	salq	$4, %r12
	addq	8(%r14), %r12
	movq	(%rax,%rbx,8), %rdi
	testq	%rdi, %rdi
	jne	.L496
	pxor	%xmm0, %xmm0
	addq	$1, %rbx
	movups	%xmm0, (%r12)
	movb	$54, 8(%r12)
	cmpl	%ebx, 32(%r13)
	jg	.L492
.L487:
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 40
	movq	%r14, %rax
	popq	%rbx
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
.L495:
	.cfi_restore_state
	leal	-1(%rbx), %esi
	movq	%r14, %rdi
	xorl	%r14d, %r14d
	call	nelist_destroy_until@PLT
	addq	$8, %rsp
	.cfi_def_cfa_offset 40
	movq	%r14, %rax
	popq	%rbx
	.cfi_def_cfa_offset 32
	popq	%r12
	.cfi_def_cfa_offset 24
	popq	%r13
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE20:
	.size	treeToList, .-treeToList
	.p2align 4
	.globl	get_address
	.type	get_address, @function
get_address:
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
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	movq	%rdi, %rbx
	subq	$16, %rsp
	.cfi_def_cfa_offset 64
	movzbl	(%rdi), %eax
	cmpb	$10, %al
	je	.L525
	cmpb	$1, %al
	je	.L526
	cmpb	$52, %al
	je	.L527
	movq	global_env(%rip), %rax
	movl	$89, (%rax)
.L524:
	xorl	%eax, %eax
.L497:
	addq	$16, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 48
	popq	%rbx
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
.L527:
	.cfi_restore_state
	movq	24(%rdi), %rax
	movq	(%rax), %rdi
	call	eval_aux
	movq	%rax, %r13
	movq	global_env(%rip), %rax
	movq	%rdx, %r12
	movl	(%rax), %eax
	testl	%eax, %eax
	jne	.L524
	movq	%r13, %rdi
	movq	%rdx, %rsi
	cmpb	$-77, %dl
	je	.L513
	call	neobject_destroy@PLT
	movq	global_env(%rip), %rax
	movl	$80, (%rax)
	jmp	.L524
	.p2align 4,,10
	.p2align 3
.L526:
	movq	24(%rdi), %rax
	movq	(%rax), %rdi
	call	get_address
	movq	(%rax), %r13
	movq	8(%rax), %r12
	movzbl	8(%rax), %r15d
	movq	global_env(%rip), %rax
	movl	(%rax), %ecx
	testl	%ecx, %ecx
	jne	.L524
	movq	24(%rbx), %rax
	movq	8(%rax), %rdi
	call	eval_aux
	movq	%rax, %r14
	movq	global_env(%rip), %rax
	movq	%rdx, %rbx
	movl	(%rax), %edx
	testl	%edx, %edx
	jne	.L524
	cmpb	$-122, %r15b
	je	.L503
	cmpb	$-125, %r15b
	je	.L503
	movl	$15, (%rax)
	jmp	.L524
	.p2align 4,,10
	.p2align 3
.L525:
	movl	56(%rdi), %edi
	addq	$16, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 48
	popq	%rbx
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
.L513:
	.cfi_restore_state
	call	neo_to_container@PLT
	movl	16(%rbx), %esi
	movq	%rax, %r14
	cmpl	$-1, %esi
	je	.L528
.L515:
	movq	%r14, %rdi
	call	get_container_field_addr@PLT
	movq	%r13, %rdi
	movq	%r12, %rsi
	movq	%rax, 8(%rsp)
	call	neobject_destroy@PLT
	movq	8(%rsp), %rax
	jmp	.L497
	.p2align 4,,10
	.p2align 3
.L503:
	movq	%r14, %rdi
	movq	%rbx, %rsi
	cmpb	$4, %bl
	je	.L504
	call	neobject_destroy@PLT
	movq	global_env(%rip), %rax
	movl	$16, (%rax)
	jmp	.L524
	.p2align 4,,10
	.p2align 3
.L528:
	movq	8(%rbx), %rsi
	movq	%rax, %rdi
	call	get_field_index@PLT
	movq	8(%rbx), %rdi
	movl	%eax, 16(%rbx)
	call	free@PLT
	movq	$0, 8(%rbx)
	movl	16(%rbx), %esi
	jmp	.L515
	.p2align 4,,10
	.p2align 3
.L504:
	call	neo_to_integer@PLT
	movq	%r14, %rdi
	movq	%rbx, %rsi
	movq	%rax, 8(%rsp)
	call	neobject_destroy@PLT
	cmpb	$-122, %r15b
	je	.L505
	cmpq	$0, 8(%rsp)
	js	.L507
	movb	$-125, %r12b
	movq	%r13, %rdi
	movq	%r12, %rsi
	call	neo_to_string@PLT
	movq	%rax, %rdi
	call	strlen@PLT
	cmpq	%rax, 8(%rsp)
	jnb	.L507
	movq	global_env(%rip), %rax
	movl	$105, (%rax)
	jmp	.L524
.L507:
	movq	global_env(%rip), %rax
	movl	$18, (%rax)
	jmp	.L524
.L505:
	movq	%r13, %rdi
	movq	%r12, %rsi
	call	neo_list_len@PLT
	movq	8(%rsp), %rcx
	testq	%rcx, %rcx
	js	.L507
	cltq
	cmpq	%rcx, %rax
	jle	.L507
	movb	$-122, %r12b
	movq	%r13, %rdi
	movq	%r12, %rsi
	call	neo_to_list@PLT
	movl	8(%rsp), %esi
	addq	$16, %rsp
	.cfi_def_cfa_offset 48
	popq	%rbx
	.cfi_def_cfa_offset 40
	movq	%rax, %rdi
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
.LFE24:
	.size	get_address, .-get_address
	.p2align 4
	.globl	execConditionBlock
	.type	execConditionBlock, @function
execConditionBlock:
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
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	subq	$32, %rsp
	.cfi_def_cfa_offset 80
	movq	global_env(%rip), %rax
	movl	32(%r13), %ebx
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	movq	(%rdi), %rax
	movq	%rax, 24(%rsp)
	testl	%ebx, %ebx
	jle	.L530
	xorl	%ebx, %ebx
.L556:
	movq	24(%r13), %rax
	movslq	%ebx, %r14
	leaq	0(,%r14,8), %r12
	movq	(%rax,%r14,8), %rax
	movq	24(%rax), %rax
	movq	(%rax), %rdi
	call	eval_aux
	movq	%rax, %rdi
	movq	global_env(%rip), %rax
	movl	(%rax), %r11d
	testl	%r11d, %r11d
	jne	.L532
	movq	%rdx, %rsi
	cmpb	$8, %dl
	je	.L569
	cmpb	$4, %dl
	je	.L570
	movl	$20, (%rax)
	call	neobject_destroy@PLT
	movq	global_env(%rip), %rsi
	movl	(%rsi), %r10d
	testl	%r10d, %r10d
	jne	.L532
.L537:
	movl	32(%r13), %edx
	addl	$1, %ebx
	cmpl	%edx, %ebx
	jge	.L544
	addq	$8, %r12
	xorl	%r15d, %r15d
	jmp	.L545
	.p2align 4,,10
	.p2align 3
.L549:
	movl	32(%r13), %edx
	addl	$1, %ebx
	addq	$8, %r12
	cmpl	%ebx, %edx
	jle	.L543
.L545:
	movq	24(%r13), %rcx
	movq	(%rcx,%r12), %rax
	cmpb	$31, (%rax)
	jne	.L571
	movq	24(%rax), %rax
	movq	(%rax), %rdi
	call	eval_aux
	movq	%rax, %rdi
	movq	%rdx, %rsi
	cmpb	$8, %dl
	je	.L572
	cmpb	$4, %dl
	je	.L573
	movq	global_env(%rip), %rax
	movl	$20, (%rax)
	call	neobject_destroy@PLT
	movq	global_env(%rip), %rsi
	movl	(%rsi), %edi
	testl	%edi, %edi
	je	.L549
.L532:
	xorl	%r15d, %r15d
.L558:
	addq	$32, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 48
	movl	%r15d, %eax
	popq	%rbx
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
.L572:
	.cfi_restore_state
	movb	$8, %sil
	movq	%rdx, 16(%rsp)
	movq	%rax, 8(%rsp)
	call	neo_to_bool@PLT
	movq	8(%rsp), %rdi
	movq	16(%rsp), %rdx
	movl	%eax, %r14d
.L547:
	movq	%rdx, %rsi
	call	neobject_destroy@PLT
	movq	global_env(%rip), %rsi
	movl	(%rsi), %ecx
	testl	%ecx, %ecx
	jne	.L532
	movl	%r15d, %eax
	xorl	$1, %eax
	andb	%al, %r14b
	je	.L549
	movq	168(%rsi), %rax
	movq	(%rax), %rax
	movq	80(%rax), %r15
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%r15, %rdi
	call	ptrlist_append@PLT
	movq	24(%r13), %rax
	movq	(%rax,%r12), %rax
	movq	24(%rax), %rax
	movq	8(%rax), %rdi
	call	exec_aux
	movl	%eax, %r15d
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	80(%rax), %rdi
	call	deleteContext
	movq	global_env(%rip), %rsi
	movl	(%rsi), %edx
	testl	%edx, %edx
	jne	.L532
	testl	%r15d, %r15d
	jne	.L558
	movl	32(%r13), %edx
	addl	$1, %ebx
	movl	%r14d, %r15d
	addq	$8, %r12
	cmpl	%ebx, %edx
	jg	.L545
	.p2align 4,,10
	.p2align 3
.L543:
	cmpl	%edx, %ebx
	jl	.L556
	.p2align 4,,10
	.p2align 3
.L544:
	movq	168(%rsi), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
.L530:
	movq	24(%rsp), %rsi
	xorl	%r15d, %r15d
	call	partialRestore@PLT
	jmp	.L558
	.p2align 4,,10
	.p2align 3
.L573:
	movb	$4, %sil
	movq	%rdx, 16(%rsp)
	movq	%rax, 8(%rsp)
	call	neo_to_integer@PLT
	movq	8(%rsp), %rdi
	movq	16(%rsp), %rdx
	testq	%rax, %rax
	setne	%r14b
	jmp	.L547
	.p2align 4,,10
	.p2align 3
.L571:
	cmpl	%ebx, %edx
	jle	.L543
	movslq	%ebx, %r12
	movq	(%rcx,%r12,8), %rax
	cmpb	$32, (%rax)
	jne	.L543
	testb	%r15b, %r15b
	je	.L554
.L555:
	addl	$1, %ebx
	cmpl	%edx, %ebx
	jl	.L556
	jmp	.L544
	.p2align 4,,10
	.p2align 3
.L569:
	movb	$8, %sil
	movq	%rdx, 16(%rsp)
	movq	%rdi, 8(%rsp)
	call	neo_to_bool@PLT
	movq	8(%rsp), %rdi
	movq	16(%rsp), %rdx
	movl	%eax, %r15d
.L534:
	movq	%rdx, %rsi
	call	neobject_destroy@PLT
	movq	global_env(%rip), %rsi
	movl	(%rsi), %r9d
	testl	%r9d, %r9d
	jne	.L532
	testb	%r15b, %r15b
	je	.L537
	movq	168(%rsi), %rax
	movq	(%rax), %rax
	movq	80(%rax), %r12
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%r12, %rdi
	call	ptrlist_append@PLT
	movq	24(%r13), %rax
	movq	(%rax,%r14,8), %rax
	movq	24(%rax), %rax
	movq	8(%rax), %rdi
	call	exec_aux
	movl	%eax, %r15d
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	80(%rax), %rdi
	call	deleteContext
	movq	global_env(%rip), %rsi
	movl	(%rsi), %r8d
	testl	%r8d, %r8d
	jne	.L532
	testl	%r15d, %r15d
	jne	.L558
	movl	32(%r13), %edx
	addl	$1, %ebx
	cmpl	%ebx, %edx
	jle	.L544
	movq	24(%r13), %rcx
	movslq	%ebx, %rbx
	jmp	.L540
	.p2align 4,,10
	.p2align 3
.L542:
	addq	$1, %rbx
	cmpl	%ebx, %edx
	jle	.L574
.L540:
	movq	(%rcx,%rbx,8), %rax
	movl	%ebx, %edi
	cmpb	$30, (%rax)
	jne	.L542
	cmpl	%ebx, %edx
	je	.L544
.L568:
	cmpl	%edx, %ebx
	jl	.L556
	jmp	.L544
	.p2align 4,,10
	.p2align 3
.L574:
	leal	1(%rdi), %ebx
	cmpl	%ebx, %edx
	jne	.L568
	jmp	.L544
	.p2align 4,,10
	.p2align 3
.L570:
	movb	$4, %sil
	movq	%rdx, 16(%rsp)
	movq	%rdi, 8(%rsp)
	call	neo_to_integer@PLT
	movq	8(%rsp), %rdi
	movq	16(%rsp), %rdx
	testq	%rax, %rax
	setne	%r15b
	jmp	.L534
.L554:
	movq	168(%rsi), %rax
	movq	(%rax), %rax
	movq	80(%rax), %r14
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%r14, %rdi
	call	ptrlist_append@PLT
	movq	24(%r13), %rax
	movq	(%rax,%r12,8), %rdi
	call	exec_aux
	movl	%eax, %r15d
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	80(%rax), %rdi
	call	deleteContext
	movq	global_env(%rip), %rsi
	movl	(%rsi), %eax
	testl	%eax, %eax
	jne	.L532
	testl	%r15d, %r15d
	jne	.L558
	movl	32(%r13), %edx
	jmp	.L555
	.cfi_endproc
.LFE25:
	.size	execConditionBlock, .-execConditionBlock
	.p2align 4
	.globl	execStatementFor
	.type	execStatementFor, @function
execStatementFor:
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
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	subq	$48, %rsp
	.cfi_def_cfa_offset 96
	movq	24(%rdi), %rax
	movq	(%rax), %rdx
	movl	32(%rdx), %eax
	cmpl	$4, %eax
	je	.L606
	cmpl	$3, %eax
	je	.L607
	cmpl	$2, %eax
	je	.L608
	movq	global_env(%rip), %rax
	movl	$108, (%rax)
.L581:
	xorl	%r15d, %r15d
.L603:
	addq	$48, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 48
	movl	%r15d, %eax
	popq	%rbx
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
.L608:
	.cfi_restore_state
	xorl	%edi, %edi
	call	neo_integer_create@PLT
	movq	%rax, %r14
	movq	global_env(%rip), %rax
	movq	%rdx, %r13
	movl	(%rax), %ecx
	testl	%ecx, %ecx
	jne	.L581
	movq	24(%r12), %rax
	movq	(%rax), %rax
	movq	24(%rax), %rax
	movq	8(%rax), %rdi
	call	eval_aux
	movq	%rax, %rbx
	movq	%rdx, %rcx
.L585:
	movl	$1, (%rsp)
.L580:
	movq	global_env(%rip), %rax
	movl	(%rax), %r15d
	testl	%r15d, %r15d
	jne	.L609
	cmpb	$4, %r13b
	jne	.L589
	cmpb	$4, %cl
	je	.L590
.L589:
	movq	%r14, %rdi
	movq	%r13, %rsi
	movq	%rcx, (%rsp)
	call	neobject_destroy@PLT
	movq	(%rsp), %rsi
	movq	%rbx, %rdi
	call	neobject_destroy@PLT
	movq	global_env(%rip), %rax
	movl	$10, (%rax)
	jmp	.L581
	.p2align 4,,10
	.p2align 3
.L606:
	movq	24(%rdx), %rax
	movq	24(%rax), %rdi
	call	eval_aux
	movq	%rax, %r13
	movq	global_env(%rip), %rax
	movq	%rdx, %rbx
	movl	(%rax), %r8d
	testl	%r8d, %r8d
	jne	.L581
	movq	%r13, %rdi
	movq	%rdx, %rsi
	cmpb	$4, %dl
	je	.L578
	call	neobject_destroy@PLT
	movq	global_env(%rip), %rax
	movl	$108, (%rax)
	jmp	.L581
	.p2align 4,,10
	.p2align 3
.L607:
	movq	24(%rdx), %rax
	movq	8(%rax), %rdi
	call	eval_aux
	movq	%rax, %r14
	movq	global_env(%rip), %rax
	movq	%rdx, %r13
	movl	(%rax), %esi
	testl	%esi, %esi
	jne	.L581
	movq	24(%r12), %rax
	movq	(%rax), %rax
	movq	24(%rax), %rax
	movq	16(%rax), %rdi
	call	eval_aux
	movq	%rax, %rbx
	movq	%rdx, %rcx
	jmp	.L585
	.p2align 4,,10
	.p2align 3
.L578:
	call	neo_to_integer@PLT
	movq	%r13, %rdi
	movq	%rbx, %rsi
	movq	%rax, %r15
	call	neobject_destroy@PLT
	movq	24(%r12), %rax
	movq	(%rax), %rax
	movq	24(%rax), %rax
	movq	8(%rax), %rdi
	call	eval_aux
	movq	%rax, %r14
	movq	global_env(%rip), %rax
	movq	%rdx, %r13
	movl	(%rax), %edi
	testl	%edi, %edi
	jne	.L581
	movq	24(%r12), %rax
	movl	%r15d, (%rsp)
	movq	(%rax), %rax
	movq	24(%rax), %rax
	movq	16(%rax), %rdi
	call	eval_aux
	movq	%rax, %rbx
	movq	%rdx, %rcx
	jmp	.L580
	.p2align 4,,10
	.p2align 3
.L609:
	movq	%r14, %rdi
	movq	%r13, %rsi
	call	neobject_destroy@PLT
	jmp	.L581
	.p2align 4,,10
	.p2align 3
.L590:
	movq	%r14, %rdi
	movq	%r13, %rsi
	movq	%rcx, 8(%rsp)
	call	neo_to_integer@PLT
	movq	%r14, %rdi
	movq	%r13, %rsi
	movq	%rax, 16(%rsp)
	call	neobject_destroy@PLT
	movq	24(%r12), %rax
	movq	8(%rsp), %rcx
	movq	(%rax), %rax
	movq	24(%rax), %rax
	movq	(%rax), %rax
	cmpb	$10, (%rax)
	je	.L591
	movq	global_env(%rip), %rax
	movq	%rbx, %rdi
	movq	%rcx, %rsi
	movl	$111, (%rax)
	call	neobject_destroy@PLT
	jmp	.L581
	.p2align 4,,10
	.p2align 3
.L591:
	movl	56(%rax), %eax
	movl	16(%rsp), %r13d
	movq	%rcx, 40(%rsp)
	movl	%eax, 8(%rsp)
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	80(%rax), %r14
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%r14, %rdi
	call	ptrlist_append@PLT
	movq	global_env(%rip), %rax
	movl	8(%rsp), %esi
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	movq	(%rdi), %rcx
	movq	%rcx, 24(%rsp)
	call	save_later@PLT
	movq	global_env(%rip), %rax
	movl	$24, %edi
	movq	168(%rax), %rax
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
	movq	%rbx, %rdi
	call	neo_to_integer@PLT
	movq	40(%rsp), %rsi
	movq	%rbx, %rdi
	movq	%rax, 32(%rsp)
	movl	%eax, %r14d
	call	neobject_destroy@PLT
	movq	32(%rsp), %rax
	cmpl	%eax, 16(%rsp)
	jl	.L595
	jmp	.L592
	.p2align 4,,10
	.p2align 3
.L593:
	movslq	%r15d, %rdx
	cmpq	$1, %rdx
	ja	.L594
	movl	(%rsp), %ecx
	addl	%ecx, %r13d
	cmpl	%r13d, %r14d
	jle	.L604
.L595:
	movslq	%r13d, %rdi
	call	neo_integer_create@PLT
	movl	8(%rsp), %edi
	movq	%rax, %rsi
	call	replace_var@PLT
	movq	24(%r12), %rax
	movq	8(%rax), %rdi
	call	exec_aux
	movl	%eax, %r15d
	movq	global_env(%rip), %rax
	movl	(%rax), %edx
	testl	%edx, %edx
	je	.L593
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	80(%rax), %rdi
	call	deleteContext
	movq	global_env(%rip), %rax
	movq	24(%rsp), %rsi
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	call	partialRestore@PLT
	jmp	.L581
.L592:
	movq	global_env(%rip), %rax
.L604:
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	80(%rax), %rdi
	call	deleteContext
	movq	global_env(%rip), %rax
	movq	24(%rsp), %rsi
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	call	partialRestore@PLT
	jmp	.L603
.L594:
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	80(%rax), %rdi
	call	deleteContext
	movq	global_env(%rip), %rax
	movq	24(%rsp), %rsi
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	call	partialRestore@PLT
	cmpl	$2, %r15d
	je	.L581
	jmp	.L603
	.cfi_endproc
.LFE26:
	.size	execStatementFor, .-execStatementFor
	.p2align 4
	.globl	execStatementForeachList
	.type	execStatementForeachList, @function
execStatementForeachList:
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
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	subq	$16, %rsp
	.cfi_def_cfa_offset 64
	call	neo_to_list@PLT
	movq	%rax, %rbx
	movq	24(%r12), %rax
	movq	(%rax), %rax
	movq	24(%rax), %rax
	movq	(%rax), %rax
	cmpb	$10, (%rax)
	je	.L611
	movq	global_env(%rip), %rax
	movl	$111, (%rax)
.L612:
	xorl	%eax, %eax
.L610:
	addq	$16, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 48
	popq	%rbx
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
.L611:
	.cfi_restore_state
	movl	56(%rax), %r13d
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	80(%rax), %r14
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%r14, %rdi
	call	ptrlist_append@PLT
	movq	global_env(%rip), %rax
	movl	%r13d, %esi
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	movq	(%rdi), %rax
	movq	%rax, (%rsp)
	call	save_later@PLT
	movq	global_env(%rip), %rax
	movl	$24, %edi
	movq	168(%rax), %rax
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
	movl	16(%rbx), %esi
	testl	%esi, %esi
	jle	.L613
	xorl	%r15d, %r15d
	jmp	.L617
	.p2align 4,,10
	.p2align 3
.L614:
	movslq	%eax, %rcx
	cmpq	$1, %rcx
	ja	.L616
	movslq	16(%rbx), %rcx
	addq	$1, %r15
	cmpq	%r15, %rcx
	jle	.L628
.L617:
	movl	%r15d, %esi
	movq	%rbx, %rdi
	call	nelist_nth@PLT
	movq	%rax, %rdi
	movq	%rdx, %rsi
	call	neo_copy@PLT
	movl	%r13d, %edi
	movq	%rax, %rsi
	call	replace_var@PLT
	movq	24(%r12), %rax
	movq	8(%rax), %rdi
	call	exec_aux
	movq	global_env(%rip), %rdx
	movl	(%rdx), %ecx
	testl	%ecx, %ecx
	je	.L614
	movq	168(%rdx), %rax
.L627:
	movq	(%rax), %rax
	movq	80(%rax), %rdi
	call	deleteContext
	movq	global_env(%rip), %rax
	movq	(%rsp), %rsi
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	call	partialRestore@PLT
	jmp	.L612
	.p2align 4,,10
	.p2align 3
.L616:
	movq	168(%rdx), %rdx
	movl	%eax, 12(%rsp)
	movq	(%rdx), %rdx
	movq	80(%rdx), %rdi
	call	deleteContext
	movq	global_env(%rip), %rdx
	movq	(%rsp), %rsi
	movq	168(%rdx), %rdx
	movq	(%rdx), %rdx
	movq	96(%rdx), %rdi
	call	partialRestore@PLT
	movl	12(%rsp), %eax
	cmpl	$2, %eax
	je	.L612
	jmp	.L610
	.p2align 4,,10
	.p2align 3
.L628:
	movq	168(%rdx), %rdx
	movl	%eax, 12(%rsp)
	movq	(%rdx), %rdx
	movq	80(%rdx), %rdi
	call	deleteContext
	movq	global_env(%rip), %rdx
	movq	(%rsp), %rsi
	movq	168(%rdx), %rdx
	movq	(%rdx), %rdx
	movq	96(%rdx), %rdi
	call	partialRestore@PLT
	movl	12(%rsp), %eax
	addq	$16, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 48
	popq	%rbx
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
.L613:
	.cfi_restore_state
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
	jmp	.L627
	.cfi_endproc
.LFE27:
	.size	execStatementForeachList, .-execStatementForeachList
	.p2align 4
	.globl	execStatementForeachString
	.type	execStatementForeachString, @function
execStatementForeachString:
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
	movq	%rdi, %r13
	movq	%rsi, %rdi
	movq	%rdx, %rsi
	pushq	%r12
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	subq	$16, %rsp
	.cfi_def_cfa_offset 64
	call	neo_to_string@PLT
	movq	%rax, %rdi
	movq	%rax, %rbx
	call	strlen@PLT
	movq	%rax, %r12
	movq	24(%r13), %rax
	movq	(%rax), %rax
	movq	24(%rax), %rax
	movq	(%rax), %rax
	cmpb	$10, (%rax)
	je	.L630
	movq	global_env(%rip), %rax
	movl	$111, (%rax)
.L631:
	xorl	%eax, %eax
.L629:
	addq	$16, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 48
	popq	%rbx
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
.L630:
	.cfi_restore_state
	movl	56(%rax), %r14d
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	80(%rax), %r15
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%r15, %rdi
	call	ptrlist_append@PLT
	movq	global_env(%rip), %rax
	movl	%r14d, %esi
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	movq	(%rdi), %rax
	movq	%rax, 8(%rsp)
	call	save_later@PLT
	movq	global_env(%rip), %rax
	movl	$24, %edi
	movq	168(%rax), %rax
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
	testl	%r12d, %r12d
	jle	.L632
	movslq	%r12d, %r12
	addq	%rbx, %r12
	jmp	.L636
	.p2align 4,,10
	.p2align 3
.L633:
	movslq	%eax, %rcx
	cmpq	$1, %rcx
	ja	.L635
	addq	$1, %rbx
	cmpq	%r12, %rbx
	je	.L647
.L636:
	movsbl	(%rbx), %edi
	call	charToString@PLT
	movq	%rax, %rdi
	call	neo_str_create@PLT
	movl	%r14d, %edi
	movq	%rax, %rsi
	call	replace_var@PLT
	movq	24(%r13), %rax
	movq	8(%rax), %rdi
	call	exec_aux
	movq	global_env(%rip), %rdx
	movl	(%rdx), %ecx
	testl	%ecx, %ecx
	je	.L633
	movq	168(%rdx), %rax
.L646:
	movq	(%rax), %rax
	movq	80(%rax), %rdi
	call	deleteContext
	movq	global_env(%rip), %rax
	movq	8(%rsp), %rsi
	movq	168(%rax), %rax
	movq	(%rax), %rax
	movq	96(%rax), %rdi
	call	partialRestore@PLT
	jmp	.L631
	.p2align 4,,10
	.p2align 3
.L635:
	movq	168(%rdx), %rdx
	movl	%eax, (%rsp)
	movq	(%rdx), %rdx
	movq	80(%rdx), %rdi
	call	deleteContext
	movq	global_env(%rip), %rdx
	movq	8(%rsp), %rsi
	movq	168(%rdx), %rdx
	movq	(%rdx), %rdx
	movq	96(%rdx), %rdi
	call	partialRestore@PLT
	movl	(%rsp), %eax
	cmpl	$2, %eax
	je	.L631
	jmp	.L629
	.p2align 4,,10
	.p2align 3
.L647:
	movq	168(%rdx), %rdx
	movl	%eax, (%rsp)
	movq	(%rdx), %rdx
	movq	80(%rdx), %rdi
	call	deleteContext
	movq	global_env(%rip), %rdx
	movq	8(%rsp), %rsi
	movq	168(%rdx), %rdx
	movq	(%rdx), %rdx
	movq	96(%rdx), %rdi
	call	partialRestore@PLT
	movl	(%rsp), %eax
	addq	$16, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 48
	popq	%rbx
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
.L632:
	.cfi_restore_state
	movq	global_env(%rip), %rax
	movq	168(%rax), %rax
	jmp	.L646
	.cfi_endproc
.LFE28:
	.size	execStatementForeachString, .-execStatementForeachString
	.p2align 4
	.globl	initRuntime
	.type	initRuntime, @function
initRuntime:
.LFB30:
	.cfi_startproc
	movl	$1, %esi
	xorl	%edi, %edi
	jmp	create_new_process@PLT
	.cfi_endproc
.LFE30:
	.size	initRuntime, .-initRuntime
	.p2align 4
	.globl	exec
	.type	exec, @function
exec:
.LFB32:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movl	$1, %esi
	movq	%rdi, %rbx
	xorl	%edi, %edi
	call	create_new_process@PLT
	movq	%rbx, %rdi
	call	exec_aux
	popq	%rbx
	.cfi_def_cfa_offset 8
	jmp	exitRuntime
	.cfi_endproc
.LFE32:
	.size	exec, .-exec
	.p2align 4
	.globl	eval
	.type	eval, @function
eval:
.LFB33:
	.cfi_startproc
	pushq	%r13
	.cfi_def_cfa_offset 16
	.cfi_offset 13, -16
	movl	$1, %esi
	pushq	%r12
	.cfi_def_cfa_offset 24
	.cfi_offset 12, -24
	movq	%rdi, %r12
	xorl	%edi, %edi
	subq	$8, %rsp
	.cfi_def_cfa_offset 32
	call	create_new_process@PLT
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
.LFE33:
	.size	eval, .-eval
	.section	.data.rel.ro,"aw"
	.align 32
	.type	operators_functions, @object
	.size	operators_functions, 320
operators_functions:
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
