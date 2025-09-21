#include "headers/constants.h"

.extern launch_process

#ifdef LINUX_AMD64

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




#endif

#ifdef WINDOWS_AMD64

.text

# ce code utilise 4 registres de travail de la taille d'un pointeur : rcx, rdx, rax, r8
# et un registre de travail de la taille d'un octet : al
# le registre d'un octet peut être la partie basse de n'importe lequel de ces quatre registres

# les registres sauvegardés par la convention de windows sont rsp, rbp, rbx, r12 -> r15, rsi, rdi et xmm6 -> xmm15
# les arguments sont passés dans rcx, rdx, r8


# empile les registres sur la pile, en utilisant r8 comme registre de pile
#define PUSH_REGISTERS()       \
    sub     $232,    %r8;      \
    mov     %rsp,    0(%r8);   \
    mov     %rbx,    8(%r8);   \
    mov     %rbp,    16(%r8);  \
    mov     %r12,    24(%r8);  \
    mov     %r13,    32(%r8);  \
    mov     %r14,    40(%r8);  \
    mov     %r15,    48(%r8);  \
    mov     %rdi,    56(%r8);  \
    mov     %rsi,    64(%r8);  \
    movdqu  %xmm6,   72(%r8);  \
    movdqu  %xmm7,   88(%r8);  \
    movdqu  %xmm8,   104(%r8); \
    movdqu  %xmm9,   120(%r8); \
    movdqu  %xmm10,  136(%r8); \
    movdqu  %xmm11,  152(%r8); \
    movdqu  %xmm12,  168(%r8); \
    movdqu  %xmm13,  184(%r8); \
    movdqu  %xmm14,  200(%r8); \
    movdqu  %xmm15,  216(%r8);


# dépile les registres dans une autre structure
# modifie rax
#define POP_REGISTERS(dest_index)       \
    mov     0(%r8), %rax;               \
    mov     %rax,    0(dest_index);     \
    mov     8(%r8), %rax;               \
    mov     %rax,    8(dest_index);     \
    mov     16(%r8), %rax;              \
    mov     %rax,    16(dest_index);    \
    mov     24(%r8), %rax;              \
    mov     %rax,    24(dest_index);    \
    mov     32(%r8), %rax;              \
    mov     %rax,    32(dest_index);    \
    mov     40(%r8), %rax;              \
    mov     %rax,    40(dest_index);    \
    mov     48(%r8), %rax;              \
    mov     %rax,    48(dest_index);    \
    mov     56(%r8), %rax;              \
    mov     %rax,    56(dest_index);    \
    mov     64(%r8), %rax;              \
    mov     %rax,   64(dest_index);     \
    movdqu  72(%r8), %xmm0;             \
    movdqu  %xmm0,  72(dest_index);     \
    movdqu  88(%r8), %xmm0;             \
    movdqu  %xmm0,  88(dest_index);     \
    movdqu  104(%r8), %xmm0;            \
    movdqu  %xmm0,  104(dest_index);    \
    movdqu  120(%r8), %xmm0;            \
    movdqu  %xmm0,  120(dest_index);    \
    movdqu  136(%r8), %xmm0;            \
    movdqu  %xmm0,  136(dest_index);    \
    movdqu  152(%r8), %xmm0;            \
    movdqu  %xmm0,  152(dest_index);    \
    movdqu  168(%r8), %xmm0;            \
    movdqu  %xmm0,  168(dest_index);    \
    movdqu  184(%r8), %xmm0;            \
    movdqu  %xmm0,  184(dest_index);    \
    movdqu  200(%r8), %xmm0;            \
    movdqu  %xmm0,  200(dest_index);    \
    movdqu  216(%r8), %xmm0;            \
    movdqu  %xmm0,  216(dest_index);    \
    add     $232,   %r8; # instruction inutile



