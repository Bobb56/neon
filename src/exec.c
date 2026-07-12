#define NEON_SOURCE_ID 28

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "headers/constants.h"
#include "headers/neobj.h"
#include "headers/neon.h"
#include "headers/objects.h"
#include "headers/gc.h"
#include "headers/runtime.h"
#include "headers/errors.h"
#include "headers/strings.h"
#include "headers/processcycle.h"
#include "headers/trees.h"
#include "headers/contexts.h"
#include "headers/nativefunctions.h"


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
        expr = eval_aux(tb, treeIEW(tb, blocks_array[bloc])->expression);


        return_on_error(0);

        bool cond = neoIsTrue(expr);
        neobject_destroy(expr);

        return_on_error(0);

        if (cond)
        {
            // on s'apprete à exécuter du code, on peut donc ouvrir un nouveau contexte
            newContext(&global_env->process_cycle->process->var_loc);

            int_ret = exec_aux(tb, treeIEW(tb, blocks_array[bloc])->code);

            deleteContext(&global_env->process_cycle->process->var_loc); // on vient d'exécuter le code, donc on a fini le if

            
            return_on_error(0);

            if (int_ret != 0) {
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

            expr = eval_aux(tb, treeIEW(tb, blocks_array[bloc])->expression);

            cond = neoIsTrue(expr);
            neobject_destroy(expr);

            return_on_error(0);


            if (!elif && cond)
            {
                // on s'apprete à exécuter du code, on peut donc ouvrir un nouveau contexte
                newContext(&global_env->process_cycle->process->var_loc);

                int_ret = exec_aux(tb, treeIEW(tb, blocks_array[bloc])->code);

                deleteContext(&global_env->process_cycle->process->var_loc); // on vient d'exécuter le code, donc on a fini le if


                return_on_error(0);

                if (int_ret != 0) {
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

                int_ret = exec_aux(tb, blocks_array[bloc]);

                deleteContext(&global_env->process_cycle->process->var_loc); // on vient d'exécuter le code, donc on a fini le if

                return_on_error(0);

                if (int_ret != 0) {
                    return int_ret;
                }

            }
            bloc++;
        }
    }

    // on enlève les variables qu'on avait marquées comme "à sauvegarder"
    // on enlève une à une toutes les variables qu'on avait rajoutées

    partialRestore(&global_env->process_cycle->process->varsToSave, cp);

    return 0;
}




