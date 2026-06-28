#define NEON_SOURCE_ID 16

#include <stdint.h>
#include <stdlib.h>

#include "headers/processcycle.h"
#include "headers/constants.h"
#include "headers/dynarrays.h"
#include "headers/neon.h"
#include "headers/errors.h"
#include "headers/objects.h"
#include "headers/trees.h"
#include "headers/contexts.h"
#include "headers/interpcontext.h"
#include "headers/neonio.h"



// dans le cas où il n'y en n'a pas, elle renvoie le même processus
// renvoie le prochain processus en zappant ceux qui sont marqués Finished
NO_INLINE ProcessCycle* loadNextLivingProcess(ProcessCycle* pc) {
    do {
        pc = pc->next;
    }
    while (pc->process->state == Finished);
    switchGlobalLocalVariables(&pc->process->varsToSave);
    return pc;
}



NO_INLINE void unloadCurrentProcess(Process* p) {
    switchGlobalLocalVariables(&p->varsToSave);
}




void* allocate_new_stack(void) {
    return neon_malloc(STACK_SIZE);
}


void set_stack_pointer(uint8_t* registers, void* stack) {
    if (stack != NULL) {
        #ifndef TI_EZ80
        ((uintptr_t*)registers)[0] = (uintptr_t)-16 & ((uintptr_t)stack + STACK_SIZE); // sommet de la pile aligné à 16 octets
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




Process* ProcessCycle_add(ProcessCycle* pc, TreeBuffer* tb, TreeBufferIndex tree, NeObj fixed_func, int id, bool isInitialized, InterpContext* interp_context) { // renvoie un pointeur vers le processus créé
    // on crée le nouveau processus
    Process* p = neon_malloc(sizeof(Process));
    // création de la nouvelle pile
    ContextStack_init(&p->var_loc);
    CapturedVars_init(&p->varsToSave);
    p->id = id;
    p->stack = NULL; // dans le cas où c'est le premier processus que l'on crée, p->stack va rester NULL, sinon on alloue une nouvelle pile

    // arguments de l'appel à la fonction
    p->state = (isInitialized) ? Running : Uninitialized;

    p->original_call_buffer = tb;
    p->original_call = tree;
    p->interp_context = interp_context;
    p->fixed_function = fixed_func;

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
            neon_fail(12, NO_ARGS);
            ContextStack_destroy(&p->var_loc);
            CapturedVars_destroy(&p->varsToSave);
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
            neon_fail(12, NO_ARGS);
            ContextStack_destroy(&p->var_loc);
            CapturedVars_destroy(&p->varsToSave);
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
    switchGlobalLocalVariables(&p->varsToSave);
}



/*
Cette fonction supprime le processus actuel et passe au processus suivant
*/
NO_INLINE ProcessCycle* ProcessCycle_remove(ProcessCycle* pc) {
    Process* p = pc->process;

    ContextStack_destroy(&p->var_loc); // on suppose que tous les contextes créés dans le cadre de ce processus ont bien été supprimés
    
    if (!neo_is_void(p->fixed_function)) {
        neobject_destroy(p->fixed_function);
    }

    if (p->stack != NULL) {
        neon_free(p->stack);
    }

    CapturedVars_destroy(&p->varsToSave);

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





void save_later(CapturedVars* variables_a_sauvegarder, Var var) {
    if (!bitmap_get(&variables_a_sauvegarder->is_captured, (size_t)var)) {
        bitmap_set(&variables_a_sauvegarder->is_captured, (size_t)var, true);
        ContextStack_append(
            &variables_a_sauvegarder->vars,
            (NeSave) {.var = var, .object = get_var_value(var)}
        );
    }
}




void switchGlobalLocalVariables(CapturedVars* varsToSave) {
    /*
    Cette fonction sauvegarde la valeur des variables locales au moment où on arrive et restaure la valeur locale des variables locales
    */
    for (int i=0 ; i < varsToSave->vars.len ; i++) {
        NeSave* ns = &varsToSave->vars.tab[i];
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
void partialRestore(CapturedVars* varsToSave, CapturedVarsCheckPoint cp) {
    while (varsToSave->vars.len != cp) {
        NeSave save = ContextStack_pop(&varsToSave->vars);
        set_var(save.var, save.object);
        bitmap_set(&varsToSave->is_captured, (size_t)save.var, false);
    }
}





/*
Crée un nouveau processus et renvoie son identifiant
Il faut également indiquer si on doit supprimer l'arbre après avoir exécuté le processus
Si on doit supprimer l'arbre, il doit obligatoirement avoir la forme des arbres que l'on met dans les nouvelles promesses
*/
int create_new_process(TreeBuffer* tb, TreeBufferIndex tree, NeObj fixed_func, bool isInitialized, InterpContext* interp_context) {
    // calcul de l'identifiant du processus que l'on ajoute
    size_t id = 0;

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

    ProcessCycle_add(global_env->process_cycle, tb, tree, fixed_func, id, isInitialized, interp_context); // le processus principal a un id de zéro

    return id;
}


