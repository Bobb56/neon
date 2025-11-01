#include <stdint.h>
#include <stdlib.h>

#include "headers/processcycle.h"
#include "headers/constants.h"
#include "headers/dynarrays.h"
#include "headers/neon.h"
#include "headers/errors.h"
#include "headers/trees.h"



// dans le cas où il n'y en n'a pas, elle renvoie le même processus
// renvoie le prochain processus en zappant ceux qui sont marqués Finished
NO_INLINE ProcessCycle* loadNextLivingProcess(ProcessCycle* pc) {
    do {
        pc = pc->next;
    }
    while (pc->process->state == Finished);

    switchGlobalLocalVariables(pc->process->varsToSave);
    return pc;
}



NO_INLINE void unloadCurrentProcess(Process* p) {
    switchGlobalLocalVariables(p->varsToSave);
}




void* allocate_new_stack(void) {
    return neon_malloc(STACK_SIZE);
}


void set_stack_pointer(uint8_t* registers, void* stack) {
    if (stack != NULL) {
        #ifndef TI_EZ80
        ((uintptr_t*)registers)[0] = -16 & ((uintptr_t)stack + STACK_SIZE); // sommet de la pile aligné à 16 octets
        #else
        ((uintptr_t*)registers)[0] = ((uintptr_t)stack + STACK_SIZE); // sommet de la pile non aligné
        #endif
    }
}



ProcessCycle* ProcessCycle_create(void) {
    ProcessCycle* pc = neon_malloc(sizeof(ProcessCycle));
    pc->process = NULL;
    pc->next = NULL;
    pc->prev = NULL;
    return pc;
}




Process* ProcessCycle_add(ProcessCycle* pc, NeTree tree, int id, bool isInitialized) { // renvoie un pointeur vers le processus créé
    // on crée le nouveau processus
    Process* p = neon_malloc(sizeof(Process));
    // création de la nouvelle pile
    p->var_loc = ptrlist_create();
    p->id = id;
    p->varsToSave = ptrlist_create();
    p->stack = NULL; // dans le cas où c'est le premier processus que l'on crée, p->stack va rester NULL, sinon on alloue une nouvelle pile

    // arguments de l'appel à la fonction
    p->state = (isInitialized) ? Running : Uninitialized;

    p->original_call = tree;

    // et maintenant on chaîne le processus au cycle des processus
    // trois cas de figure possibles

    // c'est le premier élément, donc on n'alloue pas de pile spéciale
    if (pc->process == NULL && pc->next == NULL && pc->prev == NULL) {
        pc->process = p;
        pc->next = pc; pc->prev = pc;
    }
    else if (pc->next == pc && pc->prev == pc) { // vu que c'est un cycle, la seule possibilité est qu'il n'y ait qu'un seul élément
        p->stack = allocate_new_stack();

        if (p->stack == NULL) {
            global_env->CODE_ERROR = 12;
            neon_free(p->var_loc);
            neon_free(p->varsToSave);
            neon_free(p);
            return NULL;
        }

        ProcessCycle* npc = ProcessCycle_create();
        npc->process = p;

        pc->next = npc; npc->prev = pc;
        npc->next = pc; pc->prev = npc;
        // ça y est on a un cycle
    }
    else { // aucun des deux n'est nul, donc on s'insère entre les deux
        p->stack = allocate_new_stack();

        if (p->stack == NULL) {
            global_env->CODE_ERROR = 12;
            neon_free(p->var_loc);
            neon_free(p->varsToSave);
            neon_free(p);
            return NULL;
        }

        ProcessCycle* npc = ProcessCycle_create();
        npc->process = p;

        npc->next = pc->next;
        pc->next->prev = npc;
        pc->next = npc;
        npc->prev = pc;
    }

    set_stack_pointer(p->registers, p->stack);

    // du coup le prochain processus à exécuter dans la chaîne est le processus que l'on vient d'ajouter

    return p;

}

/*
NO_INLINE void ProcessCycle_print(ProcessCycle* cycle) {
    ProcessCycle* sov = cycle;
    do {
        printString("Process n°");
        printInt(cycle->process->id);

        if (cycle->process->state == Uninitialized)
            printString(": Uninitialized");
        else if (cycle->process->state == Running)
            printString(": Running");
        else if (cycle->process->state == Finished)
            printString(": Finished");

        newLine();
        cycle = cycle->next;
    }
    while (cycle != sov);
}
*/

// renvoie vrai si et seulement si : il existe un processus dans ce cycle => ce processus a fini

NO_INLINE bool ProcessCycle_isActive(ProcessCycle* cycle) {
    ProcessCycle* cycle_sov = cycle;
    do {
        if (cycle->process->state != Finished)
            return true;
        
        cycle = cycle->next;
    } while (cycle_sov != cycle);
    return false;
}

bool ProcessCycle_isEmpty(ProcessCycle* pc) {
    return pc->process == NULL && pc->next == NULL && pc->prev == NULL;
}



