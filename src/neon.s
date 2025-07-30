	.file	"neon.c"
	.text
	.p2align 4
	.globl	handle_signal
	.type	handle_signal, @function
handle_signal:
.LFB11:
	.cfi_startproc
	movl	$104, CODE_ERROR(%rip)
	ret
	.cfi_endproc
.LFE11:
	.size	handle_signal, .-handle_signal
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC265:
	.string	" ("
.LC266:
	.string	") = "
	.text
	.p2align 4
	.globl	printRes
	.type	printRes, @function
printRes:
.LFB12:
	.cfi_startproc
	cmpb	$17, %sil
	jne	.L8
	ret
	.p2align 4,,10
	.p2align 3
.L8:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rdi, %rbp
	leaq	.LC265(%rip), %rdi
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	movq	%rsi, %rbx
	subq	$8, %rsp
	.cfi_def_cfa_offset 32
	call	printString@PLT
	movl	$3, %edi
	call	setColor@PLT
	movq	%rbx, %rsi
	movq	%rbp, %rdi
	call	type@PLT
	movq	%rax, %rdi
	call	printString@PLT
	movl	$2, %edi
	call	setColor@PLT
	leaq	.LC266(%rip), %rdi
	call	printString@PLT
	call	initRuntime@PLT
	movq	%rbp, %rdi
	movq	%rbx, %rsi
	call	neobject_aff@PLT
	call	exitRuntime@PLT
	addq	$8, %rsp
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_restore 3
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_restore 6
	.cfi_def_cfa_offset 8
	jmp	newLine@PLT
	.cfi_endproc
.LFE12:
	.size	printRes, .-printRes
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC267:
	.string	"                  Welcome to Neon"
	.section	.rodata.str1.1
.LC268:
	.string	"Neon v"
.LC269:
	.string	"3.8.5.6-beta"
.LC270:
	.string	" for "
.LC271:
	.string	"LINUX_AMD64"
.LC272:
	.string	", compiled on "
.LC273:
	.string	"Wed Jul 30 11:05:08 2025"
.LC274:
	.string	"Visit "
	.section	.rodata.str1.8
	.align 8
.LC275:
	.string	"https://langage-neon.raphaael.fr"
	.section	.rodata.str1.1
.LC276:
	.string	" for more information."
	.text
	.p2align 4
	.globl	startMessage
	.type	startMessage, @function
startMessage:
.LFB13:
	.cfi_startproc
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	xorl	%edi, %edi
	call	setColor@PLT
	leaq	.LC267(%rip), %rdi
	call	printString@PLT
	call	newLine@PLT
	movl	$3, %edi
	call	setColor@PLT
	leaq	.LC268(%rip), %rdi
	call	printString@PLT
	leaq	.LC269(%rip), %rdi
	call	printString@PLT
	movl	$2, %edi
	call	setColor@PLT
	leaq	.LC270(%rip), %rdi
	call	printString@PLT
	leaq	.LC271(%rip), %rdi
	call	printString@PLT
	leaq	.LC272(%rip), %rdi
	call	printString@PLT
	leaq	.LC273(%rip), %rdi
	call	printString@PLT
	call	newLine@PLT
	leaq	.LC274(%rip), %rdi
	call	printString@PLT
	movl	$3, %edi
	call	setColor@PLT
	leaq	.LC275(%rip), %rdi
	call	printString@PLT
	movl	$2, %edi
	call	setColor@PLT
	leaq	.LC276(%rip), %rdi
	call	printString@PLT
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	jmp	newLine@PLT
	.cfi_endproc
.LFE13:
	.size	startMessage, .-startMessage
	.section	.rodata.str1.1
.LC277:
	.string	"Ans"
	.text
	.p2align 4
	.globl	storeAns
	.type	storeAns, @function
storeAns:
.LFB14:
	.cfi_startproc
	pushq	%r12
	.cfi_def_cfa_offset 16
	.cfi_offset 12, -16
	movq	%rdi, %r12
	movq	NOMS(%rip), %rdi
	pushq	%rbp
	.cfi_def_cfa_offset 24
	.cfi_offset 6, -24
	movq	%rsi, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	leaq	.LC277(%rip), %rbx
	movq	%rbx, %rsi
	call	strlist_inList@PLT
	testb	%al, %al
	je	.L12
	movq	NOMS(%rip), %rdi
	movq	%rbx, %rsi
	call	strlist_index@PLT
	movslq	%eax, %rbx
	movq	ADRESSES(%rip), %rax
	salq	$4, %rbx
	movq	8(%rax), %rax
	movq	(%rax,%rbx), %rdi
	movq	8(%rax,%rbx), %rsi
	call	neobject_destroy@PLT
	movq	ADRESSES(%rip), %rax
	movq	8(%rax), %rax
	movq	%r12, (%rax,%rbx)
	movq	%rbp, 8(%rax,%rbx)
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
.L12:
	.cfi_restore_state
	movq	%rbx, %rdi
	call	strdup@PLT
	movq	NOMS(%rip), %rdi
	movq	%rax, %rsi
	call	strlist_append@PLT
	movq	ADRESSES(%rip), %rdi
	popq	%rbx
	.cfi_def_cfa_offset 24
	movq	%r12, %rsi
	movq	%rbp, %rdx
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r12
	.cfi_def_cfa_offset 8
	jmp	nelist_append@PLT
	.cfi_endproc
.LFE14:
	.size	storeAns, .-storeAns
	.section	.rodata.str1.1
.LC278:
	.string	">> "
	.text
	.p2align 4
	.globl	terminal
	.type	terminal, @function
terminal:
.LFB15:
	.cfi_startproc
	pushq	%r14
	.cfi_def_cfa_offset 16
	.cfi_offset 14, -16
	pushq	%r13
	.cfi_def_cfa_offset 24
	.cfi_offset 13, -24
	pushq	%r12
	.cfi_def_cfa_offset 32
	.cfi_offset 12, -32
	movabsq	$144185592119623680, %r12
	pushq	%rbp
	.cfi_def_cfa_offset 40
	.cfi_offset 6, -40
	leaq	.LC278(%rip), %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	.p2align 4,,10
	.p2align 3
.L17:
	movq	FILENAME(%rip), %rdi
	movl	$0, CODE_ERROR(%rip)
	testq	%rdi, %rdi
	je	.L18
	call	free@PLT
.L18:
	movq	$0, FILENAME(%rip)
	movq	%rbp, %rdi
	call	inputCode@PLT
	movl	CODE_ERROR(%rip), %edi
	movq	%rax, %rbx
	testq	%rax, %rax
	je	.L50
	cmpl	$1, %edi
	ja	.L21
	je	.L22
	cmpb	$0, (%rax)
	je	.L48
	xorl	%edi, %edi
	xorl	%edx, %edx
	xorl	%esi, %esi
	call	tree_create@PLT
	movl	CODE_ERROR(%rip), %edi
	movq	%rax, %r13
	testl	%edi, %edi
	jne	.L51
	movq	%rax, %rdi
	movq	%rbx, %rsi
	call	createSyntaxTree@PLT
	movq	%rbx, %rdi
	call	free@PLT
	movl	CODE_ERROR(%rip), %edi
	cmpl	$1, %edi
	ja	.L47
	movl	32(%r13), %eax
	cmpl	$1, %eax
	je	.L52
	testl	%eax, %eax
	jg	.L29
.L34:
	movq	%r13, %rdi
	call	tree_destroy@PLT
	jmp	.L17
	.p2align 4,,10
	.p2align 3
.L51:
	call	printError@PLT
.L48:
	movq	%rbx, %rdi
	call	free@PLT
	jmp	.L17
	.p2align 4,,10
	.p2align 3
.L50:
	testl	%edi, %edi
	je	.L20
	cmpl	$1, %edi
	jne	.L21
.L22:
	popq	%rbx
	.cfi_remember_state
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
.L20:
	.cfi_restore_state
	movl	$104, CODE_ERROR(%rip)
	movl	$104, %edi
.L21:
	call	printError@PLT
	jmp	.L17
	.p2align 4,,10
	.p2align 3
.L52:
	movq	24(%r13), %rax
	movq	(%rax), %rdi
	movzbl	(%rdi), %eax
	cmpb	$44, %al
	je	.L29
	cmpb	$57, %al
	ja	.L30
	btq	%rax, %r12
	jc	.L29
.L30:
	call	eval@PLT
	movl	CODE_ERROR(%rip), %edi
	movq	%rax, %r14
	movq	%rdx, %rbx
	cmpl	$1, %edi
	jbe	.L31
	.p2align 4,,10
	.p2align 3
.L47:
	call	printError@PLT
	movq	%r13, %rdi
	call	tree_destroy@PLT
	jmp	.L17
	.p2align 4,,10
	.p2align 3
.L29:
	movq	%r13, %rdi
	call	exec@PLT
	movl	CODE_ERROR(%rip), %edi
	cmpl	$1, %edi
	ja	.L47
	jne	.L34
.L49:
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 40
	movq	%r13, %rdi
	popq	%rbp
	.cfi_def_cfa_offset 32
	popq	%r12
	.cfi_def_cfa_offset 24
	popq	%r13
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	jmp	tree_destroy@PLT
	.p2align 4,,10
	.p2align 3
.L31:
	.cfi_restore_state
	je	.L49
	cmpb	$17, %dl
	je	.L33
	leaq	.LC265(%rip), %rdi
	call	printString@PLT
	movl	$3, %edi
	call	setColor@PLT
	movq	%rbx, %rsi
	movq	%r14, %rdi
	call	type@PLT
	movq	%rax, %rdi
	call	printString@PLT
	movl	$2, %edi
	call	setColor@PLT
	leaq	.LC266(%rip), %rdi
	call	printString@PLT
	call	initRuntime@PLT
	movq	%r14, %rdi
	movq	%rbx, %rsi
	call	neobject_aff@PLT
	call	exitRuntime@PLT
	call	newLine@PLT
.L33:
	movq	%r14, %rdi
	movq	%rbx, %rsi
	call	storeAns
	jmp	.L34
	.cfi_endproc
.LFE15:
	.size	terminal, .-terminal
	.p2align 4
	.globl	execFile
	.type	execFile, @function
execFile:
.LFB16:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rdi, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	subq	$8, %rsp
	.cfi_def_cfa_offset 32
	call	openFile@PLT
	movl	CODE_ERROR(%rip), %edi
	movq	%rax, %rbx
	testl	%edi, %edi
	jne	.L62
	movl	$0, CODE_ERROR(%rip)
	testq	%rbx, %rbx
	je	.L53
.L64:
	movq	%rbp, %rdi
	call	strdup@PLT
	xorl	%edx, %edx
	xorl	%esi, %esi
	xorl	%edi, %edi
	movq	%rax, FILENAME(%rip)
	call	tree_create@PLT
	movq	%rbx, %rsi
	movq	%rax, %rdi
	movq	%rax, %rbp
	call	createSyntaxTree@PLT
	movq	%rbx, %rdi
	call	free@PLT
	movl	CODE_ERROR(%rip), %edi
	testl	%edi, %edi
	jne	.L63
	movq	%rbp, %rdi
	call	exec@PLT
	movl	CODE_ERROR(%rip), %edi
	cmpl	$1, %edi
	jbe	.L57
	call	printError@PLT
.L57:
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	movq	%rbp, %rdi
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	jmp	tree_destroy@PLT
	.p2align 4,,10
	.p2align 3
.L63:
	.cfi_restore_state
	call	printError@PLT
	movq	%rbp, %rdi
	movl	$0, CODE_ERROR(%rip)
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	jmp	tree_destroy@PLT
	.p2align 4,,10
	.p2align 3
.L62:
	.cfi_restore_state
	call	printError@PLT
	movl	$0, CODE_ERROR(%rip)
	testq	%rbx, %rbx
	jne	.L64
.L53:
	addq	$8, %rsp
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE16:
	.size	execFile, .-execFile
	.p2align 4
	.globl	importFile
	.type	importFile, @function
importFile:
.LFB17:
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
	call	openFile@PLT
	movl	CODE_ERROR(%rip), %ecx
	testl	%ecx, %ecx
	jne	.L65
	movq	%rax, %rbx
	testq	%rax, %rax
	je	.L73
	movq	%rbp, %rdi
	movq	FILENAME(%rip), %r12
	call	strdup@PLT
	xorl	%edx, %edx
	xorl	%esi, %esi
	xorl	%edi, %edi
	movq	%rax, FILENAME(%rip)
	call	tree_create@PLT
	movq	%rbx, %rsi
	movq	%rax, %rdi
	movq	%rax, %rbp
	call	createSyntaxTree@PLT
	movq	%rbx, %rdi
	call	free@PLT
	movl	CODE_ERROR(%rip), %edx
	testl	%edx, %edx
	jne	.L72
	movq	%rbp, %rdi
	call	exec_aux@PLT
	movl	CODE_ERROR(%rip), %eax
	testl	%eax, %eax
	jne	.L72
	movq	FILENAME(%rip), %rdi
	call	free@PLT
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	movq	%r12, FILENAME(%rip)
	movq	%rbp, %rdi
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r12
	.cfi_def_cfa_offset 8
	jmp	tree_destroy@PLT
	.p2align 4,,10
	.p2align 3
.L73:
	.cfi_restore_state
	movl	$67, CODE_ERROR(%rip)
.L65:
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
.L72:
	.cfi_restore_state
	movq	%r12, %rdi
	call	free@PLT
	popq	%rbx
	.cfi_def_cfa_offset 24
	movq	%rbp, %rdi
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r12
	.cfi_def_cfa_offset 8
	jmp	tree_destroy@PLT
	.cfi_endproc
.LFE17:
	.size	importFile, .-importFile
	.section	.rodata.str1.1
