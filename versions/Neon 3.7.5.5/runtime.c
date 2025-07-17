#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "headers.c"


extern NeList* PROMISES;
extern intlist PROMISES_CNT;

extern int CODE_ERROR;

extern strlist* CONTAINERS;
extern NeList* ATTRIBUTES;

extern intlist TYPESBUILTINSFONC;
extern strlist NOMSBUILTINSFONC;
extern NeObject* (*BUILTINSFONC[NBBUILTINFONC])(NeList*);
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
extern NeObject* RETURN_VALUE;
extern intlist exceptions_err;
extern char* EXCEPTION;
extern ProcessCycle* process_cycle;
extern void* STACK_PTR;

extern NeObject* reset_stack_and_registers(void);
extern void switch_registers(Process* process, Process* prev, int save_stack, int load_stack);
extern NeObject* eval_epilog_unaligned(void);

// fonctions des opérateurs
void* OPFONC[NBOPERATEURS] = {_and,_or,_xor,_add,_mul,_sub,_div,_pow,_equal,_notEq,_infEqual,_supEqual,_inf,_sup,_affectNone,_addEqual,_subEqual,_mulEqual,_divEqual,_incr,_decr,_not,_mod,_eucl,_ref,_goIn,NULL, _minus, _del, _affect, NULL, _exponent, _implique, _in, NULL, NULL, _swap, NULL, NULL};

// fonctions des fonctions built-in
NeObject* (*BUILTINSFONC[NBBUILTINFONC])(NeList*)={_print_,_input_,_nbr_,_str_,_len_,_substring_,_exit_,_append_,_remove_,_insert_,_type_, _reverse_, _eval_,_clear_,_help_, _randint_,_failwith_, _time_, _assert_, _output_, _chr_, _ord_, _list_comp_, _create_exception_, _raise_, _int_, _index_, _replace_, _count_, _list_, _sort_asc_, _sort_desc_, _sin_, _cos_, _tan_, _deg_, _rad_, _sqrt_, _ln_, _exp_, _log_, _log2_, _round_, _abs_, _ceil_, _floor_, _readFile_, _writeFile_, _setFunctionDoc_, _setAtomicTime_, _copy_, _load_namespace_, _gc_, _setColor_};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////




void updateFileName(char* name)
{
    neobject_destroy(ADRESSES->tab[NAME], true);
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
        neobject_destroy(ns->object, false);
        ns->object->data = ns->data;
        ns->object->type = ns->type;
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
            neobject_destroy(ns->object, false);
            ns->object->data = ns->data;
            ns->object->type = ns->type;
            err_free(ns);
            //-------------
            
            temp = ptr->queue;
            err_free(ptr);
            ptr = temp;
        }
        //-------------
        NeSave* ns = ptr->tete;
        neobject_destroy(ns->object, false);
        ns->object->data = ns->data;
        ns->object->type = ns->type;
        err_free(ns);
        //-------------
        
        err_free(ptr);
    }

    ptrlist_remove(var_loc, ctxt2, true); // enlève le pointeur

    return ;
}


_Bool isTrue(Tree* tree) {
    NeObject* obj = eval_prolog(tree, 1);
    _Bool b = neoIsTrue(obj);
    neobject_destroy(obj, true);
    return b;
}




