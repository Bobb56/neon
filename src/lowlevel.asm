#include "headers/constants.h"

; mapping des registres :
; pile temporaire : iy
; registre volatile : hl
; arg1 : de
; arg2 : bc

assume  adl=1

section .text

public  _reset_stack_and_registers
_reset_stack_and_registers:
    pop iy ; récupère l'adresse du buffer
    pop hl ; récupère l'adresse de retour de la fonction

    ; load les registres
    ld sp, (iy)
    ld ix, (3 + iy)

    ld (sp), hl
    ret

public  _save_stack_and_registers
_save_stack_and_registers:
    pop iy ; récupère l'adresse du buffer

    ; sauvegarde les registres
    ld (iy), sp
    ld (iy + 3), ix
    ret

public  _switch_registers
_switch_registers:
    pop de ; prev_process
    pop bc ; process

    ; on sauvegarde la pile dans iy
    push sp
    pop iy

    ; 1) on push les registres dans iy sans décrémenter iy
    ld (iy + 3), sp
    ld (iy + 6), ix

    ; on garde la pile dans hl
    ld hl, iy

    ; 2) on charge les sauvegardes des registres dans les registres
    ld iy, bc
    
    ld sp, (iy)
    ld ix, (iy + 3)

    ; et on push hl dans la nouvelle pile avant de le modifier
    push hl

    ; 3) on transfère les registres à restaurer à la toute fin
    ld iy, de
    ld hl, (iy)
    ld iy, bc
    ld (iy), hl
    
    ld iy, de
    ld hl, (iy + 3)
    ld iy, bc
    ld (iy + 3), hl

    ; 4) on enregistre dans le processus précédent ses propres registres
    ; récupère le pointeur de pile
    pop hl
    push de
    push bc
    ld bc, $6
    ldir

    pop bc
    pop de

    ; si le processus est un processus non encore initialisé, on saute à launch_process
    ld iy, bc
    ld a, (iy + REG_BUFFER_SIZE)
    cp $0
    jp nz, switch_registers_skip ; on saute si a != 0

    call launch_process

    switch_registers_skip:

    ld iy, de
    ld iy, (iy)
    ld iy, (iy)

    ex (sp), iy

    ret

public  _get_stack
_get_stack:
    ld hl, sp
    ld bc, $24
    or a, a
    sbc hl, bc
    ret

extern  launch_process