#define SAVE_REGISTERS(dest_index)      \
    mov     %rsp,    0(dest_index);     \
    mov     %rbx,    8(dest_index);     \
    mov     %rbp,    16(dest_index);    \
    mov     %r12,    24(dest_index);    \
    mov     %r13,    32(dest_index);    \
    mov     %r14,    40(dest_index);    \
    mov     %r15,    48(dest_index);    \
    mov     %rdi,    56(dest_index);    \
    mov     %rsi,    64(dest_index);    \
    movdqu  %xmm6,   72(dest_index);    \
    movdqu  %xmm7,   88(dest_index);    \
    movdqu  %xmm8,   104(dest_index);   \
    movdqu  %xmm9,   120(dest_index);   \
    movdqu  %xmm10,  136(dest_index);   \
    movdqu  %xmm11,  152(dest_index);   \
    movdqu  %xmm12,  168(dest_index);   \
    movdqu  %xmm13,  184(dest_index);   \
    movdqu  %xmm14,  200(dest_index);   \
    movdqu  %xmm15,  216(dest_index);


#define LOAD_REGISTERS(source_index)    \
    mov     0(source_index),   %rsp;    \
    mov     8(source_index),   %rbx;    \
    mov     16(source_index),  %rbp;    \
    mov     24(source_index),  %r12;    \
    mov     32(source_index),  %r13;    \
    mov     40(source_index),  %r14;    \
    mov     48(source_index),  %r15;    \
    mov     56(source_index),  %rdi;    \
    mov     64(source_index),  %rsi;    \
    movdqu  72(source_index),  %xmm6;   \
    movdqu  88(source_index),  %xmm7;   \
    movdqu  104(source_index), %xmm8;   \
    movdqu  120(source_index), %xmm9;   \
    movdqu  136(source_index), %xmm10;  \
    movdqu  152(source_index), %xmm11;  \
    movdqu  168(source_index), %xmm12;  \
    movdqu  184(source_index), %xmm13;  \
    movdqu  200(source_index), %xmm14;  \
    movdqu  216(source_index), %xmm15;



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
    mov     %rax,    48(dest_index);                \
    mov     56(source_index), %rax;                 \
    mov     %rax,    56(dest_index);                \
    mov     64(source_index), %rax;                 \
    mov     %rax,   64(dest_index);                 \
    movdqu  72(source_index), %xmm0;                \
    movdqu  %xmm0,  72(dest_index);                 \
    movdqu  88(source_index), %xmm0;                \
    movdqu  %xmm0,  88(dest_index);                 \
    movdqu  104(source_index), %xmm0;               \
    movdqu  %xmm0,  104(dest_index);                \
    movdqu  120(source_index), %xmm0;               \
    movdqu  %xmm0,  120(dest_index);                \
    movdqu  136(source_index), %xmm0;               \
    movdqu  %xmm0,  136(dest_index);                \
    movdqu  152(source_index), %xmm0;               \
    movdqu  %xmm0,  152(dest_index);                \
    movdqu  168(source_index), %xmm0;               \
    movdqu  %xmm0,  168(dest_index);                \
    movdqu  184(source_index), %xmm0;               \
    movdqu  %xmm0,  184(dest_index);                \
    movdqu  200(source_index), %xmm0;               \
    movdqu  %xmm0,  200(dest_index);                \
    movdqu  216(source_index), %xmm0;               \
    movdqu  %xmm0,  216(dest_index);



.globl reset_stack_and_registers
# Cette fonction remet la pile d'origine ainsi que les registres d'origine
# void reset_stack_and_registers(process_cycle->process)
reset_stack_and_registers:
    # on récupère l'adresse à laquelle on est censé retourner
    pop     %rdx

    # restauration des registres
    LOAD_REGISTERS(%rcx)

    # on modifie dedans l'adresse de retour que l'on a récupérée sur l'ancienne pile
    mov    %rdx,    0(%rsp)

    ret


