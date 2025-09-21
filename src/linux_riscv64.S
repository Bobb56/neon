#include "headers/constants.h"

.extern launch_process


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
    bnez    t0, switch_registers_skip # la valeur Uninitialized correspond à zéro

    # et on lance le processus (on fait un call pour garder l'alignement de la pile, mais il ne reviendra jamais ici)
    call    launch_process
    
    switch_registers_skip:

    # on retourne à l'adresse de retour
    ret



.globl get_stack
get_stack:
    mv a0, sp
    ret