.LC279:
	.string	"__version__"
.LC280:
	.string	"__platform__"
.LC281:
	.string	"__name__"
.LC283:
	.string	"pi"
	.text
	.p2align 4
	.globl	defineVariables
	.type	defineVariables, @function
defineVariables:
.LFB18:
	.cfi_startproc
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	leaq	.LC279(%rip), %rdi
	call	strdup@PLT
	movq	NOMS(%rip), %rdi
	movq	%rax, %rsi
	call	strlist_append@PLT
	leaq	.LC269(%rip), %rdi
	call	strdup@PLT
	movq	%rax, %rdi
	call	neo_str_create@PLT
	movq	ADRESSES(%rip), %rdi
	movq	%rax, %rsi
	call	nelist_append@PLT
	leaq	.LC280(%rip), %rdi
	call	strdup@PLT
	movq	NOMS(%rip), %rdi
	movq	%rax, %rsi
	call	strlist_append@PLT
	leaq	.LC271(%rip), %rdi
	call	strdup@PLT
	movq	%rax, %rdi
	call	neo_str_create@PLT
	movq	ADRESSES(%rip), %rdi
	movq	%rax, %rsi
	call	nelist_append@PLT
	leaq	.LC281(%rip), %rdi
	call	strdup@PLT
	movq	NOMS(%rip), %rdi
	movq	%rax, %rsi
	call	strlist_append@PLT
	movq	ADRESSES(%rip), %rdi
	xorl	%esi, %esi
	movl	$55, %edx
	call	nelist_append@PLT
	movq	ADRESSES(%rip), %rax
	movsd	.LC282(%rip), %xmm0
	movl	16(%rax), %eax
	subl	$1, %eax
	movl	%eax, NAME(%rip)
	call	neo_double_create@PLT
	movq	ADRESSES(%rip), %rdi
	movq	%rax, %rsi
	call	nelist_append@PLT
	leaq	.LC283(%rip), %rdi
	call	strdup@PLT
	movq	NOMS(%rip), %rdi
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	movq	%rax, %rsi
	jmp	strlist_append@PLT
	.cfi_endproc
.LFE18:
	.size	defineVariables, .-defineVariables
	.section	.rodata.str1.1
.LC284:
	.string	"None"
.LC285:
	.string	"Infinity"
.LC286:
	.string	"SyntaxError"
.LC287:
	.string	"FileNotFound"
.LC288:
	.string	"UnmeasurableObject"
.LC289:
	.string	"UndefinedVariable"
.LC290:
	.string	"IncorrectFunctionCall"
.LC291:
	.string	"MemoryError"
.LC292:
	.string	"NonIndexableObject"
.LC293:
	.string	"IncorrectIndex"
.LC294:
	.string	"OutOfRange"
.LC295:
	.string	"IncorrectType"
.LC296:
	.string	"DivisionByZero"
.LC297:
	.string	"UnknownError"
.LC298:
	.string	"AssertionFailed"
.LC299:
	.string	"DefinitionError"
.LC300:
	.string	"True"
.LC301:
	.string	"False"
.LC302:
	.string	"break"
.LC303:
	.string	"continue"
.LC304:
	.string	"es"
.LC305:
	.string	"pass"
.LC306:
	.string	"atmc"
.LC307:
	.string	"tr"
.LC308:
	.string	"function"
.LC309:
	.string	"method"
.LC310:
	.string	"import"
.LC311:
	.string	"return"
.LC312:
	.string	"await"
.LC313:
	.string	"local"
.LC314:
	.string	"if"
.LC315:
	.string	"while"
.LC316:
	.string	"for"
.LC317:
	.string	"foreach"
.LC318:
	.string	"ei"
.LC319:
	.string	"expt"
.LC0:
	.string	"and"
.LC1:
	.string	"or"
.LC2:
	.string	"xor"
.LC21:
	.string	"not"
.LC28:
	.string	"del"
.LC31:
	.string	"EE"
.LC33:
	.string	"in"
.LC39:
	.string	"parallel"
.LC3:
	.string	"+"
.LC4:
	.string	"*"
.LC5:
	.string	"-"
.LC6:
	.string	"/"
.LC7:
	.string	"**"
.LC8:
	.string	"=="
.LC9:
	.string	"!="
.LC10:
	.string	"<="
.LC12:
	.string	"<"
.LC11:
	.string	">="
.LC14:
	.string	"="
.LC13:
	.string	">"
.LC15:
	.string	"+="
.LC16:
	.string	"-="
.LC17:
	.string	"*="
.LC18:
	.string	"/="
.LC19:
	.string	"++"
.LC20:
	.string	"--"
.LC22:
	.string	"%"
.LC23:
	.string	"//"
.LC24:
	.string	"&"
.LC25:
	.string	"<-"
.LC26:
	.string	"@"
.LC29:
	.string	"->"
.LC30:
	.string	"."
.LC32:
	.string	"=>"
.LC35:
	.string	":"
.LC34:
	.string	">>"
.LC36:
	.string	"<->"
.LC37:
	.string	":="
.LC320:
	.string	"!"
.LC321:
	.string	"\""
.LC322:
	.string	"'"
.LC323:
	.string	","
.LC324:
	.string	";"
.LC325:
	.string	"\n"
.LC326:
	.string	"#"
.LC327:
	.string	"$"
.LC328:
	.string	"["
.LC329:
	.string	"]"
.LC330:
	.string	"("
.LC331:
	.string	")"
.LC332:
	.string	"{"
.LC333:
	.string	"}"
.LC334:
	.string	"\\"
.LC27:
	.string	"_"
.LC335:
	.string	" "
.LC336:
	.string	"\t"
.LC337:
	.string	"~"
.LC38:
	.string	"..."
.LC387:
	.string	"KeyboardInterrupt"
.LC389:
	.string	"NaN"
	.data
	.align 32
.LC373:
	.quad	.LC0
	.quad	.LC1
	.quad	.LC2
	.quad	.LC3
	.quad	.LC4
	.quad	.LC5
	.quad	.LC6
	.quad	.LC7
	.quad	.LC8
	.quad	.LC9
	.quad	.LC10
	.quad	.LC11
	.quad	.LC12
	.quad	.LC13
	.quad	.LC14
	.quad	.LC15
	.quad	.LC16
	.quad	.LC17
	.quad	.LC18
	.quad	.LC19
	.quad	.LC20
	.quad	.LC21
	.quad	.LC22
	.quad	.LC23
	.quad	.LC24
	.quad	.LC25
	.quad	.LC26
	.quad	.LC27
	.quad	.LC28
	.quad	.LC29
	.quad	.LC30
	.quad	.LC31
	.quad	.LC32
	.quad	.LC33
	.quad	.LC34
	.quad	.LC35
	.quad	.LC36
	.quad	.LC37
	.quad	.LC38
	.quad	.LC39
	.section	.rodata.str1.1
.LC41:
	.string	"print"
.LC42:
	.string	"input"
.LC43:
	.string	"nbr"
.LC44:
	.string	"str"
.LC45:
	.string	"len"
.LC46:
	.string	"sub"
.LC47:
	.string	"exit"
.LC48:
	.string	"append"
.LC49:
	.string	"remove"
.LC50:
	.string	"insert"
.LC51:
	.string	"type"
.LC52:
	.string	"reverse"
.LC53:
	.string	"eval"
.LC54:
	.string	"clear"
.LC55:
	.string	"help"
.LC56:
	.string	"randint"
.LC57:
	.string	"failwith"
.LC58:
	.string	"time"
.LC59:
	.string	"assert"
.LC60:
	.string	"output"
.LC61:
	.string	"chr"
.LC62:
	.string	"ord"
.LC63:
	.string	"listComp"
.LC64:
	.string	"createException"
.LC65:
	.string	"raise"
.LC66:
	.string	"int"
.LC67:
	.string	"index"
.LC68:
	.string	"replace"
.LC69:
	.string	"count"
.LC70:
	.string	"list"
.LC71:
	.string	"sortAsc"
.LC72:
	.string	"sortDesc"
.LC73:
	.string	"sin"
.LC74:
	.string	"cos"
.LC75:
	.string	"tan"
.LC76:
	.string	"deg"
.LC77:
	.string	"rad"
.LC78:
	.string	"sqrt"
.LC79:
	.string	"ln"
.LC80:
	.string	"exp"
.LC81:
	.string	"log"
.LC82:
	.string	"log2"
.LC83:
	.string	"round"
.LC84:
	.string	"abs"
.LC85:
	.string	"ceil"
.LC86:
	.string	"floor"
.LC87:
	.string	"readFile"
.LC88:
	.string	"writeFile"
.LC89:
	.string	"setFunctionDoc"
.LC90:
	.string	"setAtomicTime"
.LC91:
	.string	"copy"
.LC92:
	.string	"loadNamespace"
.LC93:
	.string	"gc"
.LC94:
	.string	"setColor"
.LC95:
	.string	"yield"
	.data
	.align 32
.LC390:
	.quad	.LC41
	.quad	.LC42
	.quad	.LC43
	.quad	.LC44
	.quad	.LC45
	.quad	.LC46
	.quad	.LC47
	.quad	.LC48
	.quad	.LC49
	.quad	.LC50
	.quad	.LC51
	.quad	.LC52
	.quad	.LC53
	.quad	.LC54
	.quad	.LC55
	.quad	.LC56
	.quad	.LC57
	.quad	.LC58
	.quad	.LC59
	.quad	.LC60
	.quad	.LC61
	.quad	.LC62
	.quad	.LC63
	.quad	.LC64
	.quad	.LC65
	.quad	.LC66
	.quad	.LC67
	.quad	.LC68
	.quad	.LC69
	.quad	.LC70
	.quad	.LC71
	.quad	.LC72
	.quad	.LC73
	.quad	.LC74
	.quad	.LC75
	.quad	.LC76
	.quad	.LC77
	.quad	.LC78
	.quad	.LC79
	.quad	.LC80
	.quad	.LC81
	.quad	.LC82
	.quad	.LC83
	.quad	.LC84
	.quad	.LC85
	.quad	.LC86
	.quad	.LC87
	.quad	.LC88
	.quad	.LC89
	.quad	.LC90
	.quad	.LC91
	.quad	.LC92
	.quad	.LC93
	.quad	.LC94
	.quad	.LC95
	.section	.rodata.str1.8
	.align 8
.LC97:
	.string	"Displays arguments in the terminal"
	.align 8
.LC98:
	.string	"Displays the given argument and prompts the user to input in the terminal. Returns the entered value."
	.align 8
.LC99:
	.string	"Converts a string into a number."
	.align 8
.LC100:
	.string	"Converts any object into a string."
	.align 8
.LC101:
	.string	"Returns the length of an object."
	.align 8
.LC102:
	.string	"Performs substring extraction. sub(\"hello\", 1, 4) returns \"ell\"."
	.section	.rodata.str1.1
.LC103:
	.string	"Exits the Neon interpreter."
.LC104:
	.string	"Adds an element to a list."
	.section	.rodata.str1.8
	.align 8
.LC105:
	.string	"Removes an element from a list."
	.align 8
.LC106:
	.string	"Inserts an element into a list."
	.align 8
.LC107:
	.string	"Returns the type of an object."
	.align 8
.LC108:
	.string	"Reverses the characters in a string of characters or reverses a list."
	.align 8
.LC109:
	.string	"Evaluates the given string of characters and returns the result."
	.section	.rodata.str1.1
.LC110:
	.string	"Clears the terminal."
	.section	.rodata.str1.8
	.align 8
.LC111:
	.string	"This function displays all kinds of information on objects and modules.\nType help() for more information."
	.align 8
.LC112:
	.string	"Returns a random integer between two integers."
	.align 8
.LC113:
	.string	"Displays the provided text in the terminal and exits the program."
	.align 8
.LC114:
	.string	"Returns the time elapsed since 1970 in seconds."
	.align 8
.LC115:
	.string	"Stops the program if at least 1 assertion has failed."
	.align 8
.LC116:
	.string	"Puts the given arguments on the terminal without spaces or \\n."
	.align 8
.LC117:
	.string	"Returns the ASCII character associated to an integer."
	.align 8
.LC118:
	.string	"Returns the ASCII value of the given character."
	.align 8
.LC119:
	.ascii	"Arguments : i"
	.string	"ndex, start, end, step, condition, value\nEvaluates value for all indices between start and end according to step, and adds it to the list if condition is true.\nindex must be the name of the variable used, and condition and value must be string expressions."
	.align 8
.LC120:
	.string	"Creates an Exception object with the given name."
	.align 8
.LC121:
	.string	"Exits the program with the given exception and displays the given message"
	.align 8
.LC122:
	.string	"Converts an object into an integer"
	.align 8
.LC123:
	.string	"Returns the index of an element in a list."
	.align 8
.LC124:
	.string	"replace(str, str1, str2) returns a string in which all occurrences of str1 have been replaced by str2 in str."
	.align 8
.LC125:
	.string	"Counts the number of occurrences in a list or a string."
	.align 8
.LC126:
	.string	"Separates every character of a string in a list."
	.align 8
.LC127:
	.string	"Sorts a list in ascending order."
	.align 8
.LC128:
	.string	"Sorts a list in descending order."
	.section	.rodata.str1.1
.LC129:
	.string	"sin: Sine function"
.LC130:
	.string	"cos: Cosine function"
.LC131:
	.string	"tan: Tangent function"
	.section	.rodata.str1.8
	.align 8
.LC132:
	.string	"deg: Convert angle from radians to degrees"
	.align 8
.LC133:
	.string	"rad: Convert angle from degrees to radians"
	.section	.rodata.str1.1
.LC134:
	.string	"sqrt: Square root function"
	.section	.rodata.str1.8
	.align 8
