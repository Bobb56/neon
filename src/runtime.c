#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/cdefs.h>

#include "headers/constants.h"
#include "headers/dynarrays.h"
#include "headers/neon.h"
#include "headers/objects.h"
#include "headers/operators.h"
#include "headers/gc.h"
#include "headers/runtime.h"
#include "headers/printerror.h"
#include "headers/strings.h"
#include "headers/syntaxtrees.h"
#include "headers/lowlevel.h"
#include "headers/processcycle.h"



void updateFileName(char* name)
{
    neobject_destroy(global_env->ADRESSES->tab[global_env->NAME]);
    global_env->ADRESSES->tab[global_env->NAME] = neo_str_create(name);

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
        free(ctxt2);
    }

    else if (ctxt2->queue == NULL) // un seul élément
    {
        //-------------
        NeSave* ns = ctxt2->tete;
        replace_var(ns->var, ns->object);
        free(ns);
        //-------------

        free(ctxt2);
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
            free(ns);
            //-------------
            
            temp = ptr->queue;
            free(ptr);
            ptr = temp;
        }
        //-------------
        NeSave* ns = ptr->tete;
        replace_var(ns->var, ns->object);
        free(ns);
        //-------------
        
        free(ptr);
    }

    ptrlist_remove(var_loc, ctxt2, true); // enlève le pointeur

    return ;
}


bool isTrue(Tree* tree) {
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
    NeSave* ns = malloc(sizeof(NeSave));
    ns->object = get_var_value(var);
    ns->var = var;

    // réinitialisation de la variable
    var_reset(get_absolute_address(var));

    //ajout de la sauvegarde au contexte actuel
    
    ptrlist_append((ptrlist*)(var_loc->tete), (void*)ns);
}





__attribute__((noinline))
void neon_interp_yield(void) {
    if (global_env->atomic_counter == 0) {
        global_env->atomic_counter = global_env->ATOMIC_TIME;

        if (global_env->process_cycle->next != global_env->process_cycle) {
            // on passe au prochain processus non terminé, et on supprime tous ceux qui ont fini
            unloadCurrentProcess(global_env->process_cycle->process);
            global_env->process_cycle = loadNextLivingProcess(global_env->process_cycle);

            // on est sûrs que le processus précédent dans la chaîne était vraiment le processus précédent
            // on change les registres après les avoir sauvegardés et on change de pile

            //printf("switch from process %d to process %d\n", global_env->process_cycle->prev->process->id, global_env->process_cycle->process->id);

            switch_registers(global_env->process_cycle->process, global_env->process_cycle->prev->process);
        }
    }

    global_env->atomic_counter--;

    return;
}


