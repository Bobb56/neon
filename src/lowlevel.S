#include "headers/constants.h"

.extern STACK_PTR
.extern eval_prolog
.extern create_new_process
.extern eval_epilog

.text

#ifdef LINUX

.globl eval_epilog_unaligned
.type eval_epilog_unaligned, @function
# cette fonction doit être appelée de manière désalignée par le return de execval_prolog
eval_epilog_unaligned:
    sub    $8,     %rsp # réaligne la pile pour l'appel. Le retour se fera sur une pile différente
    jmp    eval_epilog


.globl reset_stack_and_registers
.type reset_stack_and_registers, @function
# Cette fonction remet la pile d'origine
# on renvoie le résultat de execval_prolog en ne touchant pas à rax ni à rdx
reset_stack_and_registers:
    # on récupère l'adresse à laquelle on est censé retourner
    pop     %rcx

    # on restaure la pile d'origine, courante dans execval_prolog
    mov     STACK_PTR(%rip),    %rsi

    # si STACK_PTR est non nul, on le remet dans rsp
    test    %rsi,               %rsi
    jz      next2
    mov     %rsi,               %rsp
    # on remet STACK_PTR à nul
    movq    $0,                 STACK_PTR(%rip)

    # restauration des registres
    # on charge l'adresse de process_cycle->process dans rdi
    movq	process_cycle(%rip), %rdi
	movq	(%rdi), %rdi

    mov     0(%rdi), %rbx
    mov     8(%rdi), %rbp
    mov     16(%rdi), %r12
    mov     24(%rdi), %r13
    mov     32(%rdi), %r14
    mov     40(%rdi), %r15

    next2:
    # on ajoute dedans l'adresse de retour que l'on a récupérée sur l'ancienne pile
    push    %rcx

    ret




.globl switch_registers
.type switch_registers, @function
# Cette fonction sauvegarde les registres actuels et restaure les registres du processus précédent, et change de pile
# void switch_registers(process, prev_process, save_stack, load_stack)

switch_registers:
    # sauvegarde des registres actuels
    sub     $48,     %rsp

    # d'abord on empile les valeurs des registres
    mov     %rbx,    0(%rsp)
    mov     %rbp,    8(%rsp)
    mov     %r12,    16(%rsp)
    mov     %r13,    24(%rsp)
    mov     %r14,    32(%rsp)
    mov     %r15,    40(%rsp)

    # restauration des registres de ce processus
    mov     0(%rdi), %rbx
    mov     8(%rdi), %rbp
    mov     16(%rdi), %r12
    mov     24(%rdi), %r13
    mov     32(%rdi), %r14
    mov     40(%rdi), %r15

    # si on a un processus précédent, on transfère les registres à restaurer au nouveau processus, et on enregistre les registres actuels dans l'ancien processus
    test    %rsi,   %rsi
    jz      switch_registers_skip0
    # si on a un processus précédent on sauvegarde la pile actuelle dedans
    mov     0(%rsi), %rax
    mov     %rax,    0(%rdi)
    mov     8(%rsi), %rax
    mov     %rax,    8(%rdi)
    mov     16(%rsi), %rax
    mov     %rax,    16(%rdi)
    mov     24(%rsi), %rax
    mov     %rax,    24(%rdi)
    mov     32(%rsi), %rax
    mov     %rax,    32(%rdi)
    mov     40(%rsi), %rax
    mov     %rax,    40(%rdi)
    
    # et enfin on enregistre les registres d'avant dans la structure
    mov     0(%rsp), %rax
    mov     %rax,    0(%rsi)
    mov     8(%rsp), %rax
    mov     %rax,    8(%rsi)
    mov     16(%rsp), %rax
    mov     %rax,    16(%rsi)
    mov     24(%rsp), %rax
    mov     %rax,    24(%rsi)
    mov     32(%rsp), %rax
    mov     %rax,    32(%rsi)
    mov     40(%rsp), %rax
    mov     %rax,    40(%rsi)

    add     $48,     %rsp

    # sauvegarde de la pile 8 octets plus loin (courante à execval_prolog) car on n'a pas encore enlevé l'adresse de retour
    add     $8,      %rsp
    mov     %rsp,    48(%rsi)
    sub     $8,      %rsp

    jmp     switch_registers_skip1
    switch_registers_skip0:

    mov     0(%rsp), %rax
    mov     %rax,    0(%rdi)
    mov     8(%rsp), %rax
    mov     %rax,    8(%rdi)
    mov     16(%rsp), %rax
    mov     %rax,    16(%rdi)
    mov     24(%rsp), %rax
    mov     %rax,    24(%rdi)
    mov     32(%rsp), %rax
    mov     %rax,    32(%rdi)
    mov     40(%rsp), %rax
    mov     %rax,    40(%rdi)

    add     $48,     %rsp
    switch_registers_skip1:

    # PASSE A LA NOUVELLE PILE

    # récupère l'adresse de retour, l'enlève de la pile. La pile est donc courante dans execval_prolog
    pop     %rsi

    # s'il faut sauver stack, on met rsp dans STACK_PTR
    test    %rdx,     %rdx
    jz      switch_registers_skip2
    mov     %rsp,     STACK_PTR(%rip)
    switch_registers_skip2:

    # restaure la pile du nouveau processus, cette pile est courante dans execval_prolog

    # si il faut restaurer depuis STACK_PTR
    test    %rcx,               %rcx
    jz      switch_registers_skip3

    mov     STACK_PTR(%rip),    %rax
    # si STACK_PTR est non nul
    test    %rax,               %rax
    jz      switch_registers_skip4
    mov     %rax,               %rsp
    # on remet STACK_PTR à nul
    movq    $0,                 STACK_PTR(%rip)
    jmp     switch_registers_skip4
    switch_registers_skip3:
    mov     48(%rdi), %rsp
    switch_registers_skip4:

    # et on empile l'adresse de retour
    push    %rsi

    # on retourne à l'adresse de retour que l'on a mise artificiellement dans la pile
    ret


#endif

#if defined(WINDOWS11) || defined(WINDOWS10)








#endif