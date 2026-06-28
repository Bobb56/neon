#include "headers/interpcontext.h"
#define NEON_SOURCE_ID 17

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
#include "headers/trees.h"
#include "headers/contexts.h"
#include "headers/nativefunctions.h"
#include "headers/neonio.h"


void update__name__(char* name)
{
    replace_var(global_env->NAME, neo_str_create(name));
    return ;
}





bool isTrue(TreeBuffer* tb, TreeBufferIndex tree) {
    NeObj obj = eval(tb, tree);
    return_on_error(false);

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
        neon_fail(20, neo_copy(expr));
    }
    return false;
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


void stop_execution(void) {
    strlist options = (strlist) {
        .tab = (char*[]) {
            "Stop current running program",
            "Open interactive console on current environment"
        },
        .len = 2,
        .capacity = 1
    };
    int choice = get_option(
        "You interrupted the execution. What do you want to do ?",
        "Enter your choice: ",
        &options
    );

    if (choice == 2) {
        // Initial message
        newLine();

        setColor(PURPLE);
        printString("Welcome to the interactive console.\n");

        setColor(DEFAULT);
        printString("Type ");

        setColor(GREEN); printString("exit()"); setColor(DEFAULT);
        printString(" to resume execution.\n\n");

        // Print where we stopped the execution
        neon_fail(104, NO_ARGS);
        printError();
        newLine();
        
        // Launch the terminal
terminal:
        terminal();

        // Ask before exiting
        strlist options = (strlist) {.tab = (char*[]) {"Yes", "No"}, .len = 2, .capacity = 1};
        int choice = get_option(
            "Do you really want to resume execution ?",
            "Enter your choice: ",
            &options
        );

        if (choice == 2)
            goto terminal;

        setColor(PURPLE);
        printString("You exited the interactive console.\n");
        setColor(DEFAULT);
    }
    else {
        neon_fail(104, NO_ARGS);
    }
}