.LC135:
	.string	"ln: Natural logarithm (base e)"
	.align 8
.LC136:
	.string	"exp: Exponential function (e^x)"
	.section	.rodata.str1.1
.LC137:
	.string	"log: Logarithm (base 10)"
.LC138:
	.string	"log2: Logarithm (base 2)"
	.section	.rodata.str1.8
	.align 8
.LC139:
	.string	"round: Round to the nearest decimal number with the given precision"
	.section	.rodata.str1.1
.LC140:
	.string	"abs: Absolute value"
	.section	.rodata.str1.8
	.align 8
.LC141:
	.string	"ceil: Ceiling function (smallest integer greater than or equal to)"
	.align 8
.LC142:
	.string	"floor: Floor function (largest integer less than or equal to)"
	.align 8
.LC143:
	.string	"Returns the content of the file whose name was given"
	.align 8
.LC144:
	.string	"Writes the given string in the file whose name was given. The syntax is writeFile(name, content)"
	.align 8
.LC145:
	.string	"Sets a string documentation for a user-defined function or method"
	.align 8
.LC146:
	.string	"Time to allow for each process before switching"
	.align 8
.LC147:
	.string	"Performs a deep copy of an object, preserving the pointer dependencies"
	.align 8
.LC148:
	.string	"Loads any variable from a given namespace without the namespace prefix"
	.section	.rodata.str1.1
.LC149:
	.string	"Calls the Garbage Collector"
	.section	.rodata.str1.8
	.align 8
.LC150:
	.string	"Changes the writing text color in console if available.\nColors: red, green, blue and white"
	.align 8
.LC151:
	.string	"Calling this function allows the interpreter to switch to another process"
	.data
	.align 32
.LC403:
	.quad	.LC97
	.quad	.LC98
	.quad	.LC99
	.quad	.LC100
	.quad	.LC101
	.quad	.LC102
	.quad	.LC103
	.quad	.LC104
	.quad	.LC105
	.quad	.LC106
	.quad	.LC107
	.quad	.LC108
	.quad	.LC109
	.quad	.LC110
	.quad	.LC111
	.quad	.LC112
	.quad	.LC113
	.quad	.LC114
	.quad	.LC115
	.quad	.LC116
	.quad	.LC117
	.quad	.LC118
	.quad	.LC119
	.quad	.LC120
	.quad	.LC121
	.quad	.LC122
	.quad	.LC123
	.quad	.LC124
	.quad	.LC125
	.quad	.LC126
	.quad	.LC127
	.quad	.LC128
	.quad	.LC129
	.quad	.LC130
	.quad	.LC131
	.quad	.LC132
	.quad	.LC133
	.quad	.LC134
	.quad	.LC135
	.quad	.LC136
	.quad	.LC137
	.quad	.LC138
	.quad	.LC139
	.quad	.LC140
	.quad	.LC141
	.quad	.LC142
	.quad	.LC143
	.quad	.LC144
	.quad	.LC145
	.quad	.LC146
	.quad	.LC147
	.quad	.LC148
	.quad	.LC149
	.quad	.LC150
	.quad	.LC151
	.section	.rodata.str1.1
.LC153:
	.string	""
	.section	.rodata.str1.8
	.align 8
.LC154:
	.string	"Multiple decimal points in the same number"
	.section	.rodata.str1.1
.LC155:
	.string	"File doesn't exist"
.LC156:
	.string	"This object has no length"
.LC157:
	.string	"Use of an undefined variable"
	.section	.rodata.str1.8
	.align 8
.LC158:
	.string	"Too many arguments in user-defined function call"
	.align 8
.LC159:
	.string	"Insufficient arguments in user-defined function call"
	.section	.rodata.str1.1
.LC160:
	.string	"Call to an undefined function"
.LC161:
	.string	"Call to an invalid function"
	.section	.rodata.str1.8
	.align 8
.LC162:
	.string	"Starting or ending value of the for loop is not a number"
	.section	.rodata.str1.1
.LC163:
	.string	"Incorrect function definition"
.LC164:
	.string	"Memory error"
	.section	.rodata.str1.8
	.align 8
.LC165:
	.string	"Incorrect index for subtree deletion"
	.align 8
.LC166:
	.string	"Invalid arguments provided in function call"
	.section	.rodata.str1.1
.LC167:
	.string	"This object is not indexable"
.LC168:
	.string	"List index is not a number"
	.section	.rodata.str1.8
	.align 8
.LC169:
	.string	"Attempt to index an undefined list"
	.section	.rodata.str1.1
.LC170:
	.string	"List index out of range"
	.section	.rodata.str1.8
	.align 8
.LC171:
	.string	"Undetermined error, likely a syntax issue"
	.align 8
.LC172:
	.string	"Unevaluable condition due to incorrect type"
	.align 8
.LC173:
	.string	"Multiple arguments for return statement"
	.align 8
.LC174:
	.string	"'For' loop variant is not a variable"
	.align 8
.LC175:
	.string	"Incorrect built-in function name in function call"
	.section	.rodata.str1.1
.LC176:
	.string	"Unknown operator"
.LC177:
	.string	"Unknown character"
	.section	.rodata.str1.8
	.align 8
.LC178:
	.string	"String, list, or other structure not terminated"
	.align 8
.LC179:
	.string	"Unclosed string at the end of line"
	.align 8
.LC180:
	.string	"Incompatible keyword used in an expression"
	.align 8
.LC181:
	.string	"Block instruction line not followed by an instruction block (e.g., a lone if() statement)"
	.align 8
.LC182:
	.string	"Improper use of an operator; incompatible types used consecutively or an unknown/incompatible type"
	.section	.rodata.str1.1
.LC183:
	.string	"Lone block of instructions"
	.section	.rodata.str1.8
	.align 8
.LC184:
	.string	"Incorrect statement definition"
	.align 8
.LC185:
	.string	"Incorrect number of arguments when calling a built-in function"
	.align 8
.LC186:
	.string	"Index out of range for inserting into a NeList"
	.align 8
.LC187:
	.string	"Index out of range for deleting from a NeList"
	.align 8
.LC188:
	.string	"Index out of range to access in a NeList"
	.align 8
.LC189:
	.string	"Unsupported types for addition"
	.align 8
.LC190:
	.string	"Unsupported types for subtraction"
	.align 8
.LC191:
	.string	"Unsupported types for division"
	.align 8
.LC192:
	.string	"Unsupported types for multiplication"
	.section	.rodata.str1.1
.LC193:
	.string	"Unsupported types for modulo"
.LC194:
	.string	"Euclidean division by zero"
	.section	.rodata.str1.8
	.align 8
.LC195:
	.string	"Unsupported types for Euclidean division"
	.align 8
.LC196:
	.string	"Unsupported types for 'and' operator"
	.align 8
.LC197:
	.string	"Unsupported types for 'or' operator"
	.align 8
.LC198:
	.string	"Unsupported types for 'xor' operator"
	.align 8
.LC199:
	.string	"Unsupported types for exponentiation"
	.align 8
.LC200:
	.string	"Unsupported types for equality tests"
	.section	.rodata.str1.1
.LC201:
	.string	"Unsupported types for <="
.LC202:
	.string	"Unsupported types for >="
.LC203:
	.string	"Unsupported types for <"
.LC204:
	.string	"Unsupported types for >"
	.section	.rodata.str1.8
	.align 8
.LC205:
	.string	"Attempt to assign the value of an undefined variable to another variable"
	.align 8
.LC206:
	.string	"Improper use of the '->' operator"
	.align 8
.LC207:
	.string	"Unsupported types for 'not' operator"
	.align 8
.LC208:
	.string	"Referencing an undefined variable"
	.align 8
.LC209:
	.string	"Dereferencing something other than a string"
	.align 8
.LC210:
	.string	"Unsupported types for unary minus operator"
	.align 8
.LC211:
	.string	"Unsupported types for reverse function"
	.align 8
.LC212:
	.string	"Attempt to use help on a non-function variable"
	.align 8
.LC213:
	.string	"The randint function must have two positive integers a and b as arguments, with a < b"
	.section	.rodata.str1.1
.LC214:
	.string	"Pointer is not allocated"
.LC215:
	.string	"Undefined error"
.LC216:
	.string	"Can not open file"
	.section	.rodata.str1.8
	.align 8
.LC217:
	.string	"More closing parentheses, curved brackets or square brackets than opening ones"
	.align 8
.LC218:
	.string	"local must have at least one argument"
	.align 8
.LC219:
	.string	"local can only be used in a function or in a method"
	.section	.rodata.str1.1
.LC220:
	.string	"Assertion failed"
	.section	.rodata.str1.8
	.align 8
.LC221:
	.string	"The '.' operator is only for functions or methods and can only be applied on a variable"
	.align 8
.LC222:
	.string	"Incorrect hexadecimal or binary number"
	.align 8
.LC223:
	.string	"Unsupported types for implication."
	.align 8
.LC224:
	.string	"except block not preceded by a try block."
	.align 8
.LC225:
	.string	"try block not followed by an except block"
	.section	.rodata.str1.1
.LC226:
	.string	"except expects exceptions"
	.section	.rodata.str1.8
	.align 8
.LC227:
	.string	"in can only be calculated with a list"
	.section	.rodata.str1.1
.LC228:
	.string	"'>>' is only for containers"
	.section	.rodata.str1.8
	.align 8
.LC229:
	.string	"A container field is set but the container is not used"
	.section	.rodata.str1.1
.LC230:
	.string	"Unknown container field"
	.section	.rodata.str1.8
	.align 8
.LC231:
	.string	"Same container name but not same fields"
	.align 8
.LC232:
	.string	"Containers must contain at least one field"
	.section	.rodata.str1.1
.LC233:
	.string	"Unknown container type"
	.section	.rodata.str1.8
	.align 8
.LC234:
	.string	"Two different fields cannot have the same name in a container"
	.align 8
.LC235:
	.string	"Incorrect container definition"
	.section	.rodata.str1.1
.LC236:
	.string	"Object not in list"
	.section	.rodata.str1.8
	.align 8
.LC237:
	.string	"Cannot assign a value or execute a method on a litteral object"
	.section	.rodata.str1.1
.LC238:
	.string	"That list cannot be sorted"
	.section	.rodata.str1.8
	.align 8
.LC239:
	.string	"Cannot call a function while dereferencing a container field"
	.align 8
.LC240:
	.string	"':' outside container definition"
	.align 8
.LC241:
	.string	"Unknown argument name in user-defined function call"
	.align 8
.LC242:
	.string	"You have already given a value to this argument in user-defined function call"
	.align 8
.LC243:
	.string	"':=' outside function call or function definition"
	.align 8
.LC244:
	.string	"Bad arguments definition in function definition"
	.align 8
.LC245:
	.string	"Use of '...' outside function definition"
	.align 8
.LC246:
	.string	"Can only set a documentation for a user-defined function or method"
	.align 8
.LC247:
	.string	"Use of return outside function"
	.align 8
.LC248:
	.string	"Can only call a user-defined function in parallel"
	.align 8
.LC249:
	.string	"await takes exactly one argument"
	.align 8
.LC250:
	.string	"Atomic time must be a positive integer, greater or equal to 1"
	.align 8
.LC251:
	.string	"Methods cannot be applied to volatile objects such as function or operators"
	.section	.rodata.str1.1
.LC252:
	.string	"Program interrupted"
	.section	.rodata.str1.8
	.align 8
.LC253:
	.string	"Cannot assign a value to a string index"
	.align 8
.LC254:
	.string	"Stack overflow encountered during runtime"
	.align 8
.LC255:
	.string	"This operator has not yet been implemented for this type of container"
	.align 8
.LC256:
	.string	"For loop can only be defined with 2, 3 or 4 arguments as follows : \n for (var, end)\n for (var, start, end)\n for (var, start, end, step)"
	.align 8
.LC257:
	.string	"foreach needs exactly 2 arguments : a variable and a list or a string"
	.align 8
.LC258:
	.string	"Methods must have at least one argument, and this argument cannot be optional"
	.align 8
.LC259:
	.string	"for/foreach first argument must be a variable"
	.align 8
.LC260:
	.string	"listComp range parameters must be integers"
	.align 8
.LC261:
	.string	"This object cannot be converted to an integer"
	.align 8
.LC262:
	.string	"This function can only receive integer of decimal arguments"
	.data
	.align 32
