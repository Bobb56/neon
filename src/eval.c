#define NEON_SOURCE_ID 27

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "headers/constants.h"
#include "headers/neobj.h"
#include "headers/dynarrays.h"
#include "headers/neon.h"
#include "headers/objects.h"
#include "headers/operators.h"
#include "headers/runtime.h"
#include "headers/errors.h"
#include "headers/strings.h"
#include "headers/processcycle.h"
#include "headers/trees.h"
#include "headers/nativefunctions.h"


NeObj eval_UnaryOp(TreeBuffer* tb, TreeBufferIndex tree)
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
            NeObjAddr op1 = get_address(tb, tree_un_op->expr); // si la grammaire stipule que l'opérateur doit recevoir une variable et non une valeur
            
            return_on_error(NEO_VOID);

            NeObj (*func) (NeObjAddr*) = operators_functions[tree_un_op->op];
            return func(&op1);
        }
        else
        {
            NeObj op1 = eval_aux(tb, tree_un_op->expr);

            return_on_error(NEO_VOID);

            NeObj (*func) (NeObj) = operators_functions[tree_un_op->op];
            NeObj retour = func(op1);
            neobject_destroy(op1);
            return retour;

        }
    }
}

NeObj eval_BinaryOp(TreeBuffer* tb, TreeBufferIndex tree)
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
            NeObj op2 = eval_aux(tb, tree_bin_op->right);
            return_on_error(NEO_VOID);

            NeObjAddr op1 = get_address(tb, tree_bin_op->left);
            return_on_error(NEO_VOID);

            NeObj (*func)(NeObjAddr*, NeObj) = operators_functions[tree_bin_op->op];
            NeObj result = func(&op1, op2);
            neobject_destroy(op2);
            return result;
        }
        else if (operatorIs(tree_bin_op->op, LEFT_VAR))
        {                    
            NeObj op1 = eval_aux(tb, tree_bin_op->left);
            return_on_error(NEO_VOID);
        
            NeObjAddr op2 = get_address(tb, tree_bin_op->right);
            return_on_error(NEO_VOID);

            NeObj (*func)(NeObj, NeObjAddr*) = operators_functions[tree_bin_op->op];
            NeObj result = func(op1, &op2);
            neobject_destroy(op1);
            return result;
        }
        else if (operatorIs(tree_bin_op->op, VAR_VAR))
        {
            NeObjAddr op1 = get_address(tb, tree_bin_op->left);
            return_on_error(NEO_VOID);
            
            NeObjAddr op2 = get_address(tb, tree_bin_op->right);
            return_on_error(NEO_VOID);

            NeObj (*func)(NeObjAddr*, NeObjAddr*) = operators_functions[tree_bin_op->op];
            NeObj result = func(&op1, &op2);
            return result;
        }
        else
        {
            NeObj op1 = eval_aux(tb, tree_bin_op->left);

            if_error
            {
                neobject_destroy(op1);
                return NEO_VOID;
            }

            NeObj op2 = eval_aux(tb, tree_bin_op->right);

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

NeObj eval_List(TreeBuffer* tb, TreeBufferIndex tree)
{
    // donc les enfants de tree sont les éléments de la liste
    NeList* l = neon_malloc(sizeof(NeList));
    if (l == NULL) {
        neon_fail(12, NO_ARGS);
        return NEO_VOID;
    }

    treeToList(l, tb, treeSntxTree(tb, tree)->treelist);
    return_on_error(NEO_VOID);

    return neo_list_convert(l);
}


NeObj eval_ParallelCall(TreeBuffer* tb, TreeBufferIndex tree)
{
    TreeBuffer* tree_buffer = treeParCall(tb, tree)->expr_buffer;
    TreeBufferIndex maintree = treeParCall(tb, tree)->expr;
        
    if (TREE_TYPE(tree_buffer, maintree) != TypeFunctioncall) {
        neon_fail(41, NO_ARGS);
        return NEO_VOID;
    }

    NeObj fixed_func = eval_aux(tree_buffer, treeFCall(tree_buffer, maintree)->function);

    return_on_error(NEO_VOID);

    if (NEO_TYPE(fixed_func) != TYPE_USERFUNC) {
        neon_fail(100, neo_new_const_create(type(fixed_func)));
        neobject_destroy(fixed_func);
        return NEO_VOID;
    }

    // on ajoute le processus, et il va se faire exécuter dans la chaine de processus
    // on met isInitialized = false pour que le processus entre dans eval_aux de manière normale
    int id = create_new_process(tree_buffer, maintree, fixed_func, false);

    if_error {
        neobject_destroy(fixed_func);
        return NEO_VOID;
    }

    return neo_promise_create(id);
}

NeObj eval_FunctionCall(TreeBuffer* tb, TreeBufferIndex tree)
{
    struct FunctionCall* tree_fun_call = treeFCall(tb, tree);

    // ici, appel récursif pour évaluer la fonction (1er fils), et on la met dans ->data de l'arbre
    NeObj function;

    // si le champ data de la fonction est déjà occuppé, on n'a pas besoin d'évaluer la fonction
    if (neo_is_void(tree_fun_call->function_obj)) {
        function = eval_aux(tb, tree_fun_call->function);
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
            if (args == NULL) {
                neon_fail(12, NO_ARGS);
                return NEO_VOID;
            }

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

        if (values == NULL || variables == NULL) {
            if (values) neon_free(values);
            if (variables) neon_free(variables);
            neon_fail(12, NO_ARGS);
            return NEO_VOID;
        }


        int variable_index = 0;

        // Association valeurs/variables

        fun->runningInstances += 1;
        for (int i = 0 ; i < tree_fun_call_nb_args ; i++) {
            // Si il s'agit d'un argument nommé, on affecte directement la variable
            if (TREE_TYPE(tb, arg_tree[i]) == TypeBinaryOp && treeBinOp(tb, arg_tree[i])->op == 37) {
                variables[variable_index] = treeVar(tb, treeBinOp(tb, arg_tree[i])->left)->var;
                values[variable_index] = eval_aux(tb, treeBinOp(tb, arg_tree[i])->right);
                variable_index++;
            }
            // Il s'agit d'un argument normal non nommé (c'est à dire pas un argument ayant sa place dans __local_args__)
            else if (i < fun->nbArgs - fun->nbOptArgs) {
                variables[variable_index] = fun->args[i];
                values[variable_index] = eval_aux(tb, arg_tree[i]);
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
                neo_list_append(local_args, eval_aux(tb, arg_tree[i]));
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
            NeObjAddr self_address = get_address(tb, arg_tree[0]);
            NeObj* self_address_ptr = get_unsafe_address(&self_address);
            neobject_destroy(*self_address_ptr);
            *self_address_ptr = self_final_value;
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

NeObj eval_ListIndex(TreeBuffer* tb, TreeBufferIndex tree)
{
    struct ListIndex* tree_list_index = treeLstIndx(tb, tree);

    NeObj obj = eval_aux(tb, tree_list_index->object);

    return_on_error(NEO_VOID);

    NeObj index = eval_aux(tb, tree_list_index->index);

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

NeObj eval_ContainerLit(TreeBuffer* tb, TreeBufferIndex tree)
{
    struct ContainerLit* tree_cont_lit = treeContLit(tb, tree);
    size_t nb_attributes = treelistLength(tb, tree_cont_lit->attributes);
    NeList val;
    nelist_init(&val, nb_attributes);

    for (size_t i = 0 ; i < nb_attributes ; i++)
    {

        val.tab[i] = eval_aux(tb, treeAttrLit(tb, treelistGet(tb, tree_cont_lit->attributes)[i])->expr);

        if_error
        {
            nelist_deinit_until(&val, i - 1);
            return NEO_VOID;
        }
    }
    NeObj ret = neo_container_create(tree_cont_lit->container_type, val);
    
    return ret;
}

NeObj eval_Attribute(TreeBuffer* tb, TreeBufferIndex tree)
{
    struct Attribute* tree_attr = treeAttr(tb, tree);

    NeObj neo = eval_aux(tb, tree_attr->object);

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
        int index = get_field_index(c, tree_attr->name);

        if_error {
            neobject_destroy(neo);
            return NEO_VOID;
        }

        tree_attr->index = index;
        neon_free(tree_attr->name);
        tree_attr->name = NULL;
        tree_attr->last_cont_type = c->type;
    }

    NeObj ret = neo_copy(get_container_field(c, (size_t)tree_attr->index));
    neobject_destroy(neo);
    return ret;
}


NeObj eval_Variable(TreeBuffer* tb, TreeBufferIndex tree)
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

NeObj eval_Const(TreeBuffer* tb, TreeBufferIndex tree)
{
    return neo_copy(treeConst(tb, tree)->obj);
}