int execStatementFor(TreeBuffer* tb, TreeBufferIndex tree) {

    // on évalue l'incrément de la boucle
    int incr = 0;
    NeObj start; // la borne inférieure de la boucle
    NeObj tempMax; // la borne supérieure de la boucle

    // for(var, start, end, step)
    if (FOR_NBARGS(tb, tree) == 4) {
        NeObj step = eval_aux(tb, FOR_ARG(tb, tree, 3));

        return_on_error(0);

        if (NEO_TYPE(step) != TYPE_INTEGER) {
            neon_fail(108, NO_ARGS);
            neobject_destroy(step);
            return 0;
        }

        incr = neo_to_integer(step);
        neobject_destroy(step);

        // on évalue la valeur de départ de la boucle
        start = eval_aux(tb, FOR_ARG(tb, tree, 1));

        return_on_error(0);

        tempMax = eval_aux(tb, FOR_ARG(tb, tree, 2));
    }

    // for(var, start, end)
    else if (FOR_NBARGS(tb, tree) == 3) {
        incr = 1;
        // on évalue la valeur de départ de la boucle
        start = eval_aux(tb, FOR_ARG(tb, tree, 1));

        return_on_error(0);

        tempMax = eval_aux(tb, FOR_ARG(tb, tree, 2));
    }

    // for(var, end)
    else if (FOR_NBARGS(tb, tree) == 2) {
        incr = 1;
        start = neo_integer_create(0);

        return_on_error(0);

        tempMax = eval_aux(tb, FOR_ARG(tb, tree, 1));
    }
    else {
        neon_fail(108, NO_ARGS);
        return 0;
    }

    if_error { // il y a eu une erreur lors de l'évaluation du tempMax
        neobject_destroy(start);
        return 0;
    }
    
    if (NEO_TYPE(start) != TYPE_INTEGER)
    {
        neon_fail(10, neo_copy(start));
        neobject_destroy(tempMax);
        neobject_destroy(start);
        return 0;
    }
    else if (NEO_TYPE(tempMax) != TYPE_INTEGER) {
        neon_fail(10, neo_copy(tempMax));
        neobject_destroy(tempMax);
        neobject_destroy(start);
        return 0;
    }

    // le vrai indice de la boucle sera un entier
    int ext_index = neo_to_integer(start);
    neobject_destroy(start);


    // on récupère le variant de boucle
    if (TREE_TYPE(tb, FOR_ARG(tb, tree, 0)) != TypeVariable) {
        neon_fail(111, neo_new_str_create(printable_tree_type(tb, FOR_ARG(tb, tree, 0))));
        neobject_destroy(tempMax);
        return 0;
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

        int_ret = exec_aux(tb, treeFor(tb, tree)->block);

        if_error
        {
            deleteContext(&global_env->process_cycle->process->var_loc);
            partialRestore(&global_env->process_cycle->process->varsToSave, cp);
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


    deleteContext(&global_env->process_cycle->process->var_loc);

    // on enlève les variables qu'on avait marquées comme "à sauvegarder"
    // on enlève une à une toutes les variables qu'on avait rajoutées
    partialRestore(&global_env->process_cycle->process->varsToSave, cp);

    if (int_ret != BREAK)
        return int_ret;
    else
        return 0;
}




int execStatementForeachList(TreeBuffer* tb, TreeBufferIndex tree, NeObj neo_list) {

    NeList* list = neo_to_list(neo_list);

    // on récupère le variant de boucle
    if (TREE_TYPE(tb, FOR_ARG(tb, tree, 0)) != TypeVariable) {
        neon_fail(111, neo_new_str_create(printable_tree_type(tb, FOR_ARG(tb, tree, 0))));
        return 0;
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

        int_ret = exec_aux(tb, treeFor(tb, tree)->block);

        if_error
        {
            deleteContext(&global_env->process_cycle->process->var_loc);
            partialRestore(&global_env->process_cycle->process->varsToSave, cp);
            return 0;
        }

        if (int_ret != 0 && int_ret != CONTINUE)
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
        return 0;
}



int execStatementForeachString(TreeBuffer* tb, TreeBufferIndex tree, NeObj neo_string) {

    char* string = neo_to_string(neo_string);
    int string_length = strlen(string);

    // on récupère le variant de boucle
    if (TREE_TYPE(tb, FOR_ARG(tb, tree, 0)) != TypeVariable) {
        neon_fail(111, neo_new_str_create(printable_tree_type(tb, FOR_ARG(tb, tree, 0))));
        return 0;
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

        int_ret = exec_aux(tb, treeFor(tb, tree)->block);

        if_error
        {
            deleteContext(&global_env->process_cycle->process->var_loc);
            partialRestore(&global_env->process_cycle->process->varsToSave, cp);
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

    deleteContext(&global_env->process_cycle->process->var_loc);

    // on enlève les variables qu'on avait marquées comme "à sauvegarder"
    // on enlève une à une toutes les variables qu'on avait rajoutées
    partialRestore(&global_env->process_cycle->process->varsToSave, cp);

    if (int_ret != BREAK)
        return int_ret;
    else
        return 0;
}


int exec_TryExcept(TreeBuffer* tb, TreeBufferIndex tree)
{
    TreeBufferIndex maintree = tree;

    // exécution du try
    intptr_t int_ret = exec_aux(tb, treeTE(tb, maintree)->try_tree);

    if (int_ret != EXIT_SUCCESS)
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
                NeObj exception = eval_aux(tb, exception_expr);
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
            int_ret = exec_aux(tb, treeExpt(tb, treelistGet(tb, except_blocks)[block_found])->block);

            if (global_env->CODE_ERROR != 0 || int_ret != EXIT_SUCCESS) {
                return int_ret;
            }
        }
        else // restauration du code d'erreur si aucun bloc except n'est trouvé
        {
            global_env->CODE_ERROR = sov_code_error;
        }
    }

    return 0;
}

int exec_Atomic(TreeBuffer* tb, TreeBufferIndex tree)
{
    // on va exécuter le bloc de code d'une traite, sans changer de processus
    //stack_check_for(varstack, 4);
    int temp = global_env->atomic_counter;
    global_env->atomic_counter = -1; // tant qu'on n'a pas remis une valeur positive, on ne passera pas à un autre processus
    
    intptr_t int_ret = exec_aux(tb, tree);

    // s'il reste des crédits à ce processus, on les lui rend, sinon on passe au suivant
    global_env->atomic_counter = (global_env->atomic_counter + temp > 0) ? global_env->atomic_counter + temp : 0;

    if (global_env->CODE_ERROR != 0 || int_ret != 0) {
        return int_ret;
    }

    return 0;
}

int exec_KWParam(TreeBuffer* tb, TreeBufferIndex tree)
{
    struct KWParam* tree_kw_param = treeKWParam(tb, tree);

    int param_length = treelistLength(tb, tree_kw_param->params);

    if (tree_kw_param->code == RETURN) // action de return
    {
        if (param_length > 1)
        {
            neon_fail(21, neo_integer_create(param_length));
            return 0;
        }
        
        if (!neo_is_void(global_env->RETURN_VALUE)) // c'est pas correct, car on ne peut pas renvoyer une valeur alors que la précédente n'a pas été récupérée
        {
            neon_fail(99, NO_ARGS);
            return 0;
        }

        if (param_length == 0) {
            global_env->RETURN_VALUE = neo_none_create();
            return EXIT_RETURN;
        }
        else {
            global_env->RETURN_VALUE = eval_aux(tb, treelistGet(tb, tree_kw_param->params)[0]);

            return_on_error(EXIT_SUCCESS);
            
            return EXIT_RETURN;
        }
    }

    
    else if (tree_kw_param->code == IMPORT) // action de import
    {
        for (int ext_index = 0 ; ext_index < param_length ; ext_index++)
        {
            NeObj nom = eval_aux(tb, treelistGet(tb, tree_kw_param->params)[ext_index]);
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
            return 0;
        }
        else if (global_env->process_cycle->process->var_loc.len == 0)
        {
            neon_fail(70, NO_ARGS);
            return 0;
        }

        for (int i = 0 ; i < param_length ; i++) // pour toutes les variables
        {
            // Get the tree
            TreeBufferIndex tree_var = treelistGet(tb, tree_kw_param->params)[i];
            
            // Check that it is a variable
            if (TREE_TYPE(tb, tree_var) != TypeVariable) {
                neon_fail(55, neo_new_str_create("local"), neo_new_str_create(printable_tree_type(tb, tree_var)));
                return 0;
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
            return 0;
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
            return 0;
        }

        for (int i = 0 ; i < param_length ; i++) // pour toutes les variables
        {
            TreeBufferIndex tree_var = treelistGet(tb, tree_kw_param->params)[i];
            
            if (TREE_TYPE(tb, tree_var) != TypeVariable) {
                neon_fail(55, neo_new_str_create("define"), neo_new_str_create(printable_tree_type(tb, tree_var)));
                return 0;
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
            return 0;
        }

        for (int i = 0 ; i < param_length ; i++) // pour toutes les variables
        {
            TreeBufferIndex tree_var = treelistGet(tb, tree_kw_param->params)[i];
            
            if (TREE_TYPE(tb, tree_var) != TypeVariable) {
                neon_fail(55, neo_new_str_create("init"), neo_new_str_create(printable_tree_type(tb, tree_var)));
                return 0;
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

    return 0;
}



int exec_Keyword(TreeBuffer* tb, TreeBufferIndex tree)
{
    if (treeKW(tb, tree)->code != PASS) {
        return treeKW(tb, tree)->code;
    }

    return 0;
}



int exec_Conditionblock(TreeBuffer* tb, TreeBufferIndex tree)
{
    int int_ret = execConditionBlock(tb, tree);
    
    if (int_ret != 0 || global_env->CODE_ERROR != 0) {
        return int_ret;
    }

    return 0;
}



int exec_While(TreeBuffer* tb, TreeBufferIndex tree)
{
    const struct StatementIEW* tree_while = treeIEW(tb, tree);

    int int_ret = 0;

    NeObj expr = eval_aux(tb, tree_while->expression);

    return_on_error(0);

    bool cond = neoIsTrue(expr);
    neobject_destroy(expr);

    while (cond)
    {

        int_ret = exec_aux(tb, tree_while->code);

        return_on_error(0);
        
        if (int_ret != 0 && int_ret != CONTINUE)
            break;

        // réévaluation de la condition

        expr = eval_aux(tb, tree_while->expression);

        return_on_error(0);

        cond = neoIsTrue(expr);
        neobject_destroy(expr);
        
    }

    
    if (int_ret == EXIT_RETURN) { // en sortant d'une boucle, on n'a pas de break ou continue
        return int_ret;
    }

    return 0;
}


int exec_For(TreeBuffer* tb, TreeBufferIndex tree)
{
    if (TREE_TYPE(tb, FOR_ARG(tb, tree, 0)) != TypeVariable)
    {
        neon_fail(22, neo_new_str_create(printable_tree_type(tb, FOR_ARG(tb, tree, 0))));
        return 0;
    }

    int int_ret = execStatementFor(tb, tree);

    if (global_env->CODE_ERROR != 0 || int_ret != 0)
        return int_ret;

    return 0;
}

int exec_Foreach(TreeBuffer* tb, TreeBufferIndex tree)
{
    if (TREE_TYPE(tb, FOR_ARG(tb, tree, 0)) != TypeVariable)
    {
        neon_fail(22, neo_new_str_create(printable_tree_type(tb, FOR_ARG(tb, tree, 0))));
        return 0;
    }

    if (FOR_NBARGS(tb, tree) != 2) {
        neon_fail(109, NO_ARGS);
        return 0;
    }


    NeObj iterable = eval_aux(tb, FOR_ARG(tb, tree, 1));

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
        return 0;
    }

    neobject_destroy(iterable);

    if (global_env->CODE_ERROR != 0 || int_ret != 0) {
        return int_ret;
    }

    return 0;
}



int exec_Functiondef(TreeBuffer* tb, TreeBufferIndex tree)
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

    return 0;
}



int exec_Expression(TreeBuffer* tb, TreeBufferIndex tree)
{
    NeObj res = eval_aux(tb, tree);
    neobject_destroy(res); // sinon, évaluation de l'expression, en la libérant juste après

    return 0;
}