.LC426:
	.quad	.LC153
	.quad	.LC153
	.quad	.LC154
	.quad	.LC155
	.quad	.LC156
	.quad	.LC157
	.quad	.LC158
	.quad	.LC159
	.quad	.LC160
	.quad	.LC161
	.quad	.LC162
	.quad	.LC163
	.quad	.LC164
	.quad	.LC165
	.quad	.LC166
	.quad	.LC167
	.quad	.LC168
	.quad	.LC169
	.quad	.LC170
	.quad	.LC171
	.quad	.LC172
	.quad	.LC173
	.quad	.LC174
	.quad	.LC175
	.quad	.LC176
	.quad	.LC177
	.quad	.LC178
	.quad	.LC179
	.quad	.LC180
	.quad	.LC181
	.quad	.LC182
	.quad	.LC182
	.quad	.LC182
	.quad	.LC182
	.quad	.LC183
	.quad	.LC184
	.quad	.LC185
	.quad	.LC186
	.quad	.LC187
	.quad	.LC188
	.quad	.LC189
	.quad	.LC190
	.quad	.LC191
	.quad	.LC192
	.quad	.LC193
	.quad	.LC194
	.quad	.LC195
	.quad	.LC196
	.quad	.LC197
	.quad	.LC198
	.quad	.LC199
	.quad	.LC200
	.quad	.LC201
	.quad	.LC202
	.quad	.LC203
	.quad	.LC204
	.quad	.LC205
	.quad	.LC206
	.quad	.LC207
	.quad	.LC208
	.quad	.LC209
	.quad	.LC210
	.quad	.LC211
	.quad	.LC212
	.quad	.LC213
	.quad	.LC214
	.quad	.LC215
	.quad	.LC216
	.quad	.LC217
	.quad	.LC218
	.quad	.LC219
	.quad	.LC220
	.quad	.LC221
	.quad	.LC222
	.quad	.LC153
	.quad	.LC223
	.quad	.LC224
	.quad	.LC225
	.quad	.LC226
	.quad	.LC227
	.quad	.LC228
	.quad	.LC229
	.quad	.LC230
	.quad	.LC231
	.quad	.LC232
	.quad	.LC233
	.quad	.LC234
	.quad	.LC235
	.quad	.LC236
	.quad	.LC237
	.quad	.LC238
	.quad	.LC239
	.quad	.LC240
	.quad	.LC241
	.quad	.LC242
	.quad	.LC243
	.quad	.LC244
	.quad	.LC245
	.quad	.LC246
	.quad	.LC247
	.quad	.LC248
	.quad	.LC249
	.quad	.LC250
	.quad	.LC251
	.quad	.LC252
	.quad	.LC253
	.quad	.LC254
	.quad	.LC255
	.quad	.LC256
	.quad	.LC257
	.quad	.LC258
	.quad	.LC259
	.quad	.LC260
	.quad	.LC261
	.quad	.LC262
	.section	.rodata
	.align 32
.LC264:
	.long	0
	.long	0
	.long	0
	.long	1
	.long	2
	.long	3
	.long	4
	.long	4
	.long	3
	.long	4
	.long	0
	.long	0
	.long	5
	.long	11
	.long	4
	.long	6
	.long	7
	.long	3
	.long	8
	.long	0
	.long	9
	.long	0
	.long	9
	.long	4
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	0
	.long	4
	.long	8
	.long	8
	.long	8
	.long	9
	.long	9
	.long	9
	.long	9
	.long	9
	.long	10
	.long	9
	.long	9
	.long	9
	.long	9
	.long	9
	.long	9
	.long	9
	.long	9
	.long	9
	.long	9
	.long	3
	.long	9
	.long	9
	.long	3
	.long	9
	.long	9
	.long	9
	.long	9
	.long	9
	.long	5
	.long	11
	.long	1
	.long	0
	.long	0
	.long	9
	.long	12
	.long	9
	.long	0
	.long	0
	.long	9
	.long	0
	.long	0
	.long	9
	.long	9
	.long	9
	.long	5
	.long	3
	.long	13
	.long	13
	.long	13
	.long	13
	.long	0
	.long	8
	.long	4
	.long	9
	.long	0
	.long	0
	.long	4
	.long	4
	.long	0
	.long	0
	.long	0
	.long	9
	.long	0
	.long	0
	.long	0
	.long	8
	.long	4
	.long	14
	.long	9
	.long	5
	.long	9
	.long	0
	.long	0
	.long	0
	.long	0
	.long	9
	.long	9
	.long	9
	.text
	.p2align 4
	.globl	neonInit
	.type	neonInit, @function
