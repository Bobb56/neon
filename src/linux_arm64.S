#define NEON_SOURCE_ID 29

#include "headers/constants.h"

/*
 * ASSUMPTION: adjust these macro names if your headers/constants.h uses a
 * different convention (e.g. AARCH64 instead of ARM64).
 */
#if defined(LINUX_ARM64) || defined(MINIMAL_LIBC_ARM64)

.extern launch_process


# pour ne pas avoir de pile exécutable
.section .note.GNU-stack

.text

# ----------------------------------------------------------------------------
# Register mapping notes (RISC-V -> ARM64)
# ----------------------------------------------------------------------------
# RISC-V saves 13 int regs (sp, s0-s11) and 12 FP regs (fs0-fs11) = 200 bytes.
# ARM64's callee-saved set is smaller:
#   - GPRs : sp, x19-x28, x29(fp)            -> 12 regs  (96 bytes)
#   - FPRs : d8-d15 (low 64 bits only, AAPCS) -> 8 regs   (64 bytes)
# Total context struct size is therefore 160 bytes, not 200.
# Neither ra (RISC-V) nor lr/x30 (ARM64) is part of the saved set: every
# process is always suspended at the same call site inside this function,
# so the link register's value is process-independent and needs no transfer.
# ----------------------------------------------------------------------------

# ce code utilise 4 registres de travail de la taille d'un pointeur : a0, a1, t0, t1
a0  .req x0
a1  .req x1
t0  .req x10   # scratch register (équivalent de t0 en RISC-V)
t1  .req x9    # pointeur de pile de substitution (équivalent de t1 en RISC-V)
ft0 .req d16   # scratch flottant (équivalent de ft0 en RISC-V)