// supprime les processus ayant terminé, ne change pas le processus actuel, même si il a terminé
NO_INLINE void ProcessCycle_clean(ProcessCycle* cycle) {
    ProcessCycle* cycle_sov = cycle;

    cycle = cycle->next;
    while (cycle != NULL && cycle != cycle_sov) {
        if (cycle->process->state == Finished) {
            cycle = ProcessCycle_remove(cycle);
        }
        else {
            cycle = cycle->next;
        }
    }
    return;
}



NO_INLINE void process_preRemove(Process* p) {
    //printf("End process %d\n", p->id);
    p->state = Finished;
    switchGlobalLocalVariables(p->varsToSave);
}



/*
Cette fonction supprime le processus actuel et passe au processus suivant
*/
NO_INLINE ProcessCycle* ProcessCycle_remove(ProcessCycle* pc) {
    Process* p = pc->process;

    neon_free(p->var_loc); // on suppose que tous les contextes créés dans le cadre de ce processus ont bien été supprimés
    
    if (!NeTree_isvoid(p->original_call)) {
        neobject_destroy(p->original_call.fcall->function_obj);
        neon_free(p->original_call.pointer);
    }

    if (p->stack != NULL) {
        neon_free(p->stack);
    }

    // libération de la pile

    ptrlist_destroy(p->varsToSave, true, true);

    neon_free(p);

    ProcessCycle* next = pc->next;

    if (pc->next == pc && pc->prev == pc) { // plus qu'un seul élément
        pc->next = NULL;
        pc->prev = NULL;
        pc->process = NULL;
        return pc;
    }
    else if (pc->next == pc->prev) {// il y a uniquement deux processus
        neon_free(pc);
        next->next = next;
        next->prev = next;
    }
    else {
        pc->prev->next = pc->next;
        pc->next->prev = pc->prev;
        neon_free(pc);
    }

    return next;
}





void save_later(ptrlist* variables_a_sauvegarder, Var var) {
    // on regarde si cette variable faisait déjà partie de nos variables privatisées ou pas
    bool bo = false;
    for (ptrlist* ptr = variables_a_sauvegarder ; ptr != NULL && ptr->tete != NULL && !bo ; ptr = ptr->queue) {
        NeSave* ns = (NeSave*)ptr->tete;
        bo = bo || ns->var == var;
    }

    if (!bo) { // alors il faut ajouter cette variable à nos variables privatisées
        NeSave* ns = neon_malloc(sizeof(NeSave));
        ns->object = get_var_value(var);
        ns->var = var;
        ptrlist_append(variables_a_sauvegarder, (void*)ns);
    }
}




void switchGlobalLocalVariables(ptrlist* varsToSave) {
    /*
    Cette fonction sauvegarde la valeur des variables locales au moment où on arrive et restaure la valeur locale des variables locales
    */
    for (ptrlist* ptr = varsToSave ; ptr != NULL && ptr->tete != NULL ; ptr = ptr->queue) {
        NeSave* ns = (NeSave*)ptr->tete;
        // on va switcher entre la valeur stockée dans le nesave et la valeur actuelle
        NeObj temp = ns->object;
        ns->object = get_var_value(ns->var);
        set_var(ns->var, temp);
    }
}


/*
Quand un processus n'a plus besoin d'utiliser une version locale à lui-même de certaines variables,
on restaure la valeur globale de ces variables, et on enlève les variables de la liste des variables à sauvegarder
*/
void partialRestore(ptrlist* varsToSave, ptrlist* sov_vars_to_save) {
    NeSave* save;
    while (varsToSave->tete != sov_vars_to_save) {
        save = ptrlist_pop(varsToSave);
        set_var(save->var, save->object);
        neon_free(save);
    }
}





/*
Crée un nouveau processus et renvoie son identifiant
Il faut également indiquer si on doit supprimer l'arbre après avoir exécuté le processus
Si on doit supprimer l'arbre, il doit obligatoirement avoir la forme des arbres que l'on met dans les nouvelles promesses
*/
int create_new_process(NeTree tree, bool isInitialized) {
    // calcul de l'identifiant du processus que l'on ajoute
    int id = 0;

    // on doit garder les processus n'ayant pas fini et les processus ayant fini mais n'ayant pas toutes été récupérées
    for (; id < global_env->PROMISES->len && (global_env->PROCESS_FINISH.tab[id] == false || *global_env->PROMISES_CNT.tab[id] > 0) ; id++);

    if (id >= global_env->PROMISES->len) { // l'identifiant est un nouvel identifiant
        nelist_append(global_env->PROMISES, NEO_VOID);
        intptrlist_append(&global_env->PROMISES_CNT, neon_malloc(sizeof(int))); // on alloue un nouveau pointeur ici, il y restera jusqu'à la fin de la vie de l'interpréteur
        intlist_append(&global_env->PROCESS_FINISH, 0);
    }

    neobject_destroy(global_env->PROMISES->tab[id]);
    global_env->PROMISES->tab[id] = NEO_VOID;
    *global_env->PROMISES_CNT.tab[id] = 1;
    global_env->PROCESS_FINISH.tab[id] = 0;

    ProcessCycle_add(global_env->process_cycle, tree, id, isInitialized); // le processus principal a un id de zéro

    return id;
}