neonInit:
.LFB19:
	.cfi_startproc
	pushq	%r12
	.cfi_def_cfa_offset 16
	.cfi_offset 12, -16
	movl	$1, %edi
	leaq	handle_signal(%rip), %r12
	pushq	%rbp
	.cfi_def_cfa_offset 24
	.cfi_offset 6, -24
	leaq	.LC35(%rip), %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	leaq	.LC305(%rip), %rbx
	movq	%rbx, %xmm4
	subq	$7312, %rsp
	.cfi_def_cfa_offset 7344
	leaq	500(%rsp), %rdx
	leaq	504(%rsp), %rax
	movq	%rax, %xmm1
	movq	%rdx, %xmm2
	leaq	512(%rsp), %rax
	punpcklqdq	%xmm1, %xmm2
	leaq	508(%rsp), %rdx
	movq	%rax, %xmm6
	movaps	%xmm2, 112(%rsp)
	movq	%rdx, %xmm2
	leaq	736(%rsp), %rax
	leaq	516(%rsp), %rdx
	punpcklqdq	%xmm6, %xmm2
	movq	%rdx, %xmm7
	leaq	.LC3(%rip), %rdx
	movaps	%xmm2, 96(%rsp)
	movq	%rax, %xmm2
	leaq	.LC302(%rip), %rax
	movq	%rax, %xmm0
	leaq	.LC303(%rip), %rax
	punpcklqdq	%xmm2, %xmm7
	movq	%rax, %xmm1
	leaq	.LC304(%rip), %rax
	movaps	%xmm7, 80(%rsp)
	movdqa	%xmm1, %xmm6
	movq	%rax, %xmm2
	leaq	.LC306(%rip), %rax
	punpcklqdq	%xmm0, %xmm6
	punpcklqdq	%xmm1, %xmm2
	movdqa	%xmm0, %xmm1
	movq	%rax, %xmm0
	leaq	.LC307(%rip), %rax
	punpcklqdq	%xmm4, %xmm1
	movaps	%xmm6, 128(%rsp)
	movq	%rax, %xmm4
	leaq	.LC310(%rip), %rax
	movaps	%xmm1, 160(%rsp)
	movq	%rax, %xmm1
	leaq	.LC311(%rip), %rax
	movaps	%xmm2, 144(%rsp)
	movdqa	%xmm4, %xmm7
	movq	%rax, %xmm6
	leaq	.LC312(%rip), %rax
	punpcklqdq	%xmm0, %xmm7
	movdqa	%xmm6, %xmm3
	movaps	%xmm7, 176(%rsp)
	punpcklqdq	%xmm1, %xmm3
	movaps	%xmm3, 192(%rsp)
	movq	%rax, %xmm3
	leaq	.LC313(%rip), %rax
	movq	%rax, %xmm5
	leaq	.LC318(%rip), %rax
	movq	%rax, %xmm2
	leaq	.LC319(%rip), %rax
	punpcklqdq	%xmm5, %xmm1
	movdqa	%xmm5, %xmm11
	movdqa	%xmm4, %xmm5
	movq	%rax, %xmm4
	movaps	%xmm1, 448(%rsp)
	punpcklqdq	%xmm6, %xmm2
	punpcklqdq	%xmm4, %xmm5
	movdqa	%xmm3, %xmm4
	leaq	.LC4(%rip), %rax
	punpcklqdq	%xmm0, %xmm4
	movaps	%xmm2, 464(%rsp)
	movq	%rdx, %xmm2
	movq	%rax, %xmm0
	leaq	.LC6(%rip), %rax
	movaps	%xmm4, 240(%rsp)
	leaq	.LC5(%rip), %rdx
	punpcklqdq	%xmm0, %xmm2
	movq	%rax, %xmm0
	movq	%rdx, %xmm7
	movaps	%xmm5, 224(%rsp)
	leaq	.LC12(%rip), %rax
	punpcklqdq	%xmm0, %xmm7
	punpcklqdq	%xmm3, %xmm11
	movaps	%xmm2, (%rsp)
	movq	%rax, %xmm10
	leaq	.LC11(%rip), %rax
	movaps	%xmm7, 16(%rsp)
	movq	%rax, %xmm4
	leaq	.LC14(%rip), %rax
	movdqa	%xmm10, %xmm1
	movaps	%xmm11, 208(%rsp)
	movq	%rax, %xmm8
	leaq	.LC13(%rip), %rax
	punpcklqdq	%xmm10, %xmm4
	movq	%rax, %xmm9
	leaq	.LC22(%rip), %rax
	movaps	%xmm4, 432(%rsp)
	movdqa	%xmm8, %xmm4
	movq	%rax, %xmm6
	movdqa	%xmm9, %xmm15
	punpcklqdq	%xmm9, %xmm1
	leaq	.LC23(%rip), %rax
	movdqa	%xmm6, %xmm14
	punpcklqdq	%xmm6, %xmm4
	movaps	%xmm1, 32(%rsp)
	movq	%rax, %xmm0
	punpcklqdq	%xmm8, %xmm15
	movq	%rbp, %xmm9
	movaps	%xmm4, 48(%rsp)
	leaq	.LC24(%rip), %rax
	punpcklqdq	%xmm0, %xmm14
	movaps	%xmm15, 416(%rsp)
	movq	%rbp, %xmm15
	movaps	%xmm14, 400(%rsp)
	movq	%rax, %xmm5
	leaq	.LC25(%rip), %rax
	movq	%rax, %xmm0
	movdqa	%xmm5, %xmm13
	leaq	.LC26(%rip), %rax
	punpcklqdq	%xmm0, %xmm13
	movq	%rax, %xmm0
	movdqa	%xmm5, %xmm6
	leaq	.LC29(%rip), %rax
	punpcklqdq	%xmm0, %xmm6
	movdqa	%xmm0, %xmm12
	movaps	%xmm13, 384(%rsp)
	movq	%rax, %xmm13
	leaq	.LC30(%rip), %rax
	movaps	%xmm6, 64(%rsp)
	movq	%rax, %xmm7
	leaq	.LC32(%rip), %rax
	punpcklqdq	%xmm13, %xmm12
	movaps	%xmm12, 368(%rsp)
	movq	%rax, %xmm14
	leaq	.LC34(%rip), %rax
	movdqa	%xmm7, %xmm12
	movq	%rax, %xmm3
	leaq	.LC320(%rip), %rax
	punpcklqdq	%xmm14, %xmm12
	movq	%rax, %xmm0
	leaq	.LC323(%rip), %rax
	punpcklqdq	%xmm15, %xmm3
	movaps	%xmm12, 352(%rsp)
	movq	%rax, %xmm10
	leaq	.LC334(%rip), %rax
	movdqa	%xmm0, %xmm8
	movaps	%xmm3, 336(%rsp)
	movq	%rax, %xmm6
	punpcklqdq	%xmm10, %xmm0
	punpcklqdq	%xmm7, %xmm8
	leaq	.LC336(%rip), %rax
	punpcklqdq	%xmm7, %xmm6
	movaps	%xmm0, 304(%rsp)
	movq	%rax, %xmm5
	leaq	.LC337(%rip), %rax
	movaps	%xmm8, 320(%rsp)
	movq	%rax, %xmm0
	punpcklqdq	%xmm7, %xmm5
	movaps	%xmm6, 288(%rsp)
	punpcklqdq	%xmm0, %xmm9
	movaps	%xmm5, 272(%rsp)
	movaps	%xmm9, 256(%rsp)
	call	linenoiseSetMultiLine@PLT
	movq	%r12, %rsi
	movl	$2, %edi
	call	signal@PLT
	movq	%r12, %rsi
	movl	$15, %edi
	leaq	2016(%rsp), %r12
	call	signal@PLT
	movdqa	64(%rsp), %xmm6
	movdqa	(%rsp), %xmm2
	leaq	.LC322(%rip), %rax
	movq	%rax, %xmm0
	leaq	.LC321(%rip), %rcx
	leaq	.LC325(%rip), %rax
	movdqa	256(%rsp), %xmm9
	movq	%rcx, %xmm7
	movq	%rax, %xmm10
	leaq	.LC324(%rip), %rcx
	movdqa	32(%rsp), %xmm1
	punpcklqdq	%xmm0, %xmm7
	leaq	.LC327(%rip), %rax
	movdqa	304(%rsp), %xmm0
	movdqa	48(%rsp), %xmm4
	movq	%rax, %xmm13
	leaq	.LC329(%rip), %rax
	movdqa	272(%rsp), %xmm5
	movaps	%xmm7, 3328(%rsp)
	movq	%rax, %xmm14
	movaps	%xmm0, 3424(%rsp)
	movq	%rcx, %xmm0
	leaq	.LC326(%rip), %rcx
	punpcklqdq	%xmm10, %xmm0
	leaq	.LC331(%rip), %rax
	movdqa	16(%rsp), %xmm7
	movaps	%xmm6, 3408(%rsp)
	movaps	%xmm0, 3440(%rsp)
	movq	%rcx, %xmm0
	leaq	.LC328(%rip), %rcx
	movq	%rax, %xmm15
	punpcklqdq	%xmm13, %xmm0
	leaq	.LC333(%rip), %rax
	movdqa	288(%rsp), %xmm6
	leaq	3328(%rsp), %rsi
	movaps	%xmm0, 3456(%rsp)
	movq	%rcx, %xmm0
	leaq	.LC330(%rip), %rcx
	movq	%rax, %xmm10
	punpcklqdq	%xmm14, %xmm0
	leaq	.LC335(%rip), %rax
	movl	$32, %edx
	movaps	%xmm9, 3568(%rsp)
	movaps	%xmm0, 3472(%rsp)
	movq	%rcx, %xmm0
	leaq	.LC332(%rip), %rcx
	movq	%rax, %xmm13
	punpcklqdq	%xmm15, %xmm0
	leaq	acceptedChars(%rip), %rdi
	movaps	%xmm2, 3344(%rsp)
	movaps	%xmm0, 3488(%rsp)
	movq	%rcx, %xmm0
	leaq	.LC27(%rip), %rcx
	punpcklqdq	%xmm10, %xmm0
	movaps	%xmm7, 3360(%rsp)
	movaps	%xmm0, 3504(%rsp)
	movq	%rcx, %xmm0
	punpcklqdq	%xmm13, %xmm0
	movaps	%xmm1, 3376(%rsp)
	movaps	%xmm4, 3392(%rsp)
	movaps	%xmm6, 3520(%rsp)
	movaps	%xmm0, 3536(%rsp)
	movaps	%xmm5, 3552(%rsp)
	call	strlist_copy@PLT
	movdqa	.LC339(%rip), %xmm5
	xorl	%eax, %eax
	leaq	4224(%rsp), %rdi
	movl	$44, %ecx
	movdqa	.LC338(%rip), %xmm0
	movl	$19, 976(%rsp)
	leaq	648(%rsp), %rdx
	rep stosq
	leaq	960(%rsp), %rax
	movaps	%xmm5, 1408(%rsp)
	movdqa	.LC340(%rip), %xmm5
	movq	%rax, 4224(%rsp)
	leaq	784(%rsp), %rax
	leaq	4224(%rsp), %rsi
	movq	%rax, 4240(%rsp)
	leaq	1408(%rsp), %rax
	leaq	syntax(%rip), %rdi
	movq	%rax, 4256(%rsp)
	leaq	800(%rsp), %rax
	movaps	%xmm5, 1424(%rsp)
	movdqa	.LC341(%rip), %xmm5
	movq	%rax, 4272(%rsp)
	leaq	992(%rsp), %rax
	movq	%rax, 4288(%rsp)
	leaq	1024(%rsp), %rax
	movaps	%xmm5, 1440(%rsp)
	movdqa	.LC342(%rip), %xmm5
	movq	%rax, 4304(%rsp)
	leaq	816(%rsp), %rax
	movaps	%xmm0, 960(%rsp)
	movaps	%xmm0, 784(%rsp)
	movaps	%xmm0, 800(%rsp)
	movaps	%xmm0, 992(%rsp)
	movaps	%xmm0, 1024(%rsp)
	movaps	%xmm0, 816(%rsp)
	movq	%rax, 4320(%rsp)
	leaq	832(%rsp), %rax
	movaps	%xmm5, 1456(%rsp)
	movl	$5, 4232(%rsp)
	movl	$4, 4248(%rsp)
	movl	$16, 4264(%rsp)
	movl	$4, 4280(%rsp)
	movl	$24, 1008(%rsp)
	movl	$5, 4296(%rsp)
	movl	$24, 1040(%rsp)
	movl	$5, 4312(%rsp)
	movl	$4, 4328(%rsp)
	movdqa	.LC343(%rip), %xmm5
	movq	%rax, 4336(%rsp)
	leaq	1056(%rsp), %rax
	movaps	%xmm5, 832(%rsp)
	movdqa	.LC345(%rip), %xmm5
	movq	%rax, 4352(%rsp)
	leaq	848(%rsp), %rax
	movaps	%xmm5, 1536(%rsp)
	movdqa	.LC346(%rip), %xmm5
	movq	%rax, 4368(%rsp)
	leaq	864(%rsp), %rax
	movaps	%xmm5, 1552(%rsp)
	movdqa	.LC347(%rip), %xmm5
	movq	%rdx, 4400(%rsp)
	movq	.LC349(%rip), %rdx
	movaps	%xmm5, 1568(%rsp)
	movdqa	.LC348(%rip), %xmm5
	movq	%rax, 4384(%rsp)
	movq	.LC344(%rip), %rax
	movaps	%xmm5, 1584(%rsp)
	movdqa	.LC350(%rip), %xmm5
	movq	%rdx, 1600(%rsp)
	leaq	1536(%rsp), %rdx
	movaps	%xmm0, 1056(%rsp)
	movaps	%xmm0, 848(%rsp)
	movaps	%xmm0, 864(%rsp)
	movq	%rax, 648(%rsp)
	movq	%rdx, 4416(%rsp)
	leaq	880(%rsp), %rdx
	movl	$4, 4344(%rsp)
	movl	$14, 1072(%rsp)
	movl	$5, 4360(%rsp)
	movl	$4, 4376(%rsp)
	movl	$4, 4392(%rsp)
	movl	$2, 4408(%rsp)
	movl	$20, 1608(%rsp)
	movl	$19, 4424(%rsp)
	movaps	%xmm5, 880(%rsp)
	movdqa	.LC351(%rip), %xmm5
	movq	%rax, 656(%rsp)
	leaq	656(%rsp), %rax
	movaps	%xmm5, 1248(%rsp)
	movdqa	.LC352(%rip), %xmm5
	movq	%rdx, 4432(%rsp)
	movq	.LC353(%rip), %rdx
	movaps	%xmm5, 1264(%rsp)
	movdqa	.LC354(%rip), %xmm5
	movq	%rax, 4480(%rsp)
	leaq	896(%rsp), %rax
	movaps	%xmm5, 1344(%rsp)
	movdqa	.LC355(%rip), %xmm5
	movq	%rax, 4496(%rsp)
	leaq	492(%rsp), %rax
	movq	%rdx, 1280(%rsp)
	leaq	1248(%rsp), %rdx
	movq	%rax, 4512(%rsp)
	leaq	496(%rsp), %rax
	movaps	%xmm5, 1360(%rsp)
	movdqa	.LC356(%rip), %xmm5
	movq	%rdx, 4448(%rsp)
	leaq	1344(%rsp), %rdx
	movq	%rax, 4528(%rsp)
	leaq	912(%rsp), %rax
	movaps	%xmm5, 1376(%rsp)
	movaps	%xmm0, 896(%rsp)
	movq	%rdx, 4464(%rsp)
	movl	$22, %edx
	movl	$4, 4440(%rsp)
	movl	$8, 1288(%rsp)
	movl	$11, 4456(%rsp)
	movl	$20, 1392(%rsp)
	movl	$13, 4472(%rsp)
	movl	$2, 4488(%rsp)
	movl	$4, 4504(%rsp)
	movl	$12, 492(%rsp)
	movl	$1, 4520(%rsp)
	movl	$12, 496(%rsp)
	movl	$1, 4536(%rsp)
	movaps	%xmm0, 912(%rsp)
	movq	%rax, 4544(%rsp)
	leaq	1088(%rsp), %rax
	movq	%rax, 4560(%rsp)
	movaps	%xmm0, 1088(%rsp)
	movl	$4, 4552(%rsp)
	movl	$24, 1104(%rsp)
	movl	$5, 4568(%rsp)
	call	listlist_copy@PLT
	movdqa	.LC359(%rip), %xmm5
	movl	$22, %edx
	movdqa	.LC357(%rip), %xmm0
	leaq	1712(%rsp), %rsi
	leaq	types_debut(%rip), %rdi
	movq	$20, 1792(%rsp)
	movaps	%xmm5, 1744(%rsp)
	movdqa	.LC360(%rip), %xmm5
	movaps	%xmm0, 1712(%rsp)
	movdqa	.LC358(%rip), %xmm0
	movaps	%xmm5, 1760(%rsp)
	movdqa	.LC361(%rip), %xmm5
	movaps	%xmm0, 1728(%rsp)
	movaps	%xmm5, 1776(%rsp)
	call	intlist_copy@PLT
	movdqa	.LC358(%rip), %xmm0
	movl	$21, %edx
	movdqa	.LC362(%rip), %xmm5
	leaq	1616(%rsp), %rsi
	leaq	types_fin(%rip), %rdi
	movl	$0, 1696(%rsp)
	movaps	%xmm0, 1632(%rsp)
	movdqa	.LC363(%rip), %xmm0
	movaps	%xmm5, 1616(%rsp)
	movaps	%xmm0, 1648(%rsp)
	movdqa	.LC364(%rip), %xmm0
	movaps	%xmm0, 1664(%rsp)
	movdqa	.LC365(%rip), %xmm0
	movaps	%xmm0, 1680(%rsp)
	call	intlist_copy@PLT
	movdqa	32(%rsp), %xmm1
	movdqa	(%rsp), %xmm2
	leaq	1904(%rsp), %rsi
	movdqa	48(%rsp), %xmm4
	movdqa	16(%rsp), %xmm7
	movl	$13, %edx
	leaq	sousop(%rip), %rdi
	movdqa	64(%rsp), %xmm6
	movaps	%xmm1, 1936(%rsp)
	movdqa	320(%rsp), %xmm8
	movaps	%xmm2, 1904(%rsp)
	movaps	%xmm4, 1952(%rsp)
	movq	%rbp, 2000(%rsp)
	movaps	%xmm7, 1920(%rsp)
	movaps	%xmm6, 1968(%rsp)
	movaps	%xmm8, 1984(%rsp)
	call	strlist_copy@PLT
	movdqa	.LC371(%rip), %xmm6
	movdqa	.LC367(%rip), %xmm10
	leaq	2144(%rsp), %rsi
	movdqa	.LC368(%rip), %xmm9
	movdqa	.LC366(%rip), %xmm0
	movl	$40, %edx
	leaq	gramm1(%rip), %rdi
	movdqa	.LC370(%rip), %xmm7
	movdqa	.LC369(%rip), %xmm8
	movaps	%xmm10, 2192(%rsp)
	movdqa	.LC372(%rip), %xmm5
	movaps	%xmm6, 2256(%rsp)
	movaps	%xmm9, 2208(%rsp)
	movaps	%xmm8, 2224(%rsp)
	movaps	%xmm0, 2144(%rsp)
	movaps	%xmm0, 2160(%rsp)
	movaps	%xmm0, 2176(%rsp)
	movaps	%xmm7, 2240(%rsp)
	movaps	%xmm0, 2272(%rsp)
	movaps	%xmm5, 2288(%rsp)
	call	intlist_copy@PLT
	movl	$40, %edx
	movl	$40, %ecx
	leaq	3584(%rsp), %rdi
	leaq	.LC373(%rip), %rsi
	rep movsq
	leaq	3584(%rsp), %rsi
	leaq	operateurs3(%rip), %rdi
	call	strlist_copy@PLT
	movdqa	(%rsp), %xmm2
	movl	$31, %edx
	leaq	.LC8(%rip), %rax
	leaq	.LC7(%rip), %rcx
	movq	%rax, %xmm0
	movdqa	432(%rsp), %xmm4
	leaq	.LC10(%rip), %rax
	movq	%rcx, %xmm5
	movdqa	16(%rsp), %xmm7
	leaq	.LC9(%rip), %rcx
	movdqa	336(%rsp), %xmm3
	punpcklqdq	%xmm0, %xmm5
	movq	%rax, %xmm0
	leaq	.LC16(%rip), %rax
	movaps	%xmm4, 3136(%rsp)
	movaps	%xmm5, 3104(%rsp)
	movq	%rcx, %xmm5
	leaq	.LC15(%rip), %rcx
	movdqa	384(%rsp), %xmm13
	punpcklqdq	%xmm0, %xmm5
	movq	%rcx, %xmm4
	movq	%rax, %xmm0
	movdqa	368(%rsp), %xmm12
	punpcklqdq	%xmm0, %xmm4
	leaq	.LC18(%rip), %rax
	leaq	.LC17(%rip), %rcx
	movdqa	352(%rsp), %xmm11
	movq	%rax, %xmm0
	movaps	%xmm4, 3168(%rsp)
	movq	%rcx, %xmm4
	leaq	.LC20(%rip), %rax
	punpcklqdq	%xmm0, %xmm4
	movq	%rax, %xmm0
	leaq	.LC19(%rip), %rcx
	movdqa	416(%rsp), %xmm15
	movaps	%xmm4, 3184(%rsp)
	leaq	.LC37(%rip), %rax
	movq	%rcx, %xmm4
	leaq	.LC36(%rip), %rcx
	punpcklqdq	%xmm0, %xmm4
	movaps	%xmm3, 3280(%rsp)
	movdqa	400(%rsp), %xmm14
	movq	%rax, %xmm0
	movq	%rcx, %xmm3
	leaq	.LC38(%rip), %rax
	leaq	3072(%rsp), %rsi
	movaps	%xmm13, 3232(%rsp)
	punpcklqdq	%xmm0, %xmm3
	leaq	operateurs1(%rip), %rdi
	movaps	%xmm12, 3248(%rsp)
	movaps	%xmm11, 3264(%rsp)
	movaps	%xmm2, 3072(%rsp)
	movaps	%xmm5, 3120(%rsp)
	movaps	%xmm15, 3152(%rsp)
	movaps	%xmm4, 3200(%rsp)
	movaps	%xmm14, 3216(%rsp)
	movaps	%xmm7, 3088(%rsp)
	movq	%rax, 3312(%rsp)
	movaps	%xmm3, 3296(%rsp)
	call	strlist_copy@PLT
	leaq	.LC1(%rip), %rax
	movl	$8, %edx
	leaq	.LC0(%rip), %rcx
	movq	%rax, %xmm0
	movq	%rcx, %xmm3
	leaq	.LC21(%rip), %rax
	punpcklqdq	%xmm0, %xmm3
	leaq	.LC2(%rip), %rcx
	movq	%rax, %xmm0
	movaps	%xmm3, 1472(%rsp)
	movq	%rcx, %xmm3
	leaq	.LC31(%rip), %rax
	leaq	.LC28(%rip), %rcx
	punpcklqdq	%xmm0, %xmm3
	movq	%rax, %xmm0
	leaq	.LC39(%rip), %rax
	movaps	%xmm3, 1488(%rsp)
	movq	%rcx, %xmm3
	leaq	.LC33(%rip), %rcx
	leaq	1472(%rsp), %rsi
	punpcklqdq	%xmm0, %xmm3
	movq	%rax, %xmm0
	leaq	operateurs2(%rip), %rdi
	movaps	%xmm3, 1504(%rsp)
	movq	%rcx, %xmm3
	punpcklqdq	%xmm0, %xmm3
	movaps	%xmm3, 1520(%rsp)
	call	strlist_copy@PLT
	movl	$40, %edx
	movl	$40, %ecx
	leaq	3904(%rsp), %rdi
	leaq	.LC373(%rip), %rsi
	rep movsq
	leaq	3904(%rsp), %rsi
	leaq	OPERATEURS(%rip), %rdi
	call	strlist_copy@PLT
	movdqa	.LC374(%rip), %xmm3
	leaq	2304(%rsp), %rsi
	movl	$40, %edx
	leaq	PRIORITE(%rip), %rdi
	movaps	%xmm3, 2304(%rsp)
	movdqa	.LC375(%rip), %xmm3
	movaps	%xmm3, 2320(%rsp)
	movdqa	.LC376(%rip), %xmm3
	movaps	%xmm3, 2336(%rsp)
	movdqa	.LC377(%rip), %xmm3
	movaps	%xmm3, 2352(%rsp)
	movdqa	.LC378(%rip), %xmm3
	movaps	%xmm3, 2368(%rsp)
	movdqa	.LC379(%rip), %xmm3
	movaps	%xmm3, 2384(%rsp)
	movdqa	.LC380(%rip), %xmm3
	movaps	%xmm3, 2400(%rsp)
	movdqa	.LC381(%rip), %xmm3
	movaps	%xmm3, 2416(%rsp)
	movdqa	.LC382(%rip), %xmm3
	movaps	%xmm3, 2432(%rsp)
	movdqa	.LC383(%rip), %xmm3
	movaps	%xmm3, 2448(%rsp)
	call	intlist_copy@PLT
	movdqa	.LC366(%rip), %xmm0
	movdqa	.LC367(%rip), %xmm10
	leaq	2464(%rsp), %rsi
	movdqa	.LC368(%rip), %xmm9
	movdqa	.LC371(%rip), %xmm6
	movl	$40, %edx
	leaq	OPERANDES(%rip), %rdi
	movaps	%xmm0, 2480(%rsp)
	movdqa	.LC370(%rip), %xmm7
	movdqa	.LC369(%rip), %xmm8
	movaps	%xmm0, 2496(%rsp)
	movdqa	.LC385(%rip), %xmm0
	movdqa	.LC384(%rip), %xmm3
	movaps	%xmm10, 2512(%rsp)
	movaps	%xmm0, 2592(%rsp)
	movdqa	.LC386(%rip), %xmm0
	movaps	%xmm9, 2528(%rsp)
	movaps	%xmm8, 2544(%rsp)
	movaps	%xmm6, 2576(%rsp)
	movaps	%xmm7, 2560(%rsp)
	movaps	%xmm3, 2464(%rsp)
	movaps	%xmm0, 2608(%rsp)
	call	intlist_copy@PLT
	leaq	.LC315(%rip), %rax
	movl	$12, %edx
	leaq	.LC314(%rip), %rcx
	movq	%rax, %xmm14
	movq	%rcx, %xmm0
	leaq	.LC317(%rip), %rax
	movdqa	224(%rsp), %xmm5
	punpcklqdq	%xmm14, %xmm0
	leaq	.LC316(%rip), %rcx
	movq	%rax, %xmm15
	movdqa	464(%rsp), %xmm2
	movdqa	448(%rsp), %xmm1
	movaps	%xmm0, 1808(%rsp)
	movdqa	240(%rsp), %xmm4
	movq	%rcx, %xmm0
	punpcklqdq	%xmm15, %xmm0
	leaq	1808(%rsp), %rsi
	leaq	blockwords(%rip), %rdi
	movaps	%xmm5, 1872(%rsp)
	movaps	%xmm2, 1840(%rsp)
	movaps	%xmm1, 1856(%rsp)
	movaps	%xmm0, 1824(%rsp)
	movaps	%xmm4, 1888(%rsp)
	call	strlist_copy@PLT
	leaq	1216(%rsp), %rsi
	movdqa	192(%rsp), %xmm3
	movdqa	208(%rsp), %xmm11
	movl	$4, %edx
	leaq	blockwords1Line(%rip), %rdi
	movaps	%xmm3, 1216(%rsp)
	movaps	%xmm11, 1232(%rsp)
	call	strlist_copy@PLT
	leaq	.LC308(%rip), %rcx
	movl	$2, %edx
	leaq	.LC309(%rip), %rax
	movq	%rax, %xmm4
	movq	%rcx, %xmm0
	leaq	928(%rsp), %rsi
	punpcklqdq	%xmm4, %xmm0
	leaq	keywordFunction(%rip), %rdi
	movaps	%xmm0, 928(%rsp)
	call	strlist_copy@PLT
	leaq	1296(%rsp), %rsi
	movdqa	144(%rsp), %xmm2
	movdqa	176(%rsp), %xmm7
	movdqa	160(%rsp), %xmm1
	movl	$6, %edx
	leaq	keywords(%rip), %rdi
	movaps	%xmm2, 1296(%rsp)
	movaps	%xmm7, 1328(%rsp)
	movaps	%xmm1, 1312(%rsp)
	call	strlist_copy@PLT
	leaq	1120(%rsp), %rsi
	movdqa	128(%rsp), %xmm6
	movl	$3, %edx
	leaq	lkeywords(%rip), %rdi
	movq	%rbx, 1136(%rsp)
	leaq	.LC300(%rip), %rbx
	movaps	%xmm6, 1120(%rsp)
	call	strlist_copy@PLT
	movq	%rbx, %xmm0
	movl	$2, %edx
	leaq	.LC301(%rip), %rax
	movq	%rax, %xmm1
	leaq	944(%rsp), %rsi
	leaq	neon_boolean(%rip), %rdi
	punpcklqdq	%xmm1, %xmm0
	leaq	.LC286(%rip), %rbx
	movaps	%xmm0, 944(%rsp)
	call	strlist_copy@PLT
	movq	%rbx, %xmm0
	movl	$128, %edi
	leaq	.LC287(%rip), %rax
	movq	%rax, %xmm6
	leaq	.LC288(%rip), %rcx
	leaq	.LC289(%rip), %rax
	punpcklqdq	%xmm6, %xmm0
	movq	%rax, %xmm2
	leaq	.LC290(%rip), %rbx
	movaps	%xmm0, 2016(%rsp)
	movq	%rcx, %xmm0
	leaq	.LC291(%rip), %rax
	leaq	.LC292(%rip), %rcx
	punpcklqdq	%xmm2, %xmm0
	movq	%rax, %xmm1
	leaq	.LC293(%rip), %rax
	movaps	%xmm0, 2032(%rsp)
	movq	%rbx, %xmm0
	movq	%rax, %xmm6
	leaq	.LC294(%rip), %rbx
	punpcklqdq	%xmm1, %xmm0
	leaq	.LC295(%rip), %rax
	movaps	%xmm0, 2048(%rsp)
	movq	%rcx, %xmm0
	movq	%rax, %xmm7
	leaq	.LC296(%rip), %rcx
	punpcklqdq	%xmm6, %xmm0
	leaq	.LC297(%rip), %rax
	movaps	%xmm0, 2064(%rsp)
	movq	%rbx, %xmm0
	movq	%rax, %xmm2
	leaq	.LC298(%rip), %rbx
	punpcklqdq	%xmm7, %xmm0
	leaq	.LC299(%rip), %rax
	movaps	%xmm0, 2080(%rsp)
	movq	%rcx, %xmm0
	movq	%rax, %xmm1
	leaq	.LC387(%rip), %rax
	punpcklqdq	%xmm2, %xmm0
	movq	%rax, 2128(%rsp)
	movaps	%xmm0, 2096(%rsp)
	movq	%rbx, %xmm0
	xorl	%ebx, %ebx
	punpcklqdq	%xmm1, %xmm0
	movaps	%xmm0, 2112(%rsp)
	call	malloc@PLT
	movq	%rax, exceptions(%rip)
	movq	%rax, %rbp
	movq	.LC388(%rip), %rax
	movq	%rax, 8+exceptions(%rip)
	.p2align 4,,10
	.p2align 3
