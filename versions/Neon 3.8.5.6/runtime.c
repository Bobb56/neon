#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "headers.h"


extern NeList* PROMISES;
extern intptrlist PROMISES_CNT;

extern int CODE_ERROR;

extern strlist* CONTAINERS;
extern NeList* ATTRIBUTES;

extern intlist TYPESBUILTINSFONC;
extern strlist NOMSBUILTINSFONC;
extern NeObj (*BUILTINSFONC[NBBUILTINFONC])(NeList*);
extern NeList* ADRESSES;
extern strlist HELPBUILTINSFONC;
extern strlist* NOMS;
extern intlist NBARGS;

extern strlist exceptions;
extern int LINENUMBER;
extern int NAME;
//extern ptrlist* VAR_LOC;
extern int ATOMIC_TIME;
extern int atomic_counter;
extern intlist PROCESS_FINISH;
extern intlist OPERANDES;
extern NeObj RETURN_VALUE;
extern intlist exceptions_err;
extern char* EXCEPTION;
extern ProcessCycle* process_cycle;
extern void* STACK_PTR;

extern NeObj reset_stack_and_registers(void);
extern void switch_registers(Process* process, Process* prev, int save_stack, int load_stack);
extern NeObj eval_epilog_unaligned(void);

// fonctions des opérateurs
void* OPFONC[NBOPERATEURS] = {_and,_or,_xor,_add,_mul,_sub,_div,_pow,_equal,_notEq,_infEqual,_supEqual,_inf,_sup,_affectNone,_addEqual,_subEqual,_mulEqual,_divEqual,_incr,_decr,_not,_mod,_eucl,_ref,_goIn, _deref, _minus, _del, _affect, NULL, _exponent, _implique, _in, NULL, NULL, _swap, NULL, NULL};

// fonctions des fonctions built-in
NeObj (*BUILTINSFONC[NBBUILTINFONC])(NeList*)={_print_,_input_,_nbr_,_str_,_len_,_substring_,_exit_,_append_,_remove_,_insert_,_type_, _reverse_, _eval_,_clear_,_help_, _randint_,_failwith_, _time_, _assert_, _output_, _chr_, _ord_, _list_comp_, _create_exception_, _raise_, _int_, _index_, _replace_, _count_, _list_, _sort_asc_, _sort_desc_, _sin_, _cos_, _tan_, _deg_, _rad_, _sqrt_, _ln_, _exp_, _log_, _log2_, _round_, _abs_, _ceil_, _floor_, _readFile_, _writeFile_, _setFunctionDoc_, _setAtomicTime_, _copy_, _load_namespace_, _gc_, _setColor_};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////




void updateFileName(char* name)
{
    neobject_destroy(ADRESSES->tab[NAME]);
    ADRESSES->tab[NAME] = neo_str_create(name);

    return ;
}





void newContext(ptrlist* var_loc)
{
    // création d'un nouveau contexte
    ptrlist* ctxt = ptrlist_create();
    ptrlist_append(var_loc, ctxt);
}


void deleteContext(ptrlist* var_loc)
{
    //suppression du contexte
    ptrlist* ctxt2 = (ptrlist*)var_loc->tete;

    
    if (ctxt2->tete == NULL) // aucun élément
    {
        err_free(ctxt2);
    }

    else if (ctxt2->queue == NULL) // un seul élément
    {
        //-------------
        NeSave* ns = ctxt2->tete;
        replace_var(ns->var, ns->object);
        err_free(ns);
        //-------------

        err_free(ctxt2);
    }
    else // plusieurs éléments
    {
        ptrlist* temp = NULL;
        ptrlist* ptr = ctxt2;
        while (ptr->queue != NULL)
        {
            //-------------
            NeSave* ns = ptr->tete;
            replace_var(ns->var, ns->object);
            err_free(ns);
            //-------------
            
            temp = ptr->queue;
            err_free(ptr);
            ptr = temp;
        }
        //-------------
        NeSave* ns = ptr->tete;
        replace_var(ns->var, ns->object);
        err_free(ns);
        //-------------
        
        err_free(ptr);
    }

    ptrlist_remove(var_loc, ctxt2, true); // enlève le pointeur

    return ;
}


bool isTrue(Tree* tree) {
    NeObj obj = eval_prolog(tree);
    bool b = neoIsTrue(obj);
    neobject_destroy(obj);
    return b;
}




bool neoIsTrue(NeObj expr)
{
    if (CODE_ERROR != 0)
    {
        return NULL;
    }
    
    if (NEO_TYPE(expr) == TYPE_BOOL)
    {
        bool res = neo_to_bool(expr);
        return res;
    }
    else if (NEO_TYPE(expr) == TYPE_INTEGER)
    {
        bool res = neo_to_integer(expr) != 0;
        return res;
    }
    else
    {
        CODE_ERROR = 20;
    }
    return false;
}


void* allocate_new_stack(void) {
    return err_malloc(STACK_SIZE);
}


void set_stack_pointer(uint8_t* registers, void* stack) {
    if (stack != NULL) {
        ((uint64_t*)registers)[6] = -16 & ((uint64_t)stack + STACK_SIZE); // adresse alignée à 16 octets
        // ajoute l'adresse de retour vers eval_epilog
        ((uint64_t**)registers)[6] -= 1;
        *((uint64_t**)registers)[6] = (uint64_t) eval_epilog_unaligned;
        ((uint64_t**)registers)[6] -= 1; // de cette manière la pile est alignée, mais en revanche l'appel ne se fera pas de
        // manière alignée, donc on utilise une routine assembleur qui va réaligner ça
        // organisation de la pile que l'on vient de créer :
        // ... | 0 | ret | ? | ...
        //       ^
        //       0    8    0  <- case à laquelle on arrive après le premier alignement
    }
}



void local(Var var, ptrlist* var_loc)
{
    // sauvegarde de l'object actuel
    NeSave* ns = err_malloc(sizeof(NeSave));
    ns->object = get_var_value(var);
    ns->var = var;

    // réinitialisation de la variable
    var_reset(get_absolute_address(var));

    //ajout de la sauvegarde au contexte actuel
    
    ptrlist_append((ptrlist*)(var_loc->tete), (void*)ns);
}







ProcessCycle* processCycle_create(void) {
    ProcessCycle* pc = err_malloc(sizeof(ProcessCycle));
    pc->process = NULL;
    pc->next = NULL;
    pc->prev = NULL;
    return pc;
}








void save_later(ptrlist* variables_a_sauvegarder, Var var) {
    // on regarde si cette variable faisait déjà partie de nos variables privatisées ou pas
    bool bo = false;
    for (ptrlist* ptr = variables_a_sauvegarder ; ptr != NULL && ptr->tete != NULL && !bo ; ptr = ptr->queue) {
        NeSave* ns = (NeSave*)ptr->tete;
        bo = bo || ns->var == var;
    }

    if (!bo) { // alors il faut ajouter cette variable à nos variables privatisées
        NeSave* ns = err_malloc(sizeof(NeSave));
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
        err_free(save);
    }
}



ProcessCycle* nextProcess(ProcessCycle* pc) {
    
    if (pc->next != NULL) { // && pc->next != pc

        // on ne sauvegarde que si on change de processus
        // on garde en mémoire la valeur des ces objets

        switchGlobalLocalVariables(pc->process->varsToSave);

        // on va maintenant restaurer les anciennes variables
        switchGlobalLocalVariables(pc->next->process->varsToSave);

        return pc->next;
    }

    return pc;
}