# empile les registres sur la pile, en utilisant t1 comme registre de pile
#define PUSH_REGISTERS()        \
    sub     t1,  t1, #160;      \
    mov     t0,  sp;            \
    str     t0,  [t1, #0];      \
    str     x19, [t1, #8];      \
    str     x20, [t1, #16];     \
    str     x21, [t1, #24];     \
    str     x22, [t1, #32];     \
    str     x23, [t1, #40];     \
    str     x24, [t1, #48];     \
    str     x25, [t1, #56];     \
    str     x26, [t1, #64];     \
    str     x27, [t1, #72];     \
    str     x28, [t1, #80];     \
    str     x29, [t1, #88];


#define PUSH_FP_REGISTERS()      \
    str     d8,  [t1, #96];      \
    str     d9,  [t1, #104];     \
    str     d10, [t1, #112];     \
    str     d11, [t1, #120];     \
    str     d12, [t1, #128];     \
    str     d13, [t1, #136];     \
    str     d14, [t1, #144];     \
    str     d15, [t1, #152];



# dépile les registres dans une autre structure
# modifie t0
#define POP_REGISTERS(dest_index)      \
    ldr     t0, [t1, #0];              \
    str     t0, [dest_index, #0];      \
    ldr     t0, [t1, #8];              \
    str     t0, [dest_index, #8];      \
    ldr     t0, [t1, #16];             \
    str     t0, [dest_index, #16];     \
    ldr     t0, [t1, #24];             \
    str     t0, [dest_index, #24];     \
    ldr     t0, [t1, #32];             \
    str     t0, [dest_index, #32];     \
    ldr     t0, [t1, #40];             \
    str     t0, [dest_index, #40];     \
    ldr     t0, [t1, #48];             \
    str     t0, [dest_index, #48];     \
    ldr     t0, [t1, #56];             \
    str     t0, [dest_index, #56];     \
    ldr     t0, [t1, #64];             \
    str     t0, [dest_index, #64];     \
    ldr     t0, [t1, #72];             \
    str     t0, [dest_index, #72];     \
    ldr     t0, [t1, #80];             \
    str     t0, [dest_index, #80];     \
    ldr     t0, [t1, #88];             \
    str     t0, [dest_index, #88];




#define POP_FP_REGISTERS(dest_index)     \
    ldr     ft0, [t1, #96];              \
    str     ft0, [dest_index, #96];      \
    ldr     ft0, [t1, #104];             \
    str     ft0, [dest_index, #104];     \
    ldr     ft0, [t1, #112];             \
    str     ft0, [dest_index, #112];     \
    ldr     ft0, [t1, #120];             \
    str     ft0, [dest_index, #120];     \
    ldr     ft0, [t1, #128];             \
    str     ft0, [dest_index, #128];     \
    ldr     ft0, [t1, #136];             \
    str     ft0, [dest_index, #136];     \
    ldr     ft0, [t1, #144];             \
    str     ft0, [dest_index, #144];     \
    ldr     ft0, [t1, #152];             \
    str     ft0, [dest_index, #152];




#define SAVE_REGISTERS(dest_index)   \
    mov     t0,  sp;                 \
    str     t0,  [dest_index, #0];   \
    str     x19, [dest_index, #8];   \
    str     x20, [dest_index, #16];  \
    str     x21, [dest_index, #24];  \
    str     x22, [dest_index, #32];  \
    str     x23, [dest_index, #40];  \
    str     x24, [dest_index, #48];  \
    str     x25, [dest_index, #56];  \
    str     x26, [dest_index, #64];  \
    str     x27, [dest_index, #72];  \
    str     x28, [dest_index, #80];  \
    str     x29, [dest_index, #88];


#define SAVE_FP_REGISTERS(dest_index)    \
    str     d8,  [dest_index, #96];      \
    str     d9,  [dest_index, #104];     \
    str     d10, [dest_index, #112];     \
    str     d11, [dest_index, #120];     \
    str     d12, [dest_index, #128];     \
    str     d13, [dest_index, #136];     \
    str     d14, [dest_index, #144];     \
    str     d15, [dest_index, #152];


#define LOAD_REGISTERS(dest_index)   \
    ldr     t0,  [dest_index, #0];   \
    mov     sp,  t0;                 \
    ldr     x19, [dest_index, #8];   \
    ldr     x20, [dest_index, #16];  \
    ldr     x21, [dest_index, #24];  \
    ldr     x22, [dest_index, #32];  \
    ldr     x23, [dest_index, #40];  \
    ldr     x24, [dest_index, #48];  \
    ldr     x25, [dest_index, #56];  \
    ldr     x26, [dest_index, #64];  \
    ldr     x27, [dest_index, #72];  \
    ldr     x28, [dest_index, #80];  \
    ldr     x29, [dest_index, #88];


#define LOAD_FP_REGISTERS(dest_index)    \
    ldr     d8,  [dest_index, #96];      \
    ldr     d9,  [dest_index, #104];     \
    ldr     d10, [dest_index, #112];     \
    ldr     d11, [dest_index, #120];     \
    ldr     d12, [dest_index, #128];     \
    ldr     d13, [dest_index, #136];     \
    ldr     d14, [dest_index, #144];     \
    ldr     d15, [dest_index, #152];



#define MOVE_REGISTERS(source_index, dest_index)   \
    ldr     t0, [source_index, #0];                \
    str     t0, [dest_index, #0];                  \
    ldr     t0, [source_index, #8];                \
    str     t0, [dest_index, #8];                  \
    ldr     t0, [source_index, #16];               \
    str     t0, [dest_index, #16];                 \
    ldr     t0, [source_index, #24];               \
    str     t0, [dest_index, #24];                 \
    ldr     t0, [source_index, #32];               \
    str     t0, [dest_index, #32];                 \
    ldr     t0, [source_index, #40];               \
    str     t0, [dest_index, #40];                 \
    ldr     t0, [source_index, #48];               \
    str     t0, [dest_index, #48];                 \
    ldr     t0, [source_index, #56];               \
    str     t0, [dest_index, #56];                 \
    ldr     t0, [source_index, #64];               \
    str     t0, [dest_index, #64];                 \
    ldr     t0, [source_index, #72];               \
    str     t0, [dest_index, #72];                 \
    ldr     t0, [source_index, #80];               \
    str     t0, [dest_index, #80];                 \
    ldr     t0, [source_index, #88];               \
    str     t0, [dest_index, #88];




#define MOVE_FP_REGISTERS(source_index, dest_index)  \
    ldr     ft0, [source_index, #96];                \
    str     ft0, [dest_index, #96];                  \
    ldr     ft0, [source_index, #104];               \
    str     ft0, [dest_index, #104];                 \
    ldr     ft0, [source_index, #112];               \
    str     ft0, [dest_index, #112];                 \
    ldr     ft0, [source_index, #120];               \
    str     ft0, [dest_index, #120];                 \
    ldr     ft0, [source_index, #128];               \
    str     ft0, [dest_index, #128];                 \
    ldr     ft0, [source_index, #136];               \
    str     ft0, [dest_index, #136];                 \
    ldr     ft0, [source_index, #144];               \
    str     ft0, [dest_index, #144];                 \
    ldr     ft0, [source_index, #152];               \
    str     ft0, [dest_index, #152];




.globl reset_stack_and_registers
# Cette fonction remet la pile d'origine ainsi que les registres d'origine
# void reset_stack_and_registers(process_cycle->process)
reset_stack_and_registers:
    # restauration des registres
    LOAD_REGISTERS(a0)
    LOAD_FP_REGISTERS(a0)
    ret


.globl save_stack_and_registers
# cette fonction sauvegarde la pile et les registres sauvegardés dans le processus qui tourne actuellement
# void save_stack_and_registers(process_cycle->process)
save_stack_and_registers:
    SAVE_REGISTERS(a0)
    SAVE_FP_REGISTERS(a0)
    ret



/*
Quand un processus tourne, la valeur des registres stockés à l'intérieur est la valeur à restaurer à la fin du processus
Quand un processus ne tourne pas, la valeur des registres stockés à l'intérieur est la valeur des registres sauvegardés de ce processus
Quand on passe d'un processus à l'autre, on effectue les transferts suivants :
registres du processus sortant -> stockés dans la structure du processus sortant
registres sanvegardés dans la structure du nouveau processus -> mis dans les vrais registres
registres à restaurer après l'ancien processus -> stockés dans la structure du nouveau processus

Ces transferts permettent de toujours garantir l'intégrité des processus, ainsi que le bon fonctionnement
de concert avec reset_stack_and_registers et save_stack_and_registers

Remarque :
----------
Ici, on effectue des transferts sur tous les registres sauvegardés, que ce soit les pointeurs
de cadre de pile, le pointeur de pile, ou des registres sauvegardés classiques.
On pourrait tout à fait effectuer ces transferts uniquement avec le pointeur de pile,
et empiler au préalable tous les registres dessus, et les dépiler après avoir restauré le registre
de pile. Cela résulterait en du code plus simple, plus propre, moins propice aux erreurs.
Néanmoins la version actuelle fonctionne et l'optimisation n'est que légère d'un point de vue code, très légère
 voire inexistante d'un point de vue mémoire, et également relativement légère niveau temps.

ARM64 : x30/lr n'est volontairement pas transféré, pour la même raison que ra ne l'était pas
en RISC-V : chaque processus est toujours suspendu au même site d'appel (dans cette fonction,
appelée depuis un point fixe de l'ordonnanceur), donc lr est indépendant du processus en cours.
*/
# switch_registers(process, prev_process)
# process      -> a0
# prev_process -> a1
.globl switch_registers
switch_registers:
    mov t1, sp # la pile de substitution

    # sauvegarde des registres actuels
    # d'abord on empile les valeurs des registres sur la pile pointée par t1
    PUSH_REGISTERS()
    PUSH_FP_REGISTERS()

    # restauration des registres de ce processus
    LOAD_REGISTERS(a0)
    LOAD_FP_REGISTERS(a0)

    # on transfère les registres à restaurer à la toute fin
    MOVE_REGISTERS(a1, a0)
    MOVE_FP_REGISTERS(a1, a0)

    # et enfin on enregistre les registres courants (stockés dans la pile pointée par t1) dans le processus précédent
    POP_REGISTERS(a1)
    POP_FP_REGISTERS(a1)

    # si le processus est un processus non encore initialisé, on saute à launch_process
    ldrb    w10, [a0, #REG_BUFFER_SIZE]
    cbnz    w10, switch_registers_skip # la valeur Uninitialized correspond à zéro

    # et on lance le processus (on fait un bl pour garder l'alignement de la pile, mais il ne reviendra jamais ici)
    bl      launch_process

    switch_registers_skip:

    # on retourne à l'adresse de retour
    ret



.globl get_stack
get_stack:
    mov a0, sp
    ret

#endif
