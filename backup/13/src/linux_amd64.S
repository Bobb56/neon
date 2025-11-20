#include "headers/constants.h"

.extern launch_process

# pour ne pas avoir de pile exécutable
.section .note.GNU-stack

.text

# ce code utilise 4 registres de travail de la taille d'un pointeur : rdi, rsi, rax, rdx
# et un registre de travail de la taille d'un octet : al
# le registre d'un octet peut être la partie basse de n'importe lequel de ces quatre registres

# les registres sauvegardés par la convention de linux sont rsp, rbp, rbx, r12 -> r15
# les arguments sont passés dans rdi, rsi, rdx

# empile les registres sur la pile, en utilisant rdx comme registre de pile
#define PUSH_REGISTERS()        \
    sub     $56,     %rdx;      \
    mov     %rsp,    0(%rdx);   \
    mov     %rbx,    8(%rdx);   \
    mov     %rbp,    16(%rdx);  \
    mov     %r12,    24(%rdx);  \
    mov     %r13,    32(%rdx);  \
    mov     %r14,    40(%rdx);  \
    mov     %r15,    48(%rdx);



# dépile les registres dans une autre structure
# modifie rax
#define POP_REGISTERS(dest_index)       \
    mov     0(%rdx), %rax;              \
    mov     %rax,    0(dest_index);     \
    mov     8(%rdx), %rax;              \
    mov     %rax,    8(dest_index);     \
    mov     16(%rdx), %rax;             \
    mov     %rax,    16(dest_index);    \
    mov     24(%rdx), %rax;             \
    mov     %rax,    24(dest_index);    \
    mov     32(%rdx), %rax;             \
    mov     %rax,    32(dest_index);    \
    mov     40(%rdx), %rax;             \
    mov     %rax,    40(dest_index);    \
    mov     48(%rdx), %rax;             \
    mov     %rax,    48(dest_index);    \
    add     $56,    %rdx; # instruction inutile



#define SAVE_REGISTERS(dest_index)      \
    mov     %rsp,    0(dest_index);     \
    mov     %rbx,    8(dest_index);     \
    mov     %rbp,    16(dest_index);    \
    mov     %r12,    24(dest_index);    \
    mov     %r13,    32(dest_index);    \
    mov     %r14,    40(dest_index);    \
    mov     %r15,    48(dest_index);


#define LOAD_REGISTERS(source_index)    \
    mov     0(source_index), %rsp;      \
    mov     8(source_index), %rbx;      \
    mov     16(source_index), %rbp;     \
    mov     24(source_index), %r12;     \
    mov     32(source_index), %r13;     \
    mov     40(source_index), %r14;     \
    mov     48(source_index), %r15;



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
    mov     %rax,    40(dest_index);                \
    mov     48(source_index), %rax;                 \
    mov     %rax,    48(dest_index);




.globl reset_stack_and_registers
# Cette fonction remet la pile d'origine ainsi que les registres d'origine
# void reset_stack_and_registers(process_cycle->process)
reset_stack_and_registers:
    # on récupère l'adresse à laquelle on est censé retourner
    pop     %rsi

    # restauration des registres
    LOAD_REGISTERS(%rdi)

    # on modifie dedans l'adresse de retour que l'on a récupérée sur l'ancienne pile
    mov    %rsi,    0(%rsp)

    ret


.globl save_stack_and_registers
# cette fonction sauvegarde la pile et les registres sauvegardés dans le processus qui tourne actuellement
# void save_stack_and_registers(process_cycle->process)
save_stack_and_registers:
    SAVE_REGISTERS(%rdi)

    ret



/*
Quand un processus tourne, la valeur des registres stockés à l'intérieur est la valeur à restaurer à la fin du processus
Quand un processus ne tourne pas, la valeur des registres stockés à l'intérieur est la valeur des registres sauvegardés de ce processus
Quand on passe d'un processus à l'autre :
registres actuels -> stockés dans l'ancien processus
registres du nouveau processus -> mis dans les vrais registres
registres à restaurer après l'ancien processus -> stockés dans le nouveau processus

*/
# switch_registers(process, prev_process)
# process      -> rdi
# prev_process -> rsi
.globl switch_registers
switch_registers:
    mov     %rsp,   %rdx # la pile de substitution

    # sauvegarde des registres actuels
    # d'abord on empile les valeurs des registres sur la pile pointée par rdx
    PUSH_REGISTERS()

    # restauration des registres de ce processus
    LOAD_REGISTERS(%rdi)

    # on transfère les registres à restaurer à la toute fin
    MOVE_REGISTERS(%rsi, %rdi)
    
    # et enfin on enregistre les registres courants (stockés dans la pile pointée par rdx) dans le processus précédent
    POP_REGISTERS(%rsi)

    # si le processus est un processus non encore initialisé, on saute à launch_process
    # on charge l'état du processus dans rax
    mov     REG_BUFFER_SIZE(%rdi), %al
    testb   %al, %al # la valeur Uninitialized correspond à zéro
    jnz     switch_registers_skip

    # et on lance le processus (on fait un call pour garder l'alignement de la pile, mais il ne reviendra jamais ici)
    call    launch_process
    
    switch_registers_skip:

    # on récupère la pile de l'ancien processus
    mov    0(%rsi),     %rax
    # on récupère l'adresse de retour au sommet de cette pile
    mov    0(%rax),     %rax
    # et on change notre propre adresse de retour
    mov    %rax,        0(%rsp)

    # on retourne à l'adresse de retour que l'on a mise artificiellement dans la pile
    ret



.globl get_stack
get_stack:
    movq    %rsp,   %rax
    addq    $8,     %rax
    ret