	.file	"runtime.c"
	.text
	.section	.rodata
.LC0:
	.string	"End process %d\n"
	.text
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
	subq	$16, %rsp
	movq	process_cycle(%rip), %rax
	movq	%rax, %rdi
	call	processCycle_clean@PLT
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movb	$1, 64(%rax)
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	72(%rax), %rdx
	leaq	-16(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	eval_prolog
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movl	112(%rax), %eax
	movl	%eax, %esi
	leaq	.LC0(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	process_preRemove@PLT
	movq	PROMISES_CNT(%rip), %rdx
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movl	112(%rax), %eax
	cltq
	salq	$3, %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	movl	(%rax), %eax
	testl	%eax, %eax
	je	.L2
	movq	PROMISES(%rip), %rax
	movq	8(%rax), %rdx
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movl	112(%rax), %eax
	cltq
	salq	$4, %rax
	leaq	(%rdx,%rax), %rcx
	movq	-16(%rbp), %rax
	movq	-8(%rbp), %rdx
	movq	%rax, (%rcx)
	movq	%rdx, 8(%rcx)
	jmp	.L3
.L2:
	movq	-16(%rbp), %rdx
	movq	-8(%rbp), %rax
	movq	%rdx, %rdi
	movq	%rax, %rsi
	call	neobject_destroy@PLT
.L3:
	movq	PROCESS_FINISH(%rip), %rdx
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movl	112(%rax), %eax
	cltq
	salq	$2, %rax
	addq	%rdx, %rax
	movl	$1, (%rax)
	movq	process_cycle(%rip), %rax
	movq	%rax, %rdi
	call	processCycle_isActive@PLT
	testb	%al, %al
	je	.L4
	call	neon_interp_next_process
.L4:
	call	reset_stack_and_registers@PLT
	jmp	.L5
.L6:
	movq	process_cycle(%rip), %rax
	movq	%rax, %rdi
	call	processCycle_remove@PLT
	movq	%rax, process_cycle(%rip)
.L5:
	movq	process_cycle(%rip), %rax
	movq	%rax, %rdi
	call	processCycle_isEmpty@PLT
	xorl	$1, %eax
	testb	%al, %al
	jne	.L6
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE19:
	.size	launch_process, .-launch_process
	.section	.rodata
	.align 8
.LC1:
	.string	"saut direct \303\240 eval_prolog apr\303\250s la fin du processus principal"
	.text
	.p2align 4
	.globl	exitRuntime
	.type	exitRuntime, @function
exitRuntime:
.LFB32:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	$0, %eax
	movl	$0, %edx
	movq	%rax, -16(%rbp)
	movq	%rdx, -8(%rbp)
	movb	$54, -8(%rbp)
	movq	PROCESS_FINISH(%rip), %rdx
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movl	112(%rax), %eax
	cltq
	salq	$2, %rax
	addq	%rdx, %rax
	movl	$1, (%rax)
	movq	PROMISES_CNT(%rip), %rax
	movq	(%rax), %rax
	movl	$0, (%rax)
	movq	PROCESS_FINISH(%rip), %rax
	movl	$1, (%rax)
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	process_preRemove@PLT
	movq	process_cycle(%rip), %rax
	movq	%rax, %rdi
	call	processCycle_isActive@PLT
	testb	%al, %al
	je	.L10
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	call	puts@PLT
	call	save_stack_and_registers@PLT
	call	neon_interp_next_process
	jmp	.L10
.L11:
	movq	process_cycle(%rip), %rax
	movq	%rax, %rdi
	call	processCycle_remove@PLT
	movq	%rax, process_cycle(%rip)
.L10:
	movq	process_cycle(%rip), %rax
	movq	%rax, %rdi
	call	processCycle_isEmpty@PLT
	xorl	$1, %eax
	testb	%al, %al
	jne	.L11
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE32:
	.size	exitRuntime, .-exitRuntime
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
	je	.L26
	cmpq	$0, 8(%r15)
	movl	16(%rbx), %edi
	movq	%r15, %r13
	jne	.L21
	jmp	.L27
	.p2align 4,,10
	.p2align 3
.L22:
	movq	%r12, %r13
.L21:
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
	jne	.L22
	movq	(%rbx), %rsi
	movq	8(%rbx), %rdx
	movl	16(%rbx), %edi
	call	replace_var@PLT
	movq	%rbx, %rdi
	call	free@PLT
	movq	%r12, %rdi
	call	free@PLT
.L20:
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
.L27:
	.cfi_restore_state
	movq	(%rbx), %rsi
	movq	8(%rbx), %rdx
	call	replace_var@PLT
	movq	%rbx, %rdi
	call	free@PLT
.L26:
	movq	%r15, %rdi
	call	free@PLT
	jmp	.L20
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
	je	.L36
	cmpb	$4, %sil
	je	.L37
	movl	$20, CODE_ERROR(%rip)
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L36:
	jmp	neo_to_bool@PLT
	.p2align 4,,10
	.p2align 3
.L37:
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
	.align 8
.LC2:
	.string	"switch from process %d to process %d\n"
	.text
	.p2align 4
	.globl	neon_interp_yield
	.type	neon_interp_yield, @function
neon_interp_yield:
.LFB17:
	.cfi_startproc
	movl	atomic_counter(%rip), %eax
	testl	%eax, %eax
	jne	.L43
	movl	ATOMIC_TIME(%rip), %eax
	movq	process_cycle(%rip), %rdx
	movl	%eax, atomic_counter(%rip)
	cmpq	8(%rdx), %rdx
	je	.L43
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movq	(%rdx), %rdi
	call	unloadCurrentProcess@PLT
	movq	process_cycle(%rip), %rdi
	call	loadNextLivingProcess@PLT
	leaq	.LC2(%rip), %rdi
	movq	(%rax), %rdx
	movq	%rax, process_cycle(%rip)
	movq	16(%rax), %rax
	movl	112(%rdx), %edx
	movq	(%rax), %rax
	movl	112(%rax), %esi
	xorl	%eax, %eax
	call	printf@PLT
	movq	process_cycle(%rip), %rax
	movq	16(%rax), %rdx
	movq	(%rax), %rdi
	movq	(%rdx), %rsi
	call	switch_registers@PLT
	movl	atomic_counter(%rip), %eax
	subl	$1, %eax
	movl	%eax, atomic_counter(%rip)
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L43:
	subl	$1, %eax
	movl	%eax, atomic_counter(%rip)
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
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movl	ATOMIC_TIME(%rip), %eax
	movq	process_cycle(%rip), %rdi
	movl	%eax, atomic_counter(%rip)
	call	loadNextLivingProcess@PLT
	movq	16(%rax), %rdx
	movq	(%rax), %rdi
	movq	%rax, process_cycle(%rip)
	movq	(%rdx), %rsi
	call	switch_registers@PLT
	movq	process_cycle(%rip), %rdi
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	jmp	processCycle_clean@PLT
	.cfi_endproc
.LFE18:
	.size	neon_interp_next_process, .-neon_interp_next_process
	.section	.rodata
.LC3:
	.string	".ne"
	.text
	.p2align 4
	.globl	exec_aux
	.type	exec_aux, @function
exec_aux:
.LFB30:
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
	subq	$32, %rsp
	.cfi_def_cfa_offset 80
	movl	32(%rdi), %esi
	testl	%esi, %esi
	jle	.L113
	movq	%rdi, %r12
	xorl	%ebx, %ebx
	.p2align 4,,10
	.p2align 3
.L49:
	movq	24(%r12), %rax
	leaq	0(,%rbx,8), %r13
	movq	(%rax,%rbx,8), %rdi
	movl	4(%rdi), %eax
	movl	%eax, LINENUMBER(%rip)
	movzbl	(%rdi), %eax
	subl	$16, %eax
	cmpb	$40, %al
	ja	.L51
	leaq	.L53(%rip), %rdx
	movzbl	%al, %eax
	movslq	(%rdx,%rax,4), %rax
	addq	%rdx, %rax
	jmp	*%rax
	.section	.rodata
	.align 4
	.align 4
.L53:
	.long	.L61-.L53
	.long	.L51-.L53
	.long	.L60-.L53
	.long	.L51-.L53
	.long	.L51-.L53
	.long	.L51-.L53
	.long	.L51-.L53
	.long	.L51-.L53
	.long	.L51-.L53
	.long	.L51-.L53
	.long	.L51-.L53
	.long	.L59-.L53
	.long	.L58-.L53
	.long	.L57-.L53
	.long	.L51-.L53
	.long	.L51-.L53
	.long	.L51-.L53
	.long	.L51-.L53
	.long	.L51-.L53
	.long	.L56-.L53
	.long	.L51-.L53
	.long	.L51-.L53
	.long	.L51-.L53
	.long	.L51-.L53
	.long	.L51-.L53
	.long	.L51-.L53
	.long	.L51-.L53
	.long	.L51-.L53
	.long	.L55-.L53
	.long	.L51-.L53
	.long	.L54-.L53
	.long	.L51-.L53
	.long	.L51-.L53
	.long	.L51-.L53
	.long	.L51-.L53
	.long	.L51-.L53
	.long	.L51-.L53
	.long	.L51-.L53
	.long	.L51-.L53
	.long	.L51-.L53
	.long	.L52-.L53
	.text
	.p2align 4,,10
	.p2align 3
.L51:
	call	eval_aux
	movq	%rax, %rdi
	movq	%rdx, %rsi
	call	neobject_destroy@PLT
.L156:
	movl	CODE_ERROR(%rip), %eax
	testl	%eax, %eax
	jne	.L113
.L120:
	addq	$1, %rbx
	cmpl	%ebx, 32(%r12)
	jg	.L49
.L113:
	xorl	%r15d, %r15d
.L48:
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
.L52:
	.cfi_restore_state
	movq	24(%rdi), %rax
	movq	(%rax), %rax
	movq	24(%rax), %rdx
	movq	(%rdx), %rcx
	cmpb	$10, (%rcx)
	jne	.L162
	cmpl	$2, 32(%rax)
	jne	.L168
	movq	8(%rdx), %rdi
	call	eval_aux
	movl	CODE_ERROR(%rip), %esi
	movq	%rax, %rcx
	testl	%esi, %esi
	jne	.L113
	cmpb	$-122, %dl
	je	.L169
	cmpb	$-125, %dl
	jne	.L118
	movq	24(%r12), %rax
	movq	%rcx, %rsi
	movq	%rcx, 8(%rsp)
	movq	%rdx, (%rsp)
	movq	(%rax,%r13), %rdi
	call	execStatementForeachString
	movq	(%rsp), %rdx
	movq	8(%rsp), %rcx
	movl	%eax, %r15d
.L117:
	movq	%rcx, %rdi
	movq	%rdx, %rsi
	call	neobject_destroy@PLT
	movl	%r15d, %eax
	orl	CODE_ERROR(%rip), %eax
	je	.L120
	jmp	.L48
	.p2align 4,,10
	.p2align 3
.L54:
	movl	atomic_counter(%rip), %r13d
	movl	$-1, atomic_counter(%rip)
	call	exec_aux
	addl	%r13d, atomic_counter(%rip)
	xorl	%edx, %edx
	movl	%eax, %r15d
	movl	atomic_counter(%rip), %eax
	testl	%eax, %eax
	cmovs	%edx, %eax
	movl	%eax, atomic_counter(%rip)
	movl	%r15d, %eax
	orl	CODE_ERROR(%rip), %eax
	je	.L120
	jmp	.L48
	.p2align 4,,10
	.p2align 3
.L55:
	movq	24(%rdi), %rax
	movq	(%rax), %rdi
	call	exec_aux
	movl	%eax, %r15d
	testl	%eax, %eax
	jne	.L48
	movq	24(%r12), %rax
	movl	CODE_ERROR(%rip), %esi
	movq	(%rax,%r13), %r13
	movl	%esi, %eax
	cmpl	$1, %esi
	ja	.L170
.L63:
	testl	%eax, %eax
	je	.L120
	jmp	.L113
	.p2align 4,,10
	.p2align 3
.L56:
	movq	24(%rdi), %rax
	movq	(%rax), %r14
	movl	32(%r14), %edi
	call	nelist_create@PLT
	movl	32(%r14), %ecx
	movq	%rax, %r8
	testl	%ecx, %ecx
	jle	.L124
	xorl	%r15d, %r15d
	jmp	.L125
	.p2align 4,,10
	.p2align 3
.L172:
	movq	%rcx, 8(%rsp)
	movq	%r8, (%rsp)
	call	eval_aux
	movq	8(%rsp), %rcx
	movq	(%rsp), %r8
	movq	%rdx, 8(%rcx)
	movl	CODE_ERROR(%rip), %edx
	movq	%rax, (%rcx)
	testl	%edx, %edx
	jne	.L171
	addq	$1, %r15
	cmpl	%r15d, 32(%r14)
	jle	.L124
.L125:
	movq	24(%r14), %rax
	movq	%r15, %rcx
	salq	$4, %rcx
	addq	8(%r8), %rcx
	movq	(%rax,%r15,8), %rdi
	testq	%rdi, %rdi
	jne	.L172
	pxor	%xmm0, %xmm0
	addq	$1, %r15
	movups	%xmm0, (%rcx)
	movb	$54, 8(%rcx)
	cmpl	%r15d, 32(%r14)
	jg	.L125
.L124:
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
	jmp	.L159
	.p2align 4,,10
	.p2align 3
.L57:
	movq	24(%rdi), %rax
	.p2align 4,,10
	.p2align 3
.L166:
	movq	(%rax), %rdi
	call	eval_aux
	movl	CODE_ERROR(%rip), %r8d
	movq	%rax, %rdi
	movq	%rdx, %r14
	testl	%r8d, %r8d
	jne	.L113
	movq	%rdx, %rsi
	cmpb	$8, %dl
	je	.L173
	cmpb	$4, %r14b
	je	.L174
	movl	$20, CODE_ERROR(%rip)
	movq	%r14, %rsi
	call	neobject_destroy@PLT
.L159:
	movl	CODE_ERROR(%rip), %eax
	testl	%eax, %eax
	je	.L120
	jmp	.L113
	.p2align 4,,10
	.p2align 3
.L58:
	movq	24(%rdi), %rax
	movq	(%rax), %rax
	movq	24(%rax), %rax
	movq	(%rax), %rax
	cmpb	$10, (%rax)
	jne	.L162
	call	execStatementFor
	movl	%eax, %r15d
	orl	CODE_ERROR(%rip), %eax
	je	.L120
	jmp	.L48
	.p2align 4,,10
	.p2align 3
.L59:
	call	execConditionBlock
	movl	%eax, %r15d
	testl	%eax, %eax
	je	.L156
	jmp	.L48
	.p2align 4,,10
	.p2align 3
.L60:
	movl	16(%rdi), %eax
	cmpl	$1, %eax
	je	.L175
	cmpl	$2, %eax
	je	.L176
	cmpl	$3, %eax
	je	.L177
	cmpl	$4, %eax
	jne	.L159
	cmpl	$1, 32(%rdi)
	jle	.L92
	jmp	.L178
	.p2align 4,,10
	.p2align 3
.L181:
	movq	%rdx, %rsi
	cmpb	$8, %dl
	je	.L179
	cmpb	$4, %dl
	je	.L180
	movl	$20, CODE_ERROR(%rip)
	call	neobject_destroy@PLT
.L97:
	movl	CODE_ERROR(%rip), %r14d
	testl	%r14d, %r14d
	jne	.L113
	movl	atomic_counter(%rip), %r11d
	testl	%r11d, %r11d
	js	.L158
	movl	$0, atomic_counter(%rip)
	call	neon_interp_yield
.L158:
	movq	24(%r12), %rax
	movq	(%rax,%r13), %rdi
.L92:
	movq	24(%rdi), %rax
	movq	(%rax), %rdi
	call	eval_aux
	movl	CODE_ERROR(%rip), %r10d
	movq	%rax, %rdi
	testl	%r10d, %r10d
	je	.L181
	xorl	%r15d, %r15d
	jmp	.L48
	.p2align 4,,10
	.p2align 3
.L61:
	movl	16(%rdi), %r15d
	cmpl	$3, %r15d
	je	.L159
	jmp	.L48
	.p2align 4,,10
	.p2align 3
.L170:
	movl	32(%r13), %edx
	cmpl	$1, %edx
	jle	.L113
	movl	$1, (%rsp)
	movq	24(%r13), %rdi
	movl	$8, %r14d
.L73:
	movq	(%rdi,%r14), %rax
	movq	24(%rax), %rax
	movq	(%rax), %rax
	movl	32(%rax), %ecx
	testl	%ecx, %ecx
	jne	.L182
.L65:
	movl	$0, CODE_ERROR(%rip)
	movslq	(%rsp), %r15
	movq	(%rdi,%r15,8), %rax
	movq	24(%rax), %rax
	movq	8(%rax), %rdi
	call	exec_aux
	movl	%eax, %r15d
	orl	CODE_ERROR(%rip), %eax
	je	.L120
	jmp	.L48
	.p2align 4,,10
	.p2align 3
.L173:
	movb	$8, %sil
	movq	%rax, (%rsp)
	call	neo_to_bool@PLT
	movq	(%rsp), %rdi
	movl	%eax, %r15d
.L101:
	movq	%r14, %rsi
	call	neobject_destroy@PLT
	testb	%r15b, %r15b
	je	.L159
	movq	24(%r12), %rax
	movq	(%rax,%r13), %rax
	movq	24(%rax), %rax
	movq	8(%rax), %rdi
	call	exec_aux
	movl	CODE_ERROR(%rip), %r9d
	movl	%eax, %r15d
	testl	%r9d, %r9d
	jne	.L113
	cmpl	$1, %eax
	ja	.L105
	movq	24(%r12), %rax
	movq	(%rax,%r13), %rax
	movq	24(%rax), %rax
	jmp	.L166
	.p2align 4,,10
	.p2align 3
.L174:
	movb	$4, %sil
	movq	%rdi, (%rsp)
	call	neo_to_integer@PLT
	movq	(%rsp), %rdi
	testq	%rax, %rax
	setne	%r15b
	jmp	.L101
	.p2align 4,,10
	.p2align 3
.L171:
	movq	%r8, %rdi
	leal	-1(%r15), %esi
	call	nelist_destroy_until@PLT
	xorl	%r8d, %r8d
	jmp	.L124
	.p2align 4,,10
	.p2align 3
.L169:
	movq	24(%r12), %rax
	movq	%rcx, %rsi
	movq	%rcx, 8(%rsp)
	movq	%rdx, (%rsp)
	movq	(%rax,%r13), %rdi
	call	execStatementForeachList
	movq	(%rsp), %rdx
	movq	8(%rsp), %rcx
	movl	%eax, %r15d
	jmp	.L117
	.p2align 4,,10
	.p2align 3
.L177:
	movl	32(%rdi), %edx
	testl	%edx, %edx
	je	.L183
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rcx
	cmpq	$0, (%rcx)
	je	.L86
	xorl	%r14d, %r14d
	testl	%edx, %edx
	jg	.L87
	jmp	.L159
	.p2align 4,,10
	.p2align 3
.L89:
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
.L87:
	movq	24(%rdi), %rdx
	movq	120(%rax), %rdi
	movq	(%rdx,%r14,8), %rdx
	movl	56(%rdx), %esi
	call	save_later@PLT
	movq	process_cycle(%rip), %rax
	movl	$24, %edi
	movq	(%rax), %rax
	movq	104(%rax), %r8
	movq	24(%r12), %rax
	movq	(%rax,%r13), %rax
	movq	%r8, 8(%rsp)
	movq	24(%rax), %rax
	movq	(%rax,%r14,8), %rax
	addq	$1, %r14
	movl	56(%rax), %eax
	movl	%eax, (%rsp)
	call	malloc@PLT
	movl	(%rsp), %edi
	movq	%rax, %r15
	call	get_var_value@PLT
	movq	%rax, (%r15)
	movl	(%rsp), %eax
	movq	%rdx, 8(%r15)
	movl	%eax, 16(%r15)
	movl	%eax, %edi
	call	get_absolute_address@PLT
	movq	%rax, %rdi
	call	var_reset@PLT
	movq	8(%rsp), %r8
	movq	%r15, %rsi
	movq	(%r8), %rdi
	call	ptrlist_append@PLT
	movq	24(%r12), %rax
	movq	(%rax,%r13), %rdi
	cmpl	%r14d, 32(%rdi)
	jg	.L89
	jmp	.L159
	.p2align 4,,10
	.p2align 3
.L182:
	jle	.L66
	xorl	%r15d, %r15d
	jmp	.L72
	.p2align 4,,10
	.p2align 3
.L69:
	movq	24(%r13), %rdi
	addq	$1, %r15
	movq	(%rdi,%r14), %rax
	movq	24(%rax), %rax
	movq	(%rax), %rax
	cmpl	%r15d, 32(%rax)
	jle	.L184
.L72:
	movq	24(%rax), %rax
	movq	(%rax,%r15,8), %rax
	cmpb	$20, 48(%rax)
	jne	.L185
	movq	40(%rax), %rdi
	movq	48(%rax), %rsi
	call	get_exception_code@PLT
	movslq	CODE_ERROR(%rip), %r8
	movq	exceptions_err(%rip), %rdi
	movq	%r8, %rsi
	cmpl	%eax, (%rdi,%r8,4)
	je	.L68
	testl	%r8d, %r8d
	jns	.L69
	addl	%r8d, %eax
	jne	.L69
.L70:
	movq	$0, EXCEPTION(%rip)
.L154:
	movq	24(%r13), %rdi
	jmp	.L65
	.p2align 4,,10
	.p2align 3
.L184:
	movl	32(%r13), %edx
.L66:
	addl	$1, (%rsp)
	movl	(%rsp), %eax
	addq	$8, %r14
	cmpl	%edx, %eax
	jl	.L73
	movl	%esi, %eax
	jmp	.L63
	.p2align 4,,10
	.p2align 3
.L185:
	movl	$78, CODE_ERROR(%rip)
	jmp	.L113
	.p2align 4,,10
	.p2align 3
.L68:
	testl	%r8d, %r8d
	jns	.L154
	jmp	.L70
	.p2align 4,,10
	.p2align 3
.L105:
	cmpl	$4, %eax
	je	.L48
	jmp	.L159
	.p2align 4,,10
	.p2align 3
.L176:
	movl	32(%rdi), %eax
	xorl	%r15d, %r15d
	testl	%eax, %eax
	jg	.L83
	jmp	.L159
	.p2align 4,,10
	.p2align 3
.L186:
	movl	NAME(%rip), %esi
	movq	ADRESSES(%rip), %rdi
	call	nelist_nth@PLT
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	neo_to_string@PLT
	movq	%rax, %rdi
	call	strdup@PLT
	movq	(%rsp), %rcx
	movq	%r14, %rsi
	movq	%rax, 16(%rsp)
	movq	%rcx, %rdi
	movq	%rcx, 8(%rsp)
	call	neo_to_string@PLT
	leaq	.LC3(%rip), %rsi
	movq	%rax, %rdi
	call	addStr@PLT
	movq	8(%rsp), %rdi
	movq	%r14, %rsi
	movq	%rax, (%rsp)
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
	movq	(%rsp), %rdi
	call	importFile@PLT
	movq	(%rsp), %rdi
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
	movq	%r14, %rsi
	movq	%rdi, 8(%rdx)
	movq	8(%rsp), %rdi
	call	neobject_destroy@PLT
	movl	CODE_ERROR(%rip), %eax
	testl	%eax, %eax
	jne	.L113
	call	gc_mark_and_sweep@PLT
	movq	24(%r12), %rax
	addq	$1, %r15
	movq	(%rax,%r13), %rdi
	cmpl	%r15d, 32(%rdi)
	jle	.L159
.L83:
	movq	24(%rdi), %rax
	movq	(%rax,%r15,8), %rdi
	call	eval_aux
	movq	%rax, (%rsp)
	movl	CODE_ERROR(%rip), %eax
	movq	%rdx, %r14
	testl	%eax, %eax
	je	.L186
	jmp	.L113
.L179:
	movb	$8, %sil
	movq	%rdx, 8(%rsp)
	movq	%rax, (%rsp)
	call	neo_to_bool@PLT
	movq	(%rsp), %rdi
	movq	8(%rsp), %rdx
	movl	%eax, %r15d
.L95:
	movq	%rdx, %rsi
	call	neobject_destroy@PLT
	testb	%r15b, %r15b
	je	.L97
	jmp	.L156
.L180:
	movb	$4, %sil
	movq	%rdx, 8(%rsp)
	movq	%rax, (%rsp)
	call	neo_to_integer@PLT
	movq	(%rsp), %rdi
	movq	8(%rsp), %rdx
	testq	%rax, %rax
	setne	%r15b
	jmp	.L95
.L162:
	movl	$22, CODE_ERROR(%rip)
	jmp	.L113
.L175:
	cmpl	$1, 32(%rdi)
	jle	.L76
	movl	$21, CODE_ERROR(%rip)
	jmp	.L113
.L168:
	movl	$109, CODE_ERROR(%rip)
	jmp	.L113
.L118:
	movq	%rax, %rdi
	movq	%rdx, %rsi
	call	neobject_destroy@PLT
	movl	$109, CODE_ERROR(%rip)
	jmp	.L113
.L76:
	movq	RETURN_VALUE(%rip), %rdi
	movq	8+RETURN_VALUE(%rip), %rsi
	call	neo_is_void@PLT
	testb	%al, %al
	jne	.L77
	movl	$99, CODE_ERROR(%rip)
	jmp	.L113
.L77:
	movq	24(%r12), %rax
	movq	(%rax,%r13), %rax
	movl	32(%rax), %edx
	testl	%edx, %edx
	jne	.L78
	call	neo_none_create@PLT
	movq	%rax, RETURN_VALUE(%rip)
	movq	%rdx, 8+RETURN_VALUE(%rip)
.L79:
	movl	$4, %r15d
	jmp	.L48
.L178:
	movl	$101, CODE_ERROR(%rip)
	jmp	.L113
.L78:
	movq	24(%rax), %rax
	movq	(%rax), %rdi
	call	eval_aux
	movq	%rax, RETURN_VALUE(%rip)
	movq	%rdx, 8+RETURN_VALUE(%rip)
	jmp	.L79
.L86:
	movl	$70, CODE_ERROR(%rip)
	jmp	.L113
.L183:
	movl	$69, CODE_ERROR(%rip)
	jmp	.L113
	.cfi_endproc
.LFE30:
	.size	exec_aux, .-exec_aux
	.section	.rodata
.LC4:
	.string	"__local_args__"
	.text
	.p2align 4
	.globl	callUserFunc
	.type	callUserFunc, @function
callUserFunc:
.LFB22:
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
	movq	process_cycle(%rip), %rax
	movq	%rsi, 32(%rsp)
	movq	(%rax), %rax
	movq	104(%rax), %r14
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%r14, %rdi
	call	ptrlist_append@PLT
	movq	process_cycle(%rip), %rax
	cmpb	$0, 20(%r12)
	movq	(%rax), %rax
	movq	120(%rax), %rax
	movq	(%rax), %rax
	movq	%rax, 40(%rsp)
	jne	.L188
	movl	16(%r12), %eax
.L189:
	xorl	%ebx, %ebx
	testl	%eax, %eax
	jg	.L191
	jmp	.L199
	.p2align 4,,10
	.p2align 3
.L194:
	addq	$1, %rbx
	cmpl	%ebx, 16(%r12)
	jle	.L199
.L191:
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
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	call	save_later@PLT
	movq	process_cycle(%rip), %rax
	movl	$24, %edi
	movq	(%rax), %rax
	movq	104(%rax), %r14
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
	movq	8(%rsp), %rsi
	movq	16(%rsp), %rdx
	movl	(%rax,%rbx,4), %edi
	call	replace_var@PLT
	movl	CODE_ERROR(%rip), %eax
	testl	%eax, %eax
	je	.L194
	movl	$4, %r13d
	cmpl	$1, %ebx
	jle	.L198
	.p2align 4,,10
	.p2align 3
.L197:
	movq	8(%r12), %rax
	movl	(%rax,%r13), %edi
	addq	$4, %r13
	call	free_var@PLT
	movq	24(%rsp), %rax
	cmpq	%rax, %r13
	jne	.L197
.L198:
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rdi
	call	deleteContext
	movq	process_cycle(%rip), %rax
	movq	40(%rsp), %rsi
	movq	(%rax), %rax
	movq	120(%rax), %rdi
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
.L199:
	.cfi_restore_state
	movq	32(%r12), %rdi
	call	exec_aux
	movl	%eax, %ebx
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rdi
	call	deleteContext
	movq	process_cycle(%rip), %rax
	movq	40(%rsp), %rsi
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	call	partialRestore@PLT
	movl	CODE_ERROR(%rip), %edx
	testl	%edx, %edx
	jne	.L208
	cmpl	$4, %ebx
	jne	.L200
	pxor	%xmm0, %xmm0
	movq	8+RETURN_VALUE(%rip), %rdx
	movq	RETURN_VALUE(%rip), %rax
	movups	%xmm0, RETURN_VALUE(%rip)
	movb	$54, 8+RETURN_VALUE(%rip)
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
.L188:
	.cfi_restore_state
	movq	%r13, %rcx
	movq	%rbx, %rsi
	movq	%rbx, %r13
	movq	%rcx, %rdi
	movq	%rcx, 16(%rsp)
	call	neo_list_len@PLT
	leaq	.LC4(%rip), %rdi
	movl	%eax, %ebx
	call	get_var@PLT
	movl	%eax, %r14d
	movq	process_cycle(%rip), %rax
	movl	%r14d, %esi
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	call	save_later@PLT
	movq	process_cycle(%rip), %rax
	movl	$24, %edi
	movq	(%rax), %rax
	movq	104(%rax), %rsi
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
	jle	.L189
	movq	32(%rsp), %rcx
	movl	%eax, 16(%rcx)
	jmp	.L189
	.p2align 4,,10
	.p2align 3
.L208:
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
.L200:
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
.LFE22:
	.size	callUserFunc, .-callUserFunc
	.p2align 4
	.globl	callUserMethod
	.type	callUserMethod, @function
callUserMethod:
.LFB23:
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
	movq	process_cycle(%rip), %rax
	movq	%rsi, 40(%rsp)
	movq	(%rax), %rax
	movq	%rdx, 24(%rsp)
	movq	104(%rax), %rbx
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%rbx, %rdi
	call	ptrlist_append@PLT
	movq	process_cycle(%rip), %rax
	cmpb	$0, 20(%r12)
	movq	(%rax), %rax
	movq	120(%rax), %rax
	movq	(%rax), %rax
	movq	%rax, 32(%rsp)
	jne	.L210
	movl	16(%r12), %eax
.L211:
	xorl	%ebx, %ebx
	testl	%eax, %eax
	jg	.L213
	jmp	.L221
	.p2align 4,,10
	.p2align 3
.L216:
	addq	$1, %rbx
	cmpl	%ebx, 16(%r12)
	jle	.L221
.L213:
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
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	call	save_later@PLT
	movq	process_cycle(%rip), %rax
	movl	$24, %edi
	movq	(%rax), %rax
	movq	104(%rax), %r14
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
	movq	(%rsp), %rsi
	movq	8(%rsp), %rdx
	movl	(%rax,%rbx,4), %edi
	call	replace_var@PLT
	movl	CODE_ERROR(%rip), %eax
	testl	%eax, %eax
	je	.L216
	movl	$4, %r13d
	cmpl	$1, %ebx
	jle	.L214
	.p2align 4,,10
	.p2align 3
.L219:
	movq	8(%r12), %rax
	movl	(%rax,%r13), %edi
	addq	$4, %r13
	call	free_var@PLT
	movq	16(%rsp), %rax
	cmpq	%rax, %r13
	jne	.L219
.L214:
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rdi
	call	deleteContext
	movq	process_cycle(%rip), %rax
	movq	32(%rsp), %rsi
	movq	(%rax), %rax
	movq	120(%rax), %rdi
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
.L221:
	.cfi_restore_state
	movq	32(%r12), %rdi
	call	exec_aux
	movl	CODE_ERROR(%rip), %edx
	movl	%eax, %ebx
	testl	%edx, %edx
	jne	.L214
	movq	8(%r12), %rax
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
	movq	40(%rsp), %r14
	movq	(%r14), %rdi
	movq	8(%r14), %rsi
	call	neobject_destroy@PLT
	movq	%r12, (%r14)
	movq	%r13, 8(%r14)
	cmpl	$4, %ebx
	jne	.L222
	pxor	%xmm0, %xmm0
	movq	8+RETURN_VALUE(%rip), %rdx
	movq	RETURN_VALUE(%rip), %rax
	movups	%xmm0, RETURN_VALUE(%rip)
	movb	$54, 8+RETURN_VALUE(%rip)
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
.L210:
	.cfi_restore_state
	movq	%r14, %rsi
	movq	%r13, %rdi
	call	neo_list_len@PLT
	leaq	.LC4(%rip), %rdi
	movl	%eax, 8(%rsp)
	call	get_var@PLT
	movl	%eax, %ebx
	movq	process_cycle(%rip), %rax
	movl	%ebx, %esi
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	call	save_later@PLT
	movq	process_cycle(%rip), %rax
	movl	$24, %edi
	movq	(%rax), %rax
	movq	104(%rax), %rsi
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
	jle	.L211
	movq	24(%rsp), %rcx
	movl	%eax, 16(%rcx)
	jmp	.L211
	.p2align 4,,10
	.p2align 3
.L222:
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
.LFE23:
	.size	callUserMethod, .-callUserMethod
	.p2align 4
	.globl	eval_aux
	.type	eval_aux, @function
eval_aux:
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
	subq	$64, %rsp
	.cfi_def_cfa_offset 112
	call	neon_interp_yield
	movl	CODE_ERROR(%rip), %esi
	testl	%esi, %esi
	jne	.L440
	movl	4(%rbx), %eax
	movl	%eax, LINENUMBER(%rip)
	movzbl	(%rbx), %ecx
	cmpb	$52, %cl
	ja	.L233
	cmpb	$3, %cl
	ja	.L234
	cmpb	$1, %cl
	je	.L235
	cmpb	$2, %cl
	jne	.L237
	movl	32(%rbx), %edi
	cmpl	$1, %edi
	je	.L443
	cmpl	$2, %edi
	jne	.L254
	movslq	16(%rbx), %rcx
	movq	OPERANDES(%rip), %rdx
	movl	(%rdx,%rcx,4), %edx
	testb	$1, %dh
	jne	.L444
	cmpl	$37, %ecx
	je	.L445
	cmpl	$35, %ecx
	je	.L446
	movq	24(%rbx), %rax
	movq	(%rax), %rdi
	testb	$8, %dl
	jne	.L447
	testb	$16, %dl
	jne	.L448
	andl	$64, %edx
	je	.L266
	call	get_address
	movl	CODE_ERROR(%rip), %r11d
	movq	%rax, %r12
	testl	%r11d, %r11d
	jne	.L440
	movq	24(%rbx), %rax
	movq	8(%rax), %rdi
	call	get_address
	cmpl	$0, CODE_ERROR(%rip)
	movq	%rax, %rsi
	jne	.L440
	movslq	16(%rbx), %rdx
	leaq	OPFONC(%rip), %rax
	movq	%r12, %rdi
	movq	(%rax,%rdx,8), %rax
.L441:
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
.L233:
	.cfi_restore_state
	cmpb	$-122, %cl
	je	.L241
	jbe	.L449
	cmpb	$-77, %cl
	jne	.L237
	movl	32(%rbx), %edi
	xorl	%r12d, %r12d
	movq	%rbx, %r14
	call	nelist_create@PLT
	movl	32(%rbx), %esi
	movq	%rax, %r13
	testl	%esi, %esi
	jg	.L357
.L360:
	movl	16(%rbx), %edi
	addq	$64, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 48
	movq	%r13, %rsi
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
.L358:
	.cfi_restore_state
	addq	$1, %r12
	cmpl	%r12d, 32(%r14)
	jle	.L450
.L357:
	movq	24(%r14), %rax
	movq	(%rax,%r12,8), %rax
	movq	24(%rax), %rax
	movq	(%rax), %rdi
	call	eval_aux
	movq	%rdx, %rbx
	movq	%r12, %rdx
	movq	%rax, %rcx
	salq	$4, %rdx
	addq	8(%r13), %rdx
	movq	%rcx, (%rdx)
	movl	CODE_ERROR(%rip), %ecx
	movq	%rbx, 8(%rdx)
	testl	%ecx, %ecx
	je	.L358
	leal	-1(%r12), %esi
	movq	%r13, %rdi
	call	nelist_destroy_until@PLT
	xorl	%eax, %eax
	movl	$54, %edx
.L421:
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
.L234:
	.cfi_restore_state
	movl	$1, %eax
	salq	%cl, %rax
	testl	$1179952, %eax
	jne	.L238
	cmpb	$10, %cl
	je	.L239
	cmpb	$52, %cl
	je	.L451
.L237:
	movl	$19, CODE_ERROR(%rip)
	xorl	%eax, %eax
	movl	$54, %edx
	jmp	.L421
.L449:
	cmpb	$-128, %cl
	jne	.L452
	movq	40(%rbx), %rdi
	movq	48(%rbx), %rsi
	call	neo_is_void@PLT
	testb	%al, %al
	jne	.L453
.L278:
	movzbl	48(%rbx), %eax
	cmpb	$-128, %al
	je	.L454
	leal	79(%rax), %edx
	cmpb	$1, %dl
	jbe	.L455
	cmpb	$54, %al
	je	.L456
	movl	$9, CODE_ERROR(%rip)
.L431:
	movq	40(%rbx), %rdi
	movq	48(%rbx), %rsi
.L439:
	call	neobject_destroy@PLT
	pxor	%xmm0, %xmm0
	movups	%xmm0, 40(%rbx)
	movb	$54, 48(%rbx)
.L440:
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
.L452:
	.cfi_restore_state
	cmpb	$-125, %cl
	jne	.L237
.L238:
	movq	40(%rbx), %rdi
	movq	48(%rbx), %rsi
.L442:
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
.L235:
	.cfi_restore_state
	movq	24(%rbx), %rax
	movq	(%rax), %rdi
	call	get_address
	movl	CODE_ERROR(%rip), %r8d
	movq	(%rax), %r13
	movq	8(%rax), %r12
	movzbl	8(%rax), %r15d
	testl	%r8d, %r8d
	jne	.L440
	movq	24(%rbx), %rax
	movq	8(%rax), %rdi
	call	eval_aux
	movl	CODE_ERROR(%rip), %edi
	movq	%rax, %r14
	movq	%rdx, %rbx
	testl	%edi, %edi
	jne	.L440
	cmpb	$-122, %r15b
	je	.L350
	cmpb	$-125, %r15b
	je	.L350
	movl	$15, CODE_ERROR(%rip)
	xorl	%eax, %eax
	movl	$54, %edx
	jmp	.L421
	.p2align 4,,10
	.p2align 3
.L241:
	movl	32(%rbx), %edi
.L254:
	call	nelist_create@PLT
	movl	32(%rbx), %r8d
	movq	%rax, %r14
	testl	%r8d, %r8d
	jle	.L276
	xorl	%r12d, %r12d
	jmp	.L277
	.p2align 4,,10
	.p2align 3
.L458:
	call	eval_aux
	movl	CODE_ERROR(%rip), %edi
	movq	%rax, 0(%r13)
	movq	%rdx, 8(%r13)
	testl	%edi, %edi
	jne	.L457
.L275:
	addq	$1, %r12
	cmpl	%r12d, 32(%rbx)
	jle	.L276
.L277:
	movq	24(%rbx), %rax
	movq	%r12, %r13
	salq	$4, %r13
	addq	8(%r14), %r13
	movq	(%rax,%r12,8), %rdi
	testq	%rdi, %rdi
	jne	.L458
	pxor	%xmm0, %xmm0
	movups	%xmm0, 0(%r13)
	movb	$54, 8(%r13)
	jmp	.L275
	.p2align 4,,10
	.p2align 3
.L457:
	movq	%r14, %rdi
	leal	-1(%r12), %esi
	xorl	%r14d, %r14d
	call	nelist_destroy_until@PLT
.L276:
	movl	CODE_ERROR(%rip), %esi
	testl	%esi, %esi
	jne	.L440
	addq	$64, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 48
	movq	%r14, %rdi
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
	.p2align 4,,10
	.p2align 3
.L451:
	.cfi_restore_state
	movq	24(%rbx), %rax
	movq	(%rax), %rdi
	call	eval_aux
	movq	%rdx, %r14
	movl	CODE_ERROR(%rip), %edx
	movq	%rax, %r15
	testl	%edx, %edx
	jne	.L440
	movq	%r15, %rdi
	movq	%r14, %rsi
	cmpb	$-77, %r14b
	je	.L363
	call	neobject_destroy@PLT
	xorl	%eax, %eax
	movl	$54, %edx
	movl	$80, CODE_ERROR(%rip)
	jmp	.L421
	.p2align 4,,10
	.p2align 3
.L239:
	movl	56(%rbx), %edi
	call	get_var_value@PLT
	movq	%rax, %rdi
	movq	%rdx, %rsi
	cmpb	$54, %dl
	jne	.L442
	movl	$5, CODE_ERROR(%rip)
	jmp	.L440
	.p2align 4,,10
	.p2align 3
.L443:
	movq	24(%rbx), %rdx
	movslq	16(%rbx), %rax
	movq	(%rdx), %r12
	cmpl	$39, %eax
	je	.L459
	movq	OPERANDES(%rip), %rdx
	movl	(%rdx,%rax,4), %edx
	testb	$1, %dh
	jne	.L460
	andl	$33, %edx
	movq	%r12, %rdi
	je	.L250
	call	get_address
	movl	CODE_ERROR(%rip), %edx
	testl	%edx, %edx
	jne	.L440
	movslq	16(%rbx), %rcx
	leaq	OPFONC(%rip), %rdx
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
	.p2align 4,,10
	.p2align 3
.L453:
	.cfi_restore_state
	movq	24(%rbx), %rax
	movq	(%rax), %rdi
	call	eval_aux
	movl	CODE_ERROR(%rip), %ecx
	movq	%rax, 40(%rbx)
	movq	%rdx, 48(%rbx)
	testl	%ecx, %ecx
	je	.L278
	jmp	.L440
	.p2align 4,,10
	.p2align 3
.L455:
	movq	48(%rbx), %rsi
	movq	40(%rbx), %rdi
	call	neo_to_userfunc@PLT
	movq	48(%rbx), %r10
	movq	%rax, %r13
	movq	40(%rbx), %rax
	movl	16(%r13), %r8d
	movzbl	20(%r13), %esi
	movq	%rax, 24(%rsp)
	movzbl	48(%rbx), %eax
	movb	%al, (%rsp)
	movq	24(%rbx), %rax
	movq	8(%rax), %rax
	movl	32(%rax), %ecx
	cmpl	%r8d, %ecx
	jg	.L290
	testl	%ecx, %ecx
	jle	.L461
.L291:
	movq	24(%rax), %rdi
	movl	$0, 52(%rsp)
	xorl	%eax, %eax
	jmp	.L298
	.p2align 4,,10
	.p2align 3
.L295:
	addq	$1, %rax
	cmpl	%eax, %ecx
	jle	.L292
.L298:
	movq	(%rdi,%rax,8), %rdx
	cmpb	$2, (%rdx)
	jne	.L295
	cmpl	$37, 16(%rdx)
	jne	.L295
	testl	%r8d, %r8d
	jle	.L295
	movq	24(%rdx), %rdx
	movq	8(%r13), %r9
	movq	(%rdx), %rdx
	movl	56(%rdx), %r11d
	xorl	%edx, %edx
	jmp	.L296
	.p2align 4,,10
	.p2align 3
.L297:
	addl	$1, %edx
	addq	$4, %r9
	cmpl	%edx, %r8d
	je	.L295
.L296:
	cmpl	%r11d, (%r9)
	jne	.L297
	movl	%r8d, %r9d
	subl	24(%r13), %r9d
	cmpl	%edx, %r9d
	jg	.L295
	cmpl	%edx, %r8d
	setg	%dl
	cmpb	$1, %dl
	sbbl	$-1, 52(%rsp)
	jmp	.L295
	.p2align 4,,10
	.p2align 3
.L450:
	movq	%r14, %rbx
	jmp	.L360
	.p2align 4,,10
	.p2align 3
.L363:
	call	neo_to_container@PLT
	movl	16(%rbx), %esi
	movq	%rax, %r12
	cmpl	$-1, %esi
	je	.L462
.L364:
	movq	%r12, %rdi
	call	get_container_field@PLT
	movq	%rax, %rdi
	movq	%rdx, %rsi
	call	neo_copy@PLT
.L436:
	movq	%rax, %r13
	movq	%rdx, %r12
.L438:
	movq	%r15, %rdi
	movq	%r14, %rsi
	call	neobject_destroy@PLT
	movq	%r13, %rax
	movq	%r12, %rdx
	jmp	.L421
	.p2align 4,,10
	.p2align 3
.L350:
	movq	%r14, %rdi
	movq	%rbx, %rsi
	cmpb	$4, %bl
	je	.L351
	call	neobject_destroy@PLT
	xorl	%eax, %eax
	movl	$54, %edx
	movl	$16, CODE_ERROR(%rip)
	jmp	.L421
	.p2align 4,,10
	.p2align 3
.L454:
	movdqu	40(%rbx), %xmm3
	movq	40(%rbx), %rdi
	movq	48(%rbx), %rsi
	movaps	%xmm3, (%rsp)
	call	neo_to_function@PLT
	pxor	%xmm0, %xmm0
	movq	%rax, %r15
	movq	24(%rbx), %rax
	movups	%xmm0, 40(%rbx)
	movb	$54, 48(%rbx)
	movq	8(%rax), %r13
	movl	32(%r13), %edi
	call	nelist_create@PLT
	movl	32(%r13), %edx
	movq	%rax, %r14
	testl	%edx, %edx
	jle	.L283
	movq	%rbx, 16(%rsp)
	xorl	%r12d, %r12d
	movq	%rax, %rbx
	movq	%r13, %r14
	jmp	.L284
	.p2align 4,,10
	.p2align 3
.L465:
	call	eval_aux
	movq	%rax, 0(%r13)
	movl	CODE_ERROR(%rip), %eax
	movq	%rdx, 8(%r13)
	testl	%eax, %eax
	jne	.L463
.L282:
	addq	$1, %r12
	cmpl	%r12d, 32(%r14)
	jle	.L464
.L284:
	movq	24(%r14), %rax
	movq	%r12, %r13
	salq	$4, %r13
	addq	8(%rbx), %r13
	movq	(%rax,%r12,8), %rdi
	testq	%rdi, %rdi
	jne	.L465
	pxor	%xmm0, %xmm0
	movups	%xmm0, 0(%r13)
	movb	$54, 8(%r13)
	jmp	.L282
.L464:
	movq	%rbx, %r14
	movq	16(%rsp), %rbx
.L283:
	movdqa	(%rsp), %xmm4
	movl	CODE_ERROR(%rip), %eax
	movups	%xmm4, 40(%rbx)
	testl	%eax, %eax
	jne	.L431
	movq	%r14, %rsi
	movq	%r15, %rdi
	call	funcArgsCheck@PLT
	testb	%al, %al
	je	.L466
	movq	48(%rbx), %rsi
	movq	40(%rbx), %rdi
	movq	%r14, %rdx
	call	functionCall@PLT
	movq	%r14, %rdi
	movq	%rax, %r13
	movq	%rdx, %r12
	call	nelist_destroy@PLT
	movq	40(%rbx), %rdi
	movq	48(%rbx), %rsi
	call	neobject_destroy@PLT
	movl	CODE_ERROR(%rip), %eax
	pxor	%xmm0, %xmm0
	movups	%xmm0, 40(%rbx)
	movb	$54, 48(%rbx)
	testl	%eax, %eax
	jne	.L437
	movl	4(%rbx), %eax
	movq	%r12, %rdx
	movl	%eax, LINENUMBER(%rip)
	movq	%r13, %rax
	jmp	.L421
	.p2align 4,,10
	.p2align 3
.L456:
	movl	$8, CODE_ERROR(%rip)
	jmp	.L431
.L461:
	xorl	%eax, %eax
	movl	%eax, 52(%rsp)
.L292:
	testb	%sil, %sil
	jne	.L367
.L299:
	movl	%r8d, %edi
	movq	%r10, 16(%rsp)
	call	nelist_create@PLT
	movq	16(%rsp), %r10
	movslq	16(%rax), %rdx
	movq	%rax, %r14
	testl	%edx, %edx
	jle	.L304
	movq	8(%rax), %rax
	salq	$4, %rdx
	addq	%rax, %rdx
	.p2align 4,,10
	.p2align 3
.L303:
	pxor	%xmm0, %xmm0
	addq	$16, %rax
	movups	%xmm0, -16(%rax)
	movb	$54, -8(%rax)
	cmpq	%rax, %rdx
	jne	.L303
.L304:
	movl	16(%r13), %eax
	movl	%eax, %r12d
	subl	24(%r13), %r12d
	movslq	%r12d, %rcx
	salq	$4, %rcx
	cmpl	%r12d, %eax
	jle	.L302
	movq	%r10, 16(%rsp)
	movq	%r14, %r15
	movq	%r13, %r14
	movq	%rcx, %r13
	.p2align 4,,10
	.p2align 3
.L308:
	testl	%r12d, %r12d
	jne	.L307
	cmpb	$-79, 48(%rbx)
	je	.L467
.L307:
	movq	40(%r14), %rdi
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
	jg	.L308
	movq	16(%rsp), %r10
	movq	%r14, %r13
	movq	%r15, %r14
.L302:
	movq	$0, 56(%rsp)
	movq	24(%rbx), %rdi
	xorl	%r15d, %r15d
	movq	8(%rdi), %rdx
	movl	32(%rdx), %esi
	testl	%esi, %esi
	jle	.L306
	movq	%r14, %r8
	movq	%rbx, %r14
	jmp	.L305
	.p2align 4,,10
	.p2align 3
.L309:
	addq	$1, %r15
	cmpl	%r15d, %esi
	jle	.L468
.L305:
	movq	24(%rdx), %rax
	leaq	0(,%r15,8), %r12
	movq	(%rax,%r15,8), %rax
	cmpb	$2, (%rax)
	jne	.L309
	cmpl	$37, 16(%rax)
	jne	.L309
	movq	24(%rax), %rax
	movq	(%rax), %rax
	cmpb	$10, (%rax)
	jne	.L425
	movl	16(%r13), %esi
	testl	%esi, %esi
	jle	.L369
	movl	56(%rax), %edi
	xorl	%r9d, %r9d
	movq	8(%r13), %rax
	jmp	.L312
	.p2align 4,,10
	.p2align 3
.L314:
	addl	$1, %r9d
	addq	$4, %rax
	cmpl	%esi, %r9d
	je	.L425
.L312:
	cmpl	%edi, (%rax)
	jne	.L314
.L311:
	cmpl	%esi, %r9d
	je	.L425
	movslq	%r9d, %rbx
	subl	24(%r13), %esi
	movq	8(%r8), %rax
	salq	$4, %rbx
	cmpl	%r9d, %esi
	jle	.L316
	movq	(%rax,%rbx), %rdi
	movq	8(%rax,%rbx), %rsi
	movq	%r8, 32(%rsp)
	movq	%r10, 40(%rsp)
	movl	%r9d, 16(%rsp)
	call	neo_is_void@PLT
	movq	32(%rsp), %r8
	testb	%al, %al
	je	.L317
	movq	24(%r14), %rax
	movl	16(%rsp), %r9d
	movq	40(%rsp), %r10
	movq	8(%rax), %rdx
.L316:
	pxor	%xmm2, %xmm2
	movq	%r10, 40(%rsp)
	movups	%xmm2, 40(%r14)
	movb	$54, 48(%r14)
	movq	24(%rdx), %rax
	movq	%r8, 32(%rsp)
	movq	(%rax,%r12), %rax
	movl	%r9d, 16(%rsp)
	movq	24(%rax), %rax
	movq	8(%rax), %rdi
	call	eval_aux
	movq	32(%rsp), %r8
	movq	40(%rsp), %r10
	movb	(%rsp), %r10b
	movl	16(%rsp), %r9d
	movq	8(%r8), %rcx
	movq	%rax, (%rcx,%rbx)
	movq	24(%rsp), %rax
	movq	%rdx, 8(%rcx,%rbx)
	movq	%rax, 40(%r14)
	movl	CODE_ERROR(%rip), %eax
	movq	%r10, 48(%r14)
	testl	%eax, %eax
	jne	.L469
	movq	24(%r14), %rdi
	movq	8(%rdi), %rdx
	testl	%r9d, %r9d
	jne	.L373
	cmpb	$-79, (%rsp)
	je	.L319
.L373:
	movl	32(%rdx), %esi
	jmp	.L309
.L351:
	call	neo_to_integer@PLT
	movq	%r14, %rdi
	movq	%rbx, %rsi
	movq	%rax, (%rsp)
	call	neobject_destroy@PLT
	cmpb	$-122, %r15b
	je	.L352
	cmpq	$0, (%rsp)
	js	.L354
	movb	$-125, %r12b
	movq	%r13, %rdi
	movq	%r12, %rsi
	call	neo_to_string@PLT
	movq	%rax, %rdi
	call	strlen@PLT
	movq	(%rsp), %rbx
	cmpq	%rax, %rbx
	jnb	.L354
	movq	%r12, %rsi
	movq	%r13, %rdi
	call	neo_to_string@PLT
	movsbl	(%rax,%rbx), %edi
	call	charToString@PLT
	movq	%rax, %rdi
	call	neo_str_create@PLT
	movl	%edx, %ecx
	movb	%cl, %dl
	jmp	.L421
.L425:
	movq	%r8, %rdi
	call	nelist_destroy@PLT
	movq	40(%r14), %rdi
	movq	48(%r14), %rsi
	call	neobject_destroy@PLT
	pxor	%xmm0, %xmm0
	xorl	%eax, %eax
	movl	$54, %edx
	movups	%xmm0, 40(%r14)
	movl	$93, CODE_ERROR(%rip)
	movb	$54, 48(%r14)
	jmp	.L421
.L460:
	leaq	OPFONC(%rip), %rdx
	movq	%r12, %rdi
	movq	(%rdx,%rax,8), %rax
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
.L290:
	.cfi_restore_state
	testb	%sil, %sil
	je	.L470
	testl	%ecx, %ecx
	jg	.L291
	xorl	%eax, %eax
	movl	%eax, 52(%rsp)
.L367:
	movl	24(%r13), %eax
	movl	%r8d, %edx
	movl	52(%rsp), %esi
	subl	%eax, %edx
	addl	%ecx, %eax
	subl	%esi, %eax
	cmpl	%edx, %ecx
	cmovg	%eax, %r8d
	jmp	.L299
.L250:
	call	eval_aux
	movl	CODE_ERROR(%rip), %r15d
	movq	%rax, %rdi
	movq	%rdx, %rsi
	testl	%r15d, %r15d
	jne	.L440
	movq	%rdx, %r14
	movslq	16(%rbx), %rdx
	movq	%rax, %r15
	leaq	OPFONC(%rip), %rax
	call	*(%rax,%rdx,8)
	jmp	.L436
.L468:
	movq	%r14, %rbx
	movq	%r8, %r14
.L306:
	movl	16(%r14), %r12d
	xorl	%r15d, %r15d
	testl	%r12d, %r12d
	jle	.L471
	movq	%rbx, 16(%rsp)
	movq	%r14, %rbx
	movq	%r15, %r14
	movq	%r10, %r15
	jmp	.L321
	.p2align 4,,10
	.p2align 3
.L324:
	movl	16(%rbx), %eax
	addq	$1, %r14
	cmpl	%r14d, %eax
	jle	.L472
.L321:
	movq	%r14, %rax
	movl	%r14d, %r12d
	salq	$4, %rax
	addq	8(%rbx), %rax
	movq	(%rax), %rdi
	movq	8(%rax), %rsi
	call	neo_is_void@PLT
	testb	%al, %al
	je	.L324
	movq	%r15, %r10
	movq	%r14, %r15
	movq	%rbx, %r14
	movq	16(%rsp), %rbx
	movl	16(%r14), %eax
.L323:
	movq	24(%rbx), %rdx
	movq	8(%rdx), %r8
	movl	32(%r8), %edx
	cmpl	%eax, %r15d
	jge	.L322
	xorl	%r12d, %r12d
	movq	%r13, 40(%rsp)
	movq	%rbx, %r13
	movl	%r15d, %ebx
	movq	%r12, %r15
	jmp	.L325
	.p2align 4,,10
	.p2align 3
.L327:
	addq	$1, %r15
	cmpl	%eax, %ebx
	jge	.L430
.L325:
	cmpl	%r15d, %edx
	jle	.L430
	movq	24(%r8), %rdi
	leaq	0(,%r15,8), %rsi
	movq	%rsi, 32(%rsp)
	movq	(%rdi,%r15,8), %rdi
	cmpb	$2, (%rdi)
	jne	.L326
	cmpl	$37, 16(%rdi)
	je	.L327
.L326:
	pxor	%xmm1, %xmm1
	movq	%r10, 16(%rsp)
	movslq	%ebx, %r12
	movups	%xmm1, 40(%r13)
	salq	$4, %r12
	movb	$54, 48(%r13)
	call	eval_aux
	movq	8(%r14), %rdi
	movq	16(%rsp), %r10
	movl	CODE_ERROR(%rip), %r11d
	movb	(%rsp), %r10b
	movq	%rax, (%rdi,%r12)
	movq	24(%rsp), %rax
	movq	%rdx, 8(%rdi,%r12)
	movq	%rax, 40(%r13)
	movq	%r10, 48(%r13)
	testl	%r11d, %r11d
	jne	.L473
	cmpb	$-79, (%rsp)
	jne	.L329
	testl	%ebx, %ebx
	je	.L474
.L329:
	cmpl	16(%r14), %ebx
	jge	.L427
	movq	%r15, 16(%rsp)
	movq	%r14, %r15
	movl	%ebx, %r14d
	movq	%r10, %rbx
	jmp	.L330
	.p2align 4,,10
	.p2align 3
.L332:
	addl	$1, %r14d
	addq	$16, %r12
	cmpl	%r14d, 16(%r15)
	jle	.L475
.L330:
	movq	8(%r15), %rax
	movq	(%rax,%r12), %rdi
	movq	8(%rax,%r12), %rsi
	call	neo_is_void@PLT
	testb	%al, %al
	je	.L332
	movq	24(%r13), %rdx
	movq	%rbx, %r10
	movl	%r14d, %ebx
	movq	%r15, %r14
	movl	16(%r14), %eax
	movq	16(%rsp), %r15
	movq	8(%rdx), %r8
	movl	32(%r8), %edx
	jmp	.L327
	.p2align 4,,10
	.p2align 3
.L475:
	movq	%rbx, %r10
	movq	%r13, %rbx
	movq	40(%rsp), %r13
	movq	%r15, %r14
.L331:
	movq	24(%rbx), %rax
	movq	8(%rax), %rax
	movl	32(%rax), %edx
.L322:
	movl	16(%r13), %eax
	cmpl	%edx, %eax
	jg	.L476
.L334:
	xorl	%ecx, %ecx
	cmpb	$0, 20(%r13)
	movl	$54, %r8d
	je	.L338
	movl	16(%r14), %r12d
	movl	52(%rsp), %esi
	movl	%eax, %edx
	movq	%r10, 16(%rsp)
	subl	24(%r13), %edx
	movl	%r12d, %ecx
	subl	%esi, %ecx
	cmpl	%edx, %ecx
	jg	.L339
	xorl	%edi, %edi
	call	nelist_create@PLT
	movq	16(%rsp), %r10
.L340:
	movq	%rax, %rdi
	movq	%r10, 16(%rsp)
	call	neo_list_convert@PLT
	movq	16(%rsp), %r10
	movq	%rax, %rcx
	movq	%rdx, %r8
.L338:
	pxor	%xmm0, %xmm0
	cmpb	$-79, (%rsp)
	movq	%r10, 16(%rsp)
	movups	%xmm0, 40(%rbx)
	movb	$54, 48(%rbx)
	je	.L477
	movq	%rcx, %rdx
	movq	%r13, %rdi
	movq	%r8, %rcx
	movq	%r14, %rsi
	call	callUserFunc
	movq	16(%rsp), %r10
	movq	%rax, %r13
	movq	%rdx, %r12
.L344:
	movq	24(%rsp), %rax
	movb	(%rsp), %r10b
	movq	%rax, 40(%rbx)
	movq	%rax, %rdi
	movq	%r10, %rsi
	movq	%r10, 48(%rbx)
	call	neobject_destroy@PLT
	pxor	%xmm0, %xmm0
	movq	%r14, %rdi
	movups	%xmm0, 40(%rbx)
	movb	$54, 48(%rbx)
	call	nelist_destroy@PLT
	movl	CODE_ERROR(%rip), %r9d
	xorl	%eax, %eax
	movl	$54, %edx
	testl	%r9d, %r9d
	cmove	%r13, %rax
	cmove	%r12, %rdx
	jmp	.L421
.L444:
	movq	24(%rbx), %rax
	movq	8(%rax), %rsi
	movq	(%rax), %rdi
	leaq	OPFONC(%rip), %rax
	movq	(%rax,%rcx,8), %rax
	jmp	.L441
.L459:
	movl	$64, %edi
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
	movl	CODE_ERROR(%rip), %ecx
	movq	%rax, 40(%rbx)
	movq	%rdx, 48(%rbx)
	testl	%ecx, %ecx
	jne	.L433
	cmpb	$-78, 48(%rbx)
	je	.L248
	movq	40(%rbx), %rdi
	movq	48(%rbx), %rsi
	movl	$100, CODE_ERROR(%rip)
	call	neobject_destroy@PLT
.L433:
	movq	%rbx, %rdi
	call	free@PLT
	jmp	.L440
.L462:
	movq	8(%rbx), %rsi
	movq	%rax, %rdi
	call	get_field_index@PLT
	movq	8(%rbx), %rdi
	movl	%eax, 16(%rbx)
	call	free@PLT
	movq	$0, 8(%rbx)
	movl	16(%rbx), %esi
	jmp	.L364
.L467:
	movl	$110, CODE_ERROR(%rip)
	movq	%r15, %rdi
	call	nelist_destroy@PLT
	jmp	.L431
.L463:
	movq	%rbx, %r14
	leal	-1(%r12), %esi
	movq	16(%rsp), %rbx
	movq	%r14, %rdi
	xorl	%r14d, %r14d
	call	nelist_destroy_until@PLT
	jmp	.L283
.L474:
	movq	24(%r13), %rax
	movq	32(%rsp), %rsi
	movq	%r10, 16(%rsp)
	movq	8(%rax), %rax
	movq	24(%rax), %rax
	movq	(%rax,%rsi), %rdi
	call	get_address
	movq	16(%rsp), %r10
	movq	%rax, 56(%rsp)
	jmp	.L329
.L466:
	movl	$14, CODE_ERROR(%rip)
	movq	%r14, %rdi
	call	nelist_destroy@PLT
	jmp	.L431
.L354:
	movl	$18, CODE_ERROR(%rip)
	jmp	.L440
.L472:
	movq	%rbx, %r14
	movq	%r15, %r10
	movq	16(%rsp), %rbx
	leal	1(%r12), %r15d
	jmp	.L323
.L437:
	movq	%r13, %rdi
	movq	%r12, %rsi
	call	neobject_destroy@PLT
	xorl	%eax, %eax
	movl	$54, %edx
	jmp	.L421
.L430:
	movq	%r13, %rbx
	movq	40(%rsp), %r13
	jmp	.L322
.L445:
	movl	$95, CODE_ERROR(%rip)
	xorl	%eax, %eax
	movl	$54, %edx
	jmp	.L421
.L447:
	call	get_address
	movl	CODE_ERROR(%rip), %r14d
	movq	%rax, %r12
	testl	%r14d, %r14d
	jne	.L440
	movq	24(%rbx), %rax
	movq	8(%rax), %rdi
	call	eval_aux
	movl	CODE_ERROR(%rip), %r13d
	movq	%rax, %r15
	movq	%rdx, %r14
	testl	%r13d, %r13d
	jne	.L440
	movslq	16(%rbx), %rcx
	movq	%rax, %rsi
	leaq	OPFONC(%rip), %rax
	movq	%r12, %rdi
	call	*(%rax,%rcx,8)
	jmp	.L436
.L427:
	movq	%r13, %rbx
	movq	40(%rsp), %r13
	jmp	.L331
.L446:
	movl	$92, CODE_ERROR(%rip)
	xorl	%eax, %eax
	movl	$54, %edx
	jmp	.L421
.L476:
	testl	%eax, %eax
	jle	.L334
	movq	%rbx, 16(%rsp)
	xorl	%r12d, %r12d
	movq	%r14, %rbx
	movq	%r10, %r14
.L337:
	movq	8(%rbx), %rax
	movq	%r12, %r15
	salq	$4, %r15
	movq	(%rax,%r15), %rdi
	movq	8(%rax,%r15), %rsi
	call	neo_is_void@PLT
	testb	%al, %al
	jne	.L478
.L335:
	movl	16(%r13), %eax
	addq	$1, %r12
	cmpl	%r12d, %eax
	jg	.L337
	movq	%r14, %r10
	movq	%rbx, %r14
	movq	16(%rsp), %rbx
	jmp	.L334
	.p2align 4,,10
	.p2align 3
.L266:
	call	eval_aux
	movl	CODE_ERROR(%rip), %r10d
	movq	%rax, %r13
	movq	%rdx, %r12
	testl	%r10d, %r10d
	jne	.L437
	movq	24(%rbx), %rax
	movq	8(%rax), %rdi
	call	eval_aux
	movl	CODE_ERROR(%rip), %r9d
	movq	%r13, %rdi
	movq	%r12, %rsi
	movq	%rax, %r15
	movq	%rdx, %r14
	testl	%r9d, %r9d
	jne	.L479
	movslq	16(%rbx), %r8
	movq	%rax, %rdx
	movq	%r13, 16(%rsp)
	movq	%r14, %rcx
	leaq	OPFONC(%rip), %rax
	movq	%r12, (%rsp)
	call	*(%rax,%r8,8)
	movq	16(%rsp), %rdi
	movq	(%rsp), %rsi
	movq	%rax, %r13
	movq	%rdx, %r12
	call	neobject_destroy@PLT
	jmp	.L438
.L448:
	call	eval_aux
	movl	CODE_ERROR(%rip), %r12d
	movq	%rax, %r15
	movq	%rdx, %r14
	testl	%r12d, %r12d
	jne	.L440
	movq	24(%rbx), %rax
	movq	8(%rax), %rdi
	call	get_address
	cmpl	$0, CODE_ERROR(%rip)
	movq	%rax, %rdx
	jne	.L440
	movslq	16(%rbx), %rcx
	leaq	OPFONC(%rip), %rax
	movq	%r15, %rdi
	movq	%r14, %rsi
	call	*(%rax,%rcx,8)
	jmp	.L436
.L352:
	movq	%r12, %rsi
	movq	%r13, %rdi
	call	neo_list_len@PLT
	movq	(%rsp), %rsi
	cltq
	cmpq	%rsi, %rax
	jle	.L354
	testq	%rsi, %rsi
	js	.L354
	movl	(%rsp), %edx
	movb	$-122, %r12b
	movq	%r13, %rdi
	movq	%r12, %rsi
	call	neo_list_nth@PLT
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	neo_copy@PLT
	movl	%edx, %ecx
	movb	%cl, %dl
	jmp	.L421
.L248:
	movq	%rbx, %rdi
	xorl	%edx, %edx
	movl	$1, %esi
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
.L470:
	.cfi_restore_state
	movl	$6, CODE_ERROR(%rip)
	movq	24(%rsp), %rdi
	movq	%r10, %rsi
	jmp	.L439
.L319:
	movq	24(%rdx), %rax
	movq	%r10, 32(%rsp)
	movq	%r8, 16(%rsp)
	movq	(%rax,%r12), %rax
	movq	24(%rax), %rax
	movq	8(%rax), %rdi
	call	get_address
	movq	24(%r14), %rdi
	movq	32(%rsp), %r10
	movq	%rax, 56(%rsp)
	movq	16(%rsp), %r8
	movq	8(%rdi), %rdx
	movl	32(%rdx), %esi
	jmp	.L309
.L478:
	movq	40(%r13), %rdi
	movl	%r12d, %esi
	call	nelist_nth@PLT
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	neo_copy@PLT
	movq	8(%rbx), %rcx
	movq	%rax, (%rcx,%r15)
	movq	8(%rbx), %rax
	movq	%rdx, 8(%rcx,%r15)
	movq	(%rax,%r15), %rdi
	movq	8(%rax,%r15), %rsi
	call	neo_is_void@PLT
	testb	%al, %al
	jne	.L480
	testq	%r12, %r12
	jne	.L335
	movl	$110, CODE_ERROR(%rip)
	movq	%rbx, %r14
	movq	16(%rsp), %rbx
	movq	%r14, %rdi
	call	nelist_destroy@PLT
	jmp	.L431
.L473:
	movq	%r14, %rdi
	call	nelist_destroy@PLT
	movq	40(%r13), %rdi
	movq	48(%r13), %rsi
	call	neobject_destroy@PLT
	pxor	%xmm6, %xmm6
	xorl	%eax, %eax
	movl	$54, %edx
	movups	%xmm6, 40(%r13)
	movb	$54, 48(%r13)
	jmp	.L421
.L477:
	movq	56(%rsp), %rsi
	movq	%r13, %rdi
	movq	%r14, %rdx
	call	callUserMethod
	movq	16(%rsp), %r10
	movq	%rax, %r13
	movq	%rdx, %r12
	jmp	.L344
.L339:
	subl	%eax, %r12d
	movl	%r12d, %edi
	call	nelist_create@PLT
	testl	%r12d, %r12d
	movq	16(%rsp), %r10
	jle	.L340
	movq	%r10, 32(%rsp)
	movslq	%r12d, %r15
	xorl	%r12d, %r12d
	movq	%rbx, 16(%rsp)
	movq	%rax, %rbx
	.p2align 4,,10
	.p2align 3
.L342:
	movl	16(%r13), %esi
	movq	%r14, %rdi
	addl	%r12d, %esi
	call	nelist_nth@PLT
	movq	%rdx, %rdi
	movq	%r12, %rdx
	movq	%rax, %rsi
	addq	$1, %r12
	salq	$4, %rdx
	addq	8(%rbx), %rdx
	movq	%rsi, (%rdx)
	movq	%rdi, 8(%rdx)
	cmpq	%r15, %r12
	jne	.L342
	movq	%rbx, %rax
	movq	32(%rsp), %r10
	movq	16(%rsp), %rbx
	jmp	.L340
.L369:
	xorl	%r9d, %r9d
	jmp	.L311
.L479:
	call	neobject_destroy@PLT
	movq	%r15, %rdi
	movq	%r14, %rsi
	call	neobject_destroy@PLT
	xorl	%eax, %eax
	movl	$54, %edx
	jmp	.L421
.L471:
	movq	8(%rdi), %rax
	movl	32(%rax), %edx
	jmp	.L322
.L469:
	movq	%r8, %rdi
	call	nelist_destroy@PLT
	movq	40(%r14), %rdi
	movq	48(%r14), %rsi
	call	neobject_destroy@PLT
	pxor	%xmm5, %xmm5
	xorl	%eax, %eax
	movl	$54, %edx
	movups	%xmm5, 40(%r14)
	movb	$54, 48(%r14)
	jmp	.L421
.L317:
	movq	%r8, %rdi
	call	nelist_destroy@PLT
	movq	40(%r14), %rdi
	movq	48(%r14), %rsi
	call	neobject_destroy@PLT
	pxor	%xmm0, %xmm0
	xorl	%eax, %eax
	movl	$54, %edx
	movups	%xmm0, 40(%r14)
	movl	$94, CODE_ERROR(%rip)
	movb	$54, 48(%r14)
	jmp	.L421
.L480:
	movl	$7, CODE_ERROR(%rip)
	movq	%rbx, %r14
	movq	16(%rsp), %rbx
	movq	40(%rbx), %rdi
	movq	48(%rbx), %rsi
	call	neobject_destroy@PLT
	pxor	%xmm0, %xmm0
	movq	%r14, %rdi
	movups	%xmm0, 40(%rbx)
	movb	$54, 48(%rbx)
	call	nelist_destroy@PLT
	xorl	%eax, %eax
	movl	$54, %edx
	jmp	.L421
	.cfi_endproc
.LFE24:
	.size	eval_aux, .-eval_aux
	.p2align 4
	.globl	eval_prolog
	.type	eval_prolog, @function
eval_prolog:
.LFB20:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movq	%rdi, %rbx
	movq	%rsi, %rdi
	call	eval_aux
	movq	%rax, (%rbx)
	movq	%rbx, %rax
	movq	%rdx, 8(%rbx)
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE20:
	.size	eval_prolog, .-eval_prolog
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
	subq	$16, %rsp
	.cfi_def_cfa_offset 32
	call	eval_aux
	movl	CODE_ERROR(%rip), %ecx
	testl	%ecx, %ecx
	je	.L490
	addq	$16, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 16
	movl	%r12d, %eax
	popq	%r12
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L490:
	.cfi_restore_state
	movq	%rax, %rdi
	movq	%rdx, %rsi
	cmpb	$8, %dl
	je	.L491
	cmpb	$4, %dl
	je	.L492
	movl	$20, CODE_ERROR(%rip)
.L486:
	movq	%rdx, %rsi
	call	neobject_destroy@PLT
	addq	$16, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 16
	movl	%r12d, %eax
	popq	%r12
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L491:
	.cfi_restore_state
	movb	$8, %sil
	movq	%rdx, 8(%rsp)
	movq	%rax, (%rsp)
	call	neo_to_bool@PLT
	movq	(%rsp), %rdi
	movq	8(%rsp), %rdx
	movl	%eax, %r12d
	jmp	.L486
	.p2align 4,,10
	.p2align 3
.L492:
	movb	$4, %sil
	movq	%rdx, 8(%rsp)
	movq	%rax, (%rsp)
	call	neo_to_integer@PLT
	movq	(%rsp), %rdi
	movq	8(%rsp), %rdx
	testq	%rax, %rax
	setne	%r12b
	jmp	.L486
	.cfi_endproc
.LFE14:
	.size	isTrue, .-isTrue
	.p2align 4
	.globl	treeToList
	.type	treeToList, @function
treeToList:
.LFB21:
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
	jle	.L493
	xorl	%ebx, %ebx
	jmp	.L498
	.p2align 4,,10
	.p2align 3
.L502:
	call	eval_aux
	movq	%rax, (%r12)
	movl	CODE_ERROR(%rip), %eax
	movq	%rdx, 8(%r12)
	testl	%eax, %eax
	jne	.L501
	addq	$1, %rbx
	cmpl	%ebx, 32(%r13)
	jle	.L493
.L498:
	movq	24(%r13), %rax
	movq	%rbx, %r12
	salq	$4, %r12
	addq	8(%r14), %r12
	movq	(%rax,%rbx,8), %rdi
	testq	%rdi, %rdi
	jne	.L502
	pxor	%xmm0, %xmm0
	addq	$1, %rbx
	movups	%xmm0, (%r12)
	movb	$54, 8(%r12)
	cmpl	%ebx, 32(%r13)
	jg	.L498
.L493:
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
.L501:
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
.LFE21:
	.size	treeToList, .-treeToList
	.p2align 4
	.globl	get_address
	.type	get_address, @function
get_address:
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
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	movq	%rdi, %rbx
	subq	$16, %rsp
	.cfi_def_cfa_offset 64
	movzbl	(%rdi), %eax
	cmpb	$10, %al
	je	.L531
	cmpb	$1, %al
	je	.L532
	cmpb	$52, %al
	je	.L533
	movl	$89, CODE_ERROR(%rip)
.L530:
	xorl	%eax, %eax
.L503:
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
.L533:
	.cfi_restore_state
	movq	24(%rdi), %rax
	movq	(%rax), %rdi
	call	eval_aux
	movq	%rax, %r13
	movl	CODE_ERROR(%rip), %eax
	movq	%rdx, %r12
	testl	%eax, %eax
	jne	.L530
	movq	%r13, %rdi
	movq	%rdx, %rsi
	cmpb	$-77, %dl
	je	.L519
	call	neobject_destroy@PLT
	movl	$80, CODE_ERROR(%rip)
	jmp	.L530
	.p2align 4,,10
	.p2align 3
.L532:
	movq	24(%rdi), %rax
	movq	(%rax), %rdi
	call	get_address
	movl	CODE_ERROR(%rip), %ecx
	movq	(%rax), %r13
	movq	8(%rax), %r12
	movzbl	8(%rax), %r15d
	testl	%ecx, %ecx
	jne	.L530
	movq	24(%rbx), %rax
	movq	8(%rax), %rdi
	call	eval_aux
	movq	%rdx, %rbx
	movl	CODE_ERROR(%rip), %edx
	movq	%rax, %r14
	testl	%edx, %edx
	jne	.L530
	cmpb	$-122, %r15b
	je	.L509
	cmpb	$-125, %r15b
	je	.L509
	movl	$15, CODE_ERROR(%rip)
	jmp	.L530
	.p2align 4,,10
	.p2align 3
.L531:
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
.L519:
	.cfi_restore_state
	call	neo_to_container@PLT
	movl	16(%rbx), %esi
	movq	%rax, %r14
	cmpl	$-1, %esi
	je	.L534
.L521:
	movq	%r14, %rdi
	call	get_container_field_addr@PLT
	movq	%r13, %rdi
	movq	%r12, %rsi
	movq	%rax, 8(%rsp)
	call	neobject_destroy@PLT
	movq	8(%rsp), %rax
	jmp	.L503
	.p2align 4,,10
	.p2align 3
.L509:
	movq	%r14, %rdi
	movq	%rbx, %rsi
	cmpb	$4, %bl
	je	.L510
	call	neobject_destroy@PLT
	movl	$16, CODE_ERROR(%rip)
	jmp	.L530
	.p2align 4,,10
	.p2align 3
.L534:
	movq	8(%rbx), %rsi
	movq	%rax, %rdi
	call	get_field_index@PLT
	movq	8(%rbx), %rdi
	movl	%eax, 16(%rbx)
	call	free@PLT
	movq	$0, 8(%rbx)
	movl	16(%rbx), %esi
	jmp	.L521
	.p2align 4,,10
	.p2align 3
.L510:
	call	neo_to_integer@PLT
	movq	%r14, %rdi
	movq	%rbx, %rsi
	movq	%rax, 8(%rsp)
	call	neobject_destroy@PLT
	cmpb	$-122, %r15b
	je	.L511
	cmpq	$0, 8(%rsp)
	js	.L513
	movb	$-125, %r12b
	movq	%r13, %rdi
	movq	%r12, %rsi
	call	neo_to_string@PLT
	movq	%rax, %rdi
	call	strlen@PLT
	cmpq	%rax, 8(%rsp)
	jnb	.L513
	movl	$105, CODE_ERROR(%rip)
	jmp	.L530
.L513:
	movl	$18, CODE_ERROR(%rip)
	jmp	.L530
.L511:
	movq	%r13, %rdi
	movq	%r12, %rsi
	call	neo_list_len@PLT
	movq	8(%rsp), %rcx
	testq	%rcx, %rcx
	js	.L513
	cltq
	cmpq	%rcx, %rax
	jle	.L513
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
.LFE25:
	.size	get_address, .-get_address
	.p2align 4
	.globl	execConditionBlock
	.type	execConditionBlock, @function
execConditionBlock:
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
	movq	%rdi, %r13
	pushq	%r12
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	subq	$32, %rsp
	.cfi_def_cfa_offset 80
	movq	process_cycle(%rip), %rax
	movl	32(%r13), %r11d
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	movq	(%rdi), %rax
	movq	%rax, 24(%rsp)
	testl	%r11d, %r11d
	jle	.L536
	xorl	%ebx, %ebx
.L564:
	movq	24(%r13), %rax
	movslq	%ebx, %r14
	leaq	0(,%r14,8), %r12
	movq	(%rax,%r14,8), %rax
	movq	24(%rax), %rax
	movq	(%rax), %rdi
	call	eval_aux
	movl	CODE_ERROR(%rip), %r10d
	movq	%rax, %rdi
	testl	%r10d, %r10d
	jne	.L538
	movq	%rdx, %rsi
	cmpb	$8, %dl
	je	.L574
	cmpb	$4, %dl
	je	.L575
	movl	$20, CODE_ERROR(%rip)
	call	neobject_destroy@PLT
	movl	CODE_ERROR(%rip), %r9d
	testl	%r9d, %r9d
	jne	.L538
.L543:
	movl	32(%r13), %edx
	addl	$1, %ebx
	cmpl	%edx, %ebx
	jge	.L550
	addq	$8, %r12
	xorl	%r15d, %r15d
	jmp	.L551
	.p2align 4,,10
	.p2align 3
.L557:
	movl	32(%r13), %edx
	addl	$1, %ebx
	addq	$8, %r12
	cmpl	%ebx, %edx
	jle	.L549
.L551:
	movq	24(%r13), %rcx
	movq	(%rcx,%r12), %rax
	cmpb	$31, (%rax)
	jne	.L576
	movq	24(%rax), %rax
	movq	(%rax), %rdi
	call	eval_aux
	movq	%rax, %rdi
	movq	%rdx, %rsi
	cmpb	$8, %dl
	je	.L577
	cmpb	$4, %dl
	je	.L578
	movl	$20, CODE_ERROR(%rip)
	call	neobject_destroy@PLT
	movl	CODE_ERROR(%rip), %esi
	testl	%esi, %esi
	je	.L557
.L538:
	xorl	%r15d, %r15d
.L566:
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
.L577:
	.cfi_restore_state
	movb	$8, %sil
	movq	%rdx, 16(%rsp)
	movq	%rax, 8(%rsp)
	call	neo_to_bool@PLT
	movq	8(%rsp), %rdi
	movq	16(%rsp), %rdx
	movl	%eax, %r14d
.L553:
	movq	%rdx, %rsi
	call	neobject_destroy@PLT
	movl	CODE_ERROR(%rip), %ecx
	testl	%ecx, %ecx
	jne	.L538
	movl	%r15d, %eax
	xorl	$1, %eax
	andb	%al, %r14b
	je	.L557
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	104(%rax), %r15
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
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rdi
	call	deleteContext
	movl	CODE_ERROR(%rip), %edx
	testl	%edx, %edx
	jne	.L538
	testl	%r15d, %r15d
	jne	.L566
	movl	32(%r13), %edx
	addl	$1, %ebx
	movl	%r14d, %r15d
	addq	$8, %r12
	cmpl	%ebx, %edx
	jg	.L551
	.p2align 4,,10
	.p2align 3
.L549:
	cmpl	%edx, %ebx
	jl	.L564
	.p2align 4,,10
	.p2align 3
.L550:
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	120(%rax), %rdi
.L536:
	movq	24(%rsp), %rsi
	xorl	%r15d, %r15d
	call	partialRestore@PLT
	jmp	.L566
	.p2align 4,,10
	.p2align 3
.L578:
	movb	$4, %sil
	movq	%rdx, 16(%rsp)
	movq	%rax, 8(%rsp)
	call	neo_to_integer@PLT
	movq	8(%rsp), %rdi
	movq	16(%rsp), %rdx
	testq	%rax, %rax
	setne	%r14b
	jmp	.L553
	.p2align 4,,10
	.p2align 3
.L576:
	cmpl	%ebx, %edx
	jle	.L549
	movslq	%ebx, %r12
	movq	(%rcx,%r12,8), %rax
	cmpb	$32, (%rax)
	jne	.L549
	testb	%r15b, %r15b
	je	.L562
.L563:
	addl	$1, %ebx
	cmpl	%edx, %ebx
	jl	.L564
	jmp	.L550
	.p2align 4,,10
	.p2align 3
.L574:
	movb	$8, %sil
	movq	%rdx, 16(%rsp)
	movq	%rax, 8(%rsp)
	call	neo_to_bool@PLT
	movq	8(%rsp), %rdi
	movq	16(%rsp), %rdx
	movl	%eax, %r15d
.L540:
	movq	%rdx, %rsi
	call	neobject_destroy@PLT
	movl	CODE_ERROR(%rip), %r8d
	testl	%r8d, %r8d
	jne	.L538
	testb	%r15b, %r15b
	je	.L543
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	104(%rax), %r12
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
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rdi
	call	deleteContext
	movl	CODE_ERROR(%rip), %edi
	testl	%edi, %edi
	jne	.L538
	testl	%r15d, %r15d
	jne	.L566
	movl	32(%r13), %edx
	addl	$1, %ebx
	cmpl	%ebx, %edx
	jle	.L550
	movq	24(%r13), %rcx
	movslq	%ebx, %rbx
	jmp	.L546
	.p2align 4,,10
	.p2align 3
.L548:
	addq	$1, %rbx
	cmpl	%ebx, %edx
	jle	.L579
.L546:
	movq	(%rcx,%rbx,8), %rax
	movl	%ebx, %esi
	cmpb	$30, (%rax)
	jne	.L548
	cmpl	%ebx, %edx
	je	.L550
.L573:
	cmpl	%edx, %ebx
	jl	.L564
	jmp	.L550
	.p2align 4,,10
	.p2align 3
.L579:
	leal	1(%rsi), %ebx
	cmpl	%ebx, %edx
	jne	.L573
	jmp	.L550
	.p2align 4,,10
	.p2align 3
.L575:
	movb	$4, %sil
	movq	%rdx, 16(%rsp)
	movq	%rax, 8(%rsp)
	call	neo_to_integer@PLT
	movq	8(%rsp), %rdi
	movq	16(%rsp), %rdx
	testq	%rax, %rax
	setne	%r15b
	jmp	.L540
.L562:
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	104(%rax), %r14
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%r14, %rdi
	call	ptrlist_append@PLT
	movq	24(%r13), %rax
	movq	(%rax,%r12,8), %rdi
	call	exec_aux
	movl	%eax, %r15d
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rdi
	call	deleteContext
	movl	CODE_ERROR(%rip), %eax
	testl	%eax, %eax
	jne	.L538
	testl	%r15d, %r15d
	jne	.L566
	movl	32(%r13), %edx
	jmp	.L563
	.cfi_endproc
.LFE26:
	.size	execConditionBlock, .-execConditionBlock
	.p2align 4
	.globl	execStatementFor
	.type	execStatementFor, @function
execStatementFor:
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
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	subq	$48, %rsp
	.cfi_def_cfa_offset 96
	movq	24(%rdi), %rax
	movq	(%rax), %rdx
	movl	32(%rdx), %eax
	cmpl	$4, %eax
	je	.L611
	cmpl	$3, %eax
	je	.L612
	cmpl	$2, %eax
	je	.L613
	movl	$108, CODE_ERROR(%rip)
.L586:
	xorl	%r15d, %r15d
.L608:
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
.L613:
	.cfi_restore_state
	xorl	%edi, %edi
	call	neo_integer_create@PLT
	movq	%rdx, %r13
	movl	CODE_ERROR(%rip), %edx
	movq	%rax, %r14
	testl	%edx, %edx
	jne	.L586
	movq	24(%r12), %rax
	movq	(%rax), %rax
	movq	24(%rax), %rax
	movq	8(%rax), %rdi
	call	eval_aux
	movq	%rax, %rbx
	movq	%rdx, %rcx
.L590:
	movl	$1, (%rsp)
.L585:
	movl	CODE_ERROR(%rip), %r15d
	testl	%r15d, %r15d
	jne	.L614
	cmpb	$4, %r13b
	jne	.L594
	cmpb	$4, %cl
	je	.L595
.L594:
	movq	%r14, %rdi
	movq	%r13, %rsi
	movq	%rcx, (%rsp)
	call	neobject_destroy@PLT
	movq	(%rsp), %rsi
	movq	%rbx, %rdi
	call	neobject_destroy@PLT
	movl	$10, CODE_ERROR(%rip)
	jmp	.L586
	.p2align 4,,10
	.p2align 3
.L611:
	movq	24(%rdx), %rax
	movq	24(%rax), %rdi
	call	eval_aux
	movl	CODE_ERROR(%rip), %edi
	movq	%rax, %r13
	movq	%rdx, %rbx
	testl	%edi, %edi
	jne	.L586
	movq	%rax, %rdi
	movq	%rdx, %rsi
	cmpb	$4, %dl
	je	.L583
	call	neobject_destroy@PLT
	movl	$108, CODE_ERROR(%rip)
	jmp	.L586
	.p2align 4,,10
	.p2align 3
.L612:
	movq	24(%rdx), %rax
	movq	8(%rax), %rdi
	call	eval_aux
	movl	CODE_ERROR(%rip), %ecx
	movq	%rax, %r14
	movq	%rdx, %r13
	testl	%ecx, %ecx
	jne	.L586
	movq	24(%r12), %rax
	movq	(%rax), %rax
	movq	24(%rax), %rax
	movq	16(%rax), %rdi
	call	eval_aux
	movq	%rax, %rbx
	movq	%rdx, %rcx
	jmp	.L590
	.p2align 4,,10
	.p2align 3
.L583:
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
	movl	CODE_ERROR(%rip), %esi
	movq	%rax, %r14
	movq	%rdx, %r13
	testl	%esi, %esi
	jne	.L586
	movq	24(%r12), %rax
	movl	%r15d, (%rsp)
	movq	(%rax), %rax
	movq	24(%rax), %rax
	movq	16(%rax), %rdi
	call	eval_aux
	movq	%rax, %rbx
	movq	%rdx, %rcx
	jmp	.L585
	.p2align 4,,10
	.p2align 3
.L614:
	movq	%r14, %rdi
	movq	%r13, %rsi
	call	neobject_destroy@PLT
	jmp	.L586
	.p2align 4,,10
	.p2align 3
.L595:
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
	je	.L596
	movl	$111, CODE_ERROR(%rip)
	movq	%rbx, %rdi
	movq	%rcx, %rsi
	call	neobject_destroy@PLT
	jmp	.L586
	.p2align 4,,10
	.p2align 3
.L596:
	movl	56(%rax), %eax
	movl	16(%rsp), %r13d
	movq	%rcx, 40(%rsp)
	movl	%eax, 8(%rsp)
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	104(%rax), %r14
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%r14, %rdi
	call	ptrlist_append@PLT
	movq	process_cycle(%rip), %rax
	movl	8(%rsp), %esi
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	movq	(%rdi), %rcx
	movq	%rcx, 24(%rsp)
	call	save_later@PLT
	movq	process_cycle(%rip), %rax
	movl	$24, %edi
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
	movq	%rbx, %rdi
	call	neo_to_integer@PLT
	movq	40(%rsp), %rsi
	movq	%rbx, %rdi
	movq	%rax, 32(%rsp)
	movl	%eax, %r14d
	call	neobject_destroy@PLT
	movq	32(%rsp), %rax
	cmpl	%eax, 16(%rsp)
	jl	.L600
	jmp	.L609
	.p2align 4,,10
	.p2align 3
.L598:
	movslq	%r15d, %rax
	cmpq	$1, %rax
	ja	.L599
	movl	(%rsp), %eax
	addl	%eax, %r13d
	cmpl	%r13d, %r14d
	jle	.L609
.L600:
	movslq	%r13d, %rdi
	call	neo_integer_create@PLT
	movl	8(%rsp), %edi
	movq	%rax, %rsi
	call	replace_var@PLT
	movq	24(%r12), %rax
	movq	8(%rax), %rdi
	call	exec_aux
	movl	%eax, %r15d
	movl	CODE_ERROR(%rip), %eax
	testl	%eax, %eax
	je	.L598
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rdi
	call	deleteContext
	movq	process_cycle(%rip), %rax
	movq	24(%rsp), %rsi
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	call	partialRestore@PLT
	jmp	.L586
.L609:
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rdi
	call	deleteContext
	movq	process_cycle(%rip), %rax
	movq	24(%rsp), %rsi
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	call	partialRestore@PLT
	jmp	.L608
.L599:
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rdi
	call	deleteContext
	movq	process_cycle(%rip), %rax
	movq	24(%rsp), %rsi
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	call	partialRestore@PLT
	cmpl	$2, %r15d
	je	.L586
	jmp	.L608
	.cfi_endproc
.LFE27:
	.size	execStatementFor, .-execStatementFor
	.p2align 4
	.globl	execStatementForeachList
	.type	execStatementForeachList, @function
execStatementForeachList:
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
	je	.L616
	movl	$111, CODE_ERROR(%rip)
.L617:
	xorl	%eax, %eax
.L615:
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
.L616:
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
	movl	16(%rbx), %ecx
	testl	%ecx, %ecx
	jle	.L632
	xorl	%r15d, %r15d
	jmp	.L622
	.p2align 4,,10
	.p2align 3
.L619:
	movslq	%eax, %rdx
	cmpq	$1, %rdx
	ja	.L621
	movslq	16(%rbx), %rdx
	addq	$1, %r15
	cmpq	%r15, %rdx
	jle	.L633
.L622:
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
	movl	CODE_ERROR(%rip), %edx
	testl	%edx, %edx
	je	.L619
.L632:
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rdi
	call	deleteContext
	movq	process_cycle(%rip), %rax
	movq	(%rsp), %rsi
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	call	partialRestore@PLT
	jmp	.L617
	.p2align 4,,10
	.p2align 3
.L621:
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
	je	.L617
	jmp	.L615
	.p2align 4,,10
	.p2align 3
.L633:
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
	addq	$16, %rsp
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
	.cfi_endproc
.LFE28:
	.size	execStatementForeachList, .-execStatementForeachList
	.p2align 4
	.globl	execStatementForeachString
	.type	execStatementForeachString, @function
execStatementForeachString:
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
	je	.L635
	movl	$111, CODE_ERROR(%rip)
.L636:
	xorl	%eax, %eax
.L634:
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
.L635:
	.cfi_restore_state
	movl	56(%rax), %r14d
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	104(%rax), %r15
	call	ptrlist_create@PLT
	movq	%rax, %rsi
	movq	%r15, %rdi
	call	ptrlist_append@PLT
	movq	process_cycle(%rip), %rax
	movl	%r14d, %esi
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	movq	(%rdi), %rax
	movq	%rax, 8(%rsp)
	call	save_later@PLT
	movq	process_cycle(%rip), %rax
	movl	$24, %edi
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
	testl	%r12d, %r12d
	jle	.L651
	movslq	%r12d, %r12
	addq	%rbx, %r12
	jmp	.L641
	.p2align 4,,10
	.p2align 3
.L638:
	movslq	%eax, %rdx
	cmpq	$1, %rdx
	ja	.L640
	addq	$1, %rbx
	cmpq	%r12, %rbx
	je	.L652
.L641:
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
	movl	CODE_ERROR(%rip), %edx
	testl	%edx, %edx
	je	.L638
.L651:
	movq	process_cycle(%rip), %rax
	movq	(%rax), %rax
	movq	104(%rax), %rdi
	call	deleteContext
	movq	process_cycle(%rip), %rax
	movq	8(%rsp), %rsi
	movq	(%rax), %rax
	movq	120(%rax), %rdi
	call	partialRestore@PLT
	jmp	.L636
	.p2align 4,,10
	.p2align 3
.L640:
	movq	process_cycle(%rip), %rdx
	movl	%eax, (%rsp)
	movq	(%rdx), %rdx
	movq	104(%rdx), %rdi
	call	deleteContext
	movq	process_cycle(%rip), %rdx
	movq	8(%rsp), %rsi
	movq	(%rdx), %rdx
	movq	120(%rdx), %rdi
	call	partialRestore@PLT
	movl	(%rsp), %eax
	cmpl	$2, %eax
	je	.L636
	jmp	.L634
	.p2align 4,,10
	.p2align 3
.L652:
	movq	process_cycle(%rip), %rdx
	movl	%eax, (%rsp)
	movq	(%rdx), %rdx
	movq	104(%rdx), %rdi
	call	deleteContext
	movq	process_cycle(%rip), %rdx
	movq	8(%rsp), %rsi
	movq	(%rdx), %rdx
	movq	120(%rdx), %rdi
	call	partialRestore@PLT
	movl	(%rsp), %eax
	addq	$16, %rsp
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
	.cfi_endproc
.LFE29:
	.size	execStatementForeachString, .-execStatementForeachString
	.p2align 4
	.globl	initRuntime
	.type	initRuntime, @function
initRuntime:
.LFB31:
	.cfi_startproc
	movl	$1, %edx
	xorl	%esi, %esi
	xorl	%edi, %edi
	jmp	create_new_process@PLT
	.cfi_endproc
.LFE31:
	.size	initRuntime, .-initRuntime
	.section	.rodata
.LC5:
	.string	"Fin exec"
	.text
	.p2align 4
	.globl	exec
	.type	exec, @function
exec:
.LFB33:
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
	call	exitRuntime
	leaq	.LC5(%rip), %rdi
	popq	%rbx
	.cfi_def_cfa_offset 8
	jmp	puts@PLT
	.cfi_endproc
.LFE33:
	.size	exec, .-exec
	.p2align 4
	.globl	eval
	.type	eval, @function
eval:
.LFB34:
	.cfi_startproc
	pushq	%r13
	.cfi_def_cfa_offset 16
	.cfi_offset 13, -16
	xorl	%esi, %esi
	movl	$1, %edx
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
.LFE34:
	.size	eval, .-eval
	.globl	BUILTINSFONC
	.section	.data.rel,"aw"
	.align 32
	.type	BUILTINSFONC, @object
	.size	BUILTINSFONC, 440
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
	.quad	_yield_
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