__attribute__((noinline))
void neon_interp_next_process(void) {
    global_env->atomic_counter = global_env->ATOMIC_TIME;

    // on passe au prochain processus non terminé
    global_env->process_cycle = loadNextLivingProcess(global_env->process_cycle);

    // on passe sur la pile et les registres du processus suivant
    switch_registers(global_env->process_cycle->process, global_env->process_cycle->prev->process);

    // on est sur la pile d'un processus réglementaire, donc on peut supprimer les processus qui ne sont plus actifs et qui ont été pré-supprimés
    ProcessCycle_clean(global_env->process_cycle);

    return;
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
__attribute__((noinline, optimize("O0")))
void launch_process(void) {

    // pour être sûr d'avoir cleané le dernier processus qui a terminé
    ProcessCycle_clean(global_env->process_cycle);

    global_env->process_cycle->process->state = Running;

    NeObj result = eval_aux(global_env->process_cycle->process->original_call);

    //printf("End process %d\n", global_env->process_cycle->process->id);

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
NeList* treeToList(Tree* tree) {
    NeList* l = nelist_create(tree->nbSons);

    for (int index = 0 ; index < tree->nbSons ; index++)
    {
        if (tree->sons[index] != NULL) {
            l->tab[index] = eval_aux(tree->sons[index]);
            
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





__attribute__((noinline)) // cette fonction est tellement grosse que de toute façon gcc n'aurait jamais l'idée de l'inliner
NeObj eval_aux(Tree* tree) {

    neon_interp_yield();

    // il est possible qu'entre temps un processus ait lancé une erreur
    if (global_env->CODE_ERROR != 0)
        return NEO_VOID;

    // =================== EVALUATION =====================

    global_env->LINENUMBER = tree->line;

    switch (tree->type) {

        case TYPE_OPERATOR:
        {
            // il faut envoyer les opérandes calculées à la fonction opérateur
            if (tree->nbSons == 1) // operateur unaire
            {

                if (tree->label2 == 39) // opérateur parallel
                {

                    Tree* maintree = tree->sons[0];
                    
                    // crée une copie de l'arbre avec une version figée de la fonction
                    Tree* maintree_copy = (Tree*) malloc(sizeof(Tree));
                    maintree_copy->type = maintree->type;
                    maintree_copy->label1 = maintree->label1;
                    maintree_copy->label2 = maintree->label2;
                    maintree_copy->sons = maintree->sons;
                    maintree_copy->nbSons = maintree->nbSons;
                    maintree_copy->capacity = maintree->capacity;
                    maintree_copy->data = eval_aux(maintree->sons[0]);

                    if (global_env->CODE_ERROR != 0) {
                        free(maintree_copy);
                        return NEO_VOID;
                    }
                    if (NEO_TYPE(maintree_copy->data) != TYPE_USERFUNC) {
                        global_env->CODE_ERROR = 100;
                        neobject_destroy(maintree_copy->data);
                        free(maintree_copy);
                        return NEO_VOID;
                    }

                    // on ajoute le processus, et il va se faire exécuter dans la chaine de processus
                    // on met isInitialized = false pour que le processus entre dans eval_aux de manière normale
                    int id = create_new_process(maintree_copy, false);

                    return neo_promise_create(id);
                }


                // opérateur paresseux
                else if (operatorIs(tree->label2, LAZY)) {
                    NeObj (*func)(Tree*) = operators_functions[tree->label2];
                    return func(tree->sons[0]);
                }
                else {
                    // opérateur qui prend directement des objets/global_env->ADRESSES en argument

                    if (operatorIs(tree->label2, VARRIGHT) || operatorIs(tree->label2, VARLEFT))
                    {
                        NeObj* op1 = get_address(tree->sons[0]); // si la grammaire stipule que l'opérateur doit recevoir une variable et non une valeur
                        
                        if (global_env->CODE_ERROR != 0) {
                            return NEO_VOID;
                        }

                        NeObj (*func) (NeObj*) = operators_functions[tree->label2];
                        return func(op1);
                    }
                    else
                    {
                        NeObj op1 = eval_aux(tree->sons[0]);

                        if (global_env->CODE_ERROR != 0) {
                            return NEO_VOID;
                        }

                        NeObj (*func) (NeObj) = operators_functions[tree->label2];
                        NeObj retour = func(op1);
                        neobject_destroy(op1);
                        return retour;

                    }
                }
            }
            else if (tree->nbSons == 2) // operateur binaire
            {
                // opérateur paresseux
                if (operatorIs(tree->label2, LAZY)) {
                    NeObj (*func) (Tree*, Tree*) = operators_functions[tree->label2];
                    return func(tree->sons[0], tree->sons[1]);
                }
                else {
                    // opérateur normal qui prend des objets en arguments

                    if (tree->label2 == 37) { // opérateur :=
                        global_env->CODE_ERROR = 95;
                        return NEO_VOID;
                    } else if (tree->label2 == 35) { // opérateur :
                        global_env->CODE_ERROR = 92;
                        return NEO_VOID;
                    }
                                        
                    if (operatorIs(tree->label2, VAR_RIGHT))
                    {
                        NeObj* op1 = get_address(tree->sons[0]);

                        if (global_env->CODE_ERROR != 0)
                            return NEO_VOID;
                        
                        NeObj op2 = eval_aux(tree->sons[1]);

                        if (global_env->CODE_ERROR != 0)
                            return NEO_VOID;

                        NeObj (*func)(NeObj*, NeObj) = operators_functions[tree->label2];
                        NeObj result = func(op1, op2);
                        neobject_destroy(op2);
                        return result;
                    }
                    else if (operatorIs(tree->label2, LEFT_VAR))
                    {                    
                        NeObj op1 = eval_aux(tree->sons[0]);
                        
                        if (global_env->CODE_ERROR != 0)
                            return NEO_VOID;
                    
                        NeObj* op2 = get_address(tree->sons[1]);

                        if (global_env->CODE_ERROR != 0)
                            return NEO_VOID;

                        NeObj (*func)(NeObj, NeObj*) = operators_functions[tree->label2];
                        NeObj result = func(op1, op2);
                        neobject_destroy(op1);
                        return result;
                    }
                    else if (operatorIs(tree->label2, VAR_VAR))
                    {
                        NeObj* op1 = get_address(tree->sons[0]);

                        if (global_env->CODE_ERROR != 0)
                            return NEO_VOID;
                        
                        NeObj* op2 = get_address(tree->sons[1]);

                        if (global_env->CODE_ERROR != 0)
                            return NEO_VOID;

                        NeObj (*func)(NeObj*, NeObj*) = operators_functions[tree->label2];
                        NeObj result = func(op1, op2);
                        return result;
                    }
                    else
                    {
                        NeObj op1 = eval_aux(tree->sons[0]);

                        if (global_env->CODE_ERROR != 0)
                        {
                            neobject_destroy(op1);
                            return NEO_VOID;
                        }

                        NeObj op2 = eval_aux(tree->sons[1]);

                        if (global_env->CODE_ERROR != 0)
                        {
                            neobject_destroy(op1);
                            neobject_destroy(op2);
                            return NEO_VOID;
                        }
                        NeObj (*func)(NeObj, NeObj) = operators_functions[tree->label2];
                        NeObj result = func(op1, op2);

                        neobject_destroy(op1);
                        neobject_destroy(op2);

                        return result;

                    }
                }
            }
        }
        
        case TYPE_LIST:
        {
            // donc les enfants de tree sont les éléments de la liste
            NeList* l = treeToList(tree);


            if (global_env->CODE_ERROR != 0) {                   
                return NEO_VOID;
            }

            return neo_list_convert(l);
        }
        
        case TYPE_FONCTION:
        {

            // ici, appel récursif pour évaluer la fonction (1er fils), et on la met dans ->data de l'arbre


            // si le champ data de la fonction est déjà occuppé, on n'a pas besoin d'évaluer la fonction
            if (neo_is_void(tree->data)) {
                tree->data = eval_aux(tree->sons[0]);

                if (global_env->CODE_ERROR != 0) { // erreur lors de l'évaluation de la fonction
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

                if (global_env->CODE_ERROR != 0) {
                    neobject_destroy(tree->data);
                    tree->data = NEO_VOID;
                    return NEO_VOID;
                }            
                
        
                if (!funcArgsCheck(fun, args))
                {
                    global_env->CODE_ERROR = 14;
                    nelist_destroy(args);
                    neobject_destroy(tree->data); // on supprime la fonction que l'on vient de créer
                    tree->data = NEO_VOID;
                    return NEO_VOID;
                }
                    
                NeObj retour = functionCall(tree->data, args);

                nelist_destroy(args);
                neobject_destroy(tree->data); // on supprime la fonction que l'on vient de créer
                tree->data = NEO_VOID;
                
                if (global_env->CODE_ERROR != 0) {
                    neobject_destroy(retour);
                    return NEO_VOID;
                }

                global_env->LINENUMBER = tree->line; // certaines fonctions modifient le numéro de ligne

                return retour;
            }

            else if (NEO_TYPE(tree->data) == TYPE_USERFUNC || NEO_TYPE(tree->data) == TYPE_USERMETHOD)
            {
                UserFunc* fun = neo_to_userfunc(tree->data);

                NeObj tree_data_sov = tree->data;

                if (tree->sons[1]->nbSons > fun->nbArgs && ! fun->unlimited_arguments)
                {
                    global_env->CODE_ERROR = 6;
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
                        global_env->CODE_ERROR = 110;
                        nelist_destroy(arguments);
                        neobject_destroy(tree->data);
                        tree->data = NEO_VOID;
                        return NEO_VOID;
                    }
                    arguments->tab[i] = neo_copy(nelist_nth(fun->opt_args, i));
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
                            global_env->CODE_ERROR = 93;
                            return NEO_VOID;
                        }

                        while (index < fun->nbArgs && fun->args[index] != tree->sons[1]->sons[i]->sons[0]->variable) index++;

                        if (index == fun->nbArgs) {
                            nelist_destroy(arguments);
                            neobject_destroy(tree->data); // on supprime la fonction que l'on vient de créer
                            tree->data = NEO_VOID;
                            global_env->CODE_ERROR = 93;
                            return NEO_VOID;
                        }
                        
                        if (index < fun->nbArgs - fun->nbOptArgs && !neo_is_void(arguments->tab[index])) {
                            nelist_destroy(arguments);
                            neobject_destroy(tree->data); // on supprime la fonction que l'on vient de créer
                            tree->data = NEO_VOID;
                            global_env->CODE_ERROR = 94;
                            return NEO_VOID;
                        }
                        // et on évalue l'argument envoyé
                        tree->data = NEO_VOID;
                        arguments->tab[index] = eval_aux(tree->sons[1]->sons[i]->sons[1]);
                        tree->data = tree_data_sov;

                        if (global_env->CODE_ERROR != 0) {
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
                        arguments->tab[index] = eval_aux(tree->sons[1]->sons[i]);
                        tree->data = tree_data_sov;

                        if (global_env->CODE_ERROR != 0) {
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
                            arguments->tab[i] = neo_copy(nelist_nth(fun->opt_args, i));

                            if (neo_is_void(arguments->tab[i])) {
                                global_env->CODE_ERROR = 7;
                                neobject_destroy(tree->data); // on supprime la fonction que l'on vient de créer
                                tree->data = NEO_VOID;
                                nelist_destroy(arguments);
                                return NEO_VOID;
                            }

                            if (i == 0) { // le premier argument d'une méthode ne peut pas être un argument optionnel
                                global_env->CODE_ERROR = 110;
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
                        local_args->tab[i] = nelist_nth(arguments, fun->nbArgs + i);
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

                if (global_env->CODE_ERROR != 0) {
                    return NEO_VOID;
                }

                // ainsi si une return_value n'est pas à NULL c'est que forcément la valeur n'a pas été récupérée
                return ret;
                
            }

            else if (NEO_TYPE(tree->data) == TYPE_EMPTY)
            {
                global_env->CODE_ERROR = 8;
                neobject_destroy(tree->data);
                tree->data = NEO_VOID;
                return NEO_VOID;
            }
            else
            {
                global_env->CODE_ERROR = 9;
                neobject_destroy(tree->data);
                tree->data = NEO_VOID;
                return NEO_VOID;
            }
            
        }
        
        case TYPE_LISTINDEX:
        {
            NeObj obj = *get_address(tree->sons[0]);

            if (global_env->CODE_ERROR != 0)
                return NEO_VOID;

            NeObj index = eval_aux(tree->sons[1]);

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
                global_env->CODE_ERROR = 16;
                return NEO_VOID;
            }
            
            // on récupère le véritable index
            long int index2 = neo_to_integer(index);
            neobject_destroy(index);
            
            // vérifications supplémentaires
            if (NEO_TYPE(obj) == TYPE_EMPTY)
            {
                global_env->CODE_ERROR = 17;
                return NEO_VOID;
            }
            else if ((NEO_TYPE(obj) == TYPE_LIST && index2 >= neo_list_len(obj)) || index2 < 0 || (NEO_TYPE(obj) == TYPE_STRING && index2 >= strlen(neo_to_string(obj))))
            {
                global_env->CODE_ERROR = 18;
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

        case TYPE_CONTAINER:
        {

            NeList* val = nelist_create(tree->nbSons);

            for (int ext_index = 0 ; ext_index < tree->nbSons ; ext_index++)
            {
                //stack_check_for(varstack, 4);

                val->tab[ext_index] = eval_aux(tree->sons[ext_index]->sons[0]);

                if (global_env->CODE_ERROR != 0)
                {
                    nelist_destroy_until(val, ext_index - 1);
                    return NEO_VOID;
                }
            }
            NeObj ret = neo_container_create(tree->label2, val);
            
            return ret;
        }
        
        case TYPE_ATTRIBUTE:
        {

            NeObj neo = eval_aux(tree->sons[0]);

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


            // va chercher la valeur
            NeObj ret;

            Container* c = neo_to_container(neo);

            int index = 0;

            if (tree->label2 == -1) // ça veut dire qu'on sait exactement où chercher la valeur
            {
                tree->label2 = get_field_index(c, tree->label1);
                free(tree->label1);
                tree->label1 = NULL;
            }


            ret = neo_copy(get_container_field(c, tree->label2));
            neobject_destroy(neo);
            return ret;
        }


        case TYPE_VARIABLE:
        {
            NeObj value = get_var_value(tree->variable);

            if (NEO_TYPE(value) == TYPE_EMPTY) {
                global_env->CODE_ERROR = 5;
                return NEO_VOID;
            }

            return neo_copy(value);
        }

        case TYPE_DOUBLE:
        case TYPE_STRING:
        case TYPE_BOOL:
        case TYPE_EXCEPTION:
        case TYPE_NONE:
        case TYPE_INTEGER:
        {
            return neo_copy(tree->data);
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
NeObj* get_address(Tree* tree) {

    if (tree->type == TYPE_VARIABLE) {
        NeObj* addr = get_absolute_address(tree->variable);
        return addr;
    }

    else if (tree->type == TYPE_LISTINDEX) {
        NeObj obj = *get_address(tree->sons[0]);

        if (global_env->CODE_ERROR != 0)
        {
            return NULL;
        }


        if (global_env->CODE_ERROR != 0) {
            return NULL;
        }


        NeObj index = eval_aux(tree->sons[1]);


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
        
        long int index2 = neo_to_integer(index);
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

    else if (tree->type == TYPE_ATTRIBUTE) {

        NeObj neo = eval_aux(tree->sons[0]);

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


        // va chercher la valeur
        NeObj* ret;

        Container* c = neo_to_container(neo);

        int index = 0;

        if (tree->label2 == -1) // ça veut dire qu'on sait exactement où chercher la valeur
        {
            tree->label2 = get_field_index(c, tree->label1);
            free(tree->label1);
            tree->label1 = NULL;
        }


        ret = get_container_field_addr(c, tree->label2);
        neobject_destroy(neo);
        return ret;
    }

    global_env->CODE_ERROR = 89;
    return NULL;
}















int execConditionBlock(Tree* maintree) {
    intptr_t int_ret = 0;
    int bloc = 0;
    NeObj expr;

    // on garde un pointeur vers le début de la liste quand on est arrivés
    ptrlist* sov_vars_to_save = global_env->process_cycle->process->varsToSave->tete;

    // on a comme variables locales : int_ret, bloc, inst : d'ailleurs inst doit être empilé partout

    while (bloc < maintree->nbSons)
    {
        expr = eval_aux(maintree->sons[bloc]->sons[0]);


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

            int_ret = exec_aux(maintree->sons[bloc]->sons[1]);

            deleteContext(global_env->process_cycle->process->var_loc); // on vient d'exécuter le code, donc on a fini le if

            
            if (global_env->CODE_ERROR != 0) {
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

            expr = eval_aux(maintree->sons[bloc]->sons[0]);

            cond = neoIsTrue(expr);
            neobject_destroy(expr);

            if (global_env->CODE_ERROR != 0) {
                return 0;
            }


            if (!elif && cond)
            {
                // on s'apprete à exécuter du code, on peut donc ouvrir un nouveau contexte
                newContext(global_env->process_cycle->process->var_loc);

                int_ret = exec_aux(maintree->sons[bloc]->sons[1]);

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


        if (bloc < maintree->nbSons && maintree->sons[bloc]->type == TYPE_STATEMENTELSE) // s'il y a un bloc else
        {
            if (!elif) // si on a le droit d'y aller
            {
                newContext(global_env->process_cycle->process->var_loc);

                int_ret = exec_aux(maintree->sons[bloc]);

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




int execStatementFor(Tree* tree) {

    // on évalue l'incrément de la boucle
    int incr = 0;
    NeObj start;
    NeObj tempMax;

    // for(var, start, end, step)
    if (tree->sons[0]->nbSons == 4) {
        NeObj step = eval_aux(tree->sons[0]->sons[3]);

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
        start = eval_aux(tree->sons[0]->sons[1]);

        if (global_env->CODE_ERROR != 0)
            return 0;

        tempMax = eval_aux(tree->sons[0]->sons[2]);
    }

    // for(var, start, end)
    else if (tree->sons[0]->nbSons == 3) {
        incr = 1;
        // on évalue la valeur de départ de la boucle
        start = eval_aux(tree->sons[0]->sons[1]);

        if (global_env->CODE_ERROR != 0)
            return 0;

        tempMax = eval_aux(tree->sons[0]->sons[2]);
    }

    // for(var, end)
    else if (tree->sons[0]->nbSons == 2) {
        incr = 1;
        start = neo_integer_create(0);

        if (global_env->CODE_ERROR != 0)
            return 0;

        tempMax = eval_aux(tree->sons[0]->sons[1]);
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
    if (tree->sons[0]->sons[0]->type != TYPE_VARIABLE) {
        global_env->CODE_ERROR = 111;
        neobject_destroy(tempMax);
        return 0;
    }
    
    Var var = tree->sons[0]->sons[0]->variable; // variable à incrémenter lors de la boucle

    // ouverture du nouveau contexte, sauvegarde du variant
    newContext(global_env->process_cycle->process->var_loc); // nouveau contexte pour rendre des variables locales à la boucle for
    ptrlist* sov_vars_to_save = global_env->process_cycle->process->varsToSave->tete; // pour restaurer l'ancienne liste des variables à sauvegarder, une fois qu'on aura fini

    save_later(global_env->process_cycle->process->varsToSave, var);
    local(var, global_env->process_cycle->process->var_loc); // on localise l'indice de la boucle


    
    int max = neo_to_integer(tempMax); // borne supérieure des valeurs atteintes par la variable
    neobject_destroy(tempMax); // du coup le data, on le garde donc on libère que l'enveloppe

    intptr_t int_ret = 0;
    
    while (ext_index < max)
    {
        // on restaure la valeur de l'indice au début du corps de la boucle
        replace_var(var, neo_integer_create(ext_index));

        int_ret = exec_aux(tree->sons[1]);

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




int execStatementForeachList(Tree* tree, NeObj neo_list) {

    NeList* list = neo_to_list(neo_list);

    // on récupère le variant de boucle
    if (tree->sons[0]->sons[0]->type != TYPE_VARIABLE) {
        global_env->CODE_ERROR = 111;
        return 0;
    }

    Var var = tree->sons[0]->sons[0]->variable; // variable à incrémenter lors de la boucle

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

        int_ret = exec_aux(tree->sons[1]);

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



int execStatementForeachString(Tree* tree, NeObj neo_string) {

    char* string = neo_to_string(neo_string);
    int string_length = strlen(string);

    // on récupère le variant de boucle
    if (tree->sons[0]->sons[0]->type != TYPE_VARIABLE) {
        global_env->CODE_ERROR = 111;
        return 0;
    }

    Var var = tree->sons[0]->sons[0]->variable; // variable à incrémenter lors de la boucle

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

        int_ret = exec_aux(tree->sons[1]);

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





int exec_aux(Tree* tree) {

    neon_interp_yield();

    if (global_env->CODE_ERROR != 0)
        return 0;

    for (int inst=0 ; inst < tree->nbSons ; inst++)
    {
        global_env->LINENUMBER = tree->sons[inst]->line;

        switch (tree->sons[inst]->type) {

            case TYPE_TRYEXCEPT:
            {
                
                intptr_t int_ret = exec_aux(tree->sons[inst]->sons[0]);

                if (int_ret != EXIT_SUCCESS)
                    return int_ret;

                Tree* maintree = tree->sons[inst];

                // exécution du try

                if (global_env->CODE_ERROR != 1 && global_env->CODE_ERROR != 0) // exit() n'est pas considéré comme une erreur
                {
                    // on va parcourir la liste des exceptions jusqu'à en trouver une qui corresponde
                    int block_found = 0; // va correspondre à l'indice du sous arbre que l'on va exécuter

                    // parcours des différents blocs except pour trouver le bon
                    for (int fils = 1 ; fils < maintree->nbSons && block_found == 0 ; fils++) { // on arrête dès que bo correspond à un vrai sous arbre except

                        if (maintree->sons[fils]->sons[0]->nbSons == 0) {
                            block_found = fils; // si le bloc except ne spécifie aucune exception, on peut directement l'exécuter
                        }

                        for (int i = 0 ; i < maintree->sons[fils]->sons[0]->nbSons ; i++)
                        {
                            NeObj obj = maintree->sons[fils]->sons[0]->sons[i]->data;

                            if (NEO_TYPE(obj) != TYPE_EXCEPTION)
                            {
                                global_env->CODE_ERROR = 78;
                                return int_ret;
                            }
                            else
                            {
                                int code = get_exception_code(maintree->sons[fils]->sons[0]->sons[i]->data);
                                if (get_exception_from_code_error(global_env->CODE_ERROR) == code || (global_env->CODE_ERROR < 0 && -global_env->CODE_ERROR == code)) // l'erreur correspond
                                {
                                    if (global_env->CODE_ERROR < 0)
                                        global_env->EXCEPTION = NULL;
                                    
                                    block_found = fils;
                                    break;
                                }
                            }
                        }

                    }

                    if (block_found > 0) // exécution du except
                    {
                        global_env->CODE_ERROR = 0; // du coup c'est bon on repart (la police a arrêté le programme, a vérifié ses papiers, et le programme est reparti)
                        
                        //stack_check_for(varstack, 3);

                        int_ret = exec_aux(maintree->sons[block_found]->sons[1]);

                        if (global_env->CODE_ERROR != 0 || int_ret != EXIT_SUCCESS) {
                            return int_ret;
                        }
                    }
                }

                break;
            }

            case TYPE_ATOMICBLOCK:
            {
                // on va exécuter le bloc de code d'une traite, sans changer de processus
                //stack_check_for(varstack, 4);
                int temp = global_env->atomic_counter;
                global_env->atomic_counter = -1; // tant qu'on n'a pas remis une valeur positive, on ne passera pas à un autre processus
                
                intptr_t int_ret = exec_aux(tree->sons[inst]);

                // s'il reste des crédits à ce processus, on les lui rend, sinon on passe au suivant
                global_env->atomic_counter = (global_env->atomic_counter + temp > 0) ? global_env->atomic_counter + temp : 0;

                if (global_env->CODE_ERROR != 0 || int_ret != 0) {
                    return int_ret;
                }

                break;
            }
            
            case TYPE_BLOCKWORD1LINE:
            {
                if (tree->sons[inst]->label2 == RETURN) // action de return
                {
                    if (tree->sons[inst]->nbSons > 1)
                    {
                        global_env->CODE_ERROR = 21;
                        return 0;
                    }
                    
                    if (!neo_is_void(global_env->RETURN_VALUE)) // c'est pas correct, car on ne peut pas renvoyer une valeur alors que la précédente n'a pas été récupérée
                    {
                        global_env->CODE_ERROR = 99;
                        return 0;
                    }

                    if (tree->sons[inst]->nbSons == 0) {
                        global_env->RETURN_VALUE = neo_none_create();
                        return EXIT_RETURN;
                    }
                    else {
                        //stack_check_for(varstack, 3);
                        global_env->RETURN_VALUE = eval_aux(tree->sons[inst]->sons[0]);
                        return EXIT_RETURN;
                    }
                }

                
                else if (tree->sons[inst]->label2 == IMPORT) // action de import
                {
                    for (int ext_index = 0 ; ext_index < tree->sons[inst]->nbSons ; ext_index++)
                    {
                        // on empile inst et ext_index
                        //stack_check_for(varstack, 4);

                        NeObj nom = eval_aux(tree->sons[inst]->sons[ext_index]);

                        if (global_env->CODE_ERROR != 0) {
                            return 0;
                        }

                        char* nomAct = strdup(neo_to_string(nelist_nth(global_env->ADRESSES, global_env->NAME))); // pour restaurer le nom de fichier actuel

                        #ifndef TI_EZ80
                            char* nomFichier = addStr(neo_to_string(nom), ".ne");

                            updateFileName(strdup(neo_to_string(nom)));

                            importFile(nomFichier);
                            free(nomFichier);
                        #else
                            updateFileglobal_env->NAME(strdup(neo_to_string(nom)));
                            importFile(neo_to_string(nom));
                        #endif

                        updateFileName(nomAct);

                        neobject_destroy(nom);

                        if (global_env->CODE_ERROR != 0) {
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
                        global_env->CODE_ERROR = 69;
                        return 0;
                    }
                    else if (global_env->process_cycle->process->var_loc->tete == NULL)
                    {
                        global_env->CODE_ERROR = 70;
                        return 0;
                    }

                    for (int i = 0 ; i < tree->sons[inst]->nbSons ; i++) // pour toutes les variables
                    {
                        // va traiter la variable comme étant locale
                        save_later(global_env->process_cycle->process->varsToSave, tree->sons[inst]->sons[i]->variable);
                        local(tree->sons[inst]->sons[i]->variable, global_env->process_cycle->process->var_loc);
                    }
                    
                }

                else if (tree->sons[inst]->label2 == AWAIT)
                {

                    if (tree->sons[inst]->nbSons > 1)
                    {
                        global_env->CODE_ERROR = 101;
                        return 0;
                    }

                    while (!isTrue(tree->sons[inst]->sons[0]))
                    {
                        if (global_env->CODE_ERROR != 0)
                            return 0;

                        if (global_env->atomic_counter < 0) {
                            continue;
                        }
                        else {
                            global_env->atomic_counter = 0;
                            neon_interp_yield();
                        }

                    }

                    if (global_env->CODE_ERROR != 0)
                        return 0;
                }

                break;
            }

            
            
            case TYPE_KEYWORD:
            {
                if (tree->sons[inst]->label2 != PASS) {
                    return tree->sons[inst]->label2;
                }
                break;
            }

            
        
            case TYPE_CONDITIONBLOCK:
            {
                int int_ret = execConditionBlock(tree->sons[inst]);
                
                if (int_ret != 0 || global_env->CODE_ERROR != 0) {
                    return int_ret;
                }

                break;
            }

            

            case TYPE_STATEMENTWHILE:
            {
                int int_ret = 0;

                //stack_check_for(varstack, 4);
                // évaluation de la condition

                NeObj expr = eval_aux(tree->sons[inst]->sons[0]);

                if (global_env->CODE_ERROR != 0) {
                    return 0;
                }

                bool cond = neoIsTrue(expr);
                neobject_destroy(expr);

                while (cond)
                {

                    int_ret = exec_aux(tree->sons[inst]->sons[1]);

                    if (global_env->CODE_ERROR != 0) {
                        return 0;
                    }
                    
                    if (int_ret != 0 && int_ret != CONTINUE)
                        break;

                    // réévaluation de la condition

                    expr = eval_aux(tree->sons[inst]->sons[0]);

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
            
            
            case TYPE_STATEMENTFOR:
            {
                
                if (tree->sons[inst]->sons[0]->sons[0]->type != TYPE_VARIABLE)
                {
                    global_env->CODE_ERROR = 22;
                    return 0;
                }

                int int_ret = execStatementFor(tree->sons[inst]);

                if (global_env->CODE_ERROR != 0 || int_ret != 0)
                    return int_ret;

                break;
            }

            case TYPE_STATEMENTFOREACH:
            {
                
                if (tree->sons[inst]->sons[0]->sons[0]->type != TYPE_VARIABLE)
                {
                    global_env->CODE_ERROR = 22;
                    return 0;
                }

                if (tree->sons[inst]->sons[0]->nbSons != 2) {
                    global_env->CODE_ERROR = 109;
                    return 0;
                }


                NeObj iterable = eval_aux(tree->sons[inst]->sons[0]->sons[1]);

                if (global_env->CODE_ERROR != 0) {
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
                    global_env->CODE_ERROR = 109;
                    return 0;
                }

                neobject_destroy(iterable);

                if (global_env->CODE_ERROR != 0 || int_ret != 0) {
                    return int_ret;
                }
                break;
            }

            

            case TYPE_FUNCTIONDEF:
            {
                // définition de fonction en cours d'exécution

                // il faut d'abord évaluer les arguments optionnels
                NeList* opt_args = treeToList(tree->sons[inst]->sons[0]);

                // on crée la nouvelle fonction avec ces arguments optionnels
                NeObj function = userFuncDefine(tree->sons[inst]->data, opt_args);

                // et on la met dans la variable qui porte son nom
                Var var = get_var(tree->sons[inst]->label1);
                replace_var(var, function);
                break;
            }

            default:
            {
                NeObj res = eval_aux(tree->sons[inst]);
        
                neobject_destroy(res); // sinon, évaluation de l'expression, en la libérant juste après

                if (global_env->CODE_ERROR != 0)
                    return 0;

                break;
            }

        }
        

        if (global_env->CODE_ERROR != 0) { // ben oui sinon les erreurs ne seront pas captées au bon endroit
            return 0;
        }
    
    }

    return 0;
}




void initRuntime(void) {
    // on met isInitialized = true car ce processus va entrer dans eval_aux de manière normale, pas par une restauration de registres
    create_new_process(NULL, true);
}


// cette fonction doit obligatoirement avoir exactement le même contexte que launch_process
__attribute__((noinline, optimize("O0")))
void exitRuntime(void) {
    volatile NeObj unused = NEO_VOID; // sert à avoir la même taille de pile utilisée que la fonction launch_process

    global_env->PROCESS_FINISH.tab[global_env->process_cycle->process->id] = 1; // on a fini le processus, on libère la place
    *global_env->PROMISES_CNT.tab[0] = 0; // normalement il ne peut pas être à autre chose que zéro, vu que personne n'a jamais
    // récupéré de promesse sur le processus principal
    global_env->PROCESS_FINISH.tab[0] = true;

    // supprime définitivement ce dernier processus
    // supprime le processus du point de vue du runtime, mais sans vraiment libérer les ressources dans un premier temps
    process_preRemove(global_env->process_cycle->process);

    if (ProcessCycle_isActive(global_env->process_cycle)) { // il reste des processus annexes à exécuter
        //printf("saut direct à eval_aux après la fin du processus principal\n");

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



void exec(Tree* tree) {
    initRuntime();
    exec_aux(tree);
    exitRuntime();
    return;
}

NeObj eval(Tree* tree) {
    initRuntime();
    NeObj res = eval_aux(tree);
    exitRuntime();
    return res;
}
