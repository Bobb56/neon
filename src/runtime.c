#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "headers/constants.h"
#include "headers/dynarrays.h"
#include "headers/neon.h"
#include "headers/objects.h"
#include "headers/operators.h"
#include "headers/gc.h"
#include "headers/runtime.h"
#include "headers/errors.h"
#include "headers/strings.h"
#include "headers/lowlevel.h"
#include "headers/processcycle.h"
#include "headers/neonio.h"
#include "headers/trees.h"

void update__name__(char* name)
{
    replace_var(global_env->NAME, neo_str_create(name));
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
        neon_free(ctxt2);
    }

    else if (ctxt2->queue == NULL) // un seul élément
    {
        //-------------
        NeSave* ns = ctxt2->tete;
        replace_var(ns->var, ns->object);
        neon_free(ns);
        //-------------

        neon_free(ctxt2);
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
            neon_free(ns);
            //-------------
            
            temp = ptr->queue;
            neon_free(ptr);
            ptr = temp;
        }
        //-------------
        NeSave* ns = ptr->tete;
        replace_var(ns->var, ns->object);
        neon_free(ns);
        //-------------
        
        neon_free(ptr);
    }

    ptrlist_remove(var_loc, ctxt2, true); // enlève le pointeur

    return ;
}


bool isTrue(NeTree tree) {
    NeObj obj = eval_aux(tree);

    if (global_env->CODE_ERROR != 0)
        return false;

    bool b = neoIsTrue(obj);
    neobject_destroy(obj);
    return b;
}




bool neoIsTrue(NeObj expr)
{    
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
        global_env->CODE_ERROR = 20;
    }
    return false;
}




void local(Var var, ptrlist* var_loc)
{
    // sauvegarde de l'object actuel
    NeSave* ns = neon_malloc(sizeof(NeSave));
    ns->object = get_var_value(var);
    ns->var = var;

    // réinitialisation de la variable
    var_reset(get_absolute_address(var));

    //ajout de la sauvegarde au contexte actuel
    
    ptrlist_append((ptrlist*)(var_loc->tete), (void*)ns);
}




// les deux fonctions suivantes doivent obligatoirement avoir la même taille de contexte car un appel par le contexte de l'une peut
// être amené à être démonté par l'autre

NO_OPT void neon_interp_yield(void) {
    if (global_env->process_cycle->next != global_env->process_cycle) {
        // on passe au prochain processus non terminé, et on supprime tous ceux qui ont fini
        unloadCurrentProcess(global_env->process_cycle->process);
        global_env->process_cycle = loadNextLivingProcess(global_env->process_cycle);

        // on est sûrs que le processus précédent dans la chaîne était vraiment le processus précédent
        // on change les registres après les avoir sauvegardés et on change de pile

        switch_registers(global_env->process_cycle->process, global_env->process_cycle->prev->process);
    }
}


NO_OPT void neon_interp_next_process(void) {
    global_env->atomic_counter = global_env->ATOMIC_TIME;

    // on passe au prochain processus non terminé
    global_env->process_cycle = loadNextLivingProcess(global_env->process_cycle);

    // on passe sur la pile et les registres du processus suivant
    switch_registers(global_env->process_cycle->process, global_env->process_cycle->prev->process);

    // on est sur la pile d'un processus réglementaire, donc on peut supprimer les processus qui ne sont plus actifs et qui ont été pré-supprimés
    ProcessCycle_clean(global_env->process_cycle);

    return;
}




// Cette fonction est régulièrement appelée pendant l'interprétation d'un programme Neon
void interrupt(void) {
    neon_interp_yield();

    #ifdef TI_EZ80
    bool key_state = kb_On;
    kb_ClearOnLatch();
    if (key_state) {
        global_env->CODE_ERROR = 104;
        return;
    }
    #endif
}




/*
Quand on crée un processus qui n'est pas le processus principal, on met à false le champ isInitialized
Quand switch_registers va charger ce processus pour la première fois, elle va donc directement l'envoyer
dans cette fonction qui va à son tour rappeler eval_aux
Cette fonction appelle eval_aux comme il faut, avec l'adresse d'un objet en argument, etc
Quand le processus aura terminé, il retournera ici, et on pourra le supprimer dans les règles de l'art
*/


// cette fonction doit obligatoirement avoir exactement le même contexte que exitRuntime
// en effet, la fonction reset_stack_and_registers va passer au contexte de la fonction exitRuntime, donc l'épilogue de launch_process
// doit démonter correctement le contexte de exitRuntime
NO_OPT void launch_process(void) {

    // pour être sûr d'avoir cleané le dernier processus qui a terminé
    ProcessCycle_clean(global_env->process_cycle);

    global_env->process_cycle->process->state = Running;

    NeObj result = eval_aux(global_env->process_cycle->process->original_call);

    // on marque le processus comme terminé, il sera supprimé automatiquement par neon_interp_next_process
    // supprime le processus du point de vue du runtime, mais sans vraiment libérer les ressources dans un premier temps
    process_preRemove(global_env->process_cycle->process);

    // on résoud la promesse et on continue l'exécution
    // le résultat est mis ici en attente, et neo_copy se chargera de vérifier si la valeur a changé ou pas
    if (*global_env->PROMISES_CNT.tab[global_env->process_cycle->process->id] != 0) { // si c'est zéro, je sais que toutes les promesses ont été détruites avant même de les utiliser
        global_env->PROMISES->tab[global_env->process_cycle->process->id] = result;
    }
    else {
        neobject_destroy(result);
    }

    global_env->PROCESS_FINISH.tab[global_env->process_cycle->process->id] = 1; // on a fini le processus, on libère la place

    // on regarde si on doit retourner dans l'exécution ou bien quitter pour toujours
    if (ProcessCycle_isActive(global_env->process_cycle)) { // il reste des processus annexes à exécuter
        neon_interp_next_process();
    }

    // on revient sur la pile et les registres principaux sauvegardés dans exitRuntime
    // il faut faire attention à ce que cette fonction n'utilise pas de registres sauvegardés sinon reset_stack_and_registers va les changer
    reset_stack_and_registers(global_env->process_cycle->process);

    // supprime définitivement tous les processus qui restaient

    while (!ProcessCycle_isEmpty(global_env->process_cycle)) {
        global_env->process_cycle = ProcessCycle_remove(global_env->process_cycle);
    }

    return ;
}