.globl save_stack_and_registers
# cette fonction sauvegarde la pile et les registres sauvegardés dans le processus qui tourne actuellement
# void save_stack_and_registers(process_cycle->process)
save_stack_and_registers:
    SAVE_REGISTERS(%rcx)

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
# process      -> rcx
# prev_process -> rdx
.globl switch_registers
switch_registers:
    mov     %rsp,   %r8 # la pile de substitution

    # sauvegarde des registres actuels
    # d'abord on empile les valeurs des registres sur la pile pointée par r8
    PUSH_REGISTERS()

    # restauration des registres de ce processus
    LOAD_REGISTERS(%rcx)

    # on transfère les registres à restaurer à la toute fin
    MOVE_REGISTERS(%rdx, %rcx)
    
    # et enfin on enregistre les registres courants (stockés dans la pile pointée par r8) dans le processus précédent
    POP_REGISTERS(%rdx)

    # si le processus est un processus non encore initialisé, on saute à launch_process
    # on charge l'état du processus dans rax
    mov     REG_BUFFER_SIZE(%rcx), %al
    testb   %al, %al # la valeur Uninitialized correspond à zéro
    jnz     switch_registers_skip

    # et on lance le processus (on fait un call pour garder l'alignement de la pile, mais il ne reviendra jamais ici)
    sub     $32,    %rsp # allocation du shadow space
    call    launch_process
    
    switch_registers_skip:

    # on récupère la pile de l'ancien processus
    mov    0(%rdx),     %rax
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





#endif




#ifdef LINUX_RISCV64

# pour ne pas avoir de pile exécutable
.section .note.GNU-stack

.text

# ce code utilise 4 registres de travail de la taille d'un pointeur : a0, a1, t0, t1


# empile les registres sur la pile, en utilisant rdx comme registre de pile
#define PUSH_REGISTERS()        \
    addi    t1,  t1, -200;      \
    sd      sp,  0(t1);         \
    sd      s0,  8(t1);         \
    sd      s1,  16(t1);        \
    sd      s2,  24(t1);        \
    sd      s3,  32(t1);        \
    sd      s4,  40(t1);        \
    sd      s5,  48(t1);        \
    sd      s6,  56(t1);        \
    sd      s7,  64(t1);        \
    sd      s8,  72(t1);        \
    sd      s9,  80(t1);        \
    sd      s10, 88(t1);        \
    sd      s11, 96(t1);


#define PUSH_FP_REGISTERS()       \
    fsd     fs0,  104(t1);        \
    fsd     fs1,  112(t1);        \
    fsd     fs2,  120(t1);        \
    fsd     fs3,  128(t1);        \
    fsd     fs4,  136(t1);        \
    fsd     fs5,  144(t1);        \
    fsd     fs6,  152(t1);        \
    fsd     fs7,  160(t1);        \
    fsd     fs8,  168(t1);        \
    fsd     fs9,  176(t1);        \
    fsd     fs10, 184(t1);        \
    fsd     fs11, 192(t1);



# dépile les registres dans une autre structure
# modifie t0
#define POP_REGISTERS(dest_index)   \
    ld      t0,  0(t1);             \
    sd      t0,  0(dest_index);     \
    ld      t0,  8(t1);             \
    sd      t0,  8(dest_index);     \
    ld      t0,  16(t1);            \
    sd      t0,  16(dest_index);    \
    ld      t0,  24(t1);            \
    sd      t0,  24(dest_index);    \
    ld      t0,  32(t1);            \
    sd      t0,  32(dest_index);    \
    ld      t0,  40(t1);            \
    sd      t0,  40(dest_index);    \
    ld      t0,  48(t1);            \
    sd      t0,  48(dest_index);    \
    ld      t0,  56(t1);            \
    sd      t0,  56(dest_index);    \
    ld      t0,  64(t1);            \
    sd      t0,  64(dest_index);    \
    ld      t0,  72(t1);            \
    sd      t0,  72(dest_index);    \
    ld      t0,  80(t1);            \
    sd      t0,  80(dest_index);    \
    ld      t0,  88(t1);            \
    sd      t0,  88(dest_index);    \
    ld      t0,  96(t1);            \
    sd      t0,  96(dest_index);




