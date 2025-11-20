#include <stdlib.h>
#include "headers/trees.h"
#include "headers/dynarrays.h"
#include "headers/errors.h"
#include "headers/neon.h"
#include "headers/objects.h"


size_t type_size(TreeType type) {
    switch (type) {
        case TypeBinaryOp:
            return sizeof(struct BinaryOp);
        case TypeUnaryOp:
            return sizeof(struct UnaryOp);
        case TypeConst:
            return sizeof(struct ConstObj);
        case TypeFor:
        case TypeForeach:
            return sizeof(struct StatementFor);
        case TypeWhile:
        case TypeIf:
        case TypeElif:
            return sizeof(struct StatementIEW);        
        case TypeElse:
        case TypeSyntaxtree:
        case TypeAtomic:
        case TypeConditionblock:
        case TypeList:
            return sizeof(struct SyntaxTree);
        case TypeTryExcept:
            return sizeof(struct TryExcept);
        case TypeFunctiondef:
            return sizeof(struct FunctionDef);
        case TypeVariable:
            return sizeof(struct Variable);
        case TypeListindex:
            return sizeof(struct ListIndex);
        case TypeFunctioncall:
            return sizeof(struct FunctionCall);
        case TypeAttribute:
            return sizeof(struct Attribute);
        case TypeKeyword:
            return sizeof(struct Keyword);
        case TypeKWParam:
            return sizeof(struct KWParam);
        case TypeContainerLit:
            return sizeof(struct ContainerLit);
        case TypeAttributeLit:
            return sizeof(struct AttributeLit);
        case TypeExceptBlock:
            return sizeof(struct ExceptBlock);
    }
}




NeTree NeTree_create(TreeType type, int line) {
    NeTree tree;
    tree.pointer = neon_malloc(type_size(type));

    if (tree.pointer == NULL) {
        global_env->CODE_ERROR = 12;
        return TREE_VOID;
    }

    tree.general_info->line = line;
    tree.general_info->type = type;
    return tree;
}


bool NeTree_isvoid(NeTree tree) {
    return tree.pointer == NULL;
}





void NeTree_destroy(NeTree tree) {
    if (NeTree_isvoid(tree))
        return;

    switch (tree.general_info->type) {
        case TypeBinaryOp:
            NeTree_destroy(tree.binary_op->left);
            NeTree_destroy(tree.binary_op->right);
            break;
        
        case TypeUnaryOp:
            NeTree_destroy(tree.unary_op->expr);
            break;

        case TypeConst:
            neobject_destroy(tree.const_obj->obj);
            break;
        
        case TypeFor:
        case TypeForeach:
            TreeList_destroy(&tree.for_tree->params);
            NeTree_destroy(tree.for_tree->block);
            break;

        case TypeWhile:
        case TypeIf:
        case TypeElif:
            NeTree_destroy(tree.iew->expression);
            NeTree_destroy(tree.iew->code);
            break;
        
        case TypeAtomic:
        case TypeConditionblock:
        case TypeSyntaxtree:
        case TypeElse:
        case TypeList:
            TreeList_destroy(&tree.syntaxtree->treelist);
            break;
        
        case TypeTryExcept:
            NeTree_destroy(tree.tryexcept->try_tree);
            TreeList_destroy(&tree.tryexcept->except_blocks);
            break;
        
        case TypeFunctiondef:
            neon_free(tree.functiondef->name);
            neobject_destroy(tree.functiondef->object);
            TreeList_destroy(&tree.functiondef->args);
            break;
        
        case TypeListindex:
            NeTree_destroy(tree.listindex->index);
            NeTree_destroy(tree.listindex->object);
            break;
        
        case TypeFunctioncall:
            TreeList_destroy(&tree.fcall->args);
            NeTree_destroy(tree.fcall->function);
            if (!neo_is_void(tree.fcall->function_obj))
                neobject_destroy(tree.fcall->function_obj);
            break;
        
        case TypeAttribute:
            if (tree.attribute->name != NULL)
                neon_free(tree.attribute->name);
            NeTree_destroy(tree.attribute->object);
            break;
        
        case TypeKWParam:
            TreeList_destroy(&tree.kwparam->params);
            break;
        
        case TypeContainerLit:
            TreeList_destroy(&tree.container_lit->attributes);
            break;
        
        case TypeAttributeLit:
            neon_free(tree.attribute_lit->name);
            NeTree_destroy(tree.attribute_lit->expr);
            break;
        
        case TypeExceptBlock:
            NeTree_destroy(tree.except_block->block);
            TreeList_destroy(&tree.except_block->exceptions);
            break;
        
        case TypeKeyword:
        case TypeVariable:
            break;
    }
    neon_free(tree.pointer);
}



bool NeTree_isexpr(NeTree tree) {
    intlist expressions = {
        .tab = (int[]) {
                            TypeBinaryOp,
                            TypeUnaryOp,
                            TypeConst,
                            TypeVariable,
                            TypeListindex,
                            TypeFunctioncall,
                            TypeAttribute,
                            TypeList,
                            TypeContainerLit,
                            TypeAttributeLit
                        },
        .len = 10
    };
    return intlist_inList(&expressions, TREE_TYPE(tree));
}





void TreeList_init(struct TreeList* tree_list)
{
    tree_list->trees = NULL;
    tree_list->len = 0;
}

void TreeList_append(struct TreeList* tree_list, NeTree tree) {
    NeTree* ptr;
    if (tree_list->len == 0) {
        ptr = neon_malloc(sizeof(NeTree));
    }
    else {
        ptr = neon_realloc(tree_list->trees, (tree_list->len + 1) * sizeof(NeTree));
    }

    if (ptr == NULL) {
        global_env->CODE_ERROR = 12;
        return;
    }

    tree_list->trees = ptr;
    tree_list->trees[tree_list->len++] = tree;
}