// Cette fonction est régulièrement appelée pendant l'interprétation d'un programme Neon
void interrupt(void) {
    if (global_env->INTERRUPT) {
        //global_env->INTERRUPT = false;
        //stop_execution();
        neon_fail(104, NO_ARGS);
    }

    neon_interp_yield();

    #ifdef TI_EZ80
    bool key_state = kb_On;
    kb_ClearOnLatch();
    if (key_state) {
        global_env->INTERRUPT = true;
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

    treeFCall(global_env->process_cycle->process->original_call_buffer, global_env->process_cycle->process->original_call)->function_obj = neo_copy(global_env->process_cycle->process->fixed_function);
    NeObj result = eval(global_env->process_cycle->process->original_call_buffer, global_env->process_cycle->process->original_call);

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
void treeToList(NeList* l, TreeBuffer* tb, TreeBufferIndex tree_list) {
    size_t tree_list_length = treelistLength(tb, tree_list);
    nelist_init(l, tree_list_length);

    for (int index = 0 ; index < (int)tree_list_length ; index++)
    {
        if (!TREE_ISVOID(treelistGet(tb, tree_list)[index])) {
            l->tab[index] = eval(tb, treelistGet(tb, tree_list)[index]);
            
            if_error
            {
                // si y a eu un problème dans l'évaluation d'un argument, on doit libérer toute la liste créée jusqu'alors
                nelist_deinit_until(l, index - 1);
                return;
            }
        }
        else {
            l->tab[index] = NEO_VOID;
        }
    }
}



// Cette fonction appelle une UserFunc sur les arguments [values] associés aux variables de même indice dans le tableau [variables]. [nb_arguments] correspond au premier indice libre dans les tableaux, et les tableaux doivent être de longueur [fun->nbArgs]
// La fonction assigne l'objet [local_args] à une vraie variable s'il est différent de NEO_VOID, et met l'objet final de la méthode dans [self_final_value] SANS REMPLACER LA PRECEDENTE VALEUR
// L'affectation à la vraie adresse de l'objet doit être faite manuellement après avoir appelé callUserFunc
NeObj callUserFunc(UserFunc* fun, Var* variables, NeObj* values, int variable_index, NeObj local_args, NeObj* self_final_value) {
    // Remplissage du tableau avec les valeurs par défaut des arguments optionnels
    for (int i=0 ; variable_index < fun->nbArgs && i < fun->nbArgs ; i++) {
        // Cette variable a un argument par défaut donc on regarde si l'argument a été donné
        if (!neo_is_void(fun->opt_args.tab[i])) {
            bool has_already_been_given = false;
            for (int j=0 ; j < variable_index ; j++) {
                if (variables[j] == fun->args[i]) {
                    has_already_been_given = true;
                    break;
                }
            }

            if (!has_already_been_given) {
                variables[variable_index] = fun->args[i];
                values[variable_index] = neo_copy(fun->opt_args.tab[i]);
                variable_index++;
            }
        }
    }

    // Pas assez d'arguments : même en ayant rempli avec les arguments optionnels il y en a moins que attendu
    if (variable_index < fun->nbArgs) {
        neon_fail(7, neo_integer_create(fun->nbArgs), neo_integer_create(variable_index));
        return NEO_VOID;
    }


    // Affectation des arguments

    // Ouverture du nouveau contexte
    newContext(&global_env->process_cycle->process->var_loc);
    // on sauvegarde les "variables à sauvegarder" de ce processus avant d'en ajouter d'autres
    CapturedVarsCheckPoint cp = CapturedVars_get_checkpoint(&global_env->process_cycle->process->varsToSave);

    // Affectation des valeurs de values dans les variables de variables
    for (int var_index = 0 ; var_index < fun->nbArgs ; var_index++) {
        Var variable = variables[var_index];
        Process* proc = global_env->process_cycle->process;

        if (!isLocal(variable, &proc->var_loc)) {
            local(variable, proc);
            replace_var(variable, values[var_index]);
        }
        else {
            neon_fail(94, neo_new_str_create(get_name(variable)));
            return NEO_VOID;
        }
    }

    // Affectation de __local_args__
    if (fun->unlimited_arguments) {
        Var var = get_local_args();
        Process* proc = global_env->process_cycle->process;

        if (!isLocal(var, &proc->var_loc)) {
            local(var, global_env->process_cycle->process);
            replace_var(var, neo_copy(local_args));
        }
    }

    fun->runningInstances += 1;
    int ret_code = exec(fun->tree_buffer, fun->code);
    fun->runningInstances -= 1;

    if_error {
        deleteContext(&global_env->process_cycle->process->var_loc);
        partialRestore(&global_env->process_cycle->process->varsToSave, cp);
        return NEO_VOID;
    }


    // Si la fonction est une méthode on va modifier en place l'objet
    if (fun->isMethod)
        *self_final_value = neo_copy(get_var_value(fun->args[0]));


    // et on démonte le contexte de la fonction
    deleteContext(&global_env->process_cycle->process->var_loc); // réaffecte les anciennes valeurs des variables qui ont été mises en local
    // on enlève les variables qu'on avait marquées comme "à sauvegarder"
    // on enlève une à une toutes les variables qu'on avait rajoutées
    partialRestore(&global_env->process_cycle->process->varsToSave, cp);

    if (ret_code == EXIT_RETURN) {
        NeObj sov = global_env->RETURN_VALUE;
        global_env->RETURN_VALUE = NEO_VOID; // pour dire que l'on l'a utilisé
        return sov;
    }
    else
        return neo_none_create();
}



// eval_aux suppose que les TreeBuffers donnés en arguments sont verrouillés (ou en tous cas ne peuvent
// plus être relocalisés)
NeObj eval_aux(TreeBuffer* tb, TreeBufferIndex tree) {
    global_env->LINENUMBER = TREE_LINE(tb, tree);

    switch (TREE_TYPE(tb, tree)) {

        case TypeUnaryOp:
        {
            struct UnaryOp* tree_un_op = treeUnOp(tb, tree);
            // opérateur paresseux
            if (operatorIs(tree_un_op->op, LAZY)) {
                NeObj (*func)(TreeBuffer*, TreeBufferIndex) = operators_functions[tree_un_op->op];
                return func(tb, tree_un_op->expr);
            }
            else {
                // opérateur qui prend directement des objets/global_env->ADRESSES en argument

                if (operatorIs(tree_un_op->op, VARRIGHT) || operatorIs(tree_un_op->op, VARLEFT))
                {
                    NeObj* op1 = get_address(tb, tree_un_op->expr); // si la grammaire stipule que l'opérateur doit recevoir une variable et non une valeur
                    
                    return_on_error(NEO_VOID);

                    NeObj (*func) (NeObj*) = operators_functions[tree_un_op->op];
                    return func(op1);
                }
                else
                {
                    NeObj op1 = eval(tb, tree_un_op->expr);

                    return_on_error(NEO_VOID);

                    NeObj (*func) (NeObj) = operators_functions[tree_un_op->op];
                    NeObj retour = func(op1);
                    neobject_destroy(op1);
                    return retour;

                }
            }
        }

        case TypeBinaryOp: // operateur binaire
        {
            struct BinaryOp* tree_bin_op = treeBinOp(tb, tree);

            // opérateur paresseux
            if (operatorIs(tree_bin_op->op, LAZY)) {
                NeObj (*func) (TreeBuffer*, TreeBufferIndex, TreeBufferIndex) = operators_functions[tree_bin_op->op];
                return func(tb, tree_bin_op->left, tree_bin_op->right);
            }
            else {
                // opérateur normal qui prend des objets en arguments

                if (tree_bin_op->op == 37) { // opérateur :=
                    neon_fail(95, NO_ARGS);
                    return NEO_VOID;
                } else if (tree_bin_op->op == 35) { // opérateur :
                    neon_fail(92, NO_ARGS);
                    return NEO_VOID;
                }
                                    
                if (operatorIs(tree_bin_op->op, VAR_RIGHT))
                {
                    NeObj* op1 = get_address(tb, tree_bin_op->left);

                    return_on_error(NEO_VOID);
                    
                    NeObj op2 = eval(tb, tree_bin_op->right);

                    return_on_error(NEO_VOID);

                    NeObj (*func)(NeObj*, NeObj) = operators_functions[tree_bin_op->op];
                    NeObj result = func(op1, op2);
                    neobject_destroy(op2);
                    return result;
                }
                else if (operatorIs(tree_bin_op->op, LEFT_VAR))
                {                    
                    NeObj op1 = eval(tb, tree_bin_op->left);
                    
                    return_on_error(NEO_VOID);
                
                    NeObj* op2 = get_address(tb, tree_bin_op->right);

                    return_on_error(NEO_VOID);

                    NeObj (*func)(NeObj, NeObj*) = operators_functions[tree_bin_op->op];
                    NeObj result = func(op1, op2);
                    neobject_destroy(op1);
                    return result;
                }
                else if (operatorIs(tree_bin_op->op, VAR_VAR))
                {
                    NeObj* op1 = get_address(tb, tree_bin_op->left);

                    return_on_error(NEO_VOID);
                    
                    NeObj* op2 = get_address(tb, tree_bin_op->right);

                    return_on_error(NEO_VOID);

                    NeObj (*func)(NeObj*, NeObj*) = operators_functions[tree_bin_op->op];
                    NeObj result = func(op1, op2);
                    return result;
                }
                else
                {
                    NeObj op1 = eval(tb, tree_bin_op->left);

                    if_error
                    {
                        neobject_destroy(op1);
                        return NEO_VOID;
                    }

                    NeObj op2 = eval(tb, tree_bin_op->right);

                    if_error
                    {
                        neobject_destroy(op1);
                        neobject_destroy(op2);
                        return NEO_VOID;
                    }
                    NeObj (*func)(NeObj, NeObj) = operators_functions[tree_bin_op->op];
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
            NeList* l = neon_malloc(sizeof(NeList));
            treeToList(l, tb, treeSntxTree(tb, tree)->treelist);
            return_on_error(NEO_VOID);

            return neo_list_convert(l);
        }


        case TypeParallelCall:
        {
            TreeBuffer* tree_buffer = treeParCall(tb, tree)->expr_buffer;
            TreeBufferIndex maintree = treeParCall(tb, tree)->expr;
                
            if (TREE_TYPE(tree_buffer, maintree) != TypeFunctioncall) {
                neon_fail(41, NO_ARGS);
                return NEO_VOID;
            }

            NeObj fixed_func = eval(tree_buffer, treeFCall(tree_buffer, maintree)->function);

            return_on_error(NEO_VOID);

            if (NEO_TYPE(fixed_func) != TYPE_USERFUNC) {
                neon_fail(100, neo_new_const_create(type(fixed_func)));
                neobject_destroy(fixed_func);
                return NEO_VOID;
            }

            // on ajoute le processus, et il va se faire exécuter dans la chaine de processus
            // on met isInitialized = false pour que le processus entre dans eval_aux de manière normale
            int id = create_new_process(tree_buffer, maintree, fixed_func, false, treeParCall(tb, tree)->interp_context);

            if_error {
                neobject_destroy(fixed_func);
                return NEO_VOID;
            }

            return neo_promise_create(id);
        }
        
        case TypeFunctioncall:
        {
            struct FunctionCall* tree_fun_call = treeFCall(tb, tree);

            // ici, appel récursif pour évaluer la fonction (1er fils), et on la met dans ->data de l'arbre
            NeObj function;

            // si le champ data de la fonction est déjà occuppé, on n'a pas besoin d'évaluer la fonction
            if (neo_is_void(tree_fun_call->function_obj)) {
                function = eval(tb, tree_fun_call->function);
                return_on_error(NEO_VOID);
            }
            else {
                function = tree_fun_call->function_obj;
                tree_fun_call->function_obj = NEO_VOID;
            }

            // Si on essaie d'exécuter un container ça va exécuter la fonction appellée Container~call (où Container est le type de container)
            if (NEO_TYPE(function) == TYPE_CONTAINER) {
                Container* c = neo_to_container(function);
                // on regarde si l'appel est surchargé
                char* container_name = global_env->CONTAINERS->tab[c->type];
                int index = function_module(container_name, "call");

                if (index != -1) {
                    UserFunc* overloaded_fun = neo_to_userfunc(global_env->ADRESSES->tab[index]);
                    // La fonction function_module vérifie déjà que l'objet est de type USERFUNC
                    /*if (NEO_TYPE(function) != TYPE_USERFUNC) {
                        neon_fail(52,
                            neo_new_str_create(global_env->NOMS->tab[index]),
                            neo_new_str_create("call"),
                            neo_new_str_create(container_name),
                            neo_new_str_create(global_env->NOMS->tab[index])
                        );
                        return NEO_VOID;
                    }*/
                    if (overloaded_fun->nbArgs < 2) {
                        neon_fail(53,
                            neo_new_str_create(global_env->NOMS->tab[index]),
                            neo_integer_create(2)
                        );
                        return NEO_VOID;
                    }

                    // Get the arguments of the call
                    NeList* args = neon_malloc(sizeof(NeList));
                    treeToList(args, tb, tree_fun_call->args);
                    NeObj neo_args = neo_list_convert(args);

                    NeObj result = callBinaryUserFunc(overloaded_fun, function, neo_args);
                    neobject_destroy(neo_args);
                    neobject_destroy(function);
                    return result;
                }
            }

            if (NEO_TYPE(function) == TYPE_BUILTINFUNC) {
                Function* fun = neo_to_function(function);
                NeList args;
                treeToList(&args, tb, tree_fun_call->args);

                if_error {
                    neobject_destroy(function);
                    return NEO_VOID;
                }
                
        
                if (!funcArgsCheck(fun, &args))
                {
                    const char* fname = get_function_name(fun->id, fun->module);
                    neon_fail(38, neo_new_const_create(fname), neo_new_const_create(fname));
                    nelist_deinit(&args);
                    neobject_destroy(function); // on supprime la fonction que l'on vient de créer
                    return NEO_VOID;
                }
                
                NeObj retour = functionCall(function, &args);

                nelist_deinit(&args);
                neobject_destroy(function); // on supprime la fonction que l'on vient de créer
                
                if_error {
                    neobject_destroy(retour);
                    return NEO_VOID;
                }

                global_env->LINENUMBER = tree_fun_call->line; // certaines fonctions modifient le numéro de ligne

                return retour;
            }

            else if (NEO_TYPE(function) == TYPE_USERFUNC)
            {
                // TODO : Gérer les erreurs pas assez d'arguments, trop d'arguments, multiple affectation d'arguments...
                UserFunc* fun = neo_to_userfunc(function);

                int tree_fun_call_nb_args = treelistLength(tb, tree_fun_call->args);
                TreeBufferIndex* arg_tree = treelistGet(tb, tree_fun_call->args);

                if (tree_fun_call_nb_args > fun->nbArgs && ! fun->unlimited_arguments) {
                    neon_fail(6, neo_integer_create(fun->nbArgs), neo_integer_create(tree_fun_call_nb_args));
                    neobject_destroy(function); // on supprime la fonction que l'on vient de créer
                    return NEO_VOID;
                }

                // Définition de __local_args__ dans le cas où la fonction peut prendre un nombre illimité d'arguments
                NeObj local_args = NEO_VOID;
                if (fun->unlimited_arguments) {
                    local_args = neo_list_create(0);
                }

                // On va stocker les arguments dans des tableaux avant de les affecter afin de ne pas mélanger les contextes : évaluation dans le contexte extérieur, puis affectation dans le contexte intérieur
                Var* variables = neon_malloc(sizeof(Var) * fun->nbArgs);
                NeObj* values = neon_malloc(sizeof(NeObj) * fun->nbArgs);
                int variable_index = 0;

                // Association valeurs/variables

                fun->runningInstances += 1;
                for (int i = 0 ; i < tree_fun_call_nb_args ; i++) {
                    // Si il s'agit d'un argument nommé, on affecte directement la variable
                    if (TREE_TYPE(tb, arg_tree[i]) == TypeBinaryOp && treeBinOp(tb, arg_tree[i])->op == 37) {
                        variables[variable_index] = treeVar(tb, treeBinOp(tb, arg_tree[i])->left)->var;
                        values[variable_index] = eval(tb, treeBinOp(tb, arg_tree[i])->right);
                        variable_index++;
                    }
                    // Il s'agit d'un argument normal non nommé (c'est à dire pas un argument ayant sa place dans __local_args__)
                    else if (i < fun->nbArgs - fun->nbOptArgs) {
                        variables[variable_index] = fun->args[i];
                        values[variable_index] = eval(tb, arg_tree[i]);
                        variable_index++;
                    }
                    // Il s'agit d'un argument qui doit aller dans __local_args__
                    else if (!fun->unlimited_arguments) {
                        neon_fail(42, neo_integer_create(fun->nbArgs - fun->nbOptArgs));
                        fun->runningInstances -= 1;
                        neobject_destroy(function);
                        neon_free(variables);
                        neon_free(values);
                        return NEO_VOID;
                    }
                    else {
                        neo_list_append(local_args, eval(tb, arg_tree[i]));
                    }
                }
                fun->runningInstances -= 1;

                NeObj self_final_value;
                NeObj return_value = callUserFunc(fun, variables, values, variable_index, local_args, &self_final_value);

                neobject_destroy(local_args);
                neon_free(variables);
                neon_free(values);

                if_error {
                    neobject_destroy(function);
                    return NEO_VOID;
                }

                if (fun->isMethod) {
                    NeObj* self_address = get_address(tb, arg_tree[0]);
                    neobject_destroy(*self_address);
                    *self_address = self_final_value;
                }

                neobject_destroy(function);

                return return_value;
            }

            else
            {
                neon_fail(9, neo_new_const_create(type(function)));
                neobject_destroy(function);
                return NEO_VOID;
            }
            
        }
        
        case TypeListindex:
        {
            struct ListIndex* tree_list_index = treeLstIndx(tb, tree);

            NeObj obj = eval(tb, tree_list_index->object);

            return_on_error(NEO_VOID);

            NeObj index = eval(tb, tree_list_index->index);

            return_on_error(NEO_VOID);


            // on vérifie que nos objets ont bien les types attendus
            if (NEO_TYPE(obj) != TYPE_LIST && NEO_TYPE(obj) != TYPE_STRING)
            {
                neon_fail(15, neo_copy(obj));
                return NEO_VOID;
            }

            if (NEO_TYPE(index) != TYPE_INTEGER)
            {
                neon_fail(16, neo_copy(index));
                neobject_destroy(index);
                neobject_destroy(obj);
                return NEO_VOID;
            }
            
            // on récupère le véritable index
            intptr_t index2 = neo_to_integer(index);
            neobject_destroy(index);

            int object_length = (NEO_TYPE(obj) == TYPE_LIST) ? neo_list_len(obj) : strlen(neo_to_string(obj));

            if (index2 < 0 || index2 >= object_length)
            {
                neon_fail(39, neo_integer_create(index2), neo_integer_create(object_length));
                neobject_destroy(obj);
                return NEO_VOID;
            }
            
            
            NeObj retour = NEO_VOID;

            if (NEO_TYPE(obj) == TYPE_LIST) {
                retour = neo_copy(neo_list_nth(obj,(size_t)index2));
            }
            else {
                retour = neo_str_create(charToString(neo_to_string(obj)[index2]));
            }

            neobject_destroy(obj);

            return retour;

        }

        case TypeContainerLit:
        {
            struct ContainerLit* tree_cont_lit = treeContLit(tb, tree);
            size_t nb_attributes = treelistLength(tb, tree_cont_lit->attributes);
            NeList val;
            nelist_init(&val, nb_attributes);

            for (size_t i = 0 ; i < nb_attributes ; i++)
            {

                val.tab[i] = eval(tb, treeAttrLit(tb, treelistGet(tb, tree_cont_lit->attributes)[i])->expr);

                if_error
                {
                    nelist_deinit_until(&val, i - 1);
                    return NEO_VOID;
                }
            }
            NeObj ret = neo_container_create(tree_cont_lit->container_type, val);
            
            return ret;
        }
        
        case TypeAttribute:
        {
            struct Attribute* tree_attr = treeAttr(tb, tree);

            NeObj neo = eval(tb, tree_attr->object);

            return_on_error(NEO_VOID);

            if (NEO_TYPE(neo) != TYPE_CONTAINER)
            {
                // Erreur : essaie d'accéder à un champ d'une variable qui n'est pas un container
                neon_fail(80, neo_new_const_create(type(neo)));
                neobject_destroy(neo);
                return NEO_VOID;
            }

            Container* c = neo_to_container(neo);

            // si l'indice a été invalidé
            if (tree_attr->index == -1 || tree_attr->last_cont_type != c->type)
            {
                tree_attr->index = get_field_index(c, tree_attr->name);
                neon_free(tree_attr->name);
                tree_attr->name = NULL;
                tree_attr->last_cont_type = c->type;
            }

            NeObj ret = neo_copy(get_container_field(c, (size_t)tree_attr->index));
            neobject_destroy(neo);
            return ret;
        }


        case TypeVariable:
        {
            NeObj value = get_var_value(treeVar(tb, tree)->var);

            if (NEO_TYPE(value) == TYPE_EMPTY) {
                // CONSTEST {
                // char* name = get_name(treeVar(tb, tree)->var);
                // NeObj const_value = neo_const_create(strdup(name));
                // replace_var(treeVar(tb, tree)->var, const_value);
                // return neo_copy(const_value);
                // CONSTEST }
                neon_fail(5, neo_new_str_create(get_name(treeVar(tb, tree)->var)));
                return NEO_VOID;
            }

            return neo_copy(value);
        }

        case TypeConst:
        {
            return neo_copy(treeConst(tb, tree)->obj);
        }

        default:
        {
            neon_fail(19, NO_ARGS);
            return NEO_VOID;
        }

    }
    neon_fail(19, NO_ARGS);
    return NEO_VOID;
}



NO_INLINE NeObj eval(TreeBuffer* tb, TreeBufferIndex tree) {
    InterpContext* interp_context = global_env->process_cycle->process->interp_context;

    // Try to get the value from the interpretation context
    struct InterpContextItem* item = InterpContext_get_value(interp_context, tree);
    if (item != NULL) {
        return neo_copy(item->obj);
    }

    size_t last_context = InterpContext_before_call(interp_context);
    NeObj obj = eval_aux(tb, tree);
    InterpContext_after_call(interp_context);

    return_on_error(obj);

    InterpContext_stack_obj(interp_context, obj, tree, last_context);

    return obj;
}





// cette fonction renvoie l'adresse de l'objet correspondant à une expression permettant d'effectuer une affectation sur l'objet pointé
NeObj* get_address(TreeBuffer* tb, TreeBufferIndex tree) {

    switch (TREE_TYPE(tb, tree)) {
    
        case TypeVariable:
        {
            NeObj* addr = get_absolute_address(treeVar(tb, tree)->var);
            return addr;
        }

        case TypeListindex:
        {
            NeObj* obj_ptr = get_address(tb, treeLstIndx(tb, tree)->object);
            return_on_error(NULL);

            
            NeObj index = eval(tb, treeLstIndx(tb, tree)->index);
            return_on_error(NULL);
            
            
            NeObj obj = *obj_ptr;
            if (NEO_TYPE(obj) != TYPE_LIST && NEO_TYPE(obj) != TYPE_STRING)
            {
                neon_fail(15, neo_copy(obj));
                return NULL;
            }

            
            if (NEO_TYPE(index) != TYPE_INTEGER)
            {
                neon_fail(16, neo_copy(index));
                neobject_destroy(index);
                return NULL;
            }
            
            intptr_t index2 = neo_to_integer(index);
            neobject_destroy(index);

            int object_length = (NEO_TYPE(obj) == TYPE_LIST) ? neo_list_len(obj) : strlen(neo_to_string(obj));

            if (index2 < 0 || index2 >= object_length)
            {
                neon_fail(18, neo_integer_create(object_length));
                return NULL;
            }
            
            
            if (NEO_TYPE(obj) == TYPE_LIST)
            {
                return nelist_nth_addr(neo_to_list(obj), (size_t)index2);
            }
            else {
                neon_fail(105, NO_ARGS);
                return NULL;
            }
        }

        case TypeAttribute:
        {
            struct Attribute* tree_attr = treeAttr(tb, tree);
            NeObj neo = eval(tb, tree_attr->object);

            return_on_error(NULL);

            if (NEO_TYPE(neo) != TYPE_CONTAINER)
            {
                // Erreur : essaie d'accéder à un champ d'une variable qui n'est pas un container
                neon_fail(80, neo_new_const_create(type(neo)));
                neobject_destroy(neo);
                return NULL;
            }

            Container* c = neo_to_container(neo);

            if (tree_attr->index == -1 || tree_attr->last_cont_type != c->type) // ça veut dire qu'on sait exactement où chercher la valeur
            {
                tree_attr->index = get_field_index(c, tree_attr->name);
                neon_free(tree_attr->name);
                tree_attr->name = NULL;
                tree_attr->last_cont_type = c->type;
            }

            NeObj* ret = get_container_field_addr(c, (size_t)tree_attr->index);
            neobject_destroy(neo);
            return ret;
        }

        default:
        {
            neon_fail(89, neo_new_str_create(printable_tree_type(tb, tree)));
            return NULL;
        }
    }

    neon_fail(89, neo_new_str_create(printable_tree_type(tb, tree)));
    return NULL;
}







int execConditionBlock(TreeBuffer* tb, TreeBufferIndex maintree) {
    intptr_t int_ret = 0;
    int bloc = 0;
    NeObj expr;

    // on garde un pointeur vers le début de la liste quand on est arrivés
    CapturedVarsCheckPoint cp = CapturedVars_get_checkpoint(&global_env->process_cycle->process->varsToSave);

    // on a comme variables locales : int_ret, bloc, inst : d'ailleurs inst doit être empilé partout

    TreeBufferIndex blocks = treeSntxTree(tb, maintree)->treelist;
    int n_blocks = treelistLength(tb, blocks);
    TreeBufferIndex* blocks_array = treelistGet(tb, blocks);

    while (bloc < n_blocks)
    {
        expr = eval(tb, treeIEW(tb, blocks_array[bloc])->expression);


        return_on_error(0);

        bool cond = neoIsTrue(expr);
        neobject_destroy(expr);

        return_on_error(0);

        if (cond)
        {
            // on s'apprete à exécuter du code, on peut donc ouvrir un nouveau contexte
            newContext(&global_env->process_cycle->process->var_loc);

            int_ret = exec(tb, treeIEW(tb, blocks_array[bloc])->code);

            deleteContext(&global_env->process_cycle->process->var_loc); // on vient d'exécuter le code, donc on a fini le if

            
            return_on_error(0);

            if (int_ret != NORMAL) {
                return int_ret;
            }
            
            
            bloc++;
            while (bloc < n_blocks && TREE_TYPE(tb, blocks_array[bloc]) != TypeIf)
                bloc++;

            // on vient d'exécuter le if, donc on a fini ce if, on peut supprimer le contexte

            if (bloc == n_blocks) {
                break;
            }
            else
                continue;
        }
        
        bloc++;


        bool elif = false;
        while (bloc < n_blocks && TREE_TYPE(tb, blocks_array[bloc]) == TypeElif)
        {

            expr = eval(tb, treeIEW(tb, blocks_array[bloc])->expression);

            cond = neoIsTrue(expr);
            neobject_destroy(expr);

            return_on_error(0);


            if (!elif && cond)
            {
                // on s'apprete à exécuter du code, on peut donc ouvrir un nouveau contexte
                newContext(&global_env->process_cycle->process->var_loc);

                int_ret = exec(tb, treeIEW(tb, blocks_array[bloc])->code);

                deleteContext(&global_env->process_cycle->process->var_loc); // on vient d'exécuter le code, donc on a fini le if


                return_on_error(0);

                if (int_ret != NORMAL) {
                    return int_ret;
                }

                
                elif = true;
            }
            bloc++;
        }


        if (bloc < n_blocks && TREE_TYPE(tb, blocks_array[bloc]) == TypeElse) // s'il y a un bloc else
        {
            if (!elif) // si on a le droit d'y aller
            {
                newContext(&global_env->process_cycle->process->var_loc);

                int_ret = exec(tb, blocks_array[bloc]);

                deleteContext(&global_env->process_cycle->process->var_loc); // on vient d'exécuter le code, donc on a fini le if

                return_on_error(0);

                if (int_ret != NORMAL) {
                    return int_ret;
                }

            }
            bloc++;
        }
    }

    // on enlève les variables qu'on avait marquées comme "à sauvegarder"
    // on enlève une à une toutes les variables qu'on avait rajoutées

    partialRestore(&global_env->process_cycle->process->varsToSave, cp);

    return NORMAL;
}




int execStatementFor(TreeBuffer* tb, TreeBufferIndex tree) {

    // on évalue l'incrément de la boucle
    int incr = 0;
    NeObj start; // la borne inférieure de la boucle
    NeObj tempMax; // la borne supérieure de la boucle

    // for(var, start, end, step)
    if (FOR_NBARGS(tb, tree) == 4) {
        NeObj step = eval(tb, FOR_ARG(tb, tree, 3));

        return_on_error(0);

        if (NEO_TYPE(step) != TYPE_INTEGER) {
            neon_fail(108, NO_ARGS);
            neobject_destroy(step);
            return NORMAL;
        }

        incr = neo_to_integer(step);
        neobject_destroy(step);

        // on évalue la valeur de départ de la boucle
        start = eval(tb, FOR_ARG(tb, tree, 1));

        return_on_error(0);

        tempMax = eval(tb, FOR_ARG(tb, tree, 2));
    }

    // for(var, start, end)
    else if (FOR_NBARGS(tb, tree) == 3) {
        incr = 1;
        // on évalue la valeur de départ de la boucle
        start = eval(tb, FOR_ARG(tb, tree, 1));

        return_on_error(0);

        tempMax = eval(tb, FOR_ARG(tb, tree, 2));
    }

    // for(var, end)
    else if (FOR_NBARGS(tb, tree) == 2) {
        incr = 1;
        start = neo_integer_create(0);

        return_on_error(0);

        tempMax = eval(tb, FOR_ARG(tb, tree, 1));
    }
    else {
        neon_fail(108, NO_ARGS);
        return NORMAL;
    }

    if_error { // il y a eu une erreur lors de l'évaluation du tempMax
        neobject_destroy(start);
        return NORMAL;
    }
    
    if (NEO_TYPE(start) != TYPE_INTEGER)
    {
        neon_fail(10, neo_copy(start));
        neobject_destroy(tempMax);
        neobject_destroy(start);
        return NORMAL;
    }
    else if (NEO_TYPE(tempMax) != TYPE_INTEGER) {
        neon_fail(10, neo_copy(tempMax));
        neobject_destroy(tempMax);
        neobject_destroy(start);
        return NORMAL;
    }

    // le vrai indice de la boucle sera un entier
    int ext_index = neo_to_integer(start);
    neobject_destroy(start);


    // on récupère le variant de boucle
    if (TREE_TYPE(tb, FOR_ARG(tb, tree, 0)) != TypeVariable) {
        neon_fail(111, neo_new_str_create(printable_tree_type(tb, FOR_ARG(tb, tree, 0))));
        neobject_destroy(tempMax);
        return NORMAL;
    }
    
    Var var = treeVar(tb, FOR_ARG(tb, tree, 0))->var; // variable à incrémenter lors de la boucle

    // ouverture du nouveau contexte, sauvegarde du variant
    newContext(&global_env->process_cycle->process->var_loc); // nouveau contexte pour rendre des variables locales à la boucle for
    CapturedVarsCheckPoint cp = CapturedVars_get_checkpoint(&global_env->process_cycle->process->varsToSave); // pour restaurer l'ancienne liste des variables à sauvegarder, une fois qu'on aura fini

    local(var, global_env->process_cycle->process); // on localise l'indice de la boucle


    
    int max = neo_to_integer(tempMax); // borne supérieure des valeurs atteintes par la variable
    neobject_destroy(tempMax);

    intptr_t int_ret = 0;
    
    while (ext_index < max)
    {
        // on restaure la valeur de l'indice au début du corps de la boucle
        replace_var(var, neo_integer_create(ext_index));

        int_ret = exec(tb, treeFor(tb, tree)->block);

        if_error {
            deleteContext(&global_env->process_cycle->process->var_loc);
            partialRestore(&global_env->process_cycle->process->varsToSave, cp);
            return NORMAL;
        }

        if (int_ret != NORMAL && int_ret != CONTINUE)
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


    deleteContext(&global_env->process_cycle->process->var_loc);

    // on enlève les variables qu'on avait marquées comme "à sauvegarder"
    // on enlève une à une toutes les variables qu'on avait rajoutées
    partialRestore(&global_env->process_cycle->process->varsToSave, cp);

    if (int_ret != BREAK)
        return int_ret;
    else
        return NORMAL;
}




int execStatementForeachList(TreeBuffer* tb, TreeBufferIndex tree, NeObj neo_list) {

    NeList* list = neo_to_list(neo_list);

    // on récupère le variant de boucle
    if (TREE_TYPE(tb, FOR_ARG(tb, tree, 0)) != TypeVariable) {
        neon_fail(111, neo_new_str_create(printable_tree_type(tb, FOR_ARG(tb, tree, 0))));
        return NORMAL;
    }

    Var var = treeVar(tb, FOR_ARG(tb, tree, 0))->var; // variable à incrémenter lors de la boucle

    newContext(&global_env->process_cycle->process->var_loc); // nouveau contexte pour rendre des variables locales à la boucle for
    CapturedVarsCheckPoint cp = CapturedVars_get_checkpoint(&global_env->process_cycle->process->varsToSave); // pour restaurer l'ancienne liste des variables à sauvegarder, une fois qu'on aura fini

    local(var, global_env->process_cycle->process); // on localise l'indice de la boucle


    // l'indice qui va parcourir la liste
    size_t ext_index = 0;

    intptr_t int_ret = 0;
    
    while (ext_index < list->len)
    {
        // on restaure la valeur de l'indice au début du corps de la boucle
        replace_var(var, neo_copy(nelist_nth(list, ext_index)));

        int_ret = exec(tb, treeFor(tb, tree)->block);

        if_error
        {
            deleteContext(&global_env->process_cycle->process->var_loc);
            partialRestore(&global_env->process_cycle->process->varsToSave, cp);
            return NORMAL;
        }

        if (int_ret != NORMAL && int_ret != CONTINUE)
            break;
        
        ext_index++;
    }

    deleteContext(&global_env->process_cycle->process->var_loc);

    // on enlève les variables qu'on avait marquées comme "à sauvegarder"
    // on enlève une à une toutes les variables qu'on avait rajoutées
    partialRestore(&global_env->process_cycle->process->varsToSave, cp);

    if (int_ret != BREAK)
        return int_ret;
    else
        return NORMAL;
}



int execStatementForeachString(TreeBuffer* tb, TreeBufferIndex tree, NeObj neo_string) {

    char* string = neo_to_string(neo_string);
    int string_length = strlen(string);

    // on récupère le variant de boucle
    if (TREE_TYPE(tb, FOR_ARG(tb, tree, 0)) != TypeVariable) {
        neon_fail(111, neo_new_str_create(printable_tree_type(tb, FOR_ARG(tb, tree, 0))));
        return NORMAL;
    }

    Var var = treeVar(tb, FOR_ARG(tb, tree, 0))->var; // variable à incrémenter lors de la boucle

    newContext(&global_env->process_cycle->process->var_loc); // nouveau contexte pour rendre des variables locales à la boucle for
    CapturedVarsCheckPoint cp = CapturedVars_get_checkpoint(&global_env->process_cycle->process->varsToSave); // pour restaurer l'ancienne liste des variables à sauvegarder, une fois qu'on aura fini

    local(var, global_env->process_cycle->process); // on localise l'indice de la boucle



    // l'indice qui va parcourir la liste
    intptr_t ext_index = 0;

    intptr_t int_ret = 0;
    
    while (ext_index < string_length)
    {
        // on restaure la valeur de l'indice au début du corps de la boucle
        replace_var(var, neo_str_create(charToString(string[ext_index])));

        int_ret = exec(tb, treeFor(tb, tree)->block);

        if_error
        {
            deleteContext(&global_env->process_cycle->process->var_loc);
            partialRestore(&global_env->process_cycle->process->varsToSave, cp);
            return NORMAL;
        }

        if (int_ret != NORMAL && int_ret != CONTINUE)
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

    deleteContext(&global_env->process_cycle->process->var_loc);

    // on enlève les variables qu'on avait marquées comme "à sauvegarder"
    // on enlève une à une toutes les variables qu'on avait rajoutées
    partialRestore(&global_env->process_cycle->process->varsToSave, cp);

    if (int_ret != BREAK)
        return int_ret;
    else
        return NORMAL;
}




int exec_aux(TreeBuffer* tb, TreeBufferIndex tree) {
    global_env->LINENUMBER = TREE_LINE(tb, tree);

    switch (TREE_TYPE(tb, tree)) {

        case TypeSyntaxtree:
        {
            MAY_INTERRUPT();
            return_on_error(0);

            TreeBufferIndex treelist = treeSntxTree(tb, tree)->treelist;
            TreeBufferIndex* treelist_array = treelistGet(tb, treelist);

            for (int inst=0 ; inst < treelistLength(tb, treelist) ; inst++) {
                exec(tb, treelist_array[inst]);
                return_on_error(0);
            }

            return NORMAL;
        }

        case TypeTryExcept:
        {
            TreeBufferIndex maintree = tree;

            // exécution du try
            intptr_t int_ret = exec(tb, treeTE(tb, maintree)->try_tree);

            if (int_ret != NORMAL)
                return int_ret;
            

            if_error // on peut catcher un exit()
            {
                // on réinitialise la variable de code d'erreur pour les évaluations d'exceptions
                int sov_code_error = global_env->CODE_ERROR;
                global_env->CODE_ERROR = 0;


                // on va parcourir la liste des exceptions jusqu'à en trouver une qui corresponde
                int block_found = -1; // va correspondre à l'indice du sous arbre que l'on va exécuter

                TreeBufferIndex except_blocks = treeTE(tb, maintree)->except_blocks;

                // parcours des différents blocs except pour trouver le bon
                for (int fils = 0 ; fils < treelistLength(tb, except_blocks) && block_found == -1 ; fils++) { // on arrête dès que bo correspond à un vrai sous arbre except
                    const TreeBufferIndex exceptions = treeExpt(tb, treelistGet(tb, except_blocks)[fils])->exceptions;

                    int exceptions_length = treelistLength(tb, exceptions);
                    // si le bloc except ne spécifie aucune exception, on peut directement l'exécuter
                    if (exceptions_length == 0) {
                        block_found = fils;
                        break;
                    }

                    // parcourt toutes les exceptions du bloc except
                    for (int i = 0 ; i < exceptions_length ; i++)
                    {
                        // récupère l'exception pour l'évaluer
                        TreeBufferIndex exception_expr = treelistGet(tb, exceptions)[i];
                        NeObj exception = eval(tb, exception_expr);
                        return_on_error(int_ret);

                        if (NEO_TYPE(exception) != TYPE_EXCEPTION)
                        {
                            neon_fail(78, neo_new_const_create(type(exception)));
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
                    int_ret = exec(tb, treeExpt(tb, treelistGet(tb, except_blocks)[block_found])->block);

                    if (global_env->CODE_ERROR != 0 || int_ret != NORMAL) {
                        return int_ret;
                    }
                }
                else // restauration du code d'erreur si aucun bloc except n'est trouvé
                {
                    global_env->CODE_ERROR = sov_code_error;
                }
            }

            return NORMAL;
        }

        case TypeAtomic:
        {
            // on va exécuter le bloc de code d'une traite, sans changer de processus
            //stack_check_for(varstack, 4);
            int temp = global_env->atomic_counter;
            global_env->atomic_counter = -1; // tant qu'on n'a pas remis une valeur positive, on ne passera pas à un autre processus
            
            intptr_t int_ret = exec(tb, tree);

            // s'il reste des crédits à ce processus, on les lui rend, sinon on passe au suivant
            global_env->atomic_counter = (global_env->atomic_counter + temp > 0) ? global_env->atomic_counter + temp : 0;

            if (global_env->CODE_ERROR != 0 || int_ret != NORMAL) {
                return int_ret;
            }

            return NORMAL;
        }
        
        case TypeKWParam:
        {
            struct KWParam* tree_kw_param = treeKWParam(tb, tree);

            int param_length = treelistLength(tb, tree_kw_param->params);

            if (tree_kw_param->code == RETURN) // action de return
            {
                if (param_length > 1)
                {
                    neon_fail(21, neo_integer_create(param_length));
                    return NORMAL;
                }
                
                if (!neo_is_void(global_env->RETURN_VALUE)) // c'est pas correct, car on ne peut pas renvoyer une valeur alors que la précédente n'a pas été récupérée
                {
                    neon_fail(99, NO_ARGS);
                    return NORMAL;
                }

                if (param_length == 0) {
                    global_env->RETURN_VALUE = neo_none_create();
                    return EXIT_RETURN;
                }
                else {
                    global_env->RETURN_VALUE = eval(tb, treelistGet(tb, tree_kw_param->params)[0]);

                    return_on_error(NORMAL);
                    
                    return EXIT_RETURN;
                }
            }

            
            else if (tree_kw_param->code == IMPORT) // action de import
            {
                for (int ext_index = 0 ; ext_index < param_length ; ext_index++)
                {
                    NeObj nom = eval(tb, treelistGet(tb, tree_kw_param->params)[ext_index]);
                    return_on_error(0);

                    char* nomAct = strdup(neo_to_string(nelist_nth(global_env->ADRESSES, (size_t)global_env->NAME))); // pour restaurer le nom de fichier actuel
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

            else if (tree_kw_param->code == LOCAL)
            {
                if (param_length == 0) // il faut au moins un argument
                {
                    neon_fail(69, neo_new_str_create("local"));
                    return NORMAL;
                }
                else if (global_env->process_cycle->process->var_loc.len == 0)
                {
                    neon_fail(70, NO_ARGS);
                    return NORMAL;
                }

                for (int i = 0 ; i < param_length ; i++) // pour toutes les variables
                {
                    // Get the tree
                    TreeBufferIndex tree_var = treelistGet(tb, tree_kw_param->params)[i];
                    
                    // Check that it is a variable
                    if (TREE_TYPE(tb, tree_var) != TypeVariable) {
                        neon_fail(55, neo_new_str_create("local"), neo_new_str_create(printable_tree_type(tb, tree_var)));
                        return NORMAL;
                    }
                    else {
                        // va traiter la variable comme étant locale
                        Var var = treeVar(tb, tree_var)->var;
                        Process* proc = global_env->process_cycle->process;

                        if (!isLocal(var, &proc->var_loc))
                            local(var, proc);
                    }
                }
            }

            else if (tree_kw_param->code == AWAIT)
            {

                if (param_length != 1)
                {
                    neon_fail(101, neo_integer_create(param_length));
                    return NORMAL;
                }

                while (!isTrue(tb, treelistGet(tb, tree_kw_param->params)[0]))
                {
                    return_on_error(0); // Au cas où il y a une erreur dans le isTrue

                    if (global_env->atomic_counter < 0) {
                        continue;
                    }
                    else {
                        // schedule
                        neon_interp_yield();

                        // Au cas où il y a une erreur dans le processus précédent
                        return_on_error(0);
                    }

                }

                return_on_error(0); // Au cas où il y a une erreur dans le isTrue

            }

            else if (tree_kw_param->code == DEFINE) {

                if (param_length == 0) // il faut au moins un argument
                {
                    neon_fail(69, neo_new_str_create("define"));
                    return NORMAL;
                }

                for (int i = 0 ; i < param_length ; i++) // pour toutes les variables
                {
                    TreeBufferIndex tree_var = treelistGet(tb, tree_kw_param->params)[i];
                    
                    if (TREE_TYPE(tb, tree_var) != TypeVariable) {
                        neon_fail(55, neo_new_str_create("define"), neo_new_str_create(printable_tree_type(tb, tree_var)));
                        return NORMAL;
                    }
                    else {
                        Var var = treeVar(tb, tree_var)->var;
                        char* identifier = get_name(var);
                        replace_var(var, neo_new_const_create(identifier));
                    }
                }
            }

            else if (tree_kw_param->code == INIT) {

                if (param_length == 0) // il faut au moins un argument
                {
                    neon_fail(69, neo_new_str_create("init"));
                    return NORMAL;
                }

                for (int i = 0 ; i < param_length ; i++) // pour toutes les variables
                {
                    TreeBufferIndex tree_var = treelistGet(tb, tree_kw_param->params)[i];
                    
                    if (TREE_TYPE(tb, tree_var) != TypeVariable) {
                        neon_fail(55, neo_new_str_create("init"), neo_new_str_create(printable_tree_type(tb, tree_var)));
                        return NORMAL;
                    }
                    else {
                        Var var = treeVar(tb, tree_var)->var;
                        char* identifier = get_name(var);
                        
                        Module mod = get_module_from_name(identifier);
                        return_on_error(0);

                        init_module(mod, global_env);
                    }
                }
            }

            return NORMAL;
        }

        
        
        case TypeKeyword:
        {
            if (treeKW(tb, tree)->code != PASS) {
                return treeKW(tb, tree)->code;
            }
            return NORMAL;
        }

        
    
        case TypeConditionblock:
        {
            int int_ret = execConditionBlock(tb, tree);
            
            if (int_ret != NORMAL || global_env->CODE_ERROR != 0) {
                return int_ret;
            }

            return NORMAL;
        }

        

        case TypeWhile:
        {
            const struct StatementIEW* tree_while = treeIEW(tb, tree);

            int int_ret = 0;

            NeObj expr = eval(tb, tree_while->expression);

            return_on_error(0);

            bool cond = neoIsTrue(expr);
            neobject_destroy(expr);

            while (cond)
            {

                int_ret = exec(tb, tree_while->code);

                return_on_error(0);
                
                if (int_ret != NORMAL && int_ret != CONTINUE)
                    break;

                // réévaluation de la condition

                expr = eval(tb, tree_while->expression);

                return_on_error(0);

                cond = neoIsTrue(expr);
                neobject_destroy(expr);
                
            }

            
            if (int_ret == EXIT_RETURN) { // en sortant d'une boucle, on n'a pas de break ou continue
                return int_ret;
            }

            return NORMAL;
        }
        
        
        case TypeFor:
        {
            
            if (TREE_TYPE(tb, FOR_ARG(tb, tree, 0)) != TypeVariable)
            {
                neon_fail(22, neo_new_str_create(printable_tree_type(tb, FOR_ARG(tb, tree, 0))));
                return NORMAL;
            }

            int int_ret = execStatementFor(tb, tree);

            if (global_env->CODE_ERROR != 0 || int_ret != NORMAL)
                return int_ret;

            return NORMAL;
        }

        case TypeForeach:
        {
            
            if (TREE_TYPE(tb, FOR_ARG(tb, tree, 0)) != TypeVariable)
            {
                neon_fail(22, neo_new_str_create(printable_tree_type(tb, FOR_ARG(tb, tree, 0))));
                return NORMAL;
            }

            if (FOR_NBARGS(tb, tree) != 2) {
                neon_fail(109, NO_ARGS);
                return NORMAL;
            }


            NeObj iterable = eval(tb, FOR_ARG(tb, tree, 1));

            return_on_error(0);
            
            int int_ret = 0;

            if (NEO_TYPE(iterable) == TYPE_LIST) {
                int_ret = execStatementForeachList(tb, tree, iterable);
            }
            else if (NEO_TYPE(iterable) == TYPE_STRING) {
                int_ret = execStatementForeachString(tb, tree, iterable);
            }
            else {
                neobject_destroy(iterable);
                neon_fail(109, NO_ARGS);
                return NORMAL;
            }

            neobject_destroy(iterable);

            if (global_env->CODE_ERROR != 0 || int_ret != NORMAL) {
                return int_ret;
            }

            return NORMAL;
        }

        

        case TypeFunctiondef:
        {
            // définition de fonction en cours d'exécution

            struct FunctionDef* maintree = treeFDef(tb, tree);

            // il faut d'abord évaluer les arguments optionnels
            NeList opt_args;
            treeToList(&opt_args, tb, maintree->args);

            // on crée la nouvelle fonction avec ces arguments optionnels
            NeObj function = neo_userfunc_define(maintree->object, opt_args);

            // et on la met dans la variable qui porte son nom
            Var var = get_var(maintree->name);
            replace_var(var, function);

            return NORMAL;
        }

        default:
        {
            NeObj res = eval(tb, tree);

            neobject_destroy(res); // sinon, évaluation de l'expression, en la libérant juste après

            return NORMAL;
        }
    }
}



int exec(TreeBuffer* tb, TreeBufferIndex tree) {
    InterpContext* interp_context = global_env->process_cycle->process->interp_context;

    // Try to get the return code from the interpretation context
    struct InterpContextItem* item = InterpContext_get_value(interp_context, tree);
    if (item != NULL) {
        return item->code;
    }

    size_t last_context = InterpContext_before_call(interp_context);
    int code = exec_aux(tb, tree);
    InterpContext_after_call(interp_context);

    return_on_error(code);

    InterpContext_stack_ret_code(interp_context, code, tree, last_context);

    return code;
}



void initRuntime(InterpContext* interp_context) {
    // on met isInitialized = true car ce processus va entrer dans eval_aux de manière normale, pas par une restauration de registres
    create_new_process(NULL, TREE_VOID, NEO_VOID, true, interp_context);
}


// cette fonction doit obligatoirement avoir exactement le même contexte que launch_process
NO_OPT void exitRuntime(void) {

    // pour avoir la même taille de pile utilisée que la fonction launch_process
    #if defined(LINUX_AMD64) || defined(MINIMAL_LIBC_AMD64)
    ADD_STACK_SIZE(16);
    #elif defined(LINUX_RISCV64) || defined(MINIMAL_LIBC_RISCV64)
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



// Fonctions pour lancer l'exécution ou l'interprétation d'un abre syntaxique
// Ces fonctions sont les fonctions les plus extérieures de l'interface d'exécution
// de l'interpréteur Neon
// Ces fonctions doivent être appelées si aucun runtime n'est présent
// Sinon, il faut appeler exec et eval

void launch_execution(TreeBuffer* tb, TreeBufferIndex tree, InterpContext* interp_context) {
    initRuntime(interp_context);
    exec(tb, tree);
    exitRuntime();
    return;
}

NeObj launch_evaluation(TreeBuffer* tb, TreeBufferIndex tree, InterpContext* interp_context) {
    initRuntime(interp_context);
    NeObj res = eval(tb, tree);
    exitRuntime();
    return res;
}