#define POP_FP_REGISTERS(dest_index)    \
    fld     ft0,  104(t1);              \
    fsd     ft0,  104(dest_index);      \
    fld     ft0,  112(t1);              \
    fsd     ft0,  112(dest_index);      \
    fld     ft0,  120(t1);              \
    fsd     ft0,  120(dest_index);      \
    fld     ft0,  128(t1);              \
    fsd     ft0,  128(dest_index);      \
    fld     ft0,  136(t1);              \
    fsd     ft0,  136(dest_index);      \
    fld     ft0,  144(t1);              \
    fsd     ft0,  144(dest_index);      \
    fld     ft0,  152(t1);              \
    fsd     ft0,  152(dest_index);      \
    fld     ft0,  160(t1);              \
    fsd     ft0,  160(dest_index);      \
    fld     ft0,  168(t1);              \
    fsd     ft0,  168(dest_index);      \
    fld     ft0,  176(t1);              \
    fsd     ft0,  176(dest_index);      \
    fld     ft0,  184(t1);              \
    fsd     ft0,  184(dest_index);      \
    fld     ft0,  192(t1);              \
    fsd     ft0,  192(dest_index);




#define SAVE_REGISTERS(dest_index)  \
    sd      sp,  0(dest_index);     \
    sd      s0,  8(dest_index);     \
    sd      s1,  16(dest_index);    \
    sd      s2,  24(dest_index);    \
    sd      s3,  32(dest_index);    \
    sd      s4,  40(dest_index);    \
    sd      s5,  48(dest_index);    \
    sd      s6,  56(dest_index);    \
    sd      s7,  64(dest_index);    \
    sd      s8,  72(dest_index);    \
    sd      s9,  80(dest_index);    \
    sd      s10, 88(dest_index);    \
    sd      s11, 96(dest_index);


#define SAVE_FP_REGISTERS(dest_index)   \
    fsd     fs0,  104(dest_index);      \
    fsd     fs1,  112(dest_index);      \
    fsd     fs2,  120(dest_index);      \
    fsd     fs3,  128(dest_index);      \
    fsd     fs4,  136(dest_index);      \
    fsd     fs5,  144(dest_index);      \
    fsd     fs6,  152(dest_index);      \
    fsd     fs7,  160(dest_index);      \
    fsd     fs8,  168(dest_index);      \
    fsd     fs9,  176(dest_index);      \
    fsd     fs10, 184(dest_index);      \
    fsd     fs11, 192(dest_index);


#define LOAD_REGISTERS(dest_index)  \
    ld      sp,  0(dest_index);     \
    ld      s0,  8(dest_index);     \
    ld      s1,  16(dest_index);    \
    ld      s2,  24(dest_index);    \
    ld      s3,  32(dest_index);    \
    ld      s4,  40(dest_index);    \
    ld      s5,  48(dest_index);    \
    ld      s6,  56(dest_index);    \
    ld      s7,  64(dest_index);    \
    ld      s8,  72(dest_index);    \
    ld      s9,  80(dest_index);    \
    ld      s10, 88(dest_index);    \
    ld      s11, 96(dest_index);


#define LOAD_FP_REGISTERS(dest_index)   \
    fld     fs0,  104(dest_index);      \
    fld     fs1,  112(dest_index);      \
    fld     fs2,  120(dest_index);      \
    fld     fs3,  128(dest_index);      \
    fld     fs4,  136(dest_index);      \
    fld     fs5,  144(dest_index);      \
    fld     fs6,  152(dest_index);      \
    fld     fs7,  160(dest_index);      \
    fld     fs8,  168(dest_index);      \
    fld     fs9,  176(dest_index);      \
    fld     fs10, 184(dest_index);      \
    fld     fs11, 192(dest_index);



#define MOVE_REGISTERS(source_index, dest_index)   \
    ld      t0,  0(source_index);                  \
    sd      t0,  0(dest_index);                    \
    ld      t0,  8(source_index);                  \
    sd      t0,  8(dest_index);                    \
    ld      t0,  16(source_index);                 \
    sd      t0,  16(dest_index);                   \
    ld      t0,  24(source_index);                 \
    sd      t0,  24(dest_index);                   \
    ld      t0,  32(source_index);                 \
    sd      t0,  32(dest_index);                   \
    ld      t0,  40(source_index);                 \
    sd      t0,  40(dest_index);                   \
    ld      t0,  48(source_index);                 \
    sd      t0,  48(dest_index);                   \
    ld      t0,  56(source_index);                 \
    sd      t0,  56(dest_index);                   \
    ld      t0,  64(source_index);                 \
    sd      t0,  64(dest_index);                   \
    ld      t0,  72(source_index);                 \
    sd      t0,  72(dest_index);                   \
    ld      t0,  80(source_index);                 \
    sd      t0,  80(dest_index);                   \
    ld      t0,  88(source_index);                 \
    sd      t0,  88(dest_index);                   \
    ld      t0,  96(source_index);                 \
    sd      t0,  96(dest_index);




