

REG_BUFFER_SIZE := 8

assume  adl=1

section .text


public  _reset_stack_and_registers
_reset_stack_and_registers:
    pop iy ; récupère l'adresse du buffer
    pop hl ; récupère l'adresse de retour de la fonction

    ; load les registres
    ld ix, (3 + iy)
    ld iy, (iy)
    ld sp, iy
    

    ; change l'adresse de retour de la fonction dans la pile
    ex (sp), hl
    ret

public  _save_stack_and_registers
_save_stack_and_registers:
    pop iy ; récupère l'adresse du buffer

    ; sauvegarde les registres
    ld hl, $0
    add hl, sp
    ld (iy), hl

    ld (iy + 3), ix
    ret

public  _switch_registers
_switch_registers:
    pop de ; prev_process
    pop bc ; process

    ; on sauvegarde la pile dans hl
    ld hl, $0
    add hl, sp

    ; on sauvegarde les registres sauvegardés sur la pile
    push hl
    push ix

    ; on garde la pile dans hl

    ; 2) on charge les sauvegardes des registres dans les registres
    push bc
    pop iy
    
    ld ix, (iy + 3)
    ld iy, (iy)
    ld sp, iy ; donc on a la nouvelle pile

    ; et on push hl dans la nouvelle pile avant de le modifier
    push hl

    ; 3) on transfère les registres à restaurer à la toute fin
    push de
    push bc
    exx ; sauvegarde hl, bc et de
    
    pop de
    pop hl
    ld bc, $6
    ldir

    exx
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
    push bc
    pop iy
    ld a, (iy + REG_BUFFER_SIZE)
    cp $0
    jp nz, switch_registers_skip ; on saute si a != 0

    call _launch_process

    switch_registers_skip:

    push de
    pop iy
    
    ld iy, (iy)
    ld iy, (iy)

    ; échange l'adresse contenue dans iy avec la valeur en somment de pile
    ex (sp), iy

    ret

public  _get_stack
_get_stack:
    ld hl, $0
    add hl, sp
    ld bc, $24
    or a, a
    sbc hl, bc
    ret

extern  _launch_process