.L77:
	movq	(%r12,%rbx), %rdi
	call	strdup@PLT
	movq	%rax, 0(%rbp,%rbx)
	addq	$8, %rbx
	cmpq	$120, %rbx
	jne	.L77
	leaq	.LC284(%rip), %rcx
	leaq	.LC285(%rip), %rax
	movl	$3, %edx
	movq	%rax, %xmm6
	movq	%rcx, %xmm0
	leaq	.LC389(%rip), %rax
	punpcklqdq	%xmm6, %xmm0
	leaq	1152(%rsp), %rsi
	leaq	constant(%rip), %rdi
	movq	%rax, 1168(%rsp)
	movaps	%xmm0, 1152(%rsp)
	call	strlist_copy@PLT
	movl	$55, %edx
	movl	$55, %ecx
	leaq	4576(%rsp), %rdi
	leaq	.LC390(%rip), %rsi
	rep movsq
	leaq	4576(%rsp), %rsi
	leaq	NOMSBUILTINSFONC(%rip), %rdi
	call	strlist_copy@PLT
	movdqa	.LC391(%rip), %xmm0
	movq	.LC402(%rip), %rax
	movl	$17, 2840(%rsp)
	leaq	5024(%rsp), %rdi
	leaq	.LC403(%rip), %rsi
	movl	$55, %ecx
	rep movsq
	movaps	%xmm0, 2624(%rsp)
	movdqa	.LC392(%rip), %xmm0
	leaq	1184(%rsp), %rcx
	movq	%rax, 2832(%rsp)
	leaq	6384(%rsp), %rdi
	leaq	.LC426(%rip), %rsi
	movaps	%xmm0, 2640(%rsp)
	movdqa	.LC393(%rip), %xmm0
	movaps	%xmm0, 2656(%rsp)
	movdqa	.LC394(%rip), %xmm0
	movaps	%xmm0, 2672(%rsp)
	movdqa	.LC395(%rip), %xmm0
	movaps	%xmm0, 2688(%rsp)
	movdqa	.LC396(%rip), %xmm0
	movaps	%xmm0, 2704(%rsp)
	movdqa	.LC397(%rip), %xmm0
	movaps	%xmm0, 2720(%rsp)
	movdqa	.LC398(%rip), %xmm0
	movaps	%xmm0, 2736(%rsp)
	movdqa	.LC399(%rip), %xmm0
	movaps	%xmm0, 2752(%rsp)
	movaps	%xmm0, 2768(%rsp)
	movaps	%xmm0, 2784(%rsp)
	movdqa	.LC400(%rip), %xmm0
	movaps	%xmm0, 2800(%rsp)
	movdqa	.LC401(%rip), %xmm0
	movaps	%xmm0, 2816(%rsp)
	movdqa	.LC404(%rip), %xmm0
	movaps	%xmm0, 2848(%rsp)
	movdqa	.LC405(%rip), %xmm0
	movaps	%xmm0, 2864(%rsp)
	movdqa	.LC406(%rip), %xmm0
	movdqa	.LC412(%rip), %xmm1
	movq	.LC415(%rip), %rax
	movdqa	112(%rsp), %xmm7
	movl	$0, 3064(%rsp)
	movaps	%xmm0, 2880(%rsp)
	movdqa	.LC407(%rip), %xmm0
	movaps	%xmm1, 2976(%rsp)
	movaps	%xmm0, 2896(%rsp)
	movdqa	.LC408(%rip), %xmm0
	movaps	%xmm1, 2992(%rsp)
	movdqa	.LC413(%rip), %xmm1
	movaps	%xmm0, 2912(%rsp)
	movdqa	.LC409(%rip), %xmm0
	movq	%rax, 3056(%rsp)
	movq	.LC416(%rip), %rax
	movaps	%xmm0, 2928(%rsp)
	movdqa	.LC410(%rip), %xmm0
	movaps	%xmm1, 3008(%rsp)
	movdqa	.LC414(%rip), %xmm1
	movaps	%xmm0, 2944(%rsp)
	movdqa	.LC411(%rip), %xmm0
	movq	%rax, 736(%rsp)
	leaq	664(%rsp), %rax
	movaps	%xmm0, 2960(%rsp)
	movaps	%xmm1, 3024(%rsp)
	movaps	%xmm0, 3040(%rsp)
	movl	$127, 500(%rsp)
	movl	$127, 504(%rsp)
	movl	$131, 508(%rsp)
	movl	$127, 512(%rsp)
	movl	$127, 516(%rsp)
	movl	$4, 744(%rsp)
	movaps	%xmm7, 5472(%rsp)
	movdqa	96(%rsp), %xmm2
	movdqa	80(%rsp), %xmm1
	movq	%rax, 5528(%rsp)
	movq	.LC418(%rip), %rax
	movq	.LC417(%rip), %rdx
	movaps	%xmm2, 5488(%rsp)
	movq	%rax, 672(%rsp)
	leaq	672(%rsp), %rax
	movq	%rax, 5536(%rsp)
	leaq	752(%rsp), %rax
	movq	%rax, 5544(%rsp)
	leaq	520(%rsp), %rax
	movq	%rax, 5552(%rsp)
	leaq	524(%rsp), %rax
	movq	%rax, 5560(%rsp)
	leaq	528(%rsp), %rax
	movq	%rax, 5568(%rsp)
	leaq	532(%rsp), %rax
	movq	%rax, 5584(%rsp)
	movq	.LC419(%rip), %rax
	movaps	%xmm1, 5504(%rsp)
	movq	%rax, 680(%rsp)
	leaq	680(%rsp), %rax
	movq	%rax, 5592(%rsp)
	leaq	536(%rsp), %rax
	movq	%rax, 5600(%rsp)
	leaq	540(%rsp), %rax
	movq	%rax, 5616(%rsp)
	leaq	544(%rsp), %rax
	movq	%rdx, 664(%rsp)
	movq	%rdx, 752(%rsp)
	movq	$0, 5520(%rsp)
	movl	$4, 760(%rsp)
	movl	$127, 520(%rsp)
	movl	$131, 524(%rsp)
	movl	$131, 528(%rsp)
	movq	$0, 5576(%rsp)
	movl	$127, 532(%rsp)
	movl	$131, 536(%rsp)
	movq	$0, 5608(%rsp)
	movl	$8, 540(%rsp)
	movl	$127, 544(%rsp)
	movq	%rax, 5624(%rsp)
	leaq	548(%rsp), %rax
	movdqa	.LC420(%rip), %xmm0
	movq	%rdx, 696(%rsp)
	leaq	696(%rsp), %rdx
	movq	%rdx, 5680(%rsp)
	leaq	768(%rsp), %rdx
	movq	%rdx, 5688(%rsp)
	movq	.LC423(%rip), %rdx
	movq	%rcx, 5648(%rsp)
	leaq	556(%rsp), %rcx
	movq	%rdx, 704(%rsp)
	leaq	704(%rsp), %rdx
	movq	%rdx, 5696(%rsp)
	leaq	564(%rsp), %rdx
	movq	%rcx, 5656(%rsp)
	movq	.LC422(%rip), %rcx
	movq	%rdx, 5704(%rsp)
	leaq	568(%rsp), %rdx
	movq	%rax, 5632(%rsp)
	leaq	552(%rsp), %rax
	movq	%rdx, 5712(%rsp)
	leaq	572(%rsp), %rdx
	movq	%rax, 5640(%rsp)
	movq	.LC421(%rip), %rax
	movq	%rcx, 688(%rsp)
	leaq	688(%rsp), %rcx
	movq	%rdx, 5720(%rsp)
	leaq	576(%rsp), %rdx
	movq	%rcx, 5664(%rsp)
	leaq	560(%rsp), %rcx
	movq	%rdx, 5728(%rsp)
	leaq	580(%rsp), %rdx
	movaps	%xmm0, 1184(%rsp)
	movq	%rax, 1200(%rsp)
	movq	%rcx, 5672(%rsp)
	movl	$115, %ecx
	movq	%rax, 768(%rsp)
	movl	$4, 548(%rsp)
	movl	$131, 552(%rsp)
	movl	$131, 556(%rsp)
	movl	$127, 560(%rsp)
	movl	$131, 776(%rsp)
	movl	$131, 564(%rsp)
	movl	$134, 568(%rsp)
	movl	$134, 572(%rsp)
	movl	$127, 576(%rsp)
	movl	$127, 580(%rsp)
	movq	%rdx, 5736(%rsp)
	leaq	584(%rsp), %rdx
	movq	%rdx, 5744(%rsp)
	leaq	588(%rsp), %rdx
	movq	%rdx, 5752(%rsp)
	leaq	592(%rsp), %rdx
	movq	%rdx, 5760(%rsp)
	leaq	596(%rsp), %rdx
	movq	%rdx, 5768(%rsp)
	leaq	600(%rsp), %rdx
	movq	%rdx, 5776(%rsp)
	leaq	604(%rsp), %rdx
	movq	%rdx, 5784(%rsp)
	leaq	608(%rsp), %rdx
	movq	%rdx, 5792(%rsp)
	leaq	612(%rsp), %rdx
	movq	%rdx, 5800(%rsp)
	movq	.LC424(%rip), %rdx
	movq	%rax, 720(%rsp)
	leaq	720(%rsp), %rax
	movq	%rdx, 712(%rsp)
	leaq	712(%rsp), %rdx
	movq	%rdx, 5808(%rsp)
	leaq	616(%rsp), %rdx
	movq	%rdx, 5816(%rsp)
	leaq	620(%rsp), %rdx
	movq	%rax, 5848(%rsp)
	movq	.LC425(%rip), %rax
	movq	%rdx, 5824(%rsp)
	leaq	624(%rsp), %rdx
	movq	%rdx, 5832(%rsp)
	leaq	628(%rsp), %rdx
	movq	%rax, 728(%rsp)
	leaq	728(%rsp), %rax
	movq	%rdx, 5840(%rsp)
	movl	$115, %edx
	movl	$127, 584(%rsp)
	movl	$127, 588(%rsp)
	movl	$127, 592(%rsp)
	movl	$127, 596(%rsp)
	movl	$127, 600(%rsp)
	movl	$127, 604(%rsp)
	movl	$127, 608(%rsp)
	movl	$127, 612(%rsp)
	movl	$5, 616(%rsp)
	movl	$5, 620(%rsp)
	movl	$5, 624(%rsp)
	movl	$131, 628(%rsp)
	movq	%rax, 5856(%rsp)
	leaq	632(%rsp), %rax
	rep movsq
	movq	%rax, 5864(%rsp)
	leaq	636(%rsp), %rax
	leaq	6384(%rsp), %rsi
	movq	%rax, 5872(%rsp)
	leaq	640(%rsp), %rax
	leaq	ERRORS_MESSAGES(%rip), %rdi
	movq	%rax, 5880(%rsp)
	leaq	644(%rsp), %rax
	movq	%rax, 5896(%rsp)
	movl	$4, 632(%rsp)
	movl	$127, 636(%rsp)
	movl	$131, 640(%rsp)
	movq	$0, 5888(%rsp)
	movl	$131, 644(%rsp)
	movq	$0, 5904(%rsp)
	call	strlist_copy@PLT
	movl	$57, %ecx
	movl	$115, %edx
	leaq	5920(%rsp), %rdi
	leaq	.LC264(%rip), %rsi
	rep movsq
	movl	(%rsi), %eax
	leaq	5920(%rsp), %rsi
	movl	%eax, (%rdi)
	leaq	exceptions_err(%rip), %rdi
	call	intlist_copy@PLT
	xorl	%edi, %edi
	call	strlist_create@PLT
	xorl	%edi, %edi
	movq	%rax, CONTAINERS(%rip)
	call	nelist_create@PLT
	xorl	%edi, %edi
	movq	%rax, ATTRIBUTES(%rip)
	call	nelist_create@PLT
	xorl	%edi, %edi
	movq	%rax, PROMISES(%rip)
	call	intptrlist_create@PLT
	xorl	%edi, %edi
	movq	%rax, PROMISES_CNT(%rip)
	movq	%rdx, 8+PROMISES_CNT(%rip)
	call	intlist_create@PLT
	movq	%rdx, 8+PROCESS_FINISH(%rip)
	movq	%rax, PROCESS_FINISH(%rip)
	call	processCycle_create@PLT
	xorl	%edx, %edx
	xorl	%esi, %esi
	xorl	%edi, %edi
	movq	%rax, process_cycle(%rip)
	call	tree_create@PLT
	xorl	%edi, %edi
	movq	%rax, FONCTIONS(%rip)
	call	nelist_create@PLT
	xorl	%edi, %edi
	movq	%rax, ADRESSES(%rip)
	call	strlist_create@PLT
	leaq	2624(%rsp), %rcx
	leaq	5472(%rsp), %rdx
	leaq	2848(%rsp), %rsi
	leaq	5024(%rsp), %rdi
	movq	%rax, NOMS(%rip)
	call	compFunc@PLT
	xorl	%edi, %edi
	call	time@PLT
	movl	%eax, %edi
	call	srand@PLT
	call	defineVariables
	addq	$7312, %rsp
	.cfi_def_cfa_offset 32
	popq	%rbx
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r12
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE19:
	.size	neonInit, .-neonInit
	.p2align 4
	.globl	neonExit
	.type	neonExit, @function