// transforme une liste d'arbres en une liste de NeObj en les évaluant tous
NeList* treeToList(struct TreeList* tree_list) {
    NeList* l = nelist_create(tree_list->len);

    for (int index = 0 ; index < tree_list->len ; index++)
    {
        if (!NeTree_isvoid(tree_list->trees[index])) {
            l->tab[index] = eval_aux(tree_list->trees[index]);
            
            if (global_env->CODE_ERROR != 0)
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
    newContext(global_env->process_cycle->process->var_loc);

    // on sauvegarde les "variables à sauvegarder" de ce processus avant d'en ajouter d'autres
    ptrlist* sov_vars_to_save = global_env->process_cycle->process->varsToSave->tete;

    if (fun->unlimited_arguments) {
        int len = neo_list_len(neo_local_args);

        Var var = get_var("__local_args__");

        save_later(global_env->process_cycle->process->varsToSave, var);
        local(var, global_env->process_cycle->process->var_loc);
        // on lui dit de  sauvegarder cette variable avant de switcher de processus
        replace_var(var, neo_local_args);

        if (len > 0) // du coup on enlève les éléments en trop de args
            args->len = fun->nbArgs;
    }

    
    for (int i = 0 ; i < fun->nbArgs ; i++) // affectation des arguments
    {
        NeObj temp = neo_copy(nelist_nth(args, i));
        
        save_later(global_env->process_cycle->process->varsToSave, fun->args[i]);
        local(fun->args[i], global_env->process_cycle->process->var_loc);
        
        replace_var(fun->args[i], temp);
        
        if (global_env->CODE_ERROR != 0) // erreur : libération des arguments affectés
        {
            
            // libérer la sauvegarde des variables
            //deleteContext(global_env->process_cycle->process->var_loc);
            
            for (int j = 1 ; j < i ; j++)
                free_var(fun->args[j]);

            deleteContext(global_env->process_cycle->process->var_loc);
            partialRestore(global_env->process_cycle->process->varsToSave, sov_vars_to_save);

            return NEO_VOID;
        }
    }

    // on stocke fun et args
    
    int int_ret = exec_aux(fun->code);

    // on enlève les variables qu'on avait marquées comme "à sauvegarder"
    // on enlève une à une toutes les variables qu'on avait rajoutées
    deleteContext(global_env->process_cycle->process->var_loc);
    partialRestore(global_env->process_cycle->process->varsToSave, sov_vars_to_save);


    if (global_env->CODE_ERROR != 0) // erreur
        return NEO_VOID;
    
    if (int_ret == EXIT_RETURN) {
        NeObj sov = global_env->RETURN_VALUE;
        global_env->RETURN_VALUE = NEO_VOID; // pour dire que l'on l'a utilisé
        return sov;
    }
    else
        return neo_none_create();
}








NeObj callUserMethod(UserFunc* fun, NeObj* self, NeList* args, NeObj neo_local_args) {

    // ouvre un nouveau contexte pour sauvegarder les variables locales de cet appel
    newContext(global_env->process_cycle->process->var_loc);


    // on sauvegarde les "variables à sauvegarder" de ce processus avant d'en ajouter d'autres
    ptrlist* sov_vars_to_save = global_env->process_cycle->process->varsToSave->tete;

    if (fun->unlimited_arguments) {
        int len = neo_list_len(neo_local_args);

        Var var = get_var("__local_args__");

        save_later(global_env->process_cycle->process->varsToSave, var);
        local(var, global_env->process_cycle->process->var_loc);
        // on lui dit de  sauvegarder cette variable avant de switcher de processus
        
        replace_var(var, neo_local_args);

        if (len > 0) // du coup on enlève les éléments en trop de args
            args->len = fun->nbArgs;
    }
    
    

    
    for (int i = 0 ; i < fun->nbArgs ; i++) // affectation des arguments
    {
        NeObj temp = neo_copy(nelist_nth(args, i));
        
        save_later(global_env->process_cycle->process->varsToSave, fun->args[i]);
        local(fun->args[i], global_env->process_cycle->process->var_loc);
        
        replace_var(fun->args[i], temp);
        
        if (global_env->CODE_ERROR != 0) // erreur : libération des arguments affectés
        {
            
            // libérer la sauvegarde des variables
            //deleteContext(global_env->process_cycle->process->var_loc);
            
            for (int j = 1 ; j < i ; j++)
                free_var(fun->args[j]);

            deleteContext(global_env->process_cycle->process->var_loc);
            partialRestore(global_env->process_cycle->process->varsToSave, sov_vars_to_save);

            return NEO_VOID;
        }
    }

    // on stocke fun et args
    
    int int_ret = exec_aux(fun->code);

    if (global_env->CODE_ERROR != 0) {
        deleteContext(global_env->process_cycle->process->var_loc); // réaffecte les anciennes valeurs des variables qui ont été mises en local
        partialRestore(global_env->process_cycle->process->varsToSave, sov_vars_to_save);
        return NEO_VOID;
    }

    // on récupère la valeur du premier argument avant de restaurer le contexte
    NeObj object = neo_copy(get_var_value(fun->args[0]));

    // et on démonte le contexte de la fonction
    deleteContext(global_env->process_cycle->process->var_loc); // réaffecte les anciennes valeurs des variables qui ont été mises en local
    // on enlève les variables qu'on avait marquées comme "à sauvegarder"
    // on enlève une à une toutes les variables qu'on avait rajoutées
    partialRestore(global_env->process_cycle->process->varsToSave, sov_vars_to_save);

    neobject_destroy(*self);
    *self = object;
    
    if (int_ret == EXIT_RETURN) {
        NeObj sov = global_env->RETURN_VALUE;
        global_env->RETURN_VALUE = NEO_VOID; // pour dire que l'on l'a utilisé
        return sov;
    }
    else
        return neo_none_create();
}




// cette fonction est tellement grosse que de toute façon gcc n'aurait jamais l'idée de l'inliner
NO_INLINE NeObj eval_aux(NeTree tree) {

    // il est possible qu'entre temps un processus ait lancé une erreur
    if (global_env->CODE_ERROR != 0)
        return NEO_VOID;

    // =================== EVALUATION =====================

    global_env->LINENUMBER = TREE_LINE(tree);

    switch (tree.general_info->type) {

        case TypeUnaryOp:
        {

            if (tree.unary_op->op == 39) // opérateur parallel
            {

                NeTree maintree = tree.unary_op->expr;

                if (TREE_TYPE(maintree) != TypeFunctioncall) {
                    global_env->CODE_ERROR = 100;
                    return NEO_VOID;
                }

                NeObj fixed_func = eval_aux(maintree.fcall->function);

                if (global_env->CODE_ERROR != 0) {
                    return NEO_VOID;
                }

                if (NEO_TYPE(fixed_func) != TYPE_USERFUNC) {
                    global_env->CODE_ERROR = 100;
                    neobject_destroy(fixed_func);
                    return NEO_VOID;
                }
                
                // crée une copie de l'arbre avec une version figée de la fonction
                NeTree maintree_copy = NeTree_create(TypeFunctioncall, maintree.fcall->line);
                maintree_copy.fcall->args = maintree.fcall->args;
                maintree_copy.fcall->function = maintree.fcall->function;
                maintree_copy.fcall->function_obj = fixed_func;


                // on ajoute le processus, et il va se faire exécuter dans la chaine de processus
                // on met isInitialized = false pour que le processus entre dans eval_aux de manière normale
                int id = create_new_process(maintree_copy, false);

                if (global_env->CODE_ERROR != 0) {
                    neobject_destroy(maintree_copy.fcall->function_obj);
                    neon_free(maintree_copy.pointer);
                    return NEO_VOID;
                }

                return neo_promise_create(id);
            }


            // opérateur paresseux
            else if (operatorIs(tree.unary_op->op, LAZY)) {
                NeObj (*func)(NeTree) = operators_functions[tree.unary_op->op];
                return func(tree.unary_op->expr);
            }
            else {
                // opérateur qui prend directement des objets/global_env->ADRESSES en argument

                if (operatorIs(tree.unary_op->op, VARRIGHT) || operatorIs(tree.unary_op->op, VARLEFT))
                {
                    NeObj* op1 = get_address(tree.unary_op->expr); // si la grammaire stipule que l'opérateur doit recevoir une variable et non une valeur
                    
                    if (global_env->CODE_ERROR != 0) {
                        return NEO_VOID;
                    }

                    NeObj (*func) (NeObj*) = operators_functions[tree.unary_op->op];
                    return func(op1);
                }
                else
                {
                    NeObj op1 = eval_aux(tree.unary_op->expr);

                    if (global_env->CODE_ERROR != 0) {
                        return NEO_VOID;
                    }

                    NeObj (*func) (NeObj) = operators_functions[tree.unary_op->op];
                    NeObj retour = func(op1);
                    neobject_destroy(op1);
                    return retour;

                }
            }
        }

        case TypeBinaryOp: // operateur binaire
        {
            // opérateur paresseux
            if (operatorIs(tree.binary_op->op, LAZY)) {
                NeObj (*func) (NeTree, NeTree) = operators_functions[tree.binary_op->op];
                return func(tree.binary_op->left, tree.binary_op->right);
            }
            else {
                // opérateur normal qui prend des objets en arguments

                if (tree.binary_op->op == 37) { // opérateur :=
                    global_env->CODE_ERROR = 95;
                    return NEO_VOID;
                } else if (tree.binary_op->op == 35) { // opérateur :
                    global_env->CODE_ERROR = 92;
                    return NEO_VOID;
                }
                                    
                if (operatorIs(tree.binary_op->op, VAR_RIGHT))
                {
                    NeObj* op1 = get_address(tree.binary_op->left);

                    if (global_env->CODE_ERROR != 0)
                        return NEO_VOID;
                    
                    NeObj op2 = eval_aux(tree.binary_op->right);

                    if (global_env->CODE_ERROR != 0)
                        return NEO_VOID;

                    NeObj (*func)(NeObj*, NeObj) = operators_functions[tree.binary_op->op];
                    NeObj result = func(op1, op2);
                    neobject_destroy(op2);
                    return result;
                }
                else if (operatorIs(tree.binary_op->op, LEFT_VAR))
                {                    
                    NeObj op1 = eval_aux(tree.binary_op->left);
                    
                    if (global_env->CODE_ERROR != 0)
                        return NEO_VOID;
                
                    NeObj* op2 = get_address(tree.binary_op->right);

                    if (global_env->CODE_ERROR != 0)
                        return NEO_VOID;

                    NeObj (*func)(NeObj, NeObj*) = operators_functions[tree.binary_op->op];
                    NeObj result = func(op1, op2);
                    neobject_destroy(op1);
                    return result;
                }
                else if (operatorIs(tree.binary_op->op, VAR_VAR))
                {
                    NeObj* op1 = get_address(tree.binary_op->left);

                    if (global_env->CODE_ERROR != 0)
                        return NEO_VOID;
                    
                    NeObj* op2 = get_address(tree.binary_op->right);

                    if (global_env->CODE_ERROR != 0)
                        return NEO_VOID;

                    NeObj (*func)(NeObj*, NeObj*) = operators_functions[tree.binary_op->op];
                    NeObj result = func(op1, op2);
                    return result;
                }
                else
                {
                    NeObj op1 = eval_aux(tree.binary_op->left);

                    if (global_env->CODE_ERROR != 0)
                    {
                        neobject_destroy(op1);
                        return NEO_VOID;
                    }

                    NeObj op2 = eval_aux(tree.binary_op->right);

                    if (global_env->CODE_ERROR != 0)
                    {
                        neobject_destroy(op1);
                        neobject_destroy(op2);
                        return NEO_VOID;
                    }
                    NeObj (*func)(NeObj, NeObj) = operators_functions[tree.binary_op->op];
                    NeObj result = func(op1, op2);

                    neobject_destroy(op1);
                    neobject_destroy(op2);

                    return result;

                }
            }
        }
        
        case TypeList:
        {
            // donc les enfants de tree sont les éléments de la liste
            NeList* l = treeToList(&tree.syntaxtree->treelist);


            return_on_error(NEO_VOID);

            return neo_list_convert(l);
        }
        
        case TypeFunctioncall:
        {

            // ici, appel récursif pour évaluer la fonction (1er fils), et on la met dans ->data de l'arbre


            // si le champ data de la fonction est déjà occuppé, on n'a pas besoin d'évaluer la fonction
            if (neo_is_void(tree.fcall->function_obj)) {
                tree.fcall->function_obj = eval_aux(tree.fcall->function);

                if (global_env->CODE_ERROR != 0) { // erreur lors de l'évaluation de la fonction
                    return NEO_VOID;
                }
            }

            // ensuite, pour évaluer les arguments, on fait bien la boucle sur tree->sons[1]
            
            if (NEO_TYPE(tree.fcall->function_obj) == TYPE_FONCTION)
            {
                NeObj tree_data_sov = tree.fcall->function_obj;
                Function* fun = neo_to_function(tree.fcall->function_obj);

                tree.fcall->function_obj = NEO_VOID;
                NeList* args = treeToList(&tree.fcall->args);
                tree.fcall->function_obj = tree_data_sov;

                if (global_env->CODE_ERROR != 0) {
                    neobject_destroy(tree.fcall->function_obj);
                    tree.fcall->function_obj = NEO_VOID;
                    return NEO_VOID;
                }            
                
        
                if (!funcArgsCheck(fun, args))
                {
                    global_env->CODE_ERROR = 14;
                    nelist_destroy(args);
                    neobject_destroy(tree.fcall->function_obj); // on supprime la fonction que l'on vient de créer
                    tree.fcall->function_obj = NEO_VOID;
                    return NEO_VOID;
                }
                    
                NeObj retour = functionCall(tree.fcall->function_obj, args);

                nelist_destroy(args);
                neobject_destroy(tree.fcall->function_obj); // on supprime la fonction que l'on vient de créer
                tree.fcall->function_obj = NEO_VOID;
                
                if (global_env->CODE_ERROR != 0) {
                    neobject_destroy(retour);
                    return NEO_VOID;
                }

                global_env->LINENUMBER = tree.fcall->line; // certaines fonctions modifient le numéro de ligne

                return retour;
            }

            else if (NEO_TYPE(tree.fcall->function_obj) == TYPE_USERFUNC || NEO_TYPE(tree.fcall->function_obj) == TYPE_USERMETHOD)
            {
                UserFunc* fun = neo_to_userfunc(tree.fcall->function_obj);

                NeObj tree_data_sov = tree.fcall->function_obj;

                if (tree.fcall->args.len > fun->nbArgs && ! fun->unlimited_arguments)
                {
                    global_env->CODE_ERROR = 6;
                    neobject_destroy(tree.fcall->function_obj); // on supprime la fonction que l'on vient de créer
                    tree.fcall->function_obj = NEO_VOID;
                    return NEO_VOID;
                }

                

                // on compte le nombre d'arguments très facultatifs
                int given_nbOptArgs = 0;

                for (int i = 0 ; i < tree.fcall->args.len ; i++) {
                    if (TREE_TYPE(tree.fcall->args.trees[i]) == TypeBinaryOp && tree.fcall->args.trees[i].binary_op->op == 37) { // on est sur du :=
                        int index = 0;
                        while (index < fun->nbArgs && fun->args[index] != tree.fcall->args.trees[i].binary_op->left.variable->var) index++;

                        if (index >= fun->nbArgs - fun->nbOptArgs && index < fun->nbArgs)
                            given_nbOptArgs ++;
                    }
                }

                
                // on sait maintenant que le nombre d'arguments réel final est :
                // -> Si tree->nbSons > fun->nbArgs : tree->nbSons - given_nbOptArgs
                // -> Sinon, fun->nbArgs



                // calcule le nombre d'arguments de cet appel de fonction
                int nb_arguments = (fun->unlimited_arguments && tree.fcall->args.len > fun->nbArgs - fun->nbOptArgs) ? tree.fcall->args.len + fun->nbOptArgs - given_nbOptArgs : fun->nbArgs;
                
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
                    if (i == 0 && NEO_TYPE(tree.fcall->function_obj) == TYPE_USERMETHOD) {
                        global_env->CODE_ERROR = 110;
                        nelist_destroy(arguments);
                        neobject_destroy(tree.fcall->function_obj);
                        tree.fcall->function_obj = NEO_VOID;
                        return NEO_VOID;
                    }
                    arguments->tab[i] = neo_copy(nelist_nth(fun->opt_args, i));
                }


                // 1ère boucle pour placer les arguments à des endroits où ils ne bougeront pas
                for (int i = 0 ; i < tree.fcall->args.len ; i++) {
                    // on commence par remplir les éléments donnés dans le mauvais ordre (avec :=)
                    if (TREE_TYPE(tree.fcall->args.trees[i]) == TypeBinaryOp && tree.fcall->args.trees[i].binary_op->op == 37) { // on est sur du :=
                        // on regarde la variable de gauche, et on met le truc au bon endroit
                        // on récupère l'index du vrai argument dans fun->args, et on le met au bon endroit dans arguments
                        int index = 0;

                        if (TREE_TYPE(tree.fcall->args.trees[i].binary_op->left) != TypeVariable) {
                            nelist_destroy(arguments);
                            neobject_destroy(tree.fcall->function_obj); // on supprime la fonction que l'on vient de créer
                            tree.fcall->function_obj = NEO_VOID;
                            global_env->CODE_ERROR = 93;
                            return NEO_VOID;
                        }

                        while (index < fun->nbArgs && fun->args[index] != tree.fcall->args.trees[i].binary_op->left.variable->var) index++;

                        if (index == fun->nbArgs) {
                            nelist_destroy(arguments);
                            neobject_destroy(tree.fcall->function_obj); // on supprime la fonction que l'on vient de créer
                            tree.fcall->function_obj = NEO_VOID;
                            global_env->CODE_ERROR = 93;
                            return NEO_VOID;
                        }
                        
                        if (index < fun->nbArgs - fun->nbOptArgs && !neo_is_void(arguments->tab[index])) {
                            nelist_destroy(arguments);
                            neobject_destroy(tree.fcall->function_obj); // on supprime la fonction que l'on vient de créer
                            tree.fcall->function_obj = NEO_VOID;
                            global_env->CODE_ERROR = 94;
                            return NEO_VOID;
                        }
                        // et on évalue l'argument envoyé
                        tree.fcall->function_obj = NEO_VOID;
                        arguments->tab[index] = eval_aux(tree.fcall->args.trees[i].binary_op->right);
                        tree.fcall->function_obj = tree_data_sov;

                        if (global_env->CODE_ERROR != 0) {
                            nelist_destroy(arguments);
                            neobject_destroy(tree.fcall->function_obj); // on supprime la fonction que l'on vient de créer
                            tree.fcall->function_obj = NEO_VOID;
                            return NEO_VOID;
                        }
                        else if (index == 0 && NEO_TYPE(tree.fcall->function_obj) == TYPE_USERMETHOD) {
                            self = get_address(tree.fcall->args.trees[i].binary_op->right); // on vient d'évaluer le premier argument
                        }
                    }
                }

                // deuxième boucle pour insérer dans l'ordre et en évitant les := les arguments restants
                int index = 0; // on remplit petit à petit arguments avec les arguments restants
                while (index < arguments->len && !neo_is_void(arguments->tab[index])) index++;

                for (int i = 0 ; index < arguments->len && i < tree.fcall->args.len ; i++) {
                    
                    if (TREE_TYPE(tree.fcall->args.trees[i]) != TypeBinaryOp || tree.fcall->args.trees[i].binary_op->op != 37) {
                        tree.fcall->function_obj = NEO_VOID;
                        arguments->tab[index] = eval_aux(tree.fcall->args.trees[i]);
                        tree.fcall->function_obj = tree_data_sov;

                        if (global_env->CODE_ERROR != 0) {
                            nelist_destroy(arguments);
                            neobject_destroy(tree.fcall->function_obj); // on supprime la fonction que l'on vient de créer
                            tree.fcall->function_obj = NEO_VOID;
                            return NEO_VOID;
                        }
                        else if (index == 0 && NEO_TYPE(tree.fcall->function_obj) == TYPE_USERMETHOD) { // on vient d'évaluer le premier argument
                            self = get_address(tree.fcall->args.trees[i]);
                        }

                        while (index < arguments->len && !neo_is_void(arguments->tab[index])) index++;
                    }
                }
                // c'est un choix philosophique : si f attend x, y facultatif et z, alors f(1,2) ne fonctionnera pas car il manque z
                // pour mettre uniquement le x et le z, il y a une syntaxe pour le faire

                if (tree.fcall->args.len < fun->nbArgs) {
                    // à ce stade-là, certains champs de arguments sont encore vides, on va donc parcourir une dernière fois afin de lui associer les expressions définies lors de la définition de la fonction
                    for (int i = 0 ; i < fun->nbArgs ; i++) {
                        if (neo_is_void(arguments->tab[i])) { // c'est précisément le cas où l'utilisateur n'a rien mis
                            arguments->tab[i] = neo_copy(nelist_nth(fun->opt_args, i));

                            if (neo_is_void(arguments->tab[i])) {
                                global_env->CODE_ERROR = 7;
                                neobject_destroy(tree.fcall->function_obj); // on supprime la fonction que l'on vient de créer
                                tree.fcall->function_obj = NEO_VOID;
                                nelist_destroy(arguments);
                                return NEO_VOID;
                            }

                            if (i == 0) { // le premier argument d'une méthode ne peut pas être un argument optionnel
                                global_env->CODE_ERROR = 110;
                                nelist_destroy(arguments);
                                neobject_destroy(tree.fcall->function_obj);
                                tree.fcall->function_obj = NEO_VOID;
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
                        local_args->tab[i] = nelist_nth(arguments, fun->nbArgs + i);
                    }
                    neo_local_args = neo_list_convert(local_args);
                }
                

                // exécution de la fonction

                tree.fcall->function_obj = NEO_VOID;

                NeObj ret;
                if (NEO_TYPE(tree_data_sov) == TYPE_USERMETHOD)
                    ret = callUserMethod(fun, self, arguments, neo_local_args);
                else
                    ret = callUserFunc(fun, arguments, neo_local_args);

                tree.fcall->function_obj = tree_data_sov;
                neobject_destroy(tree.fcall->function_obj);
                tree.fcall->function_obj = NEO_VOID;

                nelist_destroy(arguments);

                if (global_env->CODE_ERROR != 0) {
                    return NEO_VOID;
                }

                // ainsi si une return_value n'est pas à NULL c'est que forcément la valeur n'a pas été récupérée
                return ret;
                
            }

            else if (NEO_TYPE(tree.fcall->function_obj) == TYPE_EMPTY)
            {
                global_env->CODE_ERROR = 8;
                neobject_destroy(tree.fcall->function_obj);
                tree.fcall->function_obj = NEO_VOID;
                return NEO_VOID;
            }
            else
            {
                global_env->CODE_ERROR = 9;
                neobject_destroy(tree.fcall->function_obj);
                tree.fcall->function_obj = NEO_VOID;
                return NEO_VOID;
            }
            
        }
        
        case TypeListindex:
        {
            NeObj obj = eval_aux(tree.listindex->object);

            if (global_env->CODE_ERROR != 0)
                return NEO_VOID;

            NeObj index = eval_aux(tree.listindex->index);

            if (global_env->CODE_ERROR != 0)
                return NEO_VOID;


            // on vérifie que nos objets ont bien les types attendus
            if (NEO_TYPE(obj) != TYPE_LIST && NEO_TYPE(obj) != TYPE_STRING)
            {
                global_env->CODE_ERROR = 15;
                return NEO_VOID;
            }

            if (NEO_TYPE(index) != TYPE_INTEGER)
            {
                neobject_destroy(index);
                neobject_destroy(obj);
                global_env->CODE_ERROR = 16;
                return NEO_VOID;
            }
            
            // on récupère le véritable index
            uintptr_t index2 = neo_to_integer(index);
            neobject_destroy(index);
            
            // vérifications supplémentaires
            if (NEO_TYPE(obj) == TYPE_EMPTY)
            {
                global_env->CODE_ERROR = 17;
                neobject_destroy(obj);
                return NEO_VOID;
            }
            else if ((NEO_TYPE(obj) == TYPE_LIST && index2 >= neo_list_len(obj)) || index2 < 0 || (NEO_TYPE(obj) == TYPE_STRING && index2 >= strlen(neo_to_string(obj))))
            {
                global_env->CODE_ERROR = 18;
                neobject_destroy(obj);
                return NEO_VOID;
            }
            
            
            NeObj retour = NEO_VOID;

            if (NEO_TYPE(obj) == TYPE_LIST) {
                retour = neo_copy(neo_list_nth(obj,index2));
            }
            else {
                retour = neo_str_create(charToString(neo_to_string(obj)[index2]));
            }

            neobject_destroy(obj);

            return retour;

        }

        case TypeContainerLit:
        {

            NeList* val = nelist_create(tree.container_lit->attributes.len);

            for (int i = 0 ; i < tree.container_lit->attributes.len ; i++)
            {

                val->tab[i] = eval_aux(tree.container_lit->attributes.trees[i].attribute_lit->expr);

                if (global_env->CODE_ERROR != 0)
                {
                    nelist_destroy_until(val, i - 1);
                    return NEO_VOID;
                }
            }
            NeObj ret = neo_container_create(tree.container_lit->container_type, val);
            
            return ret;
        }
        
        case TypeAttribute:
        {

            NeObj neo = eval_aux(tree.attribute->object);

            if (global_env->CODE_ERROR != 0) {
                return NEO_VOID;
            }

            if (NEO_TYPE(neo) != TYPE_CONTAINER)
            {
                // Erreur : essaie d'accéder à un champ d'une variable qui n'est pas un container
                neobject_destroy(neo);
                global_env->CODE_ERROR = 80;
                return NEO_VOID;
            }

            Container* c = neo_to_container(neo);

            // si l'indice a été invalidé
            if (tree.attribute->index == -1 || tree.attribute->last_cont_type != c->type)
            {
                tree.attribute->index = get_field_index(c, tree.attribute->name);
                neon_free(tree.attribute->name);
                tree.attribute->name = NULL;
                tree.attribute->last_cont_type = c->type;
            }

            NeObj ret = neo_copy(get_container_field(c, tree.attribute->index));
            neobject_destroy(neo);
            return ret;
        }


        case TypeVariable:
        {
            NeObj value = get_var_value(tree.variable->var);

            if (NEO_TYPE(value) == TYPE_EMPTY) {
                // CONSTEST {
                // char* name = get_name(tree.variable->var);
                // NeObj const_value = neo_const_create(strdup(name));
                // replace_var(tree.variable->var, const_value);
                // return neo_copy(const_value);
                // CONSTEST }
                global_env->CODE_ERROR = 5;
                return NEO_VOID;
            }

            return neo_copy(value);
        }

        case TypeConst:
        {
            return neo_copy(tree.const_obj->obj);
        }

        default:
        {
            global_env->CODE_ERROR = 19;
            return NEO_VOID;
        }

    }
    global_env->CODE_ERROR = 19;
    return NEO_VOID;
}












// cette fonction renvoie l'adresse de l'objet correspondant à une expression permettant d'effectuer une affectation sur l'objet pointé
NeObj* get_address(NeTree tree) {

    switch (TREE_TYPE(tree)) {
    
        case TypeVariable:
        {
            NeObj* addr = get_absolute_address(tree.variable->var);
            return addr;
        }

        case TypeListindex:
        {
            NeObj* obj_ptr = get_address(tree.listindex->object);

            if (global_env->CODE_ERROR != 0) {
                return NULL;
            }

            NeObj obj = *obj_ptr;

            NeObj index = eval_aux(tree.listindex->index);


            if (global_env->CODE_ERROR != 0) {
                return NULL;
            }


            if (NEO_TYPE(obj) != TYPE_LIST && NEO_TYPE(obj) != TYPE_STRING)
            {
                global_env->CODE_ERROR = 15;
                return NULL;
            }

            
            if (NEO_TYPE(index) != TYPE_INTEGER)
            {
                neobject_destroy(index);
                global_env->CODE_ERROR = 16;
                return NULL;
            }
            
            uintptr_t index2 = neo_to_integer(index);
            neobject_destroy(index);
            

            if (NEO_TYPE(obj) == TYPE_EMPTY)
            {
                global_env->CODE_ERROR = 17;
                return NULL;
            }
            else if ((NEO_TYPE(obj) == TYPE_LIST && index2 >= neo_list_len(obj)) || index2 < 0 || (NEO_TYPE(obj) == TYPE_STRING && index2 >= strlen(neo_to_string(obj))))
            {
                global_env->CODE_ERROR = 18;
                return NULL;
            }
            
            
            if (NEO_TYPE(obj) == TYPE_LIST)
            {
                return nelist_nth_addr(neo_to_list(obj), index2);
            }
            else {
                global_env->CODE_ERROR = 105;
                return NULL;
            }
        }

        case TypeAttribute:
        {
            NeObj neo = eval_aux(tree.attribute->object);

            if (global_env->CODE_ERROR != 0) {
                return NULL;
            }

            if (NEO_TYPE(neo) != TYPE_CONTAINER)
            {
                // Erreur : essaie d'accéder à un champ d'une variable qui n'est pas un container
                neobject_destroy(neo);
                global_env->CODE_ERROR = 80;
                return NULL;
            }

            Container* c = neo_to_container(neo);

            if (tree.attribute->index == -1 || tree.attribute->last_cont_type != c->type) // ça veut dire qu'on sait exactement où chercher la valeur
            {
                tree.attribute->index = get_field_index(c, tree.attribute->name);
                neon_free(tree.attribute->name);
                tree.attribute->name = NULL;
                tree.attribute->last_cont_type = c->type;
            }

            NeObj* ret = get_container_field_addr(c, tree.attribute->index);
            neobject_destroy(neo);
            return ret;
        }

        default:
        {
            global_env->CODE_ERROR = 89;
            return NULL;
        }
    }

    global_env->CODE_ERROR = 89;
    return NULL;
}







int execConditionBlock(NeTree maintree) {
    intptr_t int_ret = 0;
    int bloc = 0;
    NeObj expr;

    // on garde un pointeur vers le début de la liste quand on est arrivés
    ptrlist* sov_vars_to_save = global_env->process_cycle->process->varsToSave->tete;

    // on a comme variables locales : int_ret, bloc, inst : d'ailleurs inst doit être empilé partout

    while (bloc < maintree.syntaxtree->treelist.len)
    {
        expr = eval_aux(maintree.syntaxtree->treelist.trees[bloc].iew->expression);


        if (global_env->CODE_ERROR != 0) {
            return 0;
        }

        bool cond = neoIsTrue(expr);
        neobject_destroy(expr);

        if (global_env->CODE_ERROR != 0) {
            return 0;
        }

        if (cond)
        {
            // on s'apprete à exécuter du code, on peut donc ouvrir un nouveau contexte
            newContext(global_env->process_cycle->process->var_loc);

            int_ret = exec_aux(maintree.syntaxtree->treelist.trees[bloc].iew->code);

            deleteContext(global_env->process_cycle->process->var_loc); // on vient d'exécuter le code, donc on a fini le if

            
            if (global_env->CODE_ERROR != 0) {
                return 0;
            }

            if (int_ret != 0) {
                return int_ret;
            }
            
            
            bloc++;
            while (bloc < maintree.syntaxtree->treelist.len && TREE_TYPE(maintree.syntaxtree->treelist.trees[bloc]) != TypeIf)
                bloc++;

            // on vient d'exécuter le if, donc on a fini ce if, on peut supprimer le contexte

            if (bloc == maintree.syntaxtree->treelist.len) {
                break;
            }
            else
                continue;
        }
        
        bloc++;


        bool elif = false;
        while (bloc < maintree.syntaxtree->treelist.len && TREE_TYPE(maintree.syntaxtree->treelist.trees[bloc]) == TypeElif)
        {

            expr = eval_aux(maintree.syntaxtree->treelist.trees[bloc].iew->expression);

            cond = neoIsTrue(expr);
            neobject_destroy(expr);

            if (global_env->CODE_ERROR != 0) {
                return 0;
            }


            if (!elif && cond)
            {
                // on s'apprete à exécuter du code, on peut donc ouvrir un nouveau contexte
                newContext(global_env->process_cycle->process->var_loc);

                int_ret = exec_aux(maintree.syntaxtree->treelist.trees[bloc].iew->code);

                deleteContext(global_env->process_cycle->process->var_loc); // on vient d'exécuter le code, donc on a fini le if


                if (global_env->CODE_ERROR != 0) {
                    return 0;
                }

                if (int_ret != 0) {
                    return int_ret;
                }

                
                elif = true;
            }
            bloc++;
        }


        if (bloc < maintree.syntaxtree->treelist.len && TREE_TYPE(maintree.syntaxtree->treelist.trees[bloc]) == TypeElse) // s'il y a un bloc else
        {
            if (!elif) // si on a le droit d'y aller
            {
                newContext(global_env->process_cycle->process->var_loc);

                int_ret = exec_aux(maintree.syntaxtree->treelist.trees[bloc]);

                deleteContext(global_env->process_cycle->process->var_loc); // on vient d'exécuter le code, donc on a fini le if

                if (global_env->CODE_ERROR != 0) {
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

    partialRestore(global_env->process_cycle->process->varsToSave, sov_vars_to_save);

    return 0;
}




int execStatementFor(NeTree tree) {

    // on évalue l'incrément de la boucle
    int incr = 0;
    NeObj start; // la borne inférieure de la boucle
    NeObj tempMax; // la borne supérieure de la boucle

    // for(var, start, end, step)
    if (FOR_NBARGS(tree) == 4) {
        NeObj step = eval_aux(FOR_ARG(tree, 3));

        if (global_env->CODE_ERROR != 0)
            return 0;

        if (NEO_TYPE(step) != TYPE_INTEGER) {
            neobject_destroy(step);
            global_env->CODE_ERROR = 108;
            return 0;
        }

        incr = neo_to_integer(step);
        neobject_destroy(step);

        // on évalue la valeur de départ de la boucle
        start = eval_aux(FOR_ARG(tree, 1));

        if (global_env->CODE_ERROR != 0)
            return 0;

        tempMax = eval_aux(FOR_ARG(tree, 2));
    }

    // for(var, start, end)
    else if (FOR_NBARGS(tree) == 3) {
        incr = 1;
        // on évalue la valeur de départ de la boucle
        start = eval_aux(FOR_ARG(tree, 1));

        if (global_env->CODE_ERROR != 0)
            return 0;

        tempMax = eval_aux(FOR_ARG(tree, 2));
    }

    // for(var, end)
    else if (FOR_NBARGS(tree) == 2) {
        incr = 1;
        start = neo_integer_create(0);

        if (global_env->CODE_ERROR != 0)
            return 0;

        tempMax = eval_aux(FOR_ARG(tree, 1));
    }
    else {
        global_env->CODE_ERROR = 108;
        return 0;
    }

    if (global_env->CODE_ERROR != 0) { // il y a eu une erreur lors de l'évaluation du tempMax
        neobject_destroy(start);
        return 0;
    }
    
    if (NEO_TYPE(start) != TYPE_INTEGER || NEO_TYPE(tempMax) != TYPE_INTEGER)
    {
        neobject_destroy(start);
        neobject_destroy(tempMax);
        global_env->CODE_ERROR = 10;
        return 0;
    }

    // le vrai indice de la boucle sera un entier
    int ext_index = neo_to_integer(start);
    neobject_destroy(start);


    // on récupère le variant de boucle
    if (TREE_TYPE(FOR_ARG(tree, 0)) != TypeVariable) {
        global_env->CODE_ERROR = 111;
        neobject_destroy(tempMax);
        return 0;
    }
    
    Var var = FOR_ARG(tree, 0).variable->var; // variable à incrémenter lors de la boucle

    // ouverture du nouveau contexte, sauvegarde du variant
    newContext(global_env->process_cycle->process->var_loc); // nouveau contexte pour rendre des variables locales à la boucle for
    ptrlist* sov_vars_to_save = global_env->process_cycle->process->varsToSave->tete; // pour restaurer l'ancienne liste des variables à sauvegarder, une fois qu'on aura fini

    save_later(global_env->process_cycle->process->varsToSave, var);
    local(var, global_env->process_cycle->process->var_loc); // on localise l'indice de la boucle


    
    int max = neo_to_integer(tempMax); // borne supérieure des valeurs atteintes par la variable
    neobject_destroy(tempMax);

    intptr_t int_ret = 0;
    
    while (ext_index < max)
    {
        // on restaure la valeur de l'indice au début du corps de la boucle
        replace_var(var, neo_integer_create(ext_index));

        int_ret = exec_aux(tree.for_tree->block);

        if (global_env->CODE_ERROR != 0)
        {
            deleteContext(global_env->process_cycle->process->var_loc);
            partialRestore(global_env->process_cycle->process->varsToSave, sov_vars_to_save);
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


    deleteContext(global_env->process_cycle->process->var_loc);

    // on enlève les variables qu'on avait marquées comme "à sauvegarder"
    // on enlève une à une toutes les variables qu'on avait rajoutées
    partialRestore(global_env->process_cycle->process->varsToSave, sov_vars_to_save);

    if (int_ret != BREAK)
        return int_ret;
    else
        return 0;
}




int execStatementForeachList(NeTree tree, NeObj neo_list) {

    NeList* list = neo_to_list(neo_list);

    // on récupère le variant de boucle
    if (TREE_TYPE(FOR_ARG(tree, 0)) != TypeVariable) {
        global_env->CODE_ERROR = 111;
        return 0;
    }

    Var var = FOR_ARG(tree, 0).variable->var; // variable à incrémenter lors de la boucle

    newContext(global_env->process_cycle->process->var_loc); // nouveau contexte pour rendre des variables locales à la boucle for
    ptrlist* sov_vars_to_save = global_env->process_cycle->process->varsToSave->tete; // pour restaurer l'ancienne liste des variables à sauvegarder, une fois qu'on aura fini

    save_later(global_env->process_cycle->process->varsToSave, var);
    local(var, global_env->process_cycle->process->var_loc); // on localise l'indice de la boucle


    // l'indice qui va parcourir la liste
    intptr_t ext_index = 0;

    intptr_t int_ret = 0;
    
    while (ext_index < list->len)
    {
        // on restaure la valeur de l'indice au début du corps de la boucle
        replace_var(var, neo_copy(nelist_nth(list, ext_index)));

        int_ret = exec_aux(tree.for_tree->block);

        if (global_env->CODE_ERROR != 0)
        {
            deleteContext(global_env->process_cycle->process->var_loc);
            partialRestore(global_env->process_cycle->process->varsToSave, sov_vars_to_save);
            return 0;
        }

        if (int_ret != 0 && int_ret != CONTINUE)
            break;
        
        ext_index++;
    }

    deleteContext(global_env->process_cycle->process->var_loc);

    // on enlève les variables qu'on avait marquées comme "à sauvegarder"
    // on enlève une à une toutes les variables qu'on avait rajoutées
    partialRestore(global_env->process_cycle->process->varsToSave, sov_vars_to_save);

    if (int_ret != BREAK)
        return int_ret;
    else
        return 0;
}



int execStatementForeachString(NeTree tree, NeObj neo_string) {

    char* string = neo_to_string(neo_string);
    int string_length = strlen(string);

    // on récupère le variant de boucle
    if (TREE_TYPE(FOR_ARG(tree, 0)) != TypeVariable) {
        global_env->CODE_ERROR = 111;
        return 0;
    }

    Var var = FOR_ARG(tree, 0).variable->var; // variable à incrémenter lors de la boucle

    newContext(global_env->process_cycle->process->var_loc); // nouveau contexte pour rendre des variables locales à la boucle for
    ptrlist* sov_vars_to_save = global_env->process_cycle->process->varsToSave->tete; // pour restaurer l'ancienne liste des variables à sauvegarder, une fois qu'on aura fini

    save_later(global_env->process_cycle->process->varsToSave, var);
    local(var, global_env->process_cycle->process->var_loc); // on localise l'indice de la boucle



    // l'indice qui va parcourir la liste
    intptr_t ext_index = 0;

    intptr_t int_ret = 0;
    
    while (ext_index < string_length)
    {
        // on restaure la valeur de l'indice au début du corps de la boucle
        replace_var(var, neo_str_create(charToString(string[ext_index])));

        int_ret = exec_aux(tree.for_tree->block);

        if (global_env->CODE_ERROR != 0)
        {
            deleteContext(global_env->process_cycle->process->var_loc);
            partialRestore(global_env->process_cycle->process->varsToSave, sov_vars_to_save);
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

    deleteContext(global_env->process_cycle->process->var_loc);

    // on enlève les variables qu'on avait marquées comme "à sauvegarder"
    // on enlève une à une toutes les variables qu'on avait rajoutées
    partialRestore(global_env->process_cycle->process->varsToSave, sov_vars_to_save);

    if (int_ret != BREAK)
        return int_ret;
    else
        return 0;
}





int exec_aux(NeTree tree) {

    MAY_INTERRUPT();

    if (global_env->CODE_ERROR != 0)
        return 0;

    struct TreeList treelist = tree.syntaxtree->treelist;

    for (int inst=0 ; inst < treelist.len ; inst++)
    {
        global_env->LINENUMBER = TREE_LINE(treelist.trees[inst]);

        switch (TREE_TYPE(treelist.trees[inst])) {

            case TypeTryExcept:
            {
                NeTree maintree = treelist.trees[inst];

                // exécution du try
                intptr_t int_ret = exec_aux(maintree.tryexcept->try_tree);

                if (int_ret != EXIT_SUCCESS)
                    return int_ret;
                

                if (global_env->CODE_ERROR != 0) // on peut catcher un exit()
                {
                    // on réinitialise la variable de code d'erreur pour les évaluations d'exceptions
                    int sov_code_error = global_env->CODE_ERROR;
                    global_env->CODE_ERROR = 0;


                    // on va parcourir la liste des exceptions jusqu'à en trouver une qui corresponde
                    int block_found = -1; // va correspondre à l'indice du sous arbre que l'on va exécuter

                    struct TreeList except_blocks = maintree.tryexcept->except_blocks;

                    // parcours des différents blocs except pour trouver le bon
                    for (int fils = 0 ; fils < except_blocks.len && block_found == -1 ; fils++) { // on arrête dès que bo correspond à un vrai sous arbre except

                        // si le bloc except ne spécifie aucune exception, on peut directement l'exécuter
                        if (except_blocks.trees[fils].except_block->exceptions.len == 0) {
                            block_found = fils;
                            break;
                        }

                        // parcourt toutes les exceptions du bloc except
                        for (int i = 0 ; i < except_blocks.trees[fils].except_block->exceptions.len ; i++)
                        {
                            // récupère l'exception pour l'évaluer
                            NeTree exception_expr = except_blocks.trees[fils].except_block->exceptions.trees[i];
                            NeObj exception = eval_aux(exception_expr);


                            if (NEO_TYPE(exception) != TYPE_EXCEPTION)
                            {
                                global_env->CODE_ERROR = 78;
                                return int_ret;
                            }
                            else
                            {
                                int code = get_exception_code(exception);
                                if (get_exception_from_code_error(sov_code_error) == code || (sov_code_error < 0 && -sov_code_error == code)) // l'erreur correspond
                                {
                                    if (sov_code_error < 0)
                                        global_env->EXCEPTION = NULL;
                                    
                                    block_found = fils;
                                    break;
                                }
                            }
                        }

                    }

                    if (block_found != -1) // exécution du bloc except trouvé
                    {
                        int_ret = exec_aux(except_blocks.trees[block_found].except_block->block);

                        if (global_env->CODE_ERROR != 0 || int_ret != EXIT_SUCCESS) {
                            return int_ret;
                        }
                    }
                    else // restauration du code d'erreur si aucun bloc except n'est trouvé
                    {
                        global_env->CODE_ERROR = sov_code_error;
                    }
                }

                break;
            }

            case TypeAtomic:
            {
                // on va exécuter le bloc de code d'une traite, sans changer de processus
                //stack_check_for(varstack, 4);
                int temp = global_env->atomic_counter;
                global_env->atomic_counter = -1; // tant qu'on n'a pas remis une valeur positive, on ne passera pas à un autre processus
                
                intptr_t int_ret = exec_aux(treelist.trees[inst]);

                // s'il reste des crédits à ce processus, on les lui rend, sinon on passe au suivant
                global_env->atomic_counter = (global_env->atomic_counter + temp > 0) ? global_env->atomic_counter + temp : 0;

                if (global_env->CODE_ERROR != 0 || int_ret != 0) {
                    return int_ret;
                }

                break;
            }
            
            case TypeKWParam:
            {
                if (treelist.trees[inst].kwparam->code == RETURN) // action de return
                {
                    if (treelist.trees[inst].kwparam->params.len > 1)
                    {
                        global_env->CODE_ERROR = 21;
                        return 0;
                    }
                    
                    if (!neo_is_void(global_env->RETURN_VALUE)) // c'est pas correct, car on ne peut pas renvoyer une valeur alors que la précédente n'a pas été récupérée
                    {
                        global_env->CODE_ERROR = 99;
                        return 0;
                    }

                    if (treelist.trees[inst].kwparam->params.len == 0) {
                        global_env->RETURN_VALUE = neo_none_create();
                        return EXIT_RETURN;
                    }
                    else {
                        global_env->RETURN_VALUE = eval_aux(treelist.trees[inst].kwparam->params.trees[0]);

                        if (global_env->CODE_ERROR != 0) {
                            return EXIT_SUCCESS;
                        }
                        
                        return EXIT_RETURN;
                    }
                }

                
                else if (treelist.trees[inst].kwparam->code == IMPORT) // action de import
                {
                    for (int ext_index = 0 ; ext_index < treelist.trees[inst].kwparam->params.len ; ext_index++)
                    {
                        NeObj nom = eval_aux(treelist.trees[inst].kwparam->params.trees[ext_index]);

                        return_on_error(0);

                        char* nomAct = strdup(neo_to_string(nelist_nth(global_env->ADRESSES, global_env->NAME))); // pour restaurer le nom de fichier actuel
                        update__name__(strdup(neo_to_string(nom)));

                        #ifndef TI_EZ80
                            char* nomFichier = addStr(neo_to_string(nom), ".ne");

                            importFile(nomFichier);
                            neon_free(nomFichier);
                        #else
                            importFile(neo_to_string(nom));
                        #endif

                        update__name__(nomAct);

                        neobject_destroy(nom);

                        return_on_error(0);
                        
                        // en général un import fait pas mal de choses avec la mémoire, et on n'en fait pas souvent
                        // c'est donc un bon endroit pour faire un garbage collection
                        gc_mark_and_sweep();
                    }
                }

                else if (treelist.trees[inst].kwparam->code == LOCAL)
                {
                    if (treelist.trees[inst].kwparam->params.len == 0) // il faut au moins un argument
                    {
                        global_env->CODE_ERROR = 69;
                        return 0;
                    }
                    else if (global_env->process_cycle->process->var_loc->tete == NULL)
                    {
                        global_env->CODE_ERROR = 70;
                        return 0;
                    }

                    for (int i = 0 ; i < treelist.trees[inst].kwparam->params.len ; i++) // pour toutes les variables
                    {
                        // va traiter la variable comme étant locale
                        save_later(global_env->process_cycle->process->varsToSave, treelist.trees[inst].kwparam->params.trees[i].variable->var);
                        local(treelist.trees[inst].kwparam->params.trees[i].variable->var, global_env->process_cycle->process->var_loc);
                    }
                    
                }

                else if (treelist.trees[inst].kwparam->code == AWAIT)
                {

                    if (treelist.trees[inst].kwparam->params.len > 1)
                    {
                        global_env->CODE_ERROR = 101;
                        return 0;
                    }

                    while (!isTrue(treelist.trees[inst].kwparam->params.trees[0]))
                    {
                        if (global_env->CODE_ERROR != 0)
                            return 0;

                        if (global_env->atomic_counter < 0) {
                            continue;
                        }
                        else {
                            // schedule
                            neon_interp_yield();
                        }

                    }

                    if (global_env->CODE_ERROR != 0)
                        return 0;
                }

                break;
            }

            
            
            case TypeKeyword:
            {
                if (treelist.trees[inst].keyword->code != PASS) {
                    return treelist.trees[inst].keyword->code;
                }
                break;
            }

            
        
            case TypeConditionblock:
            {
                int int_ret = execConditionBlock(treelist.trees[inst]);
                
                if (int_ret != 0 || global_env->CODE_ERROR != 0) {
                    return int_ret;
                }

                break;
            }

            

            case TypeWhile:
            {
                int int_ret = 0;

                //stack_check_for(varstack, 4);
                // évaluation de la condition

                NeObj expr = eval_aux(treelist.trees[inst].iew->expression);

                if (global_env->CODE_ERROR != 0) {
                    return 0;
                }

                bool cond = neoIsTrue(expr);
                neobject_destroy(expr);

                while (cond)
                {

                    int_ret = exec_aux(treelist.trees[inst].iew->code);

                    if (global_env->CODE_ERROR != 0) {
                        return 0;
                    }
                    
                    if (int_ret != 0 && int_ret != CONTINUE)
                        break;

                    // réévaluation de la condition

                    expr = eval_aux(treelist.trees[inst].iew->expression);

                    if (global_env->CODE_ERROR != 0) {
                        return 0;
                    }

                    cond = neoIsTrue(expr);
                    neobject_destroy(expr);
                    
                }

                
                if (int_ret == EXIT_RETURN) { // en sortant d'une boucle, on n'a pas de break ou continue
                    return int_ret;
                }

                break;
            }
            
            
            case TypeFor:
            {
                
                if (TREE_TYPE(FOR_ARG(treelist.trees[inst], 0)) != TypeVariable)
                {
                    global_env->CODE_ERROR = 22;
                    return 0;
                }

                int int_ret = execStatementFor(treelist.trees[inst]);

                if (global_env->CODE_ERROR != 0 || int_ret != 0)
                    return int_ret;

                break;
            }

            case TypeForeach:
            {
                
                if (TREE_TYPE(FOR_ARG(treelist.trees[inst], 0)) != TypeVariable)
                {
                    global_env->CODE_ERROR = 22;
                    return 0;
                }

                if (FOR_NBARGS(treelist.trees[inst]) != 2) {
                    global_env->CODE_ERROR = 109;
                    return 0;
                }


                NeObj iterable = eval_aux(FOR_ARG(treelist.trees[inst], 1));

                if (global_env->CODE_ERROR != 0) {
                    return 0;
                }
                
                int int_ret = 0;

                if (NEO_TYPE(iterable) == TYPE_LIST) {
                    int_ret = execStatementForeachList(treelist.trees[inst], iterable);
                }
                else if (NEO_TYPE(iterable) == TYPE_STRING) {
                    int_ret = execStatementForeachString(treelist.trees[inst], iterable);
                }
                else {
                    neobject_destroy(iterable);
                    global_env->CODE_ERROR = 109;
                    return 0;
                }

                neobject_destroy(iterable);

                if (global_env->CODE_ERROR != 0 || int_ret != 0) {
                    return int_ret;
                }
                break;
            }

            

            case TypeFunctiondef:
            {
                // définition de fonction en cours d'exécution

                // il faut d'abord évaluer les arguments optionnels
                NeList* opt_args = treeToList(&treelist.trees[inst].functiondef->args);

                // on crée la nouvelle fonction avec ces arguments optionnels
                NeObj function = userFuncDefine(treelist.trees[inst].functiondef->object, opt_args);

                // et on la met dans la variable qui porte son nom
                Var var = get_var(treelist.trees[inst].functiondef->name);
                replace_var(var, function);
                break;
            }

            default:
            {
                NeObj res = eval_aux(treelist.trees[inst]);

                neobject_destroy(res); // sinon, évaluation de l'expression, en la libérant juste après

                return_on_error(0);

                break;
            }

        }
        

        return_on_error(0);
    
    }

    return 0;
}




void initRuntime(void) {
    // on met isInitialized = true car ce processus va entrer dans eval_aux de manière normale, pas par une restauration de registres
    create_new_process(TREE_VOID, true);
}


// cette fonction doit obligatoirement avoir exactement le même contexte que launch_process
NO_OPT void exitRuntime(void) {

    // pour avoir la même taille de pile utilisée que la fonction launch_process
    #if defined(LINUX_AMD64)
    ADD_STACK_SIZE(16);
    #elif defined(LINUX_RISCV64)
    ADD_STACK_SIZE(16);
    #elif defined(WINDOWS_AMD64)
    ADD_STACK_SIZE(32);
    #elif defined(TI_EZ80)
    ADD_STACK_SIZE(14);
    #endif

    global_env->PROCESS_FINISH.tab[global_env->process_cycle->process->id] = 1; // on a fini le processus, on libère la place
    *global_env->PROMISES_CNT.tab[0] = 0; // normalement il ne peut pas être à autre chose que zéro, vu que personne n'a jamais
    // récupéré de promesse sur le processus principal
    global_env->PROCESS_FINISH.tab[0] = true;

    // supprime définitivement ce dernier processus
    // supprime le processus du point de vue du runtime, mais sans vraiment libérer les ressources dans un premier temps
    process_preRemove(global_env->process_cycle->process);

    if (ProcessCycle_isActive(global_env->process_cycle)) { // il reste des processus annexes à exécuter

        // ici, on sauvegarde les registres sauvegardés et la pile dans le processus actuel
        // comme ça quand on va passer au prochain processus, ils vont être transférés de processus en processus et restaurés à la fin
        save_stack_and_registers(global_env->process_cycle->process);

        neon_interp_next_process();
    }

    while (!ProcessCycle_isEmpty(global_env->process_cycle)) {
        global_env->process_cycle = ProcessCycle_remove(global_env->process_cycle);
    }

    return;
}



void exec(NeTree tree) {
    initRuntime();
    exec_aux(tree);
    exitRuntime();
    return;
}

NeObj eval(NeTree tree) {
    initRuntime();
    NeObj res = eval_aux(tree);
    exitRuntime();
    return res;
}