#define MOVE_FP_REGISTERS(source_index, dest_index) \
    fld     ft0,  104(source_index);                \
    fsd     ft0,  104(dest_index);                  \
    fld     ft0,  112(source_index);                \
    fsd     ft0,  112(dest_index);                  \
    fld     ft0,  120(source_index);                \
    fsd     ft0,  120(dest_index);                  \
    fld     ft0,  128(source_index);                \
    fsd     ft0,  128(dest_index);                  \
    fld     ft0,  136(source_index);                \
    fsd     ft0,  136(dest_index);                  \
    fld     ft0,  144(source_index);                \
    fsd     ft0,  144(dest_index);                  \
    fld     ft0,  152(source_index);                \
    fsd     ft0,  152(dest_index);                  \
    fld     ft0,  160(source_index);                \
    fsd     ft0,  160(dest_index);                  \
    fld     ft0,  168(source_index);                \
    fsd     ft0,  168(dest_index);                  \
    fld     ft0,  176(source_index);                \
    fsd     ft0,  176(dest_index);                  \
    fld     ft0,  184(source_index);                \
    fsd     ft0,  184(dest_index);                  \
    fld     ft0,  192(source_index);                \
    fsd     ft0,  192(dest_index);




.globl reset_stack_and_registers
# Cette fonction remet la pile d'origine ainsi que les registres d'origine
# void reset_stack_and_registers(process_cycle->process)
reset_stack_and_registers:
    # restauration des registres
    LOAD_REGISTERS(a0)
    #ifdef RISCV_FLOATING_POINT_EXTENSION
    LOAD_FP_REGISTERS(a0)
    #endif
    ret


.globl save_stack_and_registers
# cette fonction sauvegarde la pile et les registres sauvegardés dans le processus qui tourne actuellement
# void save_stack_and_registers(process_cycle->process)
save_stack_and_registers:
    SAVE_REGISTERS(a0)
    #ifdef RISCV_FLOATING_POINT_EXTENSION
    SAVE_FP_REGISTERS(a0)
    #endif
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
# process      -> a0
# prev_process -> a1
.globl switch_registers
switch_registers:
    mv t1, sp # la pile de substitution

    # sauvegarde des registres actuels
    # d'abord on empile les valeurs des registres sur la pile pointée par rdx
    PUSH_REGISTERS()
    #ifdef RISCV_FLOATING_POINT_EXTENSION
    PUSH_FP_REGISTERS()
    #endif

    # restauration des registres de ce processus
    LOAD_REGISTERS(a0)
    #ifdef RISCV_FLOATING_POINT_EXTENSION
    LOAD_FP_REGISTERS(a0)
    #endif

    # on transfère les registres à restaurer à la toute fin
    MOVE_REGISTERS(a1, a0)
    #ifdef RISCV_FLOATING_POINT_EXTENSION
    MOVE_FP_REGISTERS(a1, a0)
    #endif
    
    # et enfin on enregistre les registres courants (stockés dans la pile pointée par rdx) dans le processus précédent
    POP_REGISTERS(a1)
    #ifdef RISCV_FLOATING_POINT_EXTENSION
    POP_FP_REGISTERS(a1)
    #endif

    # si le processus est un processus non encore initialisé, on saute à launch_process
    # on charge l'état du processus dans rax
    lb      t0, REG_BUFFER_SIZE(a0)
    bnez    t0, zero, switch_registers_skip # la valeur Uninitialized correspond à zéro

    # et on lance le processus (on fait un call pour garder l'alignement de la pile, mais il ne reviendra jamais ici)
    call    launch_process
    
    switch_registers_skip:

    # on retourne à l'adresse de retour
    ret



.globl get_stack
get_stack:
    mv a0, sp
    ret




#endif