void TreeList_insert(struct TreeList* tree_list, NeTree tree, int index) {
    if (tree_list->len == 0) {
        tree_list->trees = neon_malloc(sizeof(NeTree));
    }
    else {
        tree_list->trees = neon_realloc(tree_list->trees, (tree_list->len + 1) * sizeof(NeTree));
    }

    tree_list->len++;

    for (int i = tree_list->len - 1 ; i > index ; i--) {
        tree_list->trees[i] = tree_list->trees[i-1];
    }

    tree_list->trees[index] = tree;
}


void TreeList_destroy(struct TreeList* tree_list) {
    for (int i = 0 ; i < tree_list->len ; i++) {
        NeTree_destroy(tree_list->trees[i]);
    }
    if (tree_list->trees != NULL)
        neon_free(tree_list->trees);
}


NeTree NeTree_make_unaryOp(int op, NeTree expr, int line) {
    NeTree tree = NeTree_create(TypeUnaryOp, line);

    return_on_error(TREE_VOID);

    tree.unary_op->op = op;
    tree.unary_op->expr = expr;
    return tree;
}

NeTree NeTree_make_binaryOp(int op, NeTree left, NeTree right, int line) {
    NeTree tree = NeTree_create(TypeBinaryOp, line);
    return_on_error(TREE_VOID);

    tree.binary_op->op = op;
    tree.binary_op->left = left;
    tree.binary_op->right = right;
    return tree;
}

NeTree NeTree_make_variable(Var variable, int line) {
    NeTree tree = NeTree_create(TypeVariable, line);
    return_on_error(TREE_VOID);

    tree.variable->var = variable;
    return tree;
}

NeTree NeTree_make_const(NeObj const_obj, int line) {
    NeTree tree = NeTree_create(TypeConst, line);
    return_on_error(TREE_VOID);

    tree.const_obj->obj = const_obj;
    return tree;
}

NeTree NeTree_make_containerlit(int line) {
    NeTree tree = NeTree_create(TypeContainerLit, line);
    return_on_error(TREE_VOID);

    TreeList_init(&tree.container_lit->attributes);
    return tree;
}

void NeTree_containerlit_add_attribute(NeTree* tree, NeTree attr) {
    TreeList_append(&tree->container_lit->attributes, attr);
}


NeTree NeTree_make_fcall(int line) {
    NeTree tree = NeTree_create(TypeFunctioncall, line);
    return_on_error(TREE_VOID);

    TreeList_init(&tree.fcall->args);

    tree.fcall->function_obj = NEO_VOID;
    return tree;
}

void NeTree_fcall_add_arg(NeTree* tree, NeTree arg) {
    TreeList_append(&tree->fcall->args, arg);
}


NeTree NeTree_make_functiondef(char* name, struct TreeList args, NeObj object, int line) {
    NeTree tree = NeTree_create(TypeFunctiondef, line);
    return_on_error(TREE_VOID);

    tree.functiondef->args = args;
    tree.functiondef->object = object;
    tree.functiondef->name = name;
    return tree;
}


NeTree NeTree_make_attribute(NeTree object, char* name, int line) {
    NeTree tree = NeTree_create(TypeAttribute, line);
    return_on_error(TREE_VOID);

    tree.attribute->last_cont_type = -1;
    tree.attribute->index = -1;
    tree.attribute->name = name;
    tree.attribute->object = object;
    return tree;
}

NeTree NeTree_make_IEWF_tree(NeTree expr, NeTree block, TreeType type, int line) {
    NeTree tree = NeTree_create(type, line);
    return_on_error(TREE_VOID);

    tree.iew->expression = expr;
    tree.iew->code = block;
    return tree;
}

NeTree NeTree_make_except_block(struct TreeList exceptions, NeTree block, int line) {
    NeTree tree = NeTree_create(TypeExceptBlock, line);
    return_on_error(TREE_VOID);

    tree.except_block->exceptions = exceptions;
    tree.except_block->block = block;
    return tree;
}


NeTree NeTree_make_tryexcept(NeTree try_tree, struct TreeList except_blocks, int line) {
    NeTree tryexcept = NeTree_create(TypeTryExcept, line);
    return_on_error(TREE_VOID);

    tryexcept.tryexcept->except_blocks = except_blocks;
    tryexcept.tryexcept->try_tree = try_tree;
    return tryexcept;
}

NeTree NeTree_make_kwparam(struct TreeList params, int code, int line) {
    NeTree tree = NeTree_create(TypeKWParam, line);
    return_on_error(TREE_VOID);

    tree.kwparam->code = code;
    tree.kwparam->params = params;
    return tree;
}

NeTree NeTree_make_for_tree(struct TreeList params, NeTree block, int line, TreeType type) {
    NeTree tree = NeTree_create(type, line);
    return_on_error(TREE_VOID);

    tree.for_tree->params = params;
    tree.for_tree->block = block;
    return tree;
}


// fonctions pour créer et modifier les arbres utilisant la structure struct Syntaxtree (listes, else, cb, atomic)

NeTree NeTree_make_syntaxtree(TreeType type, int line) {
    NeTree tree = NeTree_create(type, line);

    return_on_error(TREE_VOID);

    TreeList_init(&tree.syntaxtree->treelist);
    return tree;
}

void NeTree_add_syntaxtree(NeTree* tree, NeTree son) {
    TreeList_append(&tree->syntaxtree->treelist, son);
}
