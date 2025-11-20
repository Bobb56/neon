
REG_BUFFER_SIZE := 8

assume  adl=1


section .text

; variables globales statiques
prev_process:   dl 0
process:        dl 0
old_stack:      dl 0
ret_address:    dl 0


public  _reset_stack_and_registers
_reset_stack_and_registers:
    pop hl ; récupère l'adresse de retour de la fonction
    pop iy ; récupère l'adresse du buffer

    ; load les registres
    ld ix, (3 + iy)
    ld iy, (iy)
    ld sp, iy

    push hl ; remet un argument bidon pour conserver la taille de la pile
    push hl ; remet l'adresse de retour de la fonction sur le sommet de la pile

    ret

public  _save_stack_and_registers
_save_stack_and_registers:
    ; récupère l'adresse de retour de la fonction
    pop hl
    ld (ret_address), hl

    pop iy ; récupère l'adresse du buffer

    ; sauvegarde les registres
    ld hl, $0
    add hl, sp
    ld (iy), hl

    ld (iy + 3), ix

    ; réalloue 3 octets au pif sur la pile
    push hl

    ; remet l'adresse de retour au sommet de la pile
    ld hl, (ret_address)
    push hl

    ret

public  _switch_registers
_switch_registers:
    ; enregistre les arguments dans des variables globales
    pop hl
    ld (ret_address), hl

    pop hl
    ld (process), hl
    pop hl
    ld (prev_process), hl

    ; on sauvegarde la pile dans hl
    ld hl, $0
    add hl, sp

    ; on sauvegarde les registres sauvegardés sur la pile
    push ix
    push hl

    ; on enregistre l'adresse du sommet de pile après avoir poussé hl (sp) et ix
    ld bc, $6
    sbc hl, bc
    ld (old_stack), hl


    ; 2) on charge les sauvegardes des registres dans les registres
    ld iy, (process)
    
    ld ix, (iy + 3)
    ld iy, (iy)
    ld sp, iy ; donc on a la nouvelle pile, mais on a sauvegardé le pointeur vers l'ancienne pile


    ; 3) on transfère les registres à restaurer à la toute fin    
    ld hl, (prev_process)
    ld de, (process)
    ld bc, $6
    ldir

    ; 4) on enregistre dans le processus précédent ses propres registres
    ; récupère le pointeur de pile
    ld hl, (old_stack)
    ld de, (prev_process)
    ld bc, $6
    ldir

    ; si le processus est un processus non encore initialisé, on saute à launch_process
    ld iy, (process)
    ld a, (iy + REG_BUFFER_SIZE)
    cp $0
    jp nz, switch_registers_skip ; on saute si a != 0

    call _launch_process

    switch_registers_skip:

    ; réalloue 6 octets au pif sur la pile
    push hl
    push hl

    ; remet l'adresse de retour sur la pile
    ld hl, (ret_address)
    push hl

    ret

public  _get_stack
_get_stack:
    ld hl, $0
    add hl, sp
    ld bc, $24
    or a, a
    sbc hl, bc
    ret


; Cette fonction renvoie la pile avec laquelle elle est arrivée moins l'adresse de retour
; Elle retourne avec exactement la pile qu'on lui a envoyé en paramètre

public  _floodfill_switch_stack
_floodfill_switch_stack:
    ; récupère l'adresse de retour
    pop hl
    ld (ret_address), hl

    ; récupère la nouvelle pile
    pop iy

    push iy ; à ce stade-là, il ne manque plus qu'à empiler l'adresse de retour pour retrouver sp normal

    ; sauvegarde sp dans old_stack
    ld hl, $0
    add hl, sp
    ld (old_stack), hl

    ; charge la nouvelle pile
    ld sp, iy

    ; remet l'adresse de retour sur la pile
    ld hl, (ret_address)
    push hl

    ; on renvoie la valeur de la pile
    ld hl, (old_stack)

    ret


extern  _launch_process