neonExit:
.LFB20:
	.cfi_startproc
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movq	process_cycle(%rip), %rdi
	call	free@PLT
	movq	RETURN_VALUE(%rip), %rdi
	movq	8+RETURN_VALUE(%rip), %rsi
	call	neobject_destroy@PLT
	xorl	%esi, %esi
	leaq	acceptedChars(%rip), %rdi
	call	strlist_destroy@PLT
	leaq	syntax(%rip), %rdi
	call	listlist_destroy@PLT
	xorl	%esi, %esi
	leaq	sousop(%rip), %rdi
	call	strlist_destroy@PLT
	movq	gramm1(%rip), %rdi
	call	free@PLT
	movq	types_debut(%rip), %rdi
	call	free@PLT
	movq	types_fin(%rip), %rdi
	call	free@PLT
	xorl	%esi, %esi
	leaq	operateurs3(%rip), %rdi
	call	strlist_destroy@PLT
	xorl	%esi, %esi
	leaq	operateurs1(%rip), %rdi
	call	strlist_destroy@PLT
	xorl	%esi, %esi
	leaq	operateurs2(%rip), %rdi
	call	strlist_destroy@PLT
	xorl	%esi, %esi
	leaq	blockwords(%rip), %rdi
	call	strlist_destroy@PLT
	xorl	%esi, %esi
	leaq	blockwords1Line(%rip), %rdi
	call	strlist_destroy@PLT
	xorl	%esi, %esi
	leaq	keywords(%rip), %rdi
	call	strlist_destroy@PLT
	xorl	%esi, %esi
	leaq	lkeywords(%rip), %rdi
	call	strlist_destroy@PLT
	xorl	%esi, %esi
	leaq	keywordFunction(%rip), %rdi
	call	strlist_destroy@PLT
	xorl	%esi, %esi
	leaq	neon_boolean(%rip), %rdi
	call	strlist_destroy@PLT
	xorl	%esi, %esi
	leaq	exceptions(%rip), %rdi
	call	strlist_destroy@PLT
	xorl	%esi, %esi
	leaq	constant(%rip), %rdi
	call	strlist_destroy@PLT
	xorl	%esi, %esi
	leaq	OPERATEURS(%rip), %rdi
	call	strlist_destroy@PLT
	movq	PRIORITE(%rip), %rdi
	call	free@PLT
	movq	OPERANDES(%rip), %rdi
	call	free@PLT
	movq	exceptions_err(%rip), %rdi
	call	free@PLT
	xorl	%esi, %esi
	leaq	NOMSBUILTINSFONC(%rip), %rdi
	call	strlist_destroy@PLT
	xorl	%esi, %esi
	leaq	ERRORS_MESSAGES(%rip), %rdi
	call	strlist_destroy@PLT
	movq	FONCTIONS(%rip), %rdi
	call	tree_destroy@PLT
	movq	CONTAINERS(%rip), %rdi
	movl	$1, %esi
	call	strlist_destroy@PLT
	movq	ATTRIBUTES(%rip), %rdi
	call	nelist_destroy@PLT
	movq	ADRESSES(%rip), %rdi
	call	nelist_destroy@PLT
	movq	NOMS(%rip), %rdi
	movl	$1, %esi
	call	strlist_destroy@PLT
	call	gc_final_sweep@PLT
	movq	PROMISES(%rip), %rdi
	call	nelist_destroy@PLT
	leaq	PROMISES_CNT(%rip), %rdi
	call	intptrlist_destroy@PLT
	movq	PROCESS_FINISH(%rip), %rdi
	call	free@PLT
	movq	FILENAME(%rip), %rdi
	testq	%rdi, %rdi
	je	.L80
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	jmp	free@PLT
	.p2align 4,,10
	.p2align 3
.L80:
	.cfi_restore_state
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE20:
	.size	neonExit, .-neonExit
	.section	.rodata.str1.1
.LC432:
	.string	"__main__"
.LC433:
	.string	"__args__"
	.section	.text.startup,"ax",@progbits
	.p2align 4
	.globl	main
	.type	main, @function
main:
.LFB21:
	.cfi_startproc
	pushq	%r15
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	pushq	%r14
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	movq	%rsi, %r14
	pushq	%r13
	.cfi_def_cfa_offset 32
	.cfi_offset 13, -32
	movl	%edi, %r13d
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
	call	neonInit
	xorl	%edi, %edi
	call	neo_list_create@PLT
	leaq	.LC432(%rip), %rdi
	movq	%rax, %rbp
	movq	%rdx, %r12
	call	strdup@PLT
	movq	%rax, %rdi
	call	updateFileName@PLT
	cmpl	$2, %r13d
	jle	.L84
	leal	-3(%r13), %eax
	leaq	16(%r14), %rbx
	leaq	24(%r14,%rax,8), %r13
	.p2align 4,,10
	.p2align 3
.L85:
	movq	(%rbx), %rdi
	addq	$8, %rbx
	call	strdup@PLT
	movq	%rax, %rdi
	call	neo_str_create@PLT
	movq	%rbp, %rdi
	movq	%r12, %rsi
	movq	%rdx, %rcx
	movq	%rax, %rdx
	call	neo_list_append@PLT
	cmpq	%r13, %rbx
	jne	.L85
	movq	ADRESSES(%rip), %rdi
	movq	%r12, %rdx
	movq	%rbp, %rsi
	call	nelist_append@PLT
	leaq	.LC433(%rip), %rdi
	call	strdup@PLT
	movq	NOMS(%rip), %rdi
	movq	%rax, %rsi
	call	strlist_append@PLT
	leaq	.LC432(%rip), %rdi
	call	strdup@PLT
	movq	%rax, %rdi
	call	updateFileName@PLT
.L89:
	movq	8(%r14), %rdi
	call	execFile
.L88:
	call	neonExit
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	xorl	%eax, %eax
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
.L84:
	.cfi_restore_state
	movq	ADRESSES(%rip), %rdi
	movq	%r12, %rdx
	movq	%rbp, %rsi
	call	nelist_append@PLT
	leaq	.LC433(%rip), %rdi
	call	strdup@PLT
	movq	NOMS(%rip), %rdi
	movq	%rax, %rsi
	call	strlist_append@PLT
	leaq	.LC432(%rip), %rdi
	call	strdup@PLT
	movq	%rax, %rdi
	call	updateFileName@PLT
	cmpl	$2, %r13d
	je	.L89
	call	startMessage
	call	terminal
	jmp	.L88
	.cfi_endproc
.LFE21:
	.size	main, .-main
	.globl	NOMSBUILTINSFONC
	.bss
	.align 16
	.type	NOMSBUILTINSFONC, @object
	.size	NOMSBUILTINSFONC, 16
