#include "headers/constants.h"

.extern STACK_PTR
.extern launch_process

.text

#ifdef LINUX_AMD64


#define LOAD_REGISTERS(source_index)    \
    mov     0(source_index), %rbx;      \
    mov     8(source_index), %rbp;      \
    mov     16(source_index), %r12;     \
    mov     24(source_index), %r13;     \
    mov     32(source_index), %r14;     \
    mov     40(source_index), %r15;


#define SAVE_REGISTERS(dest_index)      \
    mov     %rbx,    0(dest_index);     \
    mov     %rbp,    8(dest_index);     \
    mov     %r12,    16(dest_index);    \
    mov     %r13,    24(dest_index);    \
    mov     %r14,    32(dest_index);    \
    mov     %r15,    40(dest_index);


#define MOVE_REGISTERS(source_index, dest_index)    \
    mov     0(source_index), %rax;                  \
    mov     %rax,    0(dest_index);                 \
    mov     8(source_index), %rax;                  \
    mov     %rax,    8(dest_index);                 \
    mov     16(source_index), %rax;                 \
    mov     %rax,    16(dest_index);                \
    mov     24(source_index), %rax;                 \
    mov     %rax,    24(dest_index);                \
    mov     32(source_index), %rax;                 \
    mov     %rax,    32(dest_index);                \
    mov     40(source_index), %rax;                 \
    mov     %rax,    40(dest_index);



.globl get_stack
get_stack:
    movq    %rsp,   %rax
    addq    $8,     %rax
    ret


# used registers : rdi, rsi, rdx, rcx, rax
# registers that may be used to contain an address : rcx, rsi, rdi, rax
# saved registers : rbx, rbp, r12 - r15




.globl reset_stack_and_registers
.type reset_stack_and_registers, @function
# Cette fonction remet la pile d'origine
# on renvoie le résultat de execval_prolog (l'adresse de l'objet à affecter passé en argument) en ne touchant pas à rax
reset_stack_and_registers:
    # on récupère l'adresse à laquelle on est censé retourner
    pop     %rdx

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

    LOAD_REGISTERS(%rdi)

    next2:
    # on ajoute dedans l'adresse de retour que l'on a récupérée sur l'ancienne pile
    push    %rdx

    ret




/*
Quand un processus tourne, la valeur des registres stockés à l'intérieur est la valeur à restaurer à la fin du processus
Quand un processus ne tourne pas, la valeur des registres stockés à l'intérieur est la valeur des registres sauvegardés de ce processus
Quand on passe d'un processus à l'autre :
registres actuels -> stockés dans l'ancien processus
registres du nouveau processus -> mis dans les vrais registres
registres à restaurer après l'ancien processus -> stockés dans le nouveau processus

La fonction a un support pour des appels sans processus précédent, tel que le soi-disant processus précédent ait été restauré (pile, registres)
Mais actuellement elle n'est jamais appelée sans processus précédent

Remarque : la version actuelle ne fonctionne que parce que eval_prolog n'utilise pas de registres sauvegardés après le changement de pile
En effet, quand on commence un processus, on restaure les registres sauvegardés du processus, mais lorsque le processus en question n'a
jamais été appelé avant, les registres sauvegardés reçoivent des valeurs arbitraires
*/
# switch_registers(process, prev_process, flags)
.globl switch_registers
.type switch_registers, @function
switch_registers:
    # sauvegarde des registres actuels
    sub     $48,     %rsp

    # d'abord on empile les valeurs des registres
    SAVE_REGISTERS(%rsp)

    # restauration des registres de ce processus
    LOAD_REGISTERS(%rdi)

    # si on a un processus précédent, on transfère les registres à restaurer au nouveau processus, et on enregistre les registres actuels dans l'ancien processus
    test    %rsi,   %rsi
    jz      switch_registers_skip0
    # si on a un processus précédent on sauvegarde la pile actuelle dedans
    MOVE_REGISTERS(%rsi, %rdi)
    
    # et enfin on enregistre les registres d'avant dans la structure
    MOVE_REGISTERS(%rsp, %rsi)

    add     $48,     %rsp

    # sauvegarde de la pile 8 octets plus loin (courante à execval_prolog) car on n'a pas encore enlevé l'adresse de retour
    add     $8,      %rsp
    mov     %rsp,    48(%rsi)
    sub     $8,      %rsp

    jmp     switch_registers_skip1
    switch_registers_skip0:

    MOVE_REGISTERS(%rsp, %rdi)

    add     $48,     %rsp
    switch_registers_skip1:

    # PASSE A LA NOUVELLE PILE

    # récupère l'adresse de retour, l'enlève de la pile. La pile est donc courante dans execval_prolog
    pop     %rsi

    # s'il faut sauver stack, on met rsp dans STACK_PTR
    test    $0b00000100,    %dl
    jz      switch_registers_skip2
    mov     %rsp,     STACK_PTR(%rip)
    switch_registers_skip2:

    # restaure la pile du nouveau processus, cette pile est courante dans execval_prolog

    # si il faut restaurer depuis STACK_PTR
    test    $0b00000010,    %dl
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
    # on remet la pile du processus
    mov     48(%rdi), %rsp

    # si le processus est un processus non encore initialisé, on saute à launch_process
    test    $0b00000001,    %dl
    jz      switch_registers_skip4

    jmp     launch_process
    
    switch_registers_skip4:

    # et on empile l'adresse de retour
    push    %rsi

    # on retourne à l'adresse de retour que l'on a mise artificiellement dans la pile
    ret




#endif

#ifdef WINDOWS_AMD64


# switch registers :
# rdx -> r8
# rcx -> r9
# rdi -> rcx
# rsi -> rdx





#endif