Process* processCycle_add(ProcessCycle* pc, Tree* tree, int id, bool delete_tree) { // renvoie un pointeur vers le processus créé
    // on crée le nouveau processus
    Process* p = err_malloc(sizeof(Process));
    // création de la nouvelle pile
    p->var_loc = ptrlist_create();
    p->id = id;
    p->varsToSave = ptrlist_create();
    p->stack = NULL; // dans le cas où c'est le premier processus que l'on crée, p->stack va rester NULL, sinon on alloue une nouvelle pile

    // arguments de l'appel à la fonction
    p->arg_tree = tree;

    p->original_call = (delete_tree) ? tree : NULL;

    // et maintenant on chaîne le processus au cycle des processus
    // trois cas de figure possibles

    // c'est le premier élément
    if (pc->process == NULL && pc->next == NULL && pc->prev == NULL) {
        pc->process = p;
        pc->next = pc; pc->prev = pc;
    }
    else if (pc->next == pc && pc->prev == pc) { // vu que c'est un cycle, la seule possibilité est qu'il n'y ait qu'un seul élément
        p->stack = allocate_new_stack();

        ProcessCycle* npc = processCycle_create();
        npc->process = p;

        pc->next = npc; npc->prev = pc;
        npc->next = pc; pc->prev = npc;
        // ça y est on a un cycle
    }
    else { // aucun des deux n'est nul, donc on s'insère entre les deux
        p->stack = allocate_new_stack();

        ProcessCycle* npc = processCycle_create();
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
Cette fonction supprime le processus actuel et passe au processus suivant
*/
ProcessCycle* processCycle_remove(ProcessCycle* pc) {
    Process* p = pc->process;

    // on remet les variables d'avant ce processus comme il faut
    switchGlobalLocalVariables(p->varsToSave);

    err_free(p->var_loc); // on suppose que tous les contextes créés dans le cadre de ce processus ont bien été supprimés
    
    if (p->original_call != NULL) {
        neobject_destroy(p->original_call->data);
        err_free(p->original_call);
    }

    if (p->stack != NULL) {
        err_free(p->stack);
    }

    // libération de la pile

    ptrlist_destroy(p->varsToSave, true, true);

    err_free(p);

    ProcessCycle* next = pc->next;

    if (pc->next == pc && pc->prev == pc) { // plus qu'un seul élément
        err_free(pc);
        return NULL;
    }
    else if (pc->next == pc->prev) {// il y a uniquement deux processus
        err_free(pc);
        next->next = next;
        next->prev = next;
    }
    else {
        pc->prev->next = pc->next;
        pc->next->prev = pc->prev;
        err_free(pc);
    }

    // changement de la pile

    // on restaure les variables
    switchGlobalLocalVariables(next->process->varsToSave);

    return next;

}

/*
Crée un nouveau processus et renvoie son identifiant
Il faut également indiquer si on doit supprimer l'arbre après avoir exécuté le processus
Si on doit supprimer l'arbre, il doit obligatoirement avoir la forme des arbres que l'on met dans les nouvelles promesses
*/
int create_new_process(Tree* tree, bool delete_tree) {
    // calcul de l'identifiant du processus que l'on ajoute
    int id = 0;

    // on doit garder les processus n'ayant pas fini et les processus ayant fini mais n'ayant pas toutes été récupérées
    for (; id < PROMISES->len && (PROCESS_FINISH.tab[id] == false || *PROMISES_CNT.tab[id] > 0) ; id++);

    if (id >= PROMISES->len) { // l'identifiant est un nouvel identifiant
        nelist_append(PROMISES, NEO_VOID);
        intptrlist_append(&PROMISES_CNT, err_malloc(sizeof(int))); // on alloue un nouveau pointeur ici, il y restera jusqu'à la fin de la vie de l'interpréteur
        intlist_append(&PROCESS_FINISH, 0);
    }

    neobject_destroy(PROMISES->tab[id]);
    PROMISES->tab[id] = NEO_VOID;
    *PROMISES_CNT.tab[id] = 1;
    PROCESS_FINISH.tab[id] = 0;

    Process* p = processCycle_add(process_cycle, tree, id, delete_tree); // le processus principal a un id de zéro

    return id;
}




// définition des fonctions no inline
// certaines fonctions sont interfacées avec de l'assembleur qui modifie leur pile.
// il est donc important que ces fonctions conservent toujours la même taille de contexte (8 octets en l'occurrence)
// pour cela, on redéfinit les fonctions qui sont appelées par ces fonctions en interdisant de les inliner
__attribute__((noinline))
int ptrlist_destroy_noinline(ptrlist* l, bool freeElements, bool freeTab) {
    return ptrlist_destroy(l, freeElements, freeTab);
}

__attribute__((noinline))
ProcessCycle* nextProcess_noinline(ProcessCycle* pc) {
    return nextProcess(pc);
}

__attribute__((noinline))
void neobject_destroy_noinline(NeObj neo) {
    neobject_destroy(neo);
}

__attribute__((noinline))
ProcessCycle* processCycle_remove_noinline(ProcessCycle* pc) {
    return processCycle_remove(pc);
}


/*
Quand un processus a terminé, le programme saute ici grâce à un bidouillage de pile :
-> Quand on alloue une pile custom, on place tout à la fin l'adresse de cette fonction, ce qui fait que cette fonction est appelée
quand eval_prolog fait son dernier return
*/

// le contexte est supposé à 8 octets, donc pas d'inlining possible
__attribute__((noinline))
NeObj eval_epilog(void) {
    static NeObj result = NEO_VOID; // le code est toujours aussi propre, mais pas de décalage intempestif de la pile
    // on se remet sur la pile normale, on restaure les registres. On récupère aussi via cette fonction le retour de eval_prolog
    
    result = reset_stack_and_registers();
    
    // on résoud la promesse et on continue l'exécution
    // le résultat est mis ici en attente, et neo_copy se chargera de vérifier si la valeur a changé ou pas
    if (*PROMISES_CNT.tab[process_cycle->process->id] != 0) { // si c'est zéro, je sais que toutes les promesses ont été détruites avant même de les utiliser
        PROMISES->tab[process_cycle->process->id] = result;
    }
    else {
        neobject_destroy_noinline(result);
    }

    PROCESS_FINISH.tab[process_cycle->process->id] = 1; // on a fini le processus, on libère la place
    

    if (process_cycle->next != process_cycle || process_cycle->prev != process_cycle) {  // il reste des processus annexes à exécuter

        // on supprime le processus, on passe au suivant            
        
        process_cycle = processCycle_remove_noinline(process_cycle);

        // on saute directement à l'exécution du processus
        __asm__ __volatile__ (
            "addq	$8,     %rsp\n\t" // on remet la pile à son état d'origine, la seule variable locale était result (c'est un pointeur)
            "mov    $0,     %rdi\n\t" // tree = NULL
            "jmp    eval_prolog\n\t"
        );
        // comme on sera sur une nouvelle pile, on retournera normalement comme si de rien n'était
    }

    // supprime définitivement ce dernier processus

    ptrlist_destroy_noinline(process_cycle->process->varsToSave, true, true);

    if (process_cycle->process->stack != NULL) {
        err_free(process_cycle->process->stack);
    }

    err_free(process_cycle->process->var_loc);

    if (process_cycle->process->original_call != NULL) {
        neobject_destroy_noinline(process_cycle->process->original_call->data);
        err_free(process_cycle->process->original_call);
    }

    err_free(process_cycle->process);
    
    process_cycle->process = NULL;
    process_cycle->prev = NULL;
    process_cycle->next = NULL;

    // renvoie la promesse associée au processus principal

    *PROMISES_CNT.tab[0] = 0;
    // récupéré de promesse sur le processus principal
    PROCESS_FINISH.tab[0] = true;
    
    result = PROMISES->tab[0];
    PROMISES->tab[0] = NEO_VOID;
    return result; // la promesse associée au processus principal
}










/*
Il faut être très prudent quand on fait des modifications à cette fonction, elle a une utilisation de la pile très contrôlée
car elle est interfacée avec de l'assembleur qui lui modifie son adresse de retour
*/
__attribute__((noinline)) // le contexte est supposé de 8 octets, donc pas d'inlining possible
NeObj eval_prolog(Tree* tree) {
    static NeObj res; // ça permet que la variable ne soit pas sur la pile
    
    if (tree == NULL)
    {        
        // on est sur la pile principale, donc dans tous les cas on peut la sauvegarder. C'est important si jamais on vient de supprimer
        // le processus qui utilisait la pile principale, sinon reset_stack_and_registers va croire qu'on est déjà sur la pile
        // principale la prochaine fois qu'il sera appelé
        switch_registers(process_cycle->process, NULL, true, process_cycle->process->stack == NULL);

        atomic_counter = ATOMIC_TIME;

        if (CODE_ERROR != 0) {
            return NEO_VOID;
        }
    }
    else {

        process_cycle->process->arg_tree = tree; // le processus recommencera avec ces arguments

        // ========= PROLOGUE ============

        LABEL_PROLOG:

        // on change de processus seulement s'il y a plus d'un processus
        if (atomic_counter == 0 && process_cycle->next != process_cycle) { // c'est le moment où l'on change de processus, juste après un appel récursif
            // on fait le travail de changement de processus

            process_cycle = nextProcess_noinline(process_cycle);

            // on est sûrs que le processus précédent dans la chaîne était vraiment le processus précédent
            // on change les registres après les avoir sauvegardés et on change de pile
            //printf("Switch from process %d to process %d\n", process_cycle->prev->process->id, process_cycle->process->id);
            switch_registers(process_cycle->process, process_cycle->prev->process, process_cycle->prev->process->stack == NULL, process_cycle->process->stack == NULL);

            // a partir de maintenant on est sur la nouvelle pile

            atomic_counter = ATOMIC_TIME;

            // on s'apprête à retourner dans un autre processus, donc on vérifie avant si le processus précédent n'avait pas déclenché une erreur
            if (CODE_ERROR != 0) {
                return NEO_VOID;
            }

        }
        else if (atomic_counter == 0) {
            atomic_counter = ATOMIC_TIME;
        }
    }

    atomic_counter--;
    
    res = eval_aux(process_cycle->process->arg_tree);

    if (IS_NEO_SPECIAL_CODE(res, -1)) { // si eval_aux retourne avec -1, c'est qu'elle veut continuer à exécuter cette expression
        goto LABEL_PROLOG;
    }

    return res;
}


// transforme une liste d'arbres en une liste de NeObj en les évaluant tous
NeList* treeToList(Tree* tree) {

    NeList* l = nelist_create(tree->nbSons);    

    for (int index = 0 ; index < tree->nbSons ; index++)
    {
        if (tree->sons[index] != NULL) {
            l->tab[index] = eval_prolog(tree->sons[index]);
            
            if (CODE_ERROR != 0)
            {
                // si y a eu un problème dans l'évaluation d'un argument, on doit libérer toute la liste créée jusqu'alors
                nelist_destroy_until(l, index - 1);
                return NULL;
            }
        }
        else {
            l->tab[index] = NEO_VOID;
        }
        
    }

    return l;
}



NeObj callUserFunc(UserFunc* fun, NeList* args, NeObj neo_local_args) {

    // ouvre un nouveau contexte pour sauvegarder les variables locales de cet appel
    newContext(process_cycle->process->var_loc);


    // on sauvegarde les "variables à sauvegarder" de ce processus avant d'en ajouter d'autres
    ptrlist* sov_vars_to_save = process_cycle->process->varsToSave->tete;

    if (fun->unlimited_arguments) {
        int len = neo_list_len(neo_local_args);

        Var var = get_var("__local_args__");

        save_later(process_cycle->process->varsToSave, var);
        local(var, process_cycle->process->var_loc);
        // on lui dit de  sauvegarder cette variable avant de switcher de processus
        replace_var(var, neo_local_args);

        if (len > 0) // du coup on enlève les éléments en trop de args
            args->len = fun->nbArgs;
    }

    
    for (int i = 0 ; i < fun->nbArgs ; i++) // affectation des arguments
    {
        NeObj temp = neo_copy(args->tab[i]);
        
        save_later(process_cycle->process->varsToSave, fun->args[i]);
        local(fun->args[i], process_cycle->process->var_loc);
        
        replace_var(fun->args[i], temp);
        
        if (CODE_ERROR != 0) // erreur : libération des arguments affectés
        {
            
            // libérer la sauvegarde des variables
            //deleteContext(process_cycle->process->var_loc);
            
            for (int j = 1 ; j < i ; j++)
                free_var(fun->args[j]);

            deleteContext(process_cycle->process->var_loc);
            partialRestore(process_cycle->process->varsToSave, sov_vars_to_save);

            return NEO_VOID;
        }
    }

    // on stocke fun et args
    
    int int_ret = exec_aux(fun->code);

    // on enlève les variables qu'on avait marquées comme "à sauvegarder"
    // on enlève une à une toutes les variables qu'on avait rajoutées
    deleteContext(process_cycle->process->var_loc);
    partialRestore(process_cycle->process->varsToSave, sov_vars_to_save);


    if (CODE_ERROR != 0) // erreur
        return NEO_VOID;
    
    if (int_ret == EXIT_RETURN) {
        NeObj sov = RETURN_VALUE;
        RETURN_VALUE = NEO_VOID; // pour dire que l'on l'a utilisé
        return sov;
    }
    else
        return neo_none_create();
}








NeObj callUserMethod(UserFunc* fun, NeObj* self, NeList* args, NeObj neo_local_args) {

    // ouvre un nouveau contexte pour sauvegarder les variables locales de cet appel
    newContext(process_cycle->process->var_loc);


    // on sauvegarde les "variables à sauvegarder" de ce processus avant d'en ajouter d'autres
    ptrlist* sov_vars_to_save = process_cycle->process->varsToSave->tete;

    if (fun->unlimited_arguments) {
        int len = neo_list_len(neo_local_args);

        Var var = get_var("__local_args__");

        save_later(process_cycle->process->varsToSave, var);
        local(var, process_cycle->process->var_loc);
        // on lui dit de  sauvegarder cette variable avant de switcher de processus
        
        replace_var(var, neo_local_args);

        if (len > 0) // du coup on enlève les éléments en trop de args
            args->len = fun->nbArgs;
    }
    
    

    
    for (int i = 0 ; i < fun->nbArgs ; i++) // affectation des arguments
    {
        NeObj temp = neo_copy(args->tab[i]);
        
        save_later(process_cycle->process->varsToSave, fun->args[i]);
        local(fun->args[i], process_cycle->process->var_loc);
        
        replace_var(fun->args[i], temp);
        
        if (CODE_ERROR != 0) // erreur : libération des arguments affectés
        {
            
            // libérer la sauvegarde des variables
            //deleteContext(process_cycle->process->var_loc);
            
            for (int j = 1 ; j < i ; j++)
                free_var(fun->args[j]);

            deleteContext(process_cycle->process->var_loc);
            partialRestore(process_cycle->process->varsToSave, sov_vars_to_save);

            return NEO_VOID;
        }
    }

    // on stocke fun et args
    
    int int_ret = exec_aux(fun->code);

    if (CODE_ERROR != 0) {
        deleteContext(process_cycle->process->var_loc); // réaffecte les anciennes valeurs des variables qui ont été mises en local
        partialRestore(process_cycle->process->varsToSave, sov_vars_to_save);
        return NEO_VOID;
    }

    // on récupère la valeur du premier argument avant de restaurer le contexte
    NeObj object = neo_copy(get_var_value(fun->args[0]));

    // et on démonte le contexte de la fonction
    deleteContext(process_cycle->process->var_loc); // réaffecte les anciennes valeurs des variables qui ont été mises en local
    // on enlève les variables qu'on avait marquées comme "à sauvegarder"
    // on enlève une à une toutes les variables qu'on avait rajoutées
    partialRestore(process_cycle->process->varsToSave, sov_vars_to_save);

    neobject_destroy(*self);
    *self = object;
    
    if (int_ret == EXIT_RETURN) {
        NeObj sov = RETURN_VALUE;
        RETURN_VALUE = NEO_VOID; // pour dire que l'on l'a utilisé
        return sov;
    }
    else
        return neo_none_create();
}





__attribute__((noinline)) // cette fonction est tellement grosse que de toute façon gcc n'aurait jamais l'idée de l'inliner
NeObj eval_aux(Tree* tree) {

    // =================== EVALUATION =====================

    LINENUMBER = tree->line;

    // disjonction de cas suivant si c'est une expression ou des instructions
    // on calcule en O(1) si notre type est une expression ou pas

    if (tree->type == TYPE_OPERATOR)
    {
        // il faut envoyer les opérandes calculées à la fonction opérateur
        if (tree->nbSons == 1) // operateur unaire
        {

            if (tree->label2 == 39) // opérateur parallel
            {

                Tree* maintree = tree->sons[0];
                
                // crée une copie de l'arbre avec une version figée de la fonction
                Tree* maintree_copy = (Tree*) err_malloc(sizeof(Tree));
                maintree_copy->type = maintree->type;
                maintree_copy->label1 = maintree->label1;
                maintree_copy->label2 = maintree->label2;
                maintree_copy->sons = maintree->sons;
                maintree_copy->nbSons = maintree->nbSons;
                maintree_copy->capacity = maintree->capacity;
                maintree_copy->data = eval_prolog(maintree->sons[0]);

                if (CODE_ERROR != 0) {
                    err_free(maintree_copy);
                    return NEO_VOID;
                }
                if (NEO_TYPE(maintree_copy->data) != TYPE_USERFUNC) {
                    CODE_ERROR = 100;
                    neobject_destroy(maintree_copy->data);
                    err_free(maintree_copy);
                    return NEO_VOID;
                }

                // on ajoute le processus, et il va se faire exécuter dans la chaine de processus
                int id = create_new_process(maintree_copy, true);

                return neo_promise_create(id);
            }


            // opérateur paresseux
            else if (OPERANDES.tab[tree->label2] & LAZY) {
                NeObj (*func)(Tree*) = OPFONC[tree->label2];
                return func(tree->sons[0]);
            }
            else {
                // opérateur qui prend directement des objets/adresses en argument

                if (OPERANDES.tab[tree->label2] & VARRIGHT || OPERANDES.tab[tree->label2] & VARLEFT)
                {
                    NeObj* op1 = get_address(tree->sons[0]); // si la grammaire stipule que l'opérateur doit recevoir une variable et non une valeur
                    
                    if (CODE_ERROR != 0) {
                        return NEO_VOID;
                    }

                    NeObj (*func) (NeObj*) = OPFONC[tree->label2];
                    return func(op1);
                }
                else
                {
                    NeObj op1 = eval_prolog(tree->sons[0]);

                    if (CODE_ERROR != 0) {
                        return NEO_VOID;
                    }

                    NeObj (*func) (NeObj) = OPFONC[tree->label2];
                    NeObj retour = func(op1);
                    neobject_destroy(op1);
                    return retour;

                }
            }
        }
        else if (tree->nbSons == 2) // operateur binaire
        {
            // opérateur paresseux
            if (OPERANDES.tab[tree->label2] & LAZY) {
                NeObj (*func) (Tree*, Tree*) = OPFONC[tree->label2];
                return func(tree->sons[0], tree->sons[1]);
            }
            else {
                // opérateur normal qui prend des objets en arguments

                if (tree->label2 == 37) { // opérateur :=
                    CODE_ERROR = 95;
                    return NEO_VOID;
                } else if (tree->label2 == 35) { // opérateur :
                    CODE_ERROR = 92;
                    return NEO_VOID;
                }
                                    
                if (OPERANDES.tab[tree->label2] & VAR_RIGHT)
                {
                    NeObj* op1 = get_address(tree->sons[0]);

                    if (CODE_ERROR != 0)
                        return NEO_VOID;
                    
                    NeObj op2 = eval_prolog(tree->sons[1]);

                    if (CODE_ERROR != 0)
                        return NEO_VOID;

                    NeObj (*func)(NeObj*, NeObj) = OPFONC[tree->label2];
                    NeObj result = func(op1, op2);
                    neobject_destroy(op2);
                    return result;
                }
                else if (OPERANDES.tab[tree->label2] & LEFT_VAR)
                {                    
                    NeObj op1 = eval_prolog(tree->sons[0]);
                    
                    if (CODE_ERROR != 0)
                        return NEO_VOID;
                
                    NeObj* op2 = get_address(tree->sons[1]);

                    if (CODE_ERROR != 0)
                        return NEO_VOID;

                    NeObj (*func)(NeObj, NeObj*) = OPFONC[tree->label2];
                    NeObj result = func(op1, op2);
                    neobject_destroy(op1);
                    return result;
                }
                else if (OPERANDES.tab[tree->label2] & VAR_VAR)
                {
                    NeObj* op1 = get_address(tree->sons[0]);

                    if (CODE_ERROR != 0)
                        return NEO_VOID;
                    
                    NeObj* op2 = get_address(tree->sons[1]);

                    if (CODE_ERROR != 0)
                        return NEO_VOID;

                    NeObj (*func)(NeObj*, NeObj*) = OPFONC[tree->label2];
                    NeObj result = func(op1, op2);
                    return result;
                }
                else
                {
                    NeObj op1 = eval_prolog(tree->sons[0]);

                    if (CODE_ERROR != 0)
                    {
                        neobject_destroy(op1);
                        return NEO_VOID;
                    }

                    NeObj op2 = eval_prolog(tree->sons[1]);

                    if (CODE_ERROR != 0)
                    {
                        neobject_destroy(op1);
                        neobject_destroy(op2);
                        return NEO_VOID;
                    }
                    NeObj (*func)(NeObj, NeObj) = OPFONC[tree->label2];
                    NeObj result = func(op1, op2);

                    neobject_destroy(op1);
                    neobject_destroy(op2);

                    return result;

                }
            }
        }
    }
    
    else if (tree->type == TYPE_LIST)
    {
        // donc les enfants de tree sont les éléments de la liste
        NeList* l = treeToList(tree);


        if (CODE_ERROR != 0) {                   
            return NEO_VOID;
        }

        return neo_list_convert(l);
    }
    
    else if (tree->type == TYPE_FONCTION)
    {

        // ici, appel récursif pour évaluer la fonction (1er fils), et on la met dans ->data de l'arbre


        // si le champ data de la fonction est déjà occuppé, on n'a pas besoin d'évaluer la fonction
        if (neo_is_void(tree->data)) {
            tree->data = eval_prolog(tree->sons[0]);

            if (CODE_ERROR != 0) { // erreur lors de l'évaluation de la fonction
                return NEO_VOID;
            }
        }

        // ensuite, pour évaluer les arguments, on fait bien la boucle sur tree->sons[1]
        
        if (NEO_TYPE(tree->data) == TYPE_FONCTION)
        {
            NeObj tree_data_sov = tree->data;
            Function* fun = neo_to_function(tree->data);

            tree->data = NEO_VOID;
            NeList* args = treeToList(tree->sons[1]);
            tree->data = tree_data_sov;

            if (CODE_ERROR != 0) {
                neobject_destroy(tree->data);
                tree->data = NEO_VOID;
                return NEO_VOID;
            }            
            
    
            if (!funcArgsCheck(fun, args))
            {
                CODE_ERROR = 14;
                nelist_destroy(args);
                neobject_destroy(tree->data); // on supprime la fonction que l'on vient de créer
                tree->data = NEO_VOID;
                return NEO_VOID;
            }
                
            NeObj retour = functionCall(tree->data, args);

            nelist_destroy(args);
            neobject_destroy(tree->data); // on supprime la fonction que l'on vient de créer
            tree->data = NEO_VOID;
            
            if (CODE_ERROR != 0) {
                neobject_destroy(retour);
                return NEO_VOID;
            }

            LINENUMBER = tree->line; // certaines fonctions modifient le numéro de ligne

            return retour;
        }

        else if (NEO_TYPE(tree->data) == TYPE_USERFUNC || NEO_TYPE(tree->data) == TYPE_USERMETHOD)
        {
            UserFunc* fun = neo_to_userfunc(tree->data);

            NeObj tree_data_sov = tree->data;

            if (tree->sons[1]->nbSons > fun->nbArgs && ! fun->unlimited_arguments)
            {
                CODE_ERROR = 6;
                neobject_destroy(tree->data); // on supprime la fonction que l'on vient de créer
                tree->data = NEO_VOID;
                return NEO_VOID;
            }

            

            // on compte le nombre d'arguments très facultatifs
            int given_nbOptArgs = 0;

            for (int i = 0 ; i < tree->sons[1]->nbSons ; i++) {
                if (tree->sons[1]->sons[i]->type == TYPE_OPERATOR && tree->sons[1]->sons[i]->label2 == 37) { // on est sur du :=
                    int index = 0;
                    while (index < fun->nbArgs && fun->args[index] != tree->sons[1]->sons[i]->sons[0]->variable) index++;

                    if (index >= fun->nbArgs - fun->nbOptArgs && index < fun->nbArgs)
                        given_nbOptArgs ++;
                }
            }

            
            // on sait maintenant que le nombre d'arguments réel final est :
            // -> Si tree->nbSons > fun->nbArgs : tree->nbSons - given_nbOptArgs
            // -> Sinon, fun->nbArgs



            // calcule le nombre d'arguments de cet appel de fonction
            int nb_arguments = (fun->unlimited_arguments && tree->sons[1]->nbSons > fun->nbArgs - fun->nbOptArgs) ? tree->sons[1]->nbSons + fun->nbOptArgs - given_nbOptArgs : fun->nbArgs;
            
            // et on crée la liste qui va contenir ces arguments
            NeList* arguments = nelist_create(nb_arguments);

            // initialise la liste avec des NEO_VOID pour savoir facilement si une case est encore vierge
            for (int i = 0 ; i < arguments->len ; i++)
                arguments->tab[i] = NEO_VOID;

            
            // pour les méthodes, on leur envoie l'adresse de l'objet à modifier
            NeObj* self = NULL;


            // on peut commencer par mettre à leur valeur par défaut les variables définies après ...
            // Ainsi elles ne seront pas remplacés sauf si on l'indique explicitement
            for (int i = fun->nbArgs - fun->nbOptArgs ; i < fun -> nbArgs ; i++) {
                if (i == 0 && NEO_TYPE(tree->data) == TYPE_USERMETHOD) {
                    CODE_ERROR = 110;
                    nelist_destroy(arguments);
                    neobject_destroy(tree->data);
                    tree->data = NEO_VOID;
                    return NEO_VOID;
                }
                arguments->tab[i] = neo_copy(fun->opt_args->tab[i]);
            }


            // 1ère boucle pour placer les arguments à des endroits où ils ne bougeront pas
            for (int i = 0 ; i < tree->sons[1]->nbSons ; i++) {
                // on commence par remplir les éléments donnés dans le mauvais ordre (avec :=)
                if (tree->sons[1]->sons[i]->type == TYPE_OPERATOR && tree->sons[1]->sons[i]->label2 == 37) { // on est sur du :=
                    // on regarde la variable de gauche, et on met le truc au bon endroit
                    // on récupère l'index du vrai argument dans fun->args, et on le met au bon endroit dans arguments
                    int index = 0;

                    if (tree->sons[1]->sons[i]->sons[0]->type != TYPE_VARIABLE) {
                        nelist_destroy(arguments);
                        neobject_destroy(tree->data); // on supprime la fonction que l'on vient de créer
                        tree->data = NEO_VOID;
                        CODE_ERROR = 93;
                        return NEO_VOID;
                    }

                    while (index < fun->nbArgs && fun->args[index] != tree->sons[1]->sons[i]->sons[0]->variable) index++;

                    if (index == fun->nbArgs) {
                        nelist_destroy(arguments);
                        neobject_destroy(tree->data); // on supprime la fonction que l'on vient de créer
                        tree->data = NEO_VOID;
                        CODE_ERROR = 93;
                        return NEO_VOID;
                    }
                    
                    if (index < fun->nbArgs - fun->nbOptArgs && !neo_is_void(arguments->tab[index])) {
                        nelist_destroy(arguments);
                        neobject_destroy(tree->data); // on supprime la fonction que l'on vient de créer
                        tree->data = NEO_VOID;
                        CODE_ERROR = 94;
                        return NEO_VOID;
                    }
                    // et on évalue l'argument envoyé
                    tree->data = NEO_VOID;
                    arguments->tab[index] = eval_prolog(tree->sons[1]->sons[i]->sons[1]);
                    tree->data = tree_data_sov;

                    if (CODE_ERROR != 0) {
                        nelist_destroy(arguments);
                        neobject_destroy(tree->data); // on supprime la fonction que l'on vient de créer
                        tree->data = NEO_VOID;
                        return NEO_VOID;
                    }
                    else if (index == 0 && NEO_TYPE(tree->data) == TYPE_USERMETHOD) {
                        self = get_address(tree->sons[1]->sons[i]->sons[1]); // on vient d'évaluer le premier argument
                    }
                }
            }

            // deuxième boucle pour insérer dans l'ordre et en évitant les := les arguments restants
            int index = 0; // on remplit petit à petit arguments avec les arguments restants
            while (index < arguments->len && !neo_is_void(arguments->tab[index])) index++;

            for (int i = 0 ; index < arguments->len && i < tree->sons[1]->nbSons ; i++) {
                
                if (tree->sons[1]->sons[i]->type != TYPE_OPERATOR || tree->sons[1]->sons[i]->label2 != 37) {
                    tree->data = NEO_VOID;
                    arguments->tab[index] = eval_prolog(tree->sons[1]->sons[i]);
                    tree->data = tree_data_sov;

                    if (CODE_ERROR != 0) {
                        nelist_destroy(arguments);
                        neobject_destroy(tree->data); // on supprime la fonction que l'on vient de créer
                        tree->data = NEO_VOID;
                        return NEO_VOID;
                    }
                    else if (index == 0 && NEO_TYPE(tree->data) == TYPE_USERMETHOD) { // on vient d'évaluer le premier argument
                        self = get_address(tree->sons[1]->sons[i]);
                    }

                    while (index < arguments->len && !neo_is_void(arguments->tab[index])) index++;
                }
            }
            // c'est un choix philosophique : si f attend x, y facultatif et z, alors f(1,2) ne fonctionnera pas car il manque z
            // pour mettre uniquement le x et le z, il y a une syntaxe pour le faire

            if (tree->sons[1]->nbSons < fun->nbArgs) {
                // à ce stade-là, certains champs de arguments sont encore vides, on va donc parcourir une dernière fois afin de lui associer les expressions définies lors de la définition de la fonction
                for (int i = 0 ; i < fun->nbArgs ; i++) {
                    if (neo_is_void(arguments->tab[i])) { // c'est précisément le cas où l'utilisateur n'a rien mis
                        arguments->tab[i] = neo_copy(fun->opt_args->tab[i]);

                        if (neo_is_void(arguments->tab[i])) {
                            CODE_ERROR = 7;
                            neobject_destroy(tree->data); // on supprime la fonction que l'on vient de créer
                            tree->data = NEO_VOID;
                            nelist_destroy(arguments);
                            return NEO_VOID;
                        }

                        if (i == 0) { // le premier argument d'une méthode ne peut pas être un argument optionnel
                            CODE_ERROR = 110;
                            nelist_destroy(arguments);
                            neobject_destroy(tree->data);
                            tree->data = NEO_VOID;
                            return NEO_VOID;
                        }
                    }
                }
            }


            // création de la liste local_args
            NeObj neo_local_args = NEO_VOID;
            if (fun->unlimited_arguments) {
                // on raccourcit args, et on crée la liste locale
                int len = (arguments->len - given_nbOptArgs > fun->nbArgs - fun->nbOptArgs) ? arguments->len - fun->nbArgs : 0;

                NeList* local_args = nelist_create(len);
                for (int i = 0 ; i < len ; i++) {
                    local_args->tab[i] = arguments->tab[fun->nbArgs + i];
                }
                neo_local_args = neo_list_convert(local_args);
            }
            

            // exécution de la fonction

            tree->data = NEO_VOID;

            NeObj ret;
            if (NEO_TYPE(tree_data_sov) == TYPE_USERMETHOD)
                ret = callUserMethod(fun, self, arguments, neo_local_args);
            else
                ret = callUserFunc(fun, arguments, neo_local_args);

            tree->data = tree_data_sov;
            neobject_destroy(tree->data);
            tree->data = NEO_VOID;

            nelist_destroy(arguments);

            if (CODE_ERROR != 0) {
                return NEO_VOID;
            }

            // ainsi si une return_value n'est pas à NULL c'est que forcément la valeur n'a pas été récupérée
            return ret;
            
        }

        else if (NEO_TYPE(tree->data) == TYPE_EMPTY)
        {
            CODE_ERROR = 8;
            neobject_destroy(tree->data);
            tree->data = NEO_VOID;
            return NEO_VOID;
        }
        else
        {
            CODE_ERROR = 9;
            neobject_destroy(tree->data);
            tree->data = NEO_VOID;
            return NEO_VOID;
        }
        
    }
    
    else if (tree->type == TYPE_LISTINDEX)
    {
        NeObj obj = *get_address(tree->sons[0]);

        if (CODE_ERROR != 0)
            return NEO_VOID;

        NeObj index = eval_prolog(tree->sons[1]);

        if (CODE_ERROR != 0)
            return NEO_VOID;


        // on vérifie que nos objets ont bien les types attendus
        if (NEO_TYPE(obj) != TYPE_LIST && NEO_TYPE(obj) != TYPE_STRING)
        {
            CODE_ERROR = 15;
            return NEO_VOID;
        }

        if (NEO_TYPE(index) != TYPE_INTEGER)
        {
            neobject_destroy(index);
            CODE_ERROR = 16;
            return NEO_VOID;
        }
        
        // on récupère le véritable index
        long int index2 = neo_to_integer(index);
        neobject_destroy(index);
        
        // vérifications supplémentaires
        if (NEO_TYPE(obj) == TYPE_EMPTY)
        {
            CODE_ERROR = 17;
            return NEO_VOID;
        }
        else if ((NEO_TYPE(obj) == TYPE_LIST && index2 >= neo_list_len(obj)) || index2 < 0 || (NEO_TYPE(obj) == TYPE_STRING && index2 >= strlen(neo_to_string(obj))))
        {
            CODE_ERROR = 18;
            return NEO_VOID;
        }
        
        
        NeObj retour = NEO_VOID;

        if (NEO_TYPE(obj) == TYPE_LIST) {
            retour = neo_copy(neo_list_nth(obj,index2));
        }
        else {
            retour = neo_str_create(charToString(neo_to_string(obj)[index2]));
        }

        return retour;

    }

    else if (tree->type == TYPE_CONTAINER)
    {

        NeList* val = nelist_create(tree->nbSons);

        for (int ext_index = 0 ; ext_index < tree->nbSons ; ext_index++)
        {
            //stack_check_for(varstack, 4);

            val->tab[ext_index] = eval_prolog(tree->sons[ext_index]->sons[0]);

            if (CODE_ERROR != 0)
            {
                nelist_destroy_until(val, ext_index - 1);
                return NEO_VOID;
            }
        }
        NeObj ret = neo_container_create(tree->label2, val);
        
        return ret;
    }
    
    else if (tree->type == TYPE_ATTRIBUTE)
    {

        NeObj neo = eval_prolog(tree->sons[0]);

        if (CODE_ERROR != 0) {
            return NEO_VOID;
        }

        if (NEO_TYPE(neo) != TYPE_CONTAINER)
        {
            // Erreur : essaie d'accéder à un champ d'une variable qui n'est pas un container
            neobject_destroy(neo);
            CODE_ERROR = 80;
            return NEO_VOID;
        }


        // va chercher la valeur
        NeObj ret;

        Container* c = neo_to_container(neo);

        int index = 0;

        if (tree->label2 == -1) // ça veut dire qu'on sait exactement où chercher la valeur
        {
            tree->label2 = get_field_index(c, tree->label1);
            err_free(tree->label1);
            tree->label1 = NULL;
        }


        ret = neo_copy(get_container_field(c, tree->label2));
        neobject_destroy(neo);
        return ret;
    }

    else if (tree->type == TYPE_AWAIT)
    {

        if (tree->nbSons > 1)
        {
            CODE_ERROR = 101;
            return NEO_VOID;
        }


        NeObj condition = eval_prolog(tree->sons[0]);

        if (CODE_ERROR != 0) {
            neobject_destroy(condition);
            return NEO_VOID;
        }

        if (neoIsTrue(condition)) { // fin de l'attente passive
            neobject_destroy(condition);
        }
        else
        {
            neobject_destroy(condition);
            
            if (atomic_counter < 0) {
                // on est dans un bloc atomique donc on peut pas passer au processus suivant
                // à cause du -1, on va retourner exécuter l'arbre d'avant cet appel, donc on restaure les arguments
                process_cycle->process->arg_tree = tree;
                return NEO_SPECIAL(-1);
            }
            else {
                atomic_counter = 0;
                // à cause du -1, on va retourner exécuter l'arbre d'avant cet appel, donc on restaure les arguments
                process_cycle->process->arg_tree = tree;
                return NEO_SPECIAL(-1); // on passe au processus suivant, et eval va enregistrer
                // tree donc la prochaine fois que ça sera notre tour, on
                // va revenir à AWAIT comme si de rien n'était
            }
        }
        return neo_none_create();
    }

    else if (tree->type == TYPE_VARIABLE)
    {
        NeObj value = get_var_value(tree->variable);

        if (NEO_TYPE(value) == TYPE_EMPTY) {
            CODE_ERROR = 5;
            return NEO_VOID;
        }

        return neo_copy(value);
    }

    // quand ça sera de nouveau un switch, on mettra les 4 case au dessus de return neo_copy(tree->data)
    else if (tree->type == TYPE_INTEGER)
    {
        return neo_copy(tree->data);
    }

    else if (tree->type == TYPE_DOUBLE)
    {
        return neo_copy(tree->data);
    }

    else if (tree->type == TYPE_STRING)
    {
        return neo_copy(tree->data);
    }
    
    else if (tree->type == TYPE_BOOL)
    {
        return neo_copy(tree->data);
    }

    else if (tree->type == TYPE_EXCEPTION)
    {
        return neo_copy(tree->data);
    }
    
    else if (tree->type == TYPE_NONE)
    {
        return neo_copy(tree->data);
    }

    CODE_ERROR = 19;
    return NEO_VOID;
}












// cette fonction renvoie l'adresse permettant d'effectuer une affectation sur l'objet pointé
NeObj* get_address(Tree* tree) {

    if (tree->type == TYPE_VARIABLE) {
        NeObj* addr = get_absolute_address(tree->variable);
        return addr;
    }

    else if (tree->type == TYPE_LISTINDEX) {
        NeObj obj = *get_address(tree->sons[0]);

        if (CODE_ERROR != 0)
        {
            return NULL;
        }


        if (CODE_ERROR != 0) {
            return NULL;
        }


        NeObj index = eval_prolog(tree->sons[1]);


        if (CODE_ERROR != 0) {
            return NULL;
        }


        if (NEO_TYPE(obj) != TYPE_LIST && NEO_TYPE(obj) != TYPE_STRING)
        {
            CODE_ERROR = 15;
            return NULL;
        }

        
        if (NEO_TYPE(index) != TYPE_INTEGER)
        {
            neobject_destroy(index);
            CODE_ERROR = 16;
            return NULL;
        }
        
        long int index2 = neo_to_integer(index);
        neobject_destroy(index);
        

        if (NEO_TYPE(obj) == TYPE_EMPTY)
        {
            CODE_ERROR = 17;
            return NULL;
        }
        else if ((NEO_TYPE(obj) == TYPE_LIST && index2 >= neo_list_len(obj)) || index2 < 0 || (NEO_TYPE(obj) == TYPE_STRING && index2 >= strlen(neo_to_string(obj))))
        {
            CODE_ERROR = 18;
            return NULL;
        }
        
        
        if (NEO_TYPE(obj) == TYPE_LIST)
        {
            return nelist_nth_addr(neo_to_list(obj), index2);
        }
        else {
            CODE_ERROR = 105;
            return NULL;
        }
    }

    else if (tree->type == TYPE_ATTRIBUTE) {

        NeObj neo = eval_prolog(tree->sons[0]);

        if (CODE_ERROR != 0) {
            return NULL;
        }

        if (NEO_TYPE(neo) != TYPE_CONTAINER)
        {
            // Erreur : essaie d'accéder à un champ d'une variable qui n'est pas un container
            neobject_destroy(neo);
            CODE_ERROR = 80;
            return NULL;
        }


        // va chercher la valeur
        NeObj* ret;

        Container* c = neo_to_container(neo);

        int index = 0;

        if (tree->label2 == -1) // ça veut dire qu'on sait exactement où chercher la valeur
        {
            tree->label2 = get_field_index(c, tree->label1);
            err_free(tree->label1);
            tree->label1 = NULL;
        }


        ret = get_container_field_addr(c, tree->label2);
        neobject_destroy(neo);
        return ret;
    }

    CODE_ERROR = 89;
    return NULL;
}
























int execConditionBlock(Tree* maintree) {
    intptr_t int_ret = 0;
    int bloc = 0;
    NeObj expr;

    // on garde un pointeur vers le début de la liste quand on est arrivés
    ptrlist* sov_vars_to_save = process_cycle->process->varsToSave->tete;

    // on a comme variables locales : int_ret, bloc, inst : d'ailleurs inst doit être empilé partout

    while (bloc < maintree->nbSons)
    {
        expr = eval_prolog(maintree->sons[bloc]->sons[0]);


        if (CODE_ERROR != 0) {
            return 0;
        }

        bool cond = neoIsTrue(expr);
        neobject_destroy(expr);

        if (CODE_ERROR != 0) {
            return 0;
        }

        if (cond)
        {
            // on s'apprete à exécuter du code, on peut donc ouvrir un nouveau contexte
            newContext(process_cycle->process->var_loc);

            int_ret = exec_aux(maintree->sons[bloc]->sons[1]);

            deleteContext(process_cycle->process->var_loc); // on vient d'exécuter le code, donc on a fini le if

            
            if (CODE_ERROR != 0) {
                return 0;
            }

            if (int_ret != 0) {
                return int_ret;
            }
            
            
            bloc++;
            while (bloc < maintree->nbSons && maintree->sons[bloc]->type != TYPE_STATEMENTIF)
                bloc++;

            // on vient d'exécuter le if, donc on a fini ce if, on peut supprimer le contexte

            if (bloc == maintree->nbSons) {
                break;
            }
            else
                continue;
        }
        
        bloc++;


        bool elif = false;
        while (bloc < maintree->nbSons && maintree->sons[bloc]->type == TYPE_STATEMENTELIF)
        {

            expr = eval_prolog(maintree->sons[bloc]->sons[0]);

            cond = neoIsTrue(expr);
            neobject_destroy(expr);

            if (CODE_ERROR != 0) {
                return 0;
            }


            if (!elif && cond)
            {
                // on s'apprete à exécuter du code, on peut donc ouvrir un nouveau contexte
                newContext(process_cycle->process->var_loc);

                int_ret = exec_aux(maintree->sons[bloc]->sons[1]);

                deleteContext(process_cycle->process->var_loc); // on vient d'exécuter le code, donc on a fini le if


                if (CODE_ERROR != 0) {
                    return 0;
                }

                if (int_ret != 0) {
                    return int_ret;
                }

                
                elif = true;
            }
            bloc++;
        }


        if (bloc < maintree->nbSons && maintree->sons[bloc]->type == TYPE_STATEMENTELSE) // s'il y a un bloc else
        {
            if (!elif) // si on a le droit d'y aller
            {
                newContext(process_cycle->process->var_loc);

                int_ret = exec_aux(maintree->sons[bloc]);

                deleteContext(process_cycle->process->var_loc); // on vient d'exécuter le code, donc on a fini le if

                if (CODE_ERROR != 0) {
                    return 0;
                }

                if (int_ret != 0) {
                    return int_ret;
                }

            }
            bloc++;
        }
    }

    // on enlève les variables qu'on avait marquées comme "à sauvegarder"
    // on enlève une à une toutes les variables qu'on avait rajoutées

    partialRestore(process_cycle->process->varsToSave, sov_vars_to_save);

    return 0;
}




int execStatementFor(Tree* tree) {

    // on évalue l'incrément de la boucle
    int incr = 0;
    NeObj start;
    NeObj tempMax;

    // for(var, start, end, step)
    if (tree->sons[0]->nbSons == 4) {
        NeObj step = eval_prolog(tree->sons[0]->sons[3]);

        if (CODE_ERROR != 0)
            return 0;

        if (NEO_TYPE(step) != TYPE_INTEGER) {
            neobject_destroy(step);
            CODE_ERROR = 108;
            return 0;
        }

        incr = neo_to_integer(step);
        neobject_destroy(step);

        // on évalue la valeur de départ de la boucle
        start = eval_prolog(tree->sons[0]->sons[1]);

        if (CODE_ERROR != 0)
            return 0;

        tempMax = eval_prolog(tree->sons[0]->sons[2]);
    }

    // for(var, start, end)
    else if (tree->sons[0]->nbSons == 3) {
        incr = 1;
        // on évalue la valeur de départ de la boucle
        start = eval_prolog(tree->sons[0]->sons[1]);

        if (CODE_ERROR != 0)
            return 0;

        tempMax = eval_prolog(tree->sons[0]->sons[2]);
    }

    // for(var, end)
    else if (tree->sons[0]->nbSons == 2) {
        incr = 1;
        start = neo_integer_create(0);

        if (CODE_ERROR != 0)
            return 0;

        tempMax = eval_prolog(tree->sons[0]->sons[1]);
    }
    else {
        CODE_ERROR = 108;
        return 0;
    }

    if (CODE_ERROR != 0) { // il y a eu une erreur lors de l'évaluation du tempMax
        neobject_destroy(start);
        return 0;
    }
    
    if (NEO_TYPE(start) != TYPE_INTEGER || NEO_TYPE(tempMax) != TYPE_INTEGER)
    {
        neobject_destroy(start);
        neobject_destroy(tempMax);
        CODE_ERROR = 10;
        return 0;
    }

    // le vrai indice de la boucle sera un entier
    int ext_index = neo_to_integer(start);
    neobject_destroy(start);


    // on récupère le variant de boucle
    if (tree->sons[0]->sons[0]->type != TYPE_VARIABLE) {
        CODE_ERROR = 111;
        neobject_destroy(tempMax);
        return 0;
    }
    
    Var var = tree->sons[0]->sons[0]->variable; // variable à incrémenter lors de la boucle

    // ouverture du nouveau contexte, sauvegarde du variant
    newContext(process_cycle->process->var_loc); // nouveau contexte pour rendre des variables locales à la boucle for
    ptrlist* sov_vars_to_save = process_cycle->process->varsToSave->tete; // pour restaurer l'ancienne liste des variables à sauvegarder, une fois qu'on aura fini

    save_later(process_cycle->process->varsToSave, var);
    local(var, process_cycle->process->var_loc); // on localise l'indice de la boucle


    
    int max = neo_to_integer(tempMax); // borne supérieure des valeurs atteintes par la variable
    neobject_destroy(tempMax); // du coup le data, on le garde donc on libère que l'enveloppe

    intptr_t int_ret = 0;
    
    while (ext_index < max)
    {
        // on restaure la valeur de l'indice au début du corps de la boucle
        replace_var(var, neo_integer_create(ext_index));

        int_ret = exec_aux(tree->sons[1]);

        if (CODE_ERROR != 0)
        {
            deleteContext(process_cycle->process->var_loc);
            partialRestore(process_cycle->process->varsToSave, sov_vars_to_save);
            return 0;
        }

        if (int_ret != 0 && int_ret != CONTINUE)
            break;
        
        ext_index += incr;
    }

    // la variable représentant le variant de boucle a été supprimée à la fin de la dernière
    // itération, et si on la laisse telle quelle, elle va se refaire supprimer lors du delete context
    // ce qui se passe : le newContext nous offre une nouvelle variable libre pour faire nos trucs
    // a chaque itération, on crée l'objet et on le supprime
    // à la fin de la boucle, on a donc un bilan neutre, on peut donc reset la variable pour qu'elle ne se
    // refasse pas supprimer
    // conclusion : quand on delete un context, il faut faire attention :
    // -> soit si on a un bilan neutre sur la variable, de la vider de ses pointeurs
    // -> sinon, si il faut la supprimer, on la laiss telle quelle et le deleteContext va la supprimer


    deleteContext(process_cycle->process->var_loc);

    // on enlève les variables qu'on avait marquées comme "à sauvegarder"
    // on enlève une à une toutes les variables qu'on avait rajoutées
    partialRestore(process_cycle->process->varsToSave, sov_vars_to_save);

    if (int_ret != BREAK)
        return int_ret;
    else
        return 0;
}




int execStatementForeachList(Tree* tree, NeObj neo_list) {

    NeList* list = neo_to_list(neo_list);

    // on récupère le variant de boucle
    if (tree->sons[0]->sons[0]->type != TYPE_VARIABLE) {
        CODE_ERROR = 111;
        return 0;
    }

    Var var = tree->sons[0]->sons[0]->variable; // variable à incrémenter lors de la boucle

    newContext(process_cycle->process->var_loc); // nouveau contexte pour rendre des variables locales à la boucle for
    ptrlist* sov_vars_to_save = process_cycle->process->varsToSave->tete; // pour restaurer l'ancienne liste des variables à sauvegarder, une fois qu'on aura fini

    save_later(process_cycle->process->varsToSave, var);
    local(var, process_cycle->process->var_loc); // on localise l'indice de la boucle


    // l'indice qui va parcourir la liste
    intptr_t ext_index = 0;

    intptr_t int_ret = 0;
    
    while (ext_index < list->len)
    {
        // on restaure la valeur de l'indice au début du corps de la boucle
        replace_var(var, neo_copy(list->tab[ext_index]));

        int_ret = exec_aux(tree->sons[1]);

        if (CODE_ERROR != 0)
        {
            deleteContext(process_cycle->process->var_loc);
            partialRestore(process_cycle->process->varsToSave, sov_vars_to_save);
            return 0;
        }

        if (int_ret != 0 && int_ret != CONTINUE)
            break;
        
        ext_index++;
    }

    // la variable représentant le variant de boucle a été supprimée à la fin de la dernière
    // itération, et si on la laisse telle quelle, elle va se refaire supprimer lors du delete context
    // ce qui se passe : le newContext nous offre une nouvelle variable libre pour faire nos trucs
    // a chaque itération, on crée l'objet et on le supprime
    // à la fin de la boucle, on a donc un bilan neutre, on peut donc reset la variable pour qu'elle ne se
    // refasse pas supprimer
    // conclusion : quand on delete un context, il faut faire attention :
    // -> soit si on a un bilan neutre sur la variable, de la vider de ses pointeurs
    // -> sinon, si il faut la supprimer, on la laiss telle quelle et le deleteContext va la supprimer

    deleteContext(process_cycle->process->var_loc);

    // on enlève les variables qu'on avait marquées comme "à sauvegarder"
    // on enlève une à une toutes les variables qu'on avait rajoutées
    partialRestore(process_cycle->process->varsToSave, sov_vars_to_save);

    if (int_ret != BREAK)
        return int_ret;
    else
        return 0;
}



int execStatementForeachString(Tree* tree, NeObj neo_string) {

    char* string = neo_to_string(neo_string);
    int string_length = strlen(string);

    // on récupère le variant de boucle
    if (tree->sons[0]->sons[0]->type != TYPE_VARIABLE) {
        CODE_ERROR = 111;
        return 0;
    }

    Var var = tree->sons[0]->sons[0]->variable; // variable à incrémenter lors de la boucle

    newContext(process_cycle->process->var_loc); // nouveau contexte pour rendre des variables locales à la boucle for
    ptrlist* sov_vars_to_save = process_cycle->process->varsToSave->tete; // pour restaurer l'ancienne liste des variables à sauvegarder, une fois qu'on aura fini

    save_later(process_cycle->process->varsToSave, var);
    local(var, process_cycle->process->var_loc); // on localise l'indice de la boucle



    // l'indice qui va parcourir la liste
    intptr_t ext_index = 0;

    intptr_t int_ret = 0;
    
    while (ext_index < string_length)
    {
        // on restaure la valeur de l'indice au début du corps de la boucle
        replace_var(var, neo_str_create(charToString(string[ext_index])));

        int_ret = exec_aux(tree->sons[1]);

        if (CODE_ERROR != 0)
        {
            deleteContext(process_cycle->process->var_loc);
            partialRestore(process_cycle->process->varsToSave, sov_vars_to_save);
            return 0;
        }

        if (int_ret != 0 && int_ret != CONTINUE)
            break;
        
        ext_index++;
    }

    // la variable représentant le variant de boucle a été supprimée à la fin de la dernière
    // itération, et si on la laisse telle quelle, elle va se refaire supprimer lors du delete context
    // ce qui se passe : le newContext nous offre une nouvelle variable libre pour faire nos trucs
    // a chaque itération, on crée l'objet et on le supprime
    // à la fin de la boucle, on a donc un bilan neutre, on peut donc reset la variable pour qu'elle ne se
    // refasse pas supprimer
    // conclusion : quand on delete un context, il faut faire attention :
    // -> soit si on a un bilan neutre sur la variable, de la vider de ses pointeurs
    // -> sinon, si il faut la supprimer, on la laiss telle quelle et le deleteContext va la supprimer

    deleteContext(process_cycle->process->var_loc);

    // on enlève les variables qu'on avait marquées comme "à sauvegarder"
    // on enlève une à une toutes les variables qu'on avait rajoutées
    partialRestore(process_cycle->process->varsToSave, sov_vars_to_save);

    if (int_ret != BREAK)
        return int_ret;
    else
        return 0;
}





int exec_aux(Tree* tree) {

    for (int inst=0 ; inst < tree->nbSons ; inst++)
    {
        LINENUMBER = tree->sons[inst]->line;

        if (tree->sons[inst]->type == TYPE_TRYEXCEPT)
        {

            // trucs qu'il y a à sauvegarder ici : inst
            //stack_check_for(varstack, 3);
            
            intptr_t int_ret = exec_aux(tree->sons[inst]->sons[0]);

            if (int_ret != EXIT_SUCCESS)
                return int_ret;

            Tree* maintree = tree->sons[inst];

            // exécution du try

            //printf("ERRREUR : %d\n", CODE_ERROR);

            if (CODE_ERROR != 1 && CODE_ERROR != 0) // exit() n'est pas considéré comme une erreur
            {
                // on va parcourir la liste des exceptions jusqu'à en trouver une qui corresponde
                int bo = 0; // va correspondre à l'indice du sous arbre que l'on va exécuter

                // parcours des différents blocs except pour trouver le bon
                for (int fils = 1 ; fils < maintree->nbSons && bo == 0 ; fils++) { // on arrête dès que bo correspond à un vrai sous arbre except

                    if (maintree->sons[fils]->sons[0]->nbSons == 0) {
                        bo = fils; // si le bloc except ne spécifie aucune exception, on peut directement l'exécuter
                    }

                    for (int i = 0 ; i < maintree->sons[fils]->sons[0]->nbSons ; i++)
                    {
                        NeObj obj = maintree->sons[fils]->sons[0]->sons[i]->data;

                        if (NEO_TYPE(obj) != TYPE_EXCEPTION)
                        {
                            CODE_ERROR = 78;
                            return int_ret;
                        }
                        else
                        {
                            int code = get_exception_code(maintree->sons[fils]->sons[0]->sons[i]->data);
                            if (exceptions_err.tab[CODE_ERROR] == code || (CODE_ERROR < 0 && -CODE_ERROR == code)) // l'erreur correspond
                            {
                                if (CODE_ERROR < 0)
                                    EXCEPTION = NULL;
                                
                                bo = fils;
                                break;
                            }
                        }
                    }

                }

                if (bo > 0) // exécution du except
                {
                    CODE_ERROR = 0; // du coup c'est bon on repart (la police a arrêté le programme, a vérifié ses papiers, et le programme est reparti)
                    
                    //stack_check_for(varstack, 3);

                    int_ret = exec_aux(maintree->sons[bo]->sons[1]);

                    if (CODE_ERROR != 0 || int_ret != EXIT_SUCCESS) {
                        return int_ret;
                    }
                }
            }
        }

        else if (tree->sons[inst]->type == TYPE_ATOMICBLOCK)
        {
            // on va exécuter le bloc de code d'une traite, sans changer de processus
            //stack_check_for(varstack, 4);
            int temp = atomic_counter;
            atomic_counter = -1; // tant qu'on n'a pas remis une valeur positive, on ne passera pas à un autre processus
            
            intptr_t int_ret = exec_aux(tree->sons[inst]);

            
            // s'il reste des crédits à ce processus, on les lui rend, sinon on passe au suivant
            atomic_counter = (atomic_counter + temp > 0) ? atomic_counter + temp : 0;

            if (CODE_ERROR != 0 || int_ret != 0) {
                return int_ret;
            }
        }
        
        else if (tree->sons[inst]->type == TYPE_BLOCKWORD1LINE)
        {
            if (tree->sons[inst]->label2 == RETURN) // action de return
            {
                if (tree->sons[inst]->nbSons > 1)
                {
                    CODE_ERROR = 21;
                    return 0;
                }
                
                if (!neo_is_void(RETURN_VALUE)) // c'est pas correct, car on ne peut pas renvoyer une valeur alors que la précédente n'a pas été récupérée
                {
                    CODE_ERROR = 99;
                    return 0;
                }

                if (tree->sons[inst]->nbSons == 0) {
                    RETURN_VALUE = neo_none_create();
                    return EXIT_RETURN;
                }
                else {
                    //stack_check_for(varstack, 3);
                    RETURN_VALUE = eval_prolog(tree->sons[inst]->sons[0]);
                    return EXIT_RETURN;
                }
            }

            
            else if (tree->sons[inst]->label2 == IMPORT) // action de import
            {
                for (int ext_index = 0 ; ext_index < tree->sons[inst]->nbSons ; ext_index++)
                {
                    // on empile inst et ext_index
                    //stack_check_for(varstack, 4);

                    NeObj nom = eval_prolog(tree->sons[inst]->sons[ext_index]);

                    if (CODE_ERROR != 0) {
                        return 0;
                    }

                    char* nomAct = strdup(neo_to_string(ADRESSES->tab[NAME])); // pour restaurer le nom de fichier actuel

                    #ifndef TI83PCE
                        char* nomFichier = addStr(neo_to_string(nom), ".ne");

                        updateFileName(strdup(neo_to_string(nom)));

                        importFile(nomFichier);
                        err_free(nomFichier);
                    #else
                        updateFileName(strdup(neo_to_string(nom)));
                        importFile(neo_to_string(nom));
                    #endif

                    updateFileName(nomAct);

                    neobject_destroy(nom);

                    if (CODE_ERROR != 0) {
                        return 0;
                    }
                    
                    // en général un import fait pas mal de choses avec la mémoire, et on n'en fait pas souvent
                    // c'est donc un bon endroit pour faire un garbage collection
                    gc_mark_and_sweep();
                }
            }

            else if (tree->sons[inst]->label2 == LOCAL)
            {
                if (tree->sons[inst]->nbSons == 0) // il faut au moins un argument
                {
                    CODE_ERROR = 69;
                    return 0;
                }
                else if (process_cycle->process->var_loc->tete == NULL)
                {
                    CODE_ERROR = 70;
                    return 0;
                }

                for (int i = 0 ; i < tree->sons[inst]->nbSons ; i++) // pour toutes les variables
                {
                    // va traiter la variable comme étant locale
                    save_later(process_cycle->process->varsToSave, tree->sons[inst]->sons[i]->variable);
                    local(tree->sons[inst]->sons[i]->variable, process_cycle->process->var_loc);
                }
                
            }
        }

        
        
        else if (tree->sons[inst]->type == TYPE_KEYWORD)
        {
            if (tree->sons[inst]->label2 != PASS) {
                return tree->sons[inst]->label2;
            }
        }

        
    
        else if (tree->sons[inst]->type == TYPE_CONDITIONBLOCK)
        {
            int int_ret = execConditionBlock(tree->sons[inst]);
            
            if (int_ret != 0 || CODE_ERROR != 0) {
                return int_ret;
            }
        }

        

        else if (tree->sons[inst]->type == TYPE_STATEMENTWHILE)
        {
            intptr_t int_ret = 0;

            //stack_check_for(varstack, 4);
            // évaluation de la condition

            NeObj expr = eval_prolog(tree->sons[inst]->sons[0]);

            if (CODE_ERROR != 0) {
                return 0;
            }

            bool cond = neoIsTrue(expr);
            neobject_destroy(expr);

            while (cond)
            {

                int_ret = exec_aux(tree->sons[inst]->sons[1]);

                if (CODE_ERROR != 0) {
                    return 0;
                }
                
                if (int_ret != 0 && int_ret != CONTINUE)
                    break;

                // réévaluation de la condition

                expr = eval_prolog(tree->sons[inst]->sons[0]);

                if (CODE_ERROR != 0) {
                    return 0;
                }

                cond = neoIsTrue(expr);
                neobject_destroy(expr);
                
            }

            
            if (int_ret == EXIT_RETURN) { // en sortant d'une boucle, on n'a pas de break ou continue
                return int_ret;
            }
        }
        
        
        else if (tree->sons[inst]->type == TYPE_STATEMENTFOR) {
            
            if (tree->sons[inst]->sons[0]->sons[0]->type != TYPE_VARIABLE)
            {
                CODE_ERROR = 22;
                return 0;
            }

            int int_ret = execStatementFor(tree->sons[inst]);

            if (CODE_ERROR != 0 || int_ret != 0)
                return int_ret;
        }

        else if (tree->sons[inst]->type == TYPE_STATEMENTFOREACH) {
            
            if (tree->sons[inst]->sons[0]->sons[0]->type != TYPE_VARIABLE)
            {
                CODE_ERROR = 22;
                return 0;
            }

            if (tree->sons[inst]->sons[0]->nbSons != 2) {
                CODE_ERROR = 109;
                return 0;
            }


            NeObj iterable = eval_prolog(tree->sons[inst]->sons[0]->sons[1]);

            if (CODE_ERROR != 0) {
                return 0;
            }
            
            int int_ret = 0;

            if (NEO_TYPE(iterable) == TYPE_LIST) {
                int_ret = execStatementForeachList(tree->sons[inst], iterable);
            }
            else if (NEO_TYPE(iterable) == TYPE_STRING) {
                int_ret = execStatementForeachString(tree->sons[inst], iterable);
            }
            else {
                neobject_destroy(iterable);
                CODE_ERROR = 109;
                return 0;
            }

            neobject_destroy(iterable);

            if (CODE_ERROR != 0 || int_ret != 0) {
                return int_ret;
            }
        }

        

        else if (tree->sons[inst]->type == TYPE_FUNCTIONDEF)
        {
            // définition de fonction en cours d'exécution

            // il faut d'abord évaluer les arguments optionnels
            NeList* opt_args = treeToList(tree->sons[inst]->sons[0]);

            // on crée la nouvelle fonction avec ces arguments optionnels
            NeObj function = userFuncDefine(tree->sons[inst]->data, opt_args);

            // et on la met dans la variable qui porte son nom
            Var var = get_var(tree->sons[inst]->label1);
            replace_var(var, function);
        }

        else
        {
            NeObj res = eval_prolog(tree->sons[inst]);
    
            neobject_destroy(res); // sinon, évaluation de l'expression, en la libérant juste après

            if (CODE_ERROR != 0)
                return 0;
        }
        

        if (CODE_ERROR != 0) { // ben oui sinon les erreurs ne seront pas captées au bon endroit
            return 0;
        }
    
    }

    return 0;
}




void initRuntime(void) {
    create_new_process(NULL, false);
}

void exitRuntime(void) {

    PROCESS_FINISH.tab[process_cycle->process->id] = 1; // on a fini le processus, on libère la place

    // supprime définitivement ce dernier processus

    ptrlist_destroy(process_cycle->process->varsToSave, true, true);

    if (process_cycle->process->stack != NULL) {
        err_free(process_cycle->process->stack);
    }

    err_free(process_cycle->process->var_loc);

    if (process_cycle->process->original_call != NULL) {
        neobject_destroy(process_cycle->process->original_call->data);
        err_free(process_cycle->process->original_call);
    }

    err_free(process_cycle->process);
    
    process_cycle->process = NULL;
    process_cycle->prev = NULL;
    process_cycle->next = NULL;

    // renvoie la promesse associée au processus principal

    *PROMISES_CNT.tab[0] = 0; // normalement il ne peut pas être à autre chose que zéro, vu que personne n'a jamais
    // récupéré de promesse sur le processus principal
    PROCESS_FINISH.tab[0] = true;
    return;
}



void exec(Tree* tree) {
    initRuntime();
    exec_aux(tree);
    exitRuntime();
    return;
}

NeObj eval(Tree* tree) {
    initRuntime();
    NeObj res = eval_prolog(tree);
    exitRuntime();
    return res;
}
