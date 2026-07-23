#define NEON_SOURCE_ID 17

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "headers/constants.h"
#include "headers/neobj.h"
#include "headers/dynarrays.h"
#include "headers/neon.h"
#include "headers/objects.h"
#include "headers/eval.h"
#include "headers/exec.h"
#include "headers/runtime.h"
#include "headers/errors.h"
#include "headers/lowlevel.h"
#include "headers/processcycle.h"
#include "headers/trees.h"
#include "headers/contexts.h"
#include "headers/nativefunctions.h"
#include "headers/neonio.h"
#include "headers/allocator.h"

#ifdef TI_EZ80
#include <keypadc.h>
#endif


void update__name__(char* name)
{
    replace_var(global_env->NAME, neo_str_create(name));
    return ;
}





bool isTrue(TreeBuffer* tb, TreeBufferIndex tree) {
    NeObj obj = eval_aux(tb, tree);
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
        global_env->INTERRUPT = false;
        stop_execution();
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
    NeObj result = eval_aux(global_env->process_cycle->process->original_call_buffer, global_env->process_cycle->process->original_call);

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
            l->tab[index] = eval_aux(tb, treelistGet(tb, tree_list)[index]);
            
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
    int ret_code = exec_aux(fun->tree_buffer, fun->code);
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



// cette fonction est tellement grosse que de toute façon gcc n'aurait jamais l'idée de l'inliner
// eval_aux suppose que les TreeBuffers donnés en arguments sont verrouillés (ou en tous cas ne peuvent
// plus être relocalisés)
NO_INLINE NeObj eval_aux(TreeBuffer* tb, TreeBufferIndex tree) {
    global_env->LINENUMBER = TREE_LINE(tb, tree);

    switch (TREE_TYPE(tb, tree)) {
        case TypeUnaryOp:
            return eval_UnaryOp(tb, tree);

        case TypeBinaryOp: // operateur binaire
            return eval_BinaryOp(tb, tree);
        
        case TypeList:
            return eval_List(tb, tree);

        case TypeParallelCall:
            return eval_ParallelCall(tb, tree);
        
        case TypeFunctioncall:
            return eval_FunctionCall(tb, tree);
        
        case TypeListindex:
            return eval_ListIndex(tb, tree);

        case TypeContainerLit:
            return eval_ContainerLit(tb, tree);
        
        case TypeAttribute:
            return eval_Attribute(tb, tree);

        case TypeVariable:
            return eval_Variable(tb, tree);

        case TypeConst:
            return eval_Const(tb, tree);

        default:
            neon_fail(19, NO_ARGS);
            return NEO_VOID;
    }
}








// cette fonction renvoie l'adresse de l'objet correspondant à une expression permettant d'effectuer une affectation sur l'objet pointé
// Les adresses renvoyées par cette fonction ne sont pas fiables, les objets peuvent être déplacés
// Fonction à utiliser en faisant attention de ne pas déclencher des redimensionnements de NeList
NeObjAddr get_address(TreeBuffer* tb, TreeBufferIndex tree) {

    switch (TREE_TYPE(tb, tree)) {
    
        case TypeVariable:
        {
            return get_absolute_address(treeVar(tb, tree)->var);
        }

        case TypeListindex:
        {
            NeObj index = eval_aux(tb, treeLstIndx(tb, tree)->index);
            return_on_error(NEOBJ_INV_ADDR);

            NeObjAddr obj_ptr = get_address(tb, treeLstIndx(tb, tree)->object);
            return_on_error(NEOBJ_INV_ADDR);
            
            
            NeObj obj = neobjaddr_deref(&obj_ptr);

            if (NEO_TYPE(obj) != TYPE_LIST && NEO_TYPE(obj) != TYPE_STRING)
            {
                neon_fail(15, neo_copy(obj));
                return NEOBJ_INV_ADDR;
            }

            
            if (NEO_TYPE(index) != TYPE_INTEGER)
            {
                neon_fail(16, neo_copy(index));
                neobject_destroy(index);
                return NEOBJ_INV_ADDR;
            }
            
            intptr_t index2 = neo_to_integer(index);
            neobject_destroy(index);

            int object_length = (NEO_TYPE(obj) == TYPE_LIST) ? neo_list_len(obj) : strlen(neo_to_string(obj));

            if (index2 < 0 || index2 >= object_length)
            {
                neon_fail(18, neo_integer_create(object_length));
                return NEOBJ_INV_ADDR;
            }
            
            
            if (NEO_TYPE(obj) == TYPE_LIST)
            {
                return NEOBJ_ADDR(neo_to_list(obj), (size_t)index2);
            }
            else {
                neon_fail(105, NO_ARGS);
                return NEOBJ_INV_ADDR;
            }
        }

        case TypeAttribute:
        {
            struct Attribute* tree_attr = treeAttr(tb, tree);
            NeObj neo = eval_aux(tb, tree_attr->object);

            return_on_error(NEOBJ_INV_ADDR);

            if (NEO_TYPE(neo) != TYPE_CONTAINER)
            {
                // Erreur : essaie d'accéder à un champ d'une variable qui n'est pas un container
                neon_fail(80, neo_new_const_create(type(neo)));
                neobject_destroy(neo);
                return NEOBJ_INV_ADDR;
            }

            Container* c = neo_to_container(neo);

            if (tree_attr->index == -1 || tree_attr->last_cont_type != c->type) // ça veut dire qu'on sait exactement où chercher la valeur
            {
                tree_attr->index = get_field_index(c, tree_attr->name);
                neon_free(tree_attr->name);
                tree_attr->name = NULL;
                tree_attr->last_cont_type = c->type;
            }

            NeObjAddr ret = get_container_field_addr(c, (size_t)tree_attr->index);
            neobject_destroy(neo);
            return ret;
        }

        default:
        {
            neon_fail(89, neo_new_str_create(printable_tree_type(tb, tree)));
            return NEOBJ_INV_ADDR;
        }
    }

    neon_fail(89, neo_new_str_create(printable_tree_type(tb, tree)));
    return NEOBJ_INV_ADDR;
}



/*
void get_addresses(TreeBuffer* tb, size_t nb_addresses, ...) {
    NeObj obj_aux[nb_addresses];

    va_list args;
    va_start(args, nb_addresses);

    // First round where we evaluate auxiliary objects
    for (size_t i=0 ; i < nb_addresses ; i++) {
        TreeBufferIndex tree = va_arg(args, TreeBufferIndex);
        NeObj** unused_address = va_arg(args, NeObj**);

        switch (TREE_TYPE(tb, tree)) {
            case TypeListindex:
                obj_aux[i] = eval_aux(tb, treeLstIndx(tb, tree)->index);
                break;

            case TypeAttribute:
                obj_aux[i] = eval_aux(tb, treeAttr(tb, tree)->object);
                break;

            default:
                break;
        }

        if_error {
            for (int k=0 ; k < i ; k++) {
                neobject_destroy(obj_aux[k]);
            }
            return;
        }
    }

    // Second round where we safely get the objects' addresses
    for (size_t i=0 ; i < nb_addresses ; i++) {
        TreeBufferIndex tree = va_arg(args, TreeBufferIndex);
        NeObj** address = va_arg(args, NeObj**);

        switch (TREE_TYPE(tb, tree)) {
            case TypeVariable:
                *address = get_absolute_address(treeVar(tb, tree)->var);
                break;

            case TypeListindex:
            {
                // Evaluated at previous round
                NeObj index = obj_aux[i];

                NeObj* obj_ptr = get_address(tb, treeLstIndx(tb, tree)->object);
                return_on_error();
                
                NeObj obj = *obj_ptr;
                if (NEO_TYPE(obj) != TYPE_LIST && NEO_TYPE(obj) != TYPE_STRING)
                {
                    neon_fail(15, neo_copy(obj));
                    break;
                }

                
                if (NEO_TYPE(index) != TYPE_INTEGER)
                {
                    neon_fail(16, neo_copy(index));
                    neobject_destroy(index);
                    break;
                }
                
                intptr_t index2 = neo_to_integer(index);
                neobject_destroy(index);

                int object_length = (NEO_TYPE(obj) == TYPE_LIST) ? neo_list_len(obj) : strlen(neo_to_string(obj));

                if (index2 < 0 || index2 >= object_length)
                {
                    neon_fail(18, neo_integer_create(object_length));
                    break;
                }
                
                
                if (NEO_TYPE(obj) == TYPE_LIST)
                {
                    *address = nelist_nth_addr(neo_to_list(obj), (size_t)index2);
                }
                else {
                    neon_fail(105, NO_ARGS);
                }
                break;
            }

            case TypeAttribute:
            {
                struct Attribute* tree_attr = treeAttr(tb, tree);
                NeObj neo = eval_aux(tb, tree_attr->object);

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
    }


    neon_fail(89, neo_new_str_create(printable_tree_type(tb, tree)));
    return NULL;
}
*/


int exec_aux(TreeBuffer* tb, TreeBufferIndex tree) {

    MAY_INTERRUPT();
    return_on_error(0);

    TreeBufferIndex treelist = treeSntxTree(tb, tree)->treelist;
    TreeBufferIndex* treelist_array = treelistGet(tb, treelist);

    for (int inst=0 ; inst < treelistLength(tb, treelist) ; inst++)
    {
        global_env->LINENUMBER = TREE_LINE(tb, treelist_array[inst]);

        int retcode = 0;

        switch (TREE_TYPE(tb, treelist_array[inst])) {

            case TypeTryExcept:
                retcode = exec_TryExcept(tb, treelist_array[inst]);
                break;

            case TypeAtomic:
                retcode = exec_Atomic(tb, treelist_array[inst]);
                break;
            
            case TypeKWParam:
                retcode = exec_KWParam(tb, treelist_array[inst]);
                break;
            
            case TypeKeyword:
                retcode = exec_Keyword(tb, treelist_array[inst]);
                break;
        
            case TypeConditionblock:
                retcode = exec_Conditionblock(tb, treelist_array[inst]);
                break;

            case TypeWhile:
                retcode = exec_While(tb, treelist_array[inst]);
                break;
            
            case TypeFor:
                retcode = exec_For(tb, treelist_array[inst]);
                break;

            case TypeForeach:
                retcode = exec_Foreach(tb, treelist_array[inst]);
                break;

            case TypeFunctiondef:
                retcode = exec_Functiondef(tb, treelist_array[inst]);
                break;

            default:
                retcode = exec_Expression(tb, treelist_array[inst]);
                break;
        }

        return_on_error(0);

        if (retcode != 0)
            return retcode;
    }

    return 0;
}




void initRuntime(void) {
    // on met isInitialized = true car ce processus va entrer dans eval_aux de manière normale, pas par une restauration de registres
    create_new_process(NULL, TREE_VOID, NEO_VOID, true);
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
    ADD_STACK_SIZE(28);
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



void exec(TreeBuffer* tb, TreeBufferIndex tree) {
    initRuntime();
    exec_aux(tb, tree);
    exitRuntime();
    return;
}

NeObj eval(TreeBuffer* tb, TreeBufferIndex tree) {
    initRuntime();
    NeObj res = eval_aux(tb, tree);
    exitRuntime();
    return res;
}