_Bool neoIsTrue(NeObject* expr)
{
    if (CODE_ERROR != 0)
    {
        return NULL;
    }
    
    if (expr->type == TYPE_BOOL)
    {
        _Bool res = neo_to_bool(expr);
        return res;
    }
    else if (expr->type == TYPE_NUMBER)
    {
        _Bool res = (int)number_toDouble(neo_to_nb(expr)) != 0;
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



void local(NeObject* var, ptrlist* var_loc)
{
    // sauvegarde de l'object actuel
    NeSave* ns = err_malloc(sizeof(NeSave));
    ns->object = var;
    ns->data = var->data;
    ns->type = var->type;

    // réinitialisation de la variable
    neobject_reset(var);

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








void save_later(ptrlist* variables_a_sauvegarder, NeObject* neo) {
    // on regarde si cette variable faisait déjà partie de nos variables privatisées ou pas
    _Bool bo = false;
    for (ptrlist* ptr = variables_a_sauvegarder ; ptr != NULL && ptr->tete != NULL && !bo ; ptr = ptr->queue) {
        NeSave* ns = (NeSave*)ptr->tete;
        bo = bo || ns->object == neo;
    }

    if (!bo) { // alors il faut ajouter cette variable à nos variables privatisées
        NeSave* ns = err_malloc(sizeof(NeSave));
        ns->object = neo;
        ns->data = neo->data;
        ns->type = neo->type;
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
        NeObjDat data_temp = ns->data; // on garde une copie de la sauvegarde de notre variable
        char type_temp = ns->type;

        ns->data = ns->object->data; // on sauvegarde la valeur actuelle
        ns->type = ns->object->type;

        ns->object->data = data_temp; // on restaure la copie de notre variable
        ns->object->type = type_temp;
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
        //neobject_destroy(save->object, false);
        save->object->data = save->data;
        save->object->type = save->type;
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






Process* processCycle_add(ProcessCycle* pc, Tree* tree, int id, _Bool delete_tree) { // renvoie un pointeur vers le processus créé
    // on crée le nouveau processus
    Process* p = err_malloc(sizeof(Process));
    // création de la nouvelle pile
    p->var_loc = ptrlist_create();
    p->id = id;
    p->varsToSave = ptrlist_create();
    p->stack = NULL; // dans le cas où c'est le premier processus que l'on crée, p->stack va rester NULL, sinon on alloue une nouvelle pile

    // arguments de l'appel à la fonction
    p->arg_tree = tree;
    p->arg_calcListIndex = 1;

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
        neobject_destroy(p->original_call->data, true);
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
int create_new_process(Tree* tree, _Bool delete_tree) {
    // calcul de l'identifiant du processus que l'on ajoute
    int id = 0;

    // on doit garder les processus n'ayant pas fini et les processus ayant fini mais n'ayant pas toutes été récupérées
    for (; id < PROMISES->len && (PROCESS_FINISH.tab[id] == false || PROMISES_CNT.tab[id] > 0) ; id++);

    if (id >= PROMISES->len) { // l'identifiant est un nouvel identifiant
        nelist_append(PROMISES, NULL);
        intlist_append(&PROMISES_CNT, 0);
        intlist_append(&PROCESS_FINISH, 0);
    }

    neobject_destroy(PROMISES->tab[id], true);
    PROMISES->tab[id] = NULL;
    PROMISES_CNT.tab[id] = 1;
    PROCESS_FINISH.tab[id] = 0;

    Process* p = processCycle_add(process_cycle, tree, id, delete_tree); // le processus principal a un id de zéro

    return id;
}





/*
Quand un processus a terminé, le programme saute ici grâce à un bidouillage de pile :
-> Quand on alloue une pile custom, on place tout à la fin l'adresse de cette fonction, ce qui fait que cette fonction est appelée
quand eval_prolog fait son dernier return
*/

// le contexte est supposé à 8 octets, donc pas d'inlining possible
__attribute__((noinline))
NeObject* eval_epilog(void) {
    // on se remet sur la pile normale, on restaure les registres. On récupère aussi via cette fonction le retour de eval_prolog
    NeObject* result = reset_stack_and_registers();
    
    // on résoud la promesse et on continue l'exécution
    // le résultat est mis ici en attente, et neo_copy se chargera de vérifier si la valeur a changé ou pas
    if (PROMISES_CNT.tab[process_cycle->process->id] != 0) { // si c'est zéro, je sais que toutes les promesses ont été détruites avant même de les utiliser
        PROMISES->tab[process_cycle->process->id] = result;
    }
    else {
        neobject_destroy(result, true);
    }

    PROCESS_FINISH.tab[process_cycle->process->id] = 1; // on a fini le processus, on libère la place
    

    if (process_cycle->next != process_cycle || process_cycle->prev != process_cycle) {  // il reste des processus annexes à exécuter

        // on supprime le processus, on passe au suivant            
        
        process_cycle = processCycle_remove(process_cycle);

        // on saute directement à l'exécution du processus
        __asm__ __volatile__ (
            "addq	$8,     %rsp\n\t" // on remet la pile à son état d'origine, la seule variable locale était result (c'est un pointeur)
            "mov    $0,     %rdi\n\t" // tree = NULL
            "jmp    eval_prolog\n\t"
        );
        // comme on sera sur une nouvelle pile, on retournera normalement comme si de rien n'était
    }

    // supprime définitivement ce dernier processus

    ptrlist_destroy(process_cycle->process->varsToSave, true, true);

    if (process_cycle->process->stack != NULL) {
        err_free(process_cycle->process->stack);
    }

    err_free(process_cycle->process->var_loc);

    if (process_cycle->process->original_call != NULL) {
        neobject_destroy(process_cycle->process->original_call->data, true);
        err_free(process_cycle->process->original_call);
    }

    err_free(process_cycle->process);
    
    process_cycle->process = NULL;
    process_cycle->prev = NULL;
    process_cycle->next = NULL;

    // renvoie la promesse associée au processus principal

    PROMISES_CNT.tab[0] = 0; // normalement il ne peut pas être à autre chose que zéro, vu que personne n'a jamais
    // récupéré de promesse sur le processus principal
    PROCESS_FINISH.tab[0] = true;
    
    result = PROMISES->tab[0];
    PROMISES->tab[0] = NULL;
    return result; // la promesse associée au processus principal
}










/*
Il faut être très prudent quand on fait des modifications à cette fonction, elle a une utilisation de la pile très contrôlée
car elle est interfacée avec de l'assembleur qui lui modifie son adresse de retour
*/
__attribute__((noinline)) // le contexte est supposé de 8 octets, donc pas d'inlining possible
NeObject* eval_prolog(Tree* tree, intptr_t calcListIndex) {
    static NeObject* res; // ça permet que la variable ne soit pas sur la pile
    
    if (tree == NULL)
    {        
        // on est sur la pile principale, donc dans tous les cas on peut la sauvegarder. C'est important si jamais on vient de supprimer
        // le processus qui utilisait la pile principale, sinon reset_stack_and_registers va croire qu'on est déjà sur la pile
        // principale la prochaine fois qu'il sera appelé
        switch_registers(process_cycle->process, NULL, true, process_cycle->process->stack == NULL);

        atomic_counter = ATOMIC_TIME;

        if (CODE_ERROR != 0) {
            return NULL;
        }
    }
    else {

        process_cycle->process->arg_tree = tree; // le processus recommencera avec ces arguments
        process_cycle->process->arg_calcListIndex = calcListIndex;

        // ========= PROLOGUE ============

        LABEL_PROLOG:

        // on change de processus seulement s'il y a plus d'un processus
        if (atomic_counter == 0 && process_cycle->next != process_cycle) { // c'est le moment où l'on change de processus, juste après un appel récursif
            // on fait le travail de changement de processus

            process_cycle = nextProcess(process_cycle);

            // on est sûrs que le processus précédent dans la chaîne était vraiment le processus précédent
            // on change les registres après les avoir sauvegardés et on change de pile
            //printf("Switch from process %d to process %d\n", process_cycle->prev->process->id, process_cycle->process->id);
            switch_registers(process_cycle->process, process_cycle->prev->process, process_cycle->prev->process->stack == NULL, process_cycle->process->stack == NULL);

            // a partir de maintenant on est sur la nouvelle pile

            atomic_counter = ATOMIC_TIME;

            // on s'apprête à retourner dans un autre processus, donc on vérifie avant si le processus précédent n'avait pas déclenché une erreur
            if (CODE_ERROR != 0) {
                return NULL;
            }

        }
        else if (atomic_counter == 0) {
            atomic_counter = ATOMIC_TIME;
        }
        else if (atomic_counter < 0) { // on est dans un atomic block, donc comme on ne va pas chanher de processus on vérifie à chaque tour
            //stack_check_for(retstack, 1);
            //stack_check_for(valstack, 1);
            //stack_check_for(varstack, 6);
        }
    }

    atomic_counter--;
    
    res = eval_aux(process_cycle->process->arg_tree, process_cycle->process->arg_calcListIndex);

    if ((intptr_t)res == -1) { // si eval_aux retourne avec -1, c'est qu'elle veut continuer à exécuter cette expression
        goto LABEL_PROLOG;
    }

    return res;
}



NeList* treeToList(Tree* tree, int nbArgsMeth) {

    NeList* l = nelist_create(tree->nbSons);

    int ext_index = 0;
    

    for (; (ext_index < nbArgsMeth && ext_index < tree->nbSons) || (nbArgsMeth == -1 && ext_index < tree->nbSons) ; ext_index++)
    {
        l->tab[ext_index] = eval_prolog(tree->sons[ext_index], 0);

        if (CODE_ERROR != 0) {
            err_free(l->tab);
            err_free(l);
            return NULL;
        }
        
        if (l->tab[ext_index]->type == TYPE_EMPTY)
        {
            CODE_ERROR = 5;
            err_free(l->tab);
            err_free(l);
            return NULL;
        }
    }


    for (;ext_index < tree->nbSons ; ext_index++)
    {
        l->tab[ext_index] = eval_prolog(tree->sons[ext_index], 1);
        
        if (CODE_ERROR != 0)
        {
            // si y a eu un problème dans l'évaluation d'un argument, on doit libérer toute la liste créée jusqu'alors
            for (int j = nbArgsMeth ; j <= ext_index ; j++) // on ne détruit que les éléments que l'on a copiés
                neobject_destroy(l->tab[j],true);
            
            err_free(l->tab);
            err_free(l);
            return NULL;
        }
        
    }

    return l;
}



NeObject* callUserFunc(UserFunc* fun, _Bool isMethod, NeList* args, NeObject* neo_local_args) {

    // ouvre un nouveau contexte pour sauvegarder les variables locales de cet appel
    newContext(process_cycle->process->var_loc);


    // on sauvegarde les "variables à sauvegarder" de ce processus avant d'en ajouter d'autres
    ptrlist* sov_vars_to_save = process_cycle->process->varsToSave->tete;

    if (fun->unlimited_arguments) {
        int len = neo_list_len(neo_local_args);

        int index = strlist_index(NOMS, "__local_args__");
        if (index < 0) {
            CODE_ERROR = 0; // strlist_index a modifié le code d'erreur en renvoyant -1
            strlist_append(NOMS, strdup("__local_args__"));
            nelist_append(ADRESSES, neo_local_args);
        }
        else {

            save_later(process_cycle->process->varsToSave, ADRESSES->tab[index]);
            local(ADRESSES->tab[index], process_cycle->process->var_loc);
            // on lui dit de  sauvegarder cette variable avant de switcher de processus

            ADRESSES->tab[index]->data = neo_local_args->data;
            ADRESSES->tab[index]->type = neo_local_args->type;
            err_free(neo_local_args);
        }

        if (len > 0) // du coup on enlève les éléments en trop de args
            args->len = fun->nbArgs;
    }
    
    

    // affectation de la variable de méthode
    /*if (fun->nbArgs > 0)
    {
        NeObject* c = neo_copy(neo_list_nth(args, 0)); // on crée une copie de l'argument
        // on empile la valeur actuelle de l'argument pour avoir champ libre dans cette fonction
        
        save_later(process_cycle->process->varsToSave, fun->args[0]);
        local(fun->args[0]);

        fun->args[0]->data = c->data;
        fun->args[0]->type = c->type;
        err_free(c);
    }*/
    
    for (int i = 0 ; i < fun->nbArgs ; i++) // affectation des arguments
    {
        NeObject* temp = neo_copy(args->tab[i]);
        
        save_later(process_cycle->process->varsToSave, fun->args[i]);
        local(fun->args[i], process_cycle->process->var_loc);
        
        fun->args[i]->data = temp->data;
        fun->args[i]->type = temp->type;


        err_free(temp);
        
        if (CODE_ERROR != 0) // erreur : libération des arguments affectés
        {
            
            // libérer la sauvegarde des variables
            //deleteContext(process_cycle->process->var_loc);
            
            if (isMethod)
                args->tab[0] = NULL;
            else
                neobject_destroy(fun->args[0], false); // on fait ici l'itération zéro de la liste que l'on fait dans la cas pas méthode

            // libération des arguments
            for (int j = 1 ; j < i ; j++)
                neobject_destroy(fun->args[j], false);

            deleteContext(process_cycle->process->var_loc);
            partialRestore(process_cycle->process->varsToSave, sov_vars_to_save);

            return NULL;
        }
    }

    // on stocke fun et args
    
    int int_ret = exec_aux(fun->code);

    if (CODE_ERROR != 0) // erreur
    {
        // supprime les valeurs des variables locales de la fonction
        // il n'y a pas besoin de le faire car on va déjà supprimer les objets en restaurant les variables sauvegardées avec local
        // peut etre que dans le cas d'une méthode il faut quand même supprimer le premier élément
        /*for (int i = 0 ; i < fun->nbArgs ; i++) {
            neobject_destroy(fun->args[i], false);
        }*/
        
        // on supprime les arguments, sauf le premier parce qu'on le réutilise
        if (isMethod)
            args->tab[0] = NULL;

        deleteContext(process_cycle->process->var_loc);
        partialRestore(process_cycle->process->varsToSave, sov_vars_to_save);

        return NULL;
    }

    // sauvegarde des valeurs obtenues à la fin de la méthode pour que le paramètre puisse éventuellement récupérer son ancienne valeur
    if (isMethod && fun->nbArgs > 0)
    {

        // sauvegarde uniquement le data, et on l'enlève de fun->args[0]
        NeObjDat s = fun->args[0]->data;
        int t = fun->args[0]->type;
        
        neobject_reset(fun->args[0]);

        deleteContext(process_cycle->process->var_loc); // supprime le contexte : restaure les variables d'avant

        neobject_destroy(args->tab[0], false);
        
        // on modifie list[0]
        args->tab[0]->data = s;
        args->tab[0]->type = t;

        args->tab[0] = NULL; // neobject au pif

    }
    else {
        deleteContext(process_cycle->process->var_loc); // réaffecte les anciennes valeurs des variables qui ont été mises en local
    }


    // on enlève les variables qu'on avait marquées comme "à sauvegarder"
    // on enlève une à une toutes les variables qu'on avait rajoutées
    partialRestore(process_cycle->process->varsToSave, sov_vars_to_save);
    
    if (int_ret == EXIT_RETURN) {
        NeObject* sov = RETURN_VALUE;
        RETURN_VALUE = NULL; // pour dire que l'on l'a utilisé
        return sov;
    }
    else
        return neo_none_create();
}







NeObject* eval_aux(Tree* tree, intptr_t calcListIndex) {

    // =================== EVALUATION =====================

    LINENUMBER = tree->line;

    // disjonction de cas suivant si c'est une expression ou des instructions
    // on calcule en O(1) si notre type est une expression ou pas

    if (tree->type == TYPE_OPERATOR)
    {
        // a partir d'ici, on n'utilise plus que les appellations de variables locales

        if (!calcListIndex && tree->label2 != 26) {
            CODE_ERROR = 103;
            return NULL;
        }

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
                maintree_copy->data = eval_prolog(maintree->sons[0], 1);

                if (CODE_ERROR != 0) {
                    err_free(maintree_copy);
                    return NULL;
                }
                if (maintree_copy->data->type != TYPE_USERFUNC) {
                    CODE_ERROR = 100;
                    neobject_destroy(maintree_copy->data, true);
                    err_free(maintree_copy);
                    return NULL;
                }

                // on ajoute le processus, et il va se faire exécuter dans la chaine de processus
                int id = create_new_process(maintree_copy, true);

                return neo_promise_create(id);
            }


            // opérateur paresseux
            if (OPERANDES.tab[tree->label2] & LAZY) {
                NeObject* (*func)(Tree*) = OPFONC[tree->label2];
                return func(tree->sons[0]);
            }
            else {
                // opérateur qui prend directement des objets en argument
                NeObject* op1;


                // évaluation de l'objet
                if (OPERANDES.tab[tree->label2] & VARRIGHT || OPERANDES.tab[tree->label2] & VARLEFT)
                {
                    op1 = eval_prolog(tree->sons[0], 0); // si la grammaire stipule que l'opérateur doit recevoir une variable et non une valeur
                    
                    if (CODE_ERROR != 0) {
                        return NULL;
                    }
                    
                }
                else
                {
                    op1 = eval_prolog(tree->sons[0], 1);

                    if (CODE_ERROR != 0) {
                        return NULL;
                    }
                }

                // calcul de la valeur de retour
                NeObject* retour;

                // l'opérateur @ a le droit à un traitement spécial car suivant calcListIndex, il renvoie l'objet ou une copie de l'objet
                if (tree->label2 == 26) { // opérateur @

                    if (op1->type != TYPE_STRING) {
                        CODE_ERROR = 60; // ceci n'est pas une chaine de caractères
                        neobject_destroy(op1, true);
                        return NULL;
                    }
                    char* nom=neo_to_string(op1);

                    if(!strlist_inList(NOMS, nom))
                    {
                        CODE_ERROR = 5;
                        neobject_destroy(op1, true);
                        return NULL;
                    }
                        
                    int index = strlist_index(NOMS,nom);

                    neobject_destroy(op1, true);
                    
                    return neo_copy(ADRESSES->tab[index]);
                }
                else {
                    NeObject* (*func) (NeObject*) = OPFONC[tree->label2];
                    retour = func(op1);
                }


                if (CODE_ERROR != 0)
                {
                    if (!(OPERANDES.tab[tree->label2] & VARRIGHT) && !(OPERANDES.tab[tree->label2] & VARLEFT))
                        neobject_destroy(op1, true);
                    neobject_destroy(retour, true);
                    return NULL;
                }
                
                
                if (!(OPERANDES.tab[tree->label2] & VARRIGHT) && !(OPERANDES.tab[tree->label2] & VARLEFT))
                    neobject_destroy(op1, true);

                return retour;
            }
        }
        else if (tree->nbSons == 2) // operateur binaire
        {
            // opérateur paresseux
            if (OPERANDES.tab[tree->label2] & LAZY) {
                NeObject* (*func) (Tree*, Tree*) = OPFONC[tree->label2];
                return func(tree->sons[0], tree->sons[1]);
            }
            else {
                // opérateur normal qui prend des objets en arguments
                NeObject* op1, *op2;

                if (tree->label2 == 37) { // opérateur :=
                    CODE_ERROR = 95;
                    return NULL;
                } else if (tree->label2 == 35) { // opérateur :
                    CODE_ERROR = 92;
                    return NULL;
                }
                                    
                if (OPERANDES.tab[tree->label2] & VAR_RIGHT)
                {
                    op1 = eval_prolog(tree->sons[0], 0);

                    if (CODE_ERROR != 0) {
                        return NULL;
                    }
                    
                    op2 = eval_prolog(tree->sons[1], 1);
                }
                else if (OPERANDES.tab[tree->label2] & LEFT_VAR)
                {                    
                    op1 = eval_prolog(tree->sons[0], 1);

                    if (CODE_ERROR != 0) {
                        return NULL;
                    }
                    
                    op2 = eval_prolog(tree->sons[1], 0);
                }
                else if (OPERANDES.tab[tree->label2] & VAR_VAR)
                {
                    op1 = eval_prolog(tree->sons[0], 0);

                    if (CODE_ERROR != 0) {
                        return NULL;
                    }
                    
                    op2 = eval_prolog(tree->sons[1], 0);
                }
                else
                {
                    op1 = eval_prolog(tree->sons[0], 1);

                    if (CODE_ERROR != 0)
                    {
                        neobject_destroy(op1, true);
                        return NULL;
                    }

                    op2 = eval_prolog(tree->sons[1], 1);
                }

                if (CODE_ERROR != 0)
                {
                    if (!(OPERANDES.tab[tree->label2] & VAR_RIGHT) && !(OPERANDES.tab[tree->label2] & VAR_VAR))
                        neobject_destroy(op1, true);
                    return NULL;
                }

                NeObject* (*func) (NeObject*, NeObject*) = OPFONC[tree->label2];
                NeObject* retour = func(op1, op2);

                if (CODE_ERROR != 0)
                {
                    if (!(OPERANDES.tab[tree->label2] & VAR_RIGHT) && !(OPERANDES.tab[tree->label2] & VAR_VAR))
                        neobject_destroy(op1, true);
                    neobject_destroy(op2, true);
                    neobject_destroy(retour, true);
                    return NULL;
                }

                if (!(OPERANDES.tab[tree->label2] & VAR_RIGHT) && !(OPERANDES.tab[tree->label2] & VAR_VAR))
                    neobject_destroy(op1, true);
                
                if (!(OPERANDES.tab[tree->label2] & LEFT_VAR) && !(OPERANDES.tab[tree->label2] & VAR_VAR))
                    neobject_destroy(op2, true);

                
                return retour;
            }
        }
        
    }
    
    // s'il n'y a qu'un seul élément final, on le calcule (traiterOperande)
    
    if (tree->type == TYPE_NUMBER)
    {
        if (!calcListIndex)
        {
            CODE_ERROR = 89;
            return NULL;
        }
        else {
            return neo_copy(tree->data);
        }
    }
    
    
    if (tree->type == TYPE_VARIABLE)
    {
        if (calcListIndex)
        {
            if (tree->data->type == TYPE_EMPTY)
            {
                CODE_ERROR = 5;
                return NULL;
            }
            else {
                return neo_copy(tree->data);
            }
        }
        else
        {
            update_if_promise(tree->data);

            return tree->data;
        }
        

    }
    
    if (tree->type == TYPE_LIST)
    {
        if (!calcListIndex) // erreur car sinon ça fait une fuite mémoire
        {
            CODE_ERROR = 89;
            return NULL;
        }

        // donc les enfants de tree sont les éléments de la liste
        NeList* l = treeToList(tree, 0);


        if (CODE_ERROR != 0) {                   
            return NULL;
        }

        return neo_list_convert(l);
    }
    
    if (tree->type == TYPE_FONCTION)
    {
        if (!calcListIndex)
        {
            CODE_ERROR = 103;
            return NULL;
        }

        // ici, appel récursif pour évaluer la fonction (1er fils), et on la met dans ->data de l'arbre


        // si le champ data de la fonction est déjà occuppé, on n'a pas besoin d'évaluer la fonction
        if (tree->data == NULL) {
            tree->data = eval_prolog(tree->sons[0], 1);

            if (CODE_ERROR != 0) { // erreur lors de l'évaluation de la fonction
                return NULL;
            }
        }


        // ensuite, pour évaluer les arguments, on fait bien la boucle sur tree->sons[1]
        
        if (tree->data->type == TYPE_FONCTION)
        {
            NeObject* tree_data_sov = tree->data;
            Function* fun = tree->data->data.ptr;

            tree->data = NULL;
            NeList* l = treeToList(tree->sons[1], fun->nbArgsMeth);
            tree->data = tree_data_sov;

            if (CODE_ERROR != 0) {
                neobject_destroy(tree->data, true);
                tree->data = NULL;
                return NULL;
            }

            // cette liste est parfaitement interne au fonctionnement de Neon et n'est jamais censée être accessible depuis ADRESSES
            NeObject* args = gc_extern_neo_list_convert(l);
            

            if (CODE_ERROR != 0)
            {
                neo_list_free(args); // supprime args, mais pas ses éléments

                neobject_destroy(tree->data, true); // on supprime la fonction que l'on vient de créer
                tree->data = NULL;
                
                return NULL;
            }
    
            if (!funcArgsCheck(fun, args->data.ptr))
            {
                if (fun->nbArgsMeth != -1)
                {
                    for (int i=fun->nbArgsMeth ; i < neo_list_len(args) ; i++) // on ne détruit que les éléments que l'on a copiés
                    {
                        neobject_destroy(neo_list_nth(args,i),true);
                    }
                }
    
                neo_list_free(args); // supprime args, mais pas ses éléments

                neobject_destroy(tree->data, true); // on supprime la fonction que l'on vient de créer
                tree->data = NULL;
                
                CODE_ERROR = 14;
                return NULL;
            }
    
            
            NeObject* retour = functionCall(tree->data, (NeList*)args->data.ptr);

            
            if (CODE_ERROR != 0)
            {
                if (fun->nbArgsMeth != -1)
                {
                    for (int i=fun->nbArgsMeth ; i < neo_list_len(args) ; i++) // on ne détruit que les éléments que l'on a copiés
                    {
                        neobject_destroy(neo_list_nth(args,i),true);
                    }
                    // et on met le tableau à zéro
                    NeList* nelist_args = args->data.ptr;
                    for (int i=0 ; i < nelist_args->len ; i++)
                        nelist_args->tab[i] = NULL;
                }
    
                neo_list_free(args); // supprime args, mais pas ses éléments

                neobject_destroy(tree->data, true); // on supprime la fonction que l'on vient de créer
                tree->data = NULL;

                neobject_destroy(retour, true);
                
                return NULL;
            }
            
            if (fun->nbArgsMeth != -1)
            {
                for (int i=fun->nbArgsMeth ; i < neo_list_len(args) ; i++) // on ne détruit que les éléments que l'on a copiés
                {
                    neobject_destroy(neo_list_nth(args,i),true);
                }
                // et on met le tableau à zéro
                NeList* nelist_args = args->data.ptr;
                for (int i=0 ; i < nelist_args->len ; i++)
                    nelist_args->tab[i] = NULL;
            }
    
    
            neo_list_free(args); // supprime args, mais pas ses éléments

            neobject_destroy(tree->data, true); // on supprime la fonction que l'on vient d'exécuter
            tree->data = NULL; // pour bien indiquer qu'il faut réévaluer la fonction la prochaine fois qu'on passe par là


            return retour;
        }

        else if (tree->data->type == TYPE_USERFUNC || tree->data->type == TYPE_USERMETHOD)
        {

            UserFunc* fun = tree->data->data.ptr;

            if (tree->sons[1]->nbSons > fun->nbArgs && ! fun->unlimited_arguments)
            {
                CODE_ERROR = 6;
                neobject_destroy(tree->data, true); // on supprime la fonction que l'on vient de créer
                tree->data = NULL;

                return NULL;
            }


            // on compte le nombre d'arguments très facultatifs
            int given_nbOptArgs = 0;

            for (int i = 0 ; i < tree->sons[1]->nbSons ; i++) {
                if (tree->sons[1]->sons[i]->type == TYPE_OPERATOR && tree->sons[1]->sons[i]->label2 == 37) { // on est sur du :=
                    int index = 0;
                    while (index < fun->nbArgs && fun->args[index] != tree->sons[1]->sons[i]->sons[0]->data) index++;

                    if (index >= fun->nbArgs - fun->nbOptArgs && index < fun->nbArgs)
                        given_nbOptArgs ++;
                }
            }

            
            // on sait maintenant que le nombre d'arguments réel final est :
            // -> Si tree->nbSons > fun->nbArgs : tree->nbSons - given_nbOptArgs
            // -> Sinon, fun->nbArgs



            // on va commencer par construire un tableau d'expressions à évaluer (un tableau d'arbres) à partir des arguments qu'on nous donne
            Tree* arguments = err_malloc(sizeof(Tree));
            arguments->nbSons = (fun->unlimited_arguments && tree->sons[1]->nbSons > fun->nbArgs - fun->nbOptArgs) ? tree->sons[1]->nbSons + fun->nbOptArgs - given_nbOptArgs : fun->nbArgs;
            arguments->sons = err_malloc(sizeof(Tree*) * arguments->nbSons);
            

            // on va faire un tableau de booléens qui compte si on donne la valeur des arguments très optionnels (après ...)

            
            for (int i = 0 ; i < arguments->nbSons ; i++) arguments->sons[i] = NULL; // pour savoir s'il y a des problèmes



            // on peut commencer par mettre à leur valeur par défaut les variables définies après ...
            // Ainsi elles ne seront pas remplacés sauf si on l'indique explicitement
            for (int i = fun->nbArgs - fun->nbOptArgs ; i < fun -> nbArgs ; i++) {
                arguments->sons[i] = fun->opt_args->sons[i];
            }


            // 1ère boucle pour placer les arguments à des endroits où ils ne bougeront pas
            for (int i = 0 ; i < tree->sons[1]->nbSons ; i++) {
                // on commence par remplir les éléments donnés dans le mauvais ordre (avec :=)
                if (tree->sons[1]->sons[i]->type == TYPE_OPERATOR && tree->sons[1]->sons[i]->label2 == 37) { // on est sur du :=
                    // on regarde la variable de gauche, et on met le truc au bon endroit
                    // on récupère l'index du vrai argument dans fun->args, et on le met au bon endroit dans arguments
                    int index = 0;

                    if (tree->sons[1]->sons[i]->sons[0]->type != TYPE_VARIABLE) {
                        err_free(arguments->sons);
                        err_free(arguments);
                        neobject_destroy(tree->data, true); // on supprime la fonction que l'on vient de créer
                        tree->data = NULL;
                        CODE_ERROR = 93;
                        return NULL;
                    }

                    while (index < fun->nbArgs && fun->args[index] != tree->sons[1]->sons[i]->sons[0]->data) index++;

                    if (index == fun->nbArgs) {
                        err_free(arguments->sons);
                        err_free(arguments);
                        neobject_destroy(tree->data, true); // on supprime la fonction que l'on vient de créer
                        tree->data = NULL;
                        CODE_ERROR = 93;
                        return NULL;
                    }
                    
                    if (index < fun->nbArgs - fun->nbOptArgs && arguments->sons[index] != NULL) {
                        err_free(arguments->sons);
                        err_free(arguments);
                        neobject_destroy(tree->data, true); // on supprime la fonction que l'on vient de créer
                        tree->data = NULL;
                        CODE_ERROR = 94;
                        return NULL;
                    }
                    arguments->sons[index] = tree->sons[1]->sons[i]->sons[1];
                }
            }

            // deuxième boucle pour insérer dans l'ordre et en évitant les := les arguments restants
            int index = 0; // on remplit petit à petit arguments avec les arguments restants
            while (index < arguments->nbSons && arguments->sons[index] != NULL) index++;

            for (int i = 0 ; index < arguments->nbSons && i < tree->sons[1]->nbSons ; i++) {
                
                if (tree->sons[1]->sons[i]->type != TYPE_OPERATOR || tree->sons[1]->sons[i]->label2 != 37) {
                    arguments->sons[index] = tree->sons[1]->sons[i];
                    while (index < arguments->nbSons && arguments->sons[index] != NULL) index++;
                }
            }
            // c'est un choix philosophique : si f attend x, y facultatif et z, alors f(1,2) ne fonctionnera pas car il manque z
            // pour mettre uniquement le x et le z, il y a une syntaxe pour le faire

            if (tree->sons[1]->nbSons < fun->nbArgs) {
                // à ce stade-là, certains champs de arguments sont encore vides, on va donc parcourir une dernière fois afin de lui associer les expressions définies lors de la définition de la fonction
                for (int i = 0 ; i < fun->nbArgs ; i++) {
                    if (arguments->sons[i] == NULL) { // c'est précisément le cas où l'utilisateur n'a rien mis
                        arguments->sons[i] = fun->opt_args->sons[i];
                        if (tree_isEmpty(arguments->sons[i])) {
                            CODE_ERROR = 7;
                            neobject_destroy(tree->data, true); // on supprime la fonction que l'on vient de créer
                            tree->data = NULL;
                            err_free(arguments->sons);
                            err_free(arguments);
                            return NULL;
                        }
                    }
                }
            }

            // maintenant, normalement arguments est un tableau plein d'arbres, on peut donc l'évaluer

            // calcul des arguments et affectation des arguments
            NeObject* tree_data_sov = tree->data;
            tree->data = NULL; // pour que les appels futurs n'oublient pas d'évaluer la fonction

            NeList* liste;
            if (tree_data_sov->type == TYPE_USERMETHOD)
                liste = treeToList(arguments, 1);
            else
                liste = treeToList(arguments, 0);

            tree->data = tree_data_sov;

            err_free(arguments->sons);
            err_free(arguments);

            if (CODE_ERROR != 0) {
                neobject_destroy(tree->data, true); // on supprime la fonction que l'on vient de créer
                tree->data = NULL;
                return NULL;
            }


            // création de la liste local_args
            NeObject* neo_local_args = NULL;
            if (fun->unlimited_arguments) {
                // on raccourcit args, et on crée la liste locale
                int len = (liste->len - given_nbOptArgs > fun->nbArgs - fun->nbOptArgs) ? liste->len - fun->nbArgs : 0;

                NeList* local_args = nelist_create(len);
                for (int i = 0 ; i < len ; i++) {
                    local_args->tab[i] = liste->tab[fun->nbArgs + i];
                }
                neo_local_args = neo_list_convert(local_args);
            }
            
            // exécution de la fonction

            tree->data = NULL;
            // cette liste n'est pas censée être gérée par le garbage collector, elle n'est à aucun moment censée être accessible depuis ADRESSES
            NeObject* ret = callUserFunc(fun, tree_data_sov->type == TYPE_USERMETHOD, liste, neo_local_args);
            tree->data = tree_data_sov;
            neobject_destroy(tree->data, true);
            tree->data = NULL;

            nelist_destroy(liste, true);

            if (CODE_ERROR != 0) {
                return NULL;
            }

            // ainsi si une return_value n'est pas à NULL c'est que forcément la valeur n'a pas été récupérée
            return ret;
            
        }

        else if (tree->data->type == TYPE_EMPTY)
        {
            CODE_ERROR = 8;
            neobject_destroy(tree->data, true);
            tree->data = NULL;
            return NULL;
        }
        else
        {
            CODE_ERROR = 9;
            neobject_destroy(tree->data, true);
            tree->data = NULL;
            return NULL;
        }
        
    }
    
    if (tree->type == TYPE_LISTINDEX)
    {
        NeObject* obj = eval_prolog(tree->sons[0], 0);

        if (CODE_ERROR != 0)
        {
            return NULL;
        }


        if (CODE_ERROR != 0) {
            return NULL;
        }


        NeObject* index = eval_prolog(tree->sons[1], 1);


        if (CODE_ERROR != 0) {
            return NULL;
        }


        if (obj->type != TYPE_LIST && obj->type != TYPE_STRING)
        {
            CODE_ERROR = 15;
            return NULL;
        }

        
        if (index->type != TYPE_NUMBER)
        {
            neobject_destroy(index, true);
            CODE_ERROR = 16;
            return NULL;
        }
        
        long int index2 = (long int)number_toDouble(neo_to_nb(index));
        neobject_destroy(index,true);
        

        if (obj->type == TYPE_EMPTY)
        {
            CODE_ERROR = 17;
            return NULL;
        }
        else if ((obj->type == TYPE_LIST && index2 >= neo_list_len(obj)) || index2 < 0 || (obj->type == TYPE_STRING && index2 >= strlen(neo_to_string(obj))))
        {
            CODE_ERROR = 18;
            return NULL;
        }
        
        
        if (calcListIndex)
        {
            NeObject* retour = NULL;

            if (obj->type == TYPE_LIST) {
                retour = neo_copy(neo_list_nth(obj,index2));
            }
            else {
                retour = neo_str_create(charToString(neo_to_string(obj)[index2]));
            }

            return retour;
        }
        else if (obj->type == TYPE_LIST)
        {
            NeObject* neo = neo_list_nth(obj,index2);
            update_if_promise(neo);
            // dans ce cas-là, on va devoir retourner directement le pointeur de l'élément
            return neo; // au lieu de faire une copie, on renvoie directement le bon pointeur
        }
        else {
            CODE_ERROR = 105;
            return NULL;
        }

    }

    if (tree->type == TYPE_CONTAINER)
    {
        if (!calcListIndex)
        {
            CODE_ERROR = 81; // affectation d'un champ de container littéral : illégal car -> fuite mémoire
            return NULL;
        }

        NeList* val = nelist_create(tree->nbSons);

        for (int ext_index = 0 ; ext_index < tree->nbSons ; ext_index++)
        {
            //stack_check_for(varstack, 4);

            val->tab[ext_index] = eval_prolog(tree->sons[ext_index]->sons[0], 1);

            if (CODE_ERROR != 0)
            {
                for (int k=0 ; k < ext_index ; k++) // libération de tous les éléments alloués jusqu'a i exclus
                {
                    neobject_destroy(val->tab[k], true);
                }
                err_free(val->tab);
                err_free(val);
                return NULL;
            }
        }
        NeObject* ret = neo_container_create(tree->label2, val);
        
        return ret;
    }
    
    if (tree->type == TYPE_ATTRIBUTE)
    {

        NeObject* neo = eval_prolog(tree->sons[0], calcListIndex);

        if (CODE_ERROR != 0) {
            return NULL;
        }

        if (neo->type != TYPE_CONTAINER)
        {
            // Erreur : essaie d'accéder à un champ d'une variable qui n'est pas un container
            neobject_destroy(neo, calcListIndex);
            CODE_ERROR = 80;
            return NULL;
        }


        // va chercher la valeur
        NeObject* ret;

        Container* c = (Container*)neo->data.ptr;

        int index = 0;

        if (tree->label2 != -1) // ça veut dire qu'on sait exactement où chercher la valeur
        {
            // passage optimisé
            index = tree->label2;
        }
        else
        {
            // passage complexe
            // il faut au préalable calculer l'index de où est la valeur dans le tableau
            NeList* list = (NeList*)(ATTRIBUTES->tab[c->type]->data.ptr);
            
            for (; index < list->len && strcmp(neo_to_string(list->tab[index]), tree->label1) != 0 ; index++);

            if (index == list->len - 1 && strcmp(neo_to_string(list->tab[index]), tree->label1) != 0)
            {
                CODE_ERROR = 82;
                neobject_destroy(neo, calcListIndex);
                return NULL;
            }

            tree->label2 = index;

            // pour l'affichage de l'erreur on garde ça
            //err_free(tree->label1) ; tree->label1 = NULL;
        }


        if (calcListIndex) {
            ret = neo_copy(c->data->tab[index]);
            neobject_destroy(neo, true);
        }
        else {
            NeObject* neo2 = c->data->tab[index];
            update_if_promise(neo2);
            ret = neo2;
        }
        
        return ret;
    }

    if (tree->type == TYPE_AWAIT)
    {

        if (tree->nbSons > 1)
        {
            CODE_ERROR = 101;
            return 0;
        }


        NeObject* condition = eval_prolog(tree->sons[0], 1);

        if (CODE_ERROR != 0) {
            neobject_destroy(condition, true);
            return 0;
        }

        if (neoIsTrue(condition)) { // fin de l'attente passive
            neobject_destroy(condition, true);
        }
        else
        {
            neobject_destroy(condition, true);
            
            if (atomic_counter < 0) {
                // on est dans un bloc atomique donc on peut pas passer au processus suivant
                // à cause du -1, on va retourner exécuter l'arbre d'avant cet appel, donc on restaure les arguments
                process_cycle->process->arg_tree = tree;
                process_cycle->process->arg_calcListIndex = calcListIndex;
                return (NeObject*)-1;
            }
            else {
                atomic_counter = 0;
                // à cause du -1, on va retourner exécuter l'arbre d'avant cet appel, donc on restaure les arguments
                process_cycle->process->arg_tree = tree;
                process_cycle->process->arg_calcListIndex = calcListIndex;
                return (NeObject*)-1; // on passe au processus suivant, et eval va enregistrer
                // tree et calcListIndex donc la prochaine fois que ça sera notre tour, on
                // va revenir à AWAIT comme si de rien n'était
            }
        }
        return neo_none_create();
    }
    
    if (tree->type == TYPE_STRING)
    {
        if (calcListIndex)
        {
            return neo_copy(tree->data);
        }
        else
        {
            CODE_ERROR = 89;
            return NULL;
        }
        
    }
    
    if (tree->type == TYPE_BOOL)
    {
        if (calcListIndex)
            return neo_copy(tree->data);
        else
        {
            CODE_ERROR = 89;
            return NULL;
        }
    }

    if (tree->type == TYPE_EXCEPTION)
    {
        if (calcListIndex)
            return neo_copy(tree->data);
        else
        {
            CODE_ERROR = 89;
            return NULL;
        }
    }
    
    if (tree->type == TYPE_NONE)
    {
        if (calcListIndex)
            return neo_copy(tree->data);
        else
        {
            CODE_ERROR = 89;
            return NULL;
        }
    }

    CODE_ERROR = 19;
    return NULL;
}












// cette fonction renvoie l'adresse permettant d'effectuer une affectation sur l'objet pointé
NeObj* get_address(Tree* tree) {

    if (tree->type == TYPE_VARIABLE) {
        return tree->var;
    }

    else if (tree->type == TYPE_LISTINDEX) {
        NeObject* obj = eval_prolog(tree->sons[0], 0);

        if (CODE_ERROR != 0)
        {
            return NULL;
        }


        if (CODE_ERROR != 0) {
            return NULL;
        }


        NeObject* index = eval_prolog(tree->sons[1], 1);


        if (CODE_ERROR != 0) {
            return NULL;
        }


        if (obj->type != TYPE_LIST && obj->type != TYPE_STRING)
        {
            CODE_ERROR = 15;
            return NULL;
        }

        
        if (index->type != TYPE_NUMBER)
        {
            neobject_destroy(index, true);
            CODE_ERROR = 16;
            return NULL;
        }
        
        long int index2 = (long int)number_toDouble(neo_to_nb(index));
        neobject_destroy(index,true);
        

        if (obj->type == TYPE_EMPTY)
        {
            CODE_ERROR = 17;
            return NULL;
        }
        else if ((obj->type == TYPE_LIST && index2 >= neo_list_len(obj)) || index2 < 0 || (obj->type == TYPE_STRING && index2 >= strlen(neo_to_string(obj))))
        {
            CODE_ERROR = 18;
            return NULL;
        }
        
        
        if (obj->type == TYPE_LIST)
        {
            NeList* nelist = neo_to_list(obj);
            // dans ce cas-là, on va devoir retourner directement le pointeur de l'élément
            return &nelist->tab[index2];
        }
        else {
            CODE_ERROR = 105;
            return NULL;
        }
    }

    else if (tree->type == TYPE_ATTRIBUTE) {

        NeObject* neo = eval_prolog(tree->sons[0], 1);

        if (CODE_ERROR != 0) {
            return NULL;
        }

        if (neo->type != TYPE_CONTAINER)
        {
            // Erreur : essaie d'accéder à un champ d'une variable qui n'est pas un container
            neobject_destroy(neo, 1);
            CODE_ERROR = 80;
            return NULL;
        }


        // va chercher la valeur
        NeObj* ret;

        Container* c = (Container*)neo->data.ptr;

        int index = 0;

        if (tree->label2 != -1) // ça veut dire qu'on sait exactement où chercher la valeur
        {
            // passage optimisé
            index = tree->label2;
        }
        else
        {
            // passage complexe
            // il faut au préalable calculer l'index de où est la valeur dans le tableau
            NeList* list = (NeList*)(ATTRIBUTES->tab[c->type]->data.ptr);
            
            // on cherche l'indice du nom d'attribut dans la liste d'attributs
            for (; index < list->len && strcmp(neo_to_string(list->tab[index]), tree->label1) != 0 ; index++);

            if (index == list->len - 1 && strcmp(neo_to_string(list->tab[index]), tree->label1) != 0)
            {
                CODE_ERROR = 82;
                neobject_destroy(neo, 1);
                return NULL;
            }

            tree->label2 = index;
        }


        return &c->data->tab[index];
    }

    CODE_ERROR = 19;
    return NULL;
}
























int execConditionBlock(Tree* maintree) {
    intptr_t int_ret = 0;
    int bloc = 0;
    NeObject* expr;

    // on garde un pointeur vers le début de la liste quand on est arrivés
    ptrlist* sov_vars_to_save = process_cycle->process->varsToSave->tete;

    // on a comme variables locales : int_ret, bloc, inst : d'ailleurs inst doit être empilé partout

    while (bloc < maintree->nbSons)
    {
        expr = eval_prolog(maintree->sons[bloc]->sons[0], 1);


        if (CODE_ERROR != 0) {
            return 0;
        }

        _Bool cond = neoIsTrue(expr);
        neobject_destroy(expr, true);

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


        _Bool elif = false;
        while (bloc < maintree->nbSons && maintree->sons[bloc]->type == TYPE_STATEMENTELIF)
        {

            expr = eval_prolog(maintree->sons[bloc]->sons[0], 1);

            cond = neoIsTrue(expr);
            neobject_destroy(expr, true);

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
    if (tree->sons[0]->nbSons == 4) {
        NeObject* step = eval_prolog(tree->sons[0]->sons[3], 1);

        if (CODE_ERROR != 0) {
            return 0;
        }

        incr = (int)number_toDouble(neo_to_nb(step));
        neobject_destroy(step, true);
    }
    else if (tree->sons[0]->nbSons == 3) {
        incr = 1;
    }
    else {
        CODE_ERROR = 108;
        return 0;
    }

    // on évalue la valeur de départ de la boucle
    NeObject* valeur = eval_prolog(tree->sons[0]->sons[1], 1);

    if (CODE_ERROR != 0) {
        return 0;
    }
    
    if (valeur -> type != TYPE_NUMBER)
    {
        neobject_destroy(valeur, true);
        CODE_ERROR = 10;
        return 0;
    }

    // on récupère le variant de boucle
    NeObject* var = tree->sons[0]->sons[0]->data; // variable à incrémenter lors de la boucle

    // ouverture du nouveau contexte, sauvegarde du variant
    newContext(process_cycle->process->var_loc); // nouveau contexte pour rendre des variables locales à la boucle for
    ptrlist* sov_vars_to_save = process_cycle->process->varsToSave->tete; // pour restaurer l'ancienne liste des variables à sauvegarder, une fois qu'on aura fini

    save_later(process_cycle->process->varsToSave, var);
    local(var, process_cycle->process->var_loc); // on localise l'indice de la boucle

    neobject_destroy(var, false); // on va mettre des nouveaux trucs dedans


    // le vrai indice de la boucle sera un entier
    intptr_t ext_index = (intptr_t)number_toDouble(neo_to_nb(valeur));
    neobject_destroy(valeur,true);


    NeObject* tempMax = eval_prolog(tree->sons[0]->sons[2], 1);


    if (CODE_ERROR != 0)
    {
        neobject_destroy(tempMax, true);
        deleteContext(process_cycle->process->var_loc);
        partialRestore(process_cycle->process->varsToSave, sov_vars_to_save);
        return 0;
    }

    if (tempMax -> type != TYPE_NUMBER) {
        CODE_ERROR = 10;
        neobject_destroy(tempMax, true);

        deleteContext(process_cycle->process->var_loc);
        partialRestore(process_cycle->process->varsToSave, sov_vars_to_save);
        return 0;
    }

    Number zero = number_fromDouble((double)0);
    Number tempMaxNb = neo_to_nb(tempMax);

    _Bool isinfinity = number_isInf(tempMaxNb) && number_inf(zero, tempMaxNb);
    _Bool isneginfinity = number_isInf(tempMaxNb) && number_inf(tempMaxNb, zero);

    number_destroy(zero);
    
    intptr_t max = (intptr_t)number_toDouble(neo_to_nb(tempMax)); // borne supérieure des valeurs atteintes par la variable

    neobject_destroy(tempMax, true); // du coup le data, on le garde donc on libère que l'enveloppe

    intptr_t int_ret = 0;
    
    while ((ext_index < max || isinfinity) && !isneginfinity)
    {
        // on restaure la valeur de l'indice au début du corps de la boucle
        NeObject* tempnb = neo_nb_create(number_fromDouble((double)ext_index));
        var->data = tempnb->data;
        var->type = tempnb->type;
        err_free(tempnb);

        int_ret = exec_aux(tree->sons[1]);
        neobject_destroy(var, false);

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
    neobject_reset(var);

    deleteContext(process_cycle->process->var_loc);

    // on enlève les variables qu'on avait marquées comme "à sauvegarder"
    // on enlève une à une toutes les variables qu'on avait rajoutées
    partialRestore(process_cycle->process->varsToSave, sov_vars_to_save);

    if (int_ret != BREAK)
        return int_ret;
    else
        return 0;
}




int execStatementForeachList(Tree* tree, NeObject* neo_list) {

    NeList* list = (NeList*)neo_list->data.ptr;

    NeObject* var = tree->sons[0]->sons[0]->data; // variable à incrémenter lors de la boucle

    newContext(process_cycle->process->var_loc); // nouveau contexte pour rendre des variables locales à la boucle for
    ptrlist* sov_vars_to_save = process_cycle->process->varsToSave->tete; // pour restaurer l'ancienne liste des variables à sauvegarder, une fois qu'on aura fini

    save_later(process_cycle->process->varsToSave, var);
    local(var, process_cycle->process->var_loc); // on localise l'indice de la boucle

    neobject_destroy(var, false); // on va mettre des nouveaux trucs dedans


    // l'indice qui va parcourir la liste
    intptr_t ext_index = 0;

    intptr_t int_ret = 0;
    
    while (ext_index < list->len)
    {
        // on restaure la valeur de l'indice au début du corps de la boucle
        NeObject* tempElement = neo_copy(list->tab[ext_index]);
        var->data = tempElement->data;
        var->type = tempElement->type;
        err_free(tempElement);

        int_ret = exec_aux(tree->sons[1]);
        neobject_destroy(var, false);

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
    neobject_reset(var);

    deleteContext(process_cycle->process->var_loc);

    // on enlève les variables qu'on avait marquées comme "à sauvegarder"
    // on enlève une à une toutes les variables qu'on avait rajoutées
    partialRestore(process_cycle->process->varsToSave, sov_vars_to_save);

    if (int_ret != BREAK)
        return int_ret;
    else
        return 0;
}



int execStatementForeachString(Tree* tree, NeObject* neo_string) {

    char* string = neo_to_string(neo_string);
    int string_length = strlen(string);

    NeObject* var = tree->sons[0]->sons[0]->data; // variable à incrémenter lors de la boucle

    newContext(process_cycle->process->var_loc); // nouveau contexte pour rendre des variables locales à la boucle for
    ptrlist* sov_vars_to_save = process_cycle->process->varsToSave->tete; // pour restaurer l'ancienne liste des variables à sauvegarder, une fois qu'on aura fini

    save_later(process_cycle->process->varsToSave, var);
    local(var, process_cycle->process->var_loc); // on localise l'indice de la boucle

    neobject_destroy(var, false); // on va mettre des nouveaux trucs dedans


    // l'indice qui va parcourir la liste
    intptr_t ext_index = 0;

    intptr_t int_ret = 0;
    
    while (ext_index < string_length)
    {
        // on restaure la valeur de l'indice au début du corps de la boucle
        NeObject* tempElement = neo_str_create(charToString(string[ext_index]));
        var->data = tempElement->data;
        var->type = tempElement->type;
        err_free(tempElement);

        int_ret = exec_aux(tree->sons[1]);
        neobject_destroy(var, false);

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
    neobject_reset(var);

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
                        NeObject* obj = maintree->sons[fils]->sons[0]->sons[i]->data;

                        if (obj->type != TYPE_EXCEPTION)
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
                
                if (RETURN_VALUE != NULL) // c'est pas correct, car on ne peut pas renvoyer une valeur alors que la précédente n'a pas été récupérée
                {
                    CODE_ERROR = 99;
                    return 0;
                }

                if (tree->sons[inst]->nbSons == 0) {
                    RETURN_VALUE = neo_none_create();
                    return 0;
                }
                else {
                    //stack_check_for(varstack, 3);
                    RETURN_VALUE = eval_prolog(tree->sons[inst]->sons[0], 1);
                    return EXIT_RETURN;
                }
            }

            
            else if (tree->sons[inst]->label2 == IMPORT) // action de import
            {
                for (int ext_index = 0 ; ext_index < tree->sons[inst]->nbSons ; ext_index++)
                {
                    // on empile inst et ext_index
                    //stack_check_for(varstack, 4);

                    NeObject* nom = eval_prolog(tree->sons[inst]->sons[ext_index], 1);

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

                    neobject_destroy(nom, true);

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
                    save_later(process_cycle->process->varsToSave, tree->sons[inst]->sons[i]->data);
                    local(tree->sons[inst]->sons[i]->data, process_cycle->process->var_loc);
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

            NeObject* expr = eval_prolog(tree->sons[inst]->sons[0], 1);

            if (CODE_ERROR != 0) {
                return 0;
            }

            _Bool cond = neoIsTrue(expr);
            neobject_destroy(expr, true);

            while (cond)
            {

                int_ret = exec_aux(tree->sons[inst]->sons[1]);

                if (CODE_ERROR != 0) {
                    return 0;
                }
                
                if (int_ret != 0 && int_ret != CONTINUE)
                    break;

                // réévaluation de la condition

                expr = eval_prolog(tree->sons[inst]->sons[0], 1);

                if (CODE_ERROR != 0) {
                    return 0;
                }

                cond = neoIsTrue(expr);
                neobject_destroy(expr, true);
                
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


            NeObject* iterable = eval_prolog(tree->sons[inst]->sons[0]->sons[1], 1);

            if (CODE_ERROR != 0) {
                return 0;
            }
            
            int int_ret = 0;

            if (iterable -> type == TYPE_LIST) {
                int_ret = execStatementForeachList(tree->sons[inst], iterable);
            }
            else if (iterable -> type == TYPE_STRING) {
                int_ret = execStatementForeachString(tree->sons[inst], iterable);
            }
            else {
                neobject_destroy(iterable, true);
                CODE_ERROR = 109;
                return 0;
            }

            neobject_destroy(iterable, true);

            if (CODE_ERROR != 0 || int_ret != 0) {
                return int_ret;
            }
        }

        

        else if (tree->sons[inst]->type == TYPE_FUNCTIONDEF)
        {
            // définition de fonction en cours d'exécution
            int index = strlist_index(NOMS, tree->sons[inst]->label1);
            if (ADRESSES->tab[index] != tree->sons[inst]->data)
                _affect2(ADRESSES->tab[index], tree->sons[inst]->data);

        }

        else
        {
            NeObject* res = eval_prolog(tree->sons[inst], 1);
    
            neobject_destroy(res, true); // sinon, évaluation de l'expression, en la libérant juste après

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
        neobject_destroy(process_cycle->process->original_call->data, true);
        err_free(process_cycle->process->original_call);
    }

    err_free(process_cycle->process);
    
    process_cycle->process = NULL;
    process_cycle->prev = NULL;
    process_cycle->next = NULL;

    // renvoie la promesse associée au processus principal

    PROMISES_CNT.tab[0] = 0; // normalement il ne peut pas être à autre chose que zéro, vu que personne n'a jamais
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

NeObject* eval(Tree* tree) {
    initRuntime();
    NeObject* res = eval_prolog(tree, 1);
    exitRuntime();
    return res;
}