NOMSBUILTINSFONC:
	.zero	16
	.globl	exceptions_err
	.align 16
	.type	exceptions_err, @object
	.size	exceptions_err, 16
exceptions_err:
	.zero	16
	.globl	OPERANDES
	.align 16
	.type	OPERANDES, @object
	.size	OPERANDES, 16
OPERANDES:
	.zero	16
	.globl	PRIORITE
	.align 16
	.type	PRIORITE, @object
	.size	PRIORITE, 16
PRIORITE:
	.zero	16
	.globl	OPERATEURS
	.align 16
	.type	OPERATEURS, @object
	.size	OPERATEURS, 16
OPERATEURS:
	.zero	16
	.globl	constant
	.align 16
	.type	constant, @object
	.size	constant, 16
constant:
	.zero	16
	.globl	exceptions
	.align 16
	.type	exceptions, @object
	.size	exceptions, 16
exceptions:
	.zero	16
	.globl	neon_boolean
	.align 16
	.type	neon_boolean, @object
	.size	neon_boolean, 16
neon_boolean:
	.zero	16
	.globl	lkeywords
	.align 16
	.type	lkeywords, @object
	.size	lkeywords, 16
lkeywords:
	.zero	16
	.globl	keywords
	.align 16
	.type	keywords, @object
	.size	keywords, 16
keywords:
	.zero	16
	.globl	keywordFunction
	.align 16
	.type	keywordFunction, @object
	.size	keywordFunction, 16
keywordFunction:
	.zero	16
	.globl	blockwords1Line
	.align 16
	.type	blockwords1Line, @object
	.size	blockwords1Line, 16
blockwords1Line:
	.zero	16
	.globl	blockwords
	.align 16
	.type	blockwords, @object
	.size	blockwords, 16
blockwords:
	.zero	16
	.globl	operateurs2
	.align 16
	.type	operateurs2, @object
	.size	operateurs2, 16
operateurs2:
	.zero	16
	.globl	operateurs1
	.align 16
	.type	operateurs1, @object
	.size	operateurs1, 16
operateurs1:
	.zero	16
	.globl	operateurs3
	.align 16
	.type	operateurs3, @object
	.size	operateurs3, 16
operateurs3:
	.zero	16
	.globl	gramm1
	.align 16
	.type	gramm1, @object
	.size	gramm1, 16
gramm1:
	.zero	16
	.globl	sousop
	.align 16
	.type	sousop, @object
	.size	sousop, 16
sousop:
	.zero	16
	.globl	types_fin
	.align 16
	.type	types_fin, @object
	.size	types_fin, 16
types_fin:
	.zero	16
	.globl	types_debut
	.align 16
	.type	types_debut, @object
	.size	types_debut, 16
types_debut:
	.zero	16
	.globl	syntax
	.align 16
	.type	syntax, @object
	.size	syntax, 16
syntax:
	.zero	16
	.globl	acceptedChars
	.align 16
	.type	acceptedChars, @object
	.size	acceptedChars, 16
acceptedChars:
	.zero	16
	.globl	process_cycle
	.align 8
	.type	process_cycle, @object
	.size	process_cycle, 8
process_cycle:
	.zero	8
	.globl	OBJECTS_LIST
	.data
	.align 16
	.type	OBJECTS_LIST, @object
	.size	OBJECTS_LIST, 16
OBJECTS_LIST:
	.zero	8
	.byte	55
	.zero	7
	.globl	ADRESSES
	.bss
	.align 8
	.type	ADRESSES, @object
	.size	ADRESSES, 8
ADRESSES:
	.zero	8
	.globl	NOMS
	.align 8
	.type	NOMS, @object
	.size	NOMS, 8
NOMS:
	.zero	8
	.globl	FONCTIONS
	.align 8
	.type	FONCTIONS, @object
	.size	FONCTIONS, 8
FONCTIONS:
	.zero	8
	.globl	ATTRIBUTES
	.align 8
	.type	ATTRIBUTES, @object
	.size	ATTRIBUTES, 8
ATTRIBUTES:
	.zero	8
	.globl	CONTAINERS
	.align 8
	.type	CONTAINERS, @object
	.size	CONTAINERS, 8
CONTAINERS:
	.zero	8
	.globl	RETURN_VALUE
	.data
	.align 16
	.type	RETURN_VALUE, @object
	.size	RETURN_VALUE, 16
RETURN_VALUE:
	.zero	8
	.byte	55
	.zero	7
	.globl	ERRORS_MESSAGES
	.bss
	.align 16
	.type	ERRORS_MESSAGES, @object
	.size	ERRORS_MESSAGES, 16
ERRORS_MESSAGES:
	.zero	16
	.globl	LINENUMBER
	.align 4
	.type	LINENUMBER, @object
	.size	LINENUMBER, 4
LINENUMBER:
	.zero	4
	.globl	FILENAME
	.align 8
	.type	FILENAME, @object
	.size	FILENAME, 8
FILENAME:
	.zero	8
	.globl	NAME
	.align 4
	.type	NAME, @object
	.size	NAME, 4
NAME:
	.zero	4
	.globl	EXCEPTION
	.align 8
	.type	EXCEPTION, @object
	.size	EXCEPTION, 8
EXCEPTION:
	.zero	8
	.globl	PROCESS_FINISH
	.align 16
	.type	PROCESS_FINISH, @object
	.size	PROCESS_FINISH, 16
PROCESS_FINISH:
	.zero	16
	.globl	PROMISES_CNT
	.align 16
	.type	PROMISES_CNT, @object
	.size	PROMISES_CNT, 16
PROMISES_CNT:
	.zero	16
	.globl	PROMISES
	.align 8
	.type	PROMISES, @object
	.size	PROMISES, 8
PROMISES:
	.zero	8
	.globl	atomic_counter
	.align 4
	.type	atomic_counter, @object
	.size	atomic_counter, 4
atomic_counter:
	.zero	4
	.globl	ATOMIC_TIME
	.data
	.align 4
	.type	ATOMIC_TIME, @object
	.size	ATOMIC_TIME, 4
ATOMIC_TIME:
	.long	1500
	.globl	CODE_ERROR
	.bss
	.align 4
	.type	CODE_ERROR, @object
	.size	CODE_ERROR, 4
CODE_ERROR:
	.zero	4
	.section	.rodata.cst8,"aM",@progbits,8
	.align 8
.LC282:
	.long	1413754136
	.long	1074340347
	.section	.rodata.cst16,"aM",@progbits,16
	.align 16
.LC338:
	.long	13
	.long	2
	.long	12
	.long	15
	.align 16
.LC339:
	.long	13
	.long	5
	.long	4
	.long	21
	.align 16
.LC340:
	.long	131
	.long	134
	.long	10
	.long	128
	.align 16
.LC341:
	.long	8
	.long	1
	.long	12
	.long	2
	.align 16
.LC342:
	.long	14
	.long	137
	.long	15
	.long	20
	.align 16
.LC343:
	.long	12
	.long	11
	.long	16
	.long	128
	.section	.rodata.cst8
	.align 8
.LC344:
	.long	12
	.long	7
	.section	.rodata.cst16
	.align 16
.LC345:
	.long	15
	.long	5
	.long	4
	.long	21
	.align 16
.LC346:
	.long	13
	.long	10
	.long	1
	.long	8
	.align 16
.LC347:
	.long	2
	.long	134
	.long	131
	.long	11
	.align 16
.LC348:
	.long	7
	.long	128
	.long	14
	.long	16
	.section	.rodata.cst8
	.align 8
.LC349:
	.long	137
	.long	18
	.section	.rodata.cst16
	.align 16
.LC350:
	.long	2
	.long	12
	.long	15
	.long	13
	.align 16
.LC351:
	.long	2
	.long	5
	.long	4
	.long	21
	.align 16
.LC352:
	.long	131
	.long	134
	.long	128
	.long	1
	.section	.rodata.cst8
	.align 8
.LC353:
	.long	10
	.long	14
	.section	.rodata.cst16
	.align 16
.LC354:
	.long	5
	.long	4
	.long	21
	.long	131
	.align 16
.LC355:
	.long	134
	.long	10
	.long	128
	.long	8
	.align 16
.LC356:
	.long	1
	.long	12
	.long	2
	.long	137
	.align 16
.LC357:
	.long	128
	.long	13
	.long	2
	.long	12
	.align 16
.LC358:
	.long	1
	.long	5
	.long	4
	.long	131
	.align 16
.LC359:
	.long	134
	.long	10
	.long	8
	.long	11
	.align 16
.LC360:
	.long	16
	.long	137
	.long	23
	.long	22
	.align 16
.LC361:
	.long	24
	.long	25
	.long	18
	.long	21
	.align 16
.LC362:
	.long	128
	.long	14
	.long	2
	.long	12
	.align 16
.LC363:
	.long	134
	.long	10
	.long	8
	.long	16
	.align 16
.LC364:
	.long	137
	.long	23
	.long	22
	.long	24
	.align 16
.LC365:
	.long	21
	.long	20
	.long	7
	.long	18
	.align 16
.LC366:
	.long	4
	.long	4
	.long	4
	.long	4
	.align 16
.LC367:
	.long	4
	.long	4
	.long	8
	.long	8
	.align 16
.LC368:
	.long	8
	.long	8
	.long	8
	.long	1
	.align 16
.LC369:
	.long	1
	.long	2
	.long	4
	.long	4
	.align 16
.LC370:
	.long	32
	.long	4
	.long	2
	.long	2
	.align 16
.LC371:
	.long	32
	.long	16
	.long	4
	.long	4
	.align 16
.LC372:
	.long	64
	.long	8
	.long	128
	.long	2
	.align 16
.LC374:
	.long	7
	.long	7
	.long	7
	.long	5
	.align 16
.LC375:
	.long	4
	.long	5
	.long	4
	.long	3
	.align 16
.LC376:
	.long	6
	.long	6
	.long	6
	.long	6
	.align 16
.LC377:
	.long	6
	.long	6
	.long	8
	.long	8
	.align 16
.LC378:
	.long	8
	.long	8
	.long	8
	.long	3
	.align 16
.LC379:
	.long	3
	.long	7
	.long	4
	.long	4
	.align 16
.LC380:
	.long	2
	.long	8
	.long	2
	.long	1
	.align 16
.LC381:
	.long	8
	.long	8
	.long	2
	.long	3
	.align 16
.LC382:
	.long	7
	.long	6
	.long	0
	.long	8
	.align 16
.LC383:
	.long	8
	.long	8
	.long	8
	.long	7
	.align 16
.LC384:
	.long	260
	.long	260
	.long	4
	.long	4
	.align 16
.LC385:
	.long	260
	.long	4
	.long	4
	.long	4
	.align 16
.LC386:
	.long	64
	.long	8
	.long	0
	.long	2
	.section	.rodata.cst8
	.align 8
.LC388:
	.long	15
	.long	4
	.section	.rodata.cst16
	.align 16
.LC391:
	.long	17
	.long	131
	.long	127
	.long	131
	.align 16
.LC392:
	.long	4
	.long	131
	.long	17
	.long	17
	.align 16
.LC393:
	.long	17
	.long	17
	.long	131
	.long	131
	.align 16
.LC394:
	.long	131
	.long	17
	.long	17
	.long	4
	.align 16
.LC395:
	.long	17
	.long	4
	.long	17
	.long	17
	.align 16
.LC396:
	.long	131
	.long	4
	.long	134
	.long	20
	.align 16
.LC397:
	.long	17
	.long	4
	.long	4
	.long	131
	.align 16
.LC398:
	.long	4
	.long	134
	.long	17
	.long	17
	.align 16
.LC399:
	.long	5
	.long	5
	.long	5
	.long	5
	.align 16
.LC400:
	.long	5
	.long	5
	.long	131
	.long	17
	.align 16
.LC401:
	.long	17
	.long	17
	.long	127
	.long	17
	.set	.LC402,.LC392+8
	.align 16
.LC404:
	.long	-1
	.long	-1
	.long	1
	.long	1
	.align 16
.LC405:
	.long	1
	.long	3
	.long	0
	.long	2
	.align 16
.LC406:
	.long	2
	.long	3
	.long	1
	.long	1
	.align 16
.LC407:
	.long	1
	.long	0
	.long	-1
	.long	2
	.align 16
.LC408:
	.long	1
	.long	0
	.long	-1
	.long	-1
	.align 16
.LC409:
	.long	1
	.long	1
	.long	6
	.long	1
	.align 16
.LC410:
	.long	2
	.long	1
	.long	2
	.long	3
	.align 16
.LC411:
	.long	2
	.long	1
	.long	1
	.long	1
	.align 16
.LC412:
	.long	1
	.long	1
	.long	1
	.long	1
	.align 16
.LC413:
	.long	1
	.long	1
	.long	2
	.long	1
	.align 16
.LC414:
	.long	1
	.long	1
	.long	1
	.long	2
	.section	.rodata.cst8
	.align 8
.LC415:
	.long	0
	.long	1
	.set	.LC416,.LC396
	.align 8
.LC417:
	.long	134
	.long	127
	.align 8
.LC418:
	.long	134
	.long	4
	.set	.LC419,.LC366
	.section	.rodata.cst16
	.align 16
.LC420:
	.long	131
	.long	4
	.long	4
	.long	4
	.set	.LC421,.LC393+8
	.section	.rodata.cst8
	.align 8
.LC422:
	.long	20
	.long	131
	.align 8
.LC423:
	.long	127
	.long	127
	.set	.LC424,.LC399
	.set	.LC425,.LC391+8
	.ident	"GCC: (Debian 12.2.0-14+deb12u1) 12.2.0"
	.section	.note.GNU-stack,"",@progbits
