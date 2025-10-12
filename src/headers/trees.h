#ifndef TREES_H
#define TREES_H

#include <stdbool.h>
#include <stdint.h>
#include "neobj.h"


#define GENERAL_INFO            TreeType type; unsigned int line;
#define TREE_TYPE(tree)         (tree.general_info->type)
#define TREE_LINE(tree)         (tree.general_info->line)

#define FOR_NBARGS(tree)        (tree.for_tree->params.len)
#define FOR_ARG(tree, n)        (tree.for_tree->params.trees[n])


typedef enum {
    TypeBinaryOp = 1,
    TypeUnaryOp = 2,
    TypeConst = 3,
    TypeFor = 4,
    TypeForeach = 5,
    TypeWhile = 6,
    TypeIf = 7,
    TypeElif = 8,
    TypeElse = 9,
    TypeTryExcept = 10,
    TypeFunctiondef = 11,
    TypeVariable = 12,
    TypeListindex = 13,
    TypeFunctioncall = 14,
    TypeAttribute = 15,
    TypeAtomic = 16,
    TypeConditionblock = 17,
    TypeList = 18,
    TypeKeyword = 19,
    TypeKWParam = 20,
    TypeSyntaxtree = 21,
    TypeContainerLit = 22,
    TypeAttributeLit = 23,
    TypeExceptBlock = 24
} TreeType;


typedef union NeTree {
    void* pointer;                          // pour accéder au pointeur brut
    struct {GENERAL_INFO} *general_info;    // pour accéder aux deux champs communs à toutes les structures

    // structures spécifiques à chaque type d'arbres, à utiliser en fonction du type
    struct BinaryOp* binary_op;
    struct UnaryOp* unary_op;
    struct Variable* variable;
    struct ConstObj* const_obj;
    struct StatementIEW* iew;
    struct StatementFor* for_tree;
    struct SyntaxTree* syntaxtree;
    struct FunctionCall* fcall;
    struct TryExcept* tryexcept;
    struct Enum* enum_tree;
    struct FunctionDef* functiondef;
    struct ListIndex* listindex;
    struct Attribute* attribute;
    struct Keyword* keyword;
    struct KWParam* kwparam;
    struct ContainerLit* container_lit;
    struct AttributeLit* attribute_lit;
    struct ExceptBlock* except_block;
} NeTree;



#define TREE_VOID               ((NeTree) {.pointer = NULL})



struct TreeList {
    NeTree* trees;
    int len;
};

struct BinaryOp {
    GENERAL_INFO
    int op;
    NeTree left;
    NeTree right;
};

struct Variable {
    GENERAL_INFO
    Var var;
};

struct ListIndex {
    GENERAL_INFO
    NeTree object;
    NeTree index;
};

struct ContainerLit {
    GENERAL_INFO
    int container_type;
    struct TreeList attributes;
};

struct AttributeLit {
    GENERAL_INFO
    char* name;
    NeTree expr;
};

struct Attribute {
    GENERAL_INFO
    NeTree object;
    int index;
    int last_cont_type;
    char* name;
};

struct UnaryOp {
    GENERAL_INFO
    int op;
    NeTree expr;
};

struct ConstObj {
    GENERAL_INFO
    NeObj obj;
};

struct StatementIEW {
    GENERAL_INFO
    NeTree expression;
    NeTree code;
};

struct StatementFor {
    GENERAL_INFO
    NeTree block;
    struct TreeList params;
};


struct FunctionCall {
    GENERAL_INFO
    NeTree function;
    NeObj function_obj;
    struct TreeList args;
};


struct TryExcept {
    GENERAL_INFO
    NeTree try_tree;
    struct TreeList except_blocks;
};

struct FunctionDef {
    GENERAL_INFO
    char* name;
    NeObj object;
    struct TreeList args;
};

struct Keyword {
    GENERAL_INFO
    int code;
};

struct KWParam {
    GENERAL_INFO
    int code;
    struct TreeList params;
};

struct SyntaxTree {
    GENERAL_INFO
    struct TreeList treelist;
};

struct ExceptBlock {
    GENERAL_INFO
    struct TreeList exceptions;
    NeTree block;
};



bool NeTree_isvoid(NeTree tree);
NeTree NeTree_create(TreeType type, int line);
void NeTree_destroy(NeTree tree);
void TreeList_init(struct TreeList* tree_list);
void TreeList_append(struct TreeList* tree_list, NeTree tree);
void TreeList_insert(struct TreeList* tree_list, NeTree tree, int index);
void TreeList_destroy(struct TreeList* tree_list);
NeTree NeTree_make_syntaxtree(TreeType type, int line);
void NeTree_add_syntaxtree(NeTree* tree, NeTree son);
NeTree NeTree_make_binaryOp(int op, NeTree left, NeTree right, int line);
NeTree NeTree_make_unaryOp(int op, NeTree expr, int line);
NeTree NeTree_make_variable(Var variable, int line);
NeTree NeTree_make_const(NeObj const_obj, int line);
NeTree NeTree_make_containerlit(int line);
NeTree NeTree_make_IEWF_tree(NeTree expr, NeTree block, TreeType type, int line);
void NeTree_containerlit_add_attribute(NeTree* tree, NeTree attr);
NeTree NeTree_make_fcall(int line);
void NeTree_fcall_add_arg(NeTree* tree, NeTree arg);
NeTree NeTree_make_attribute(NeTree object, char* name, int line);
NeTree NeTree_make_functiondef(char* name, struct TreeList args, NeObj object, int line);
NeTree NeTree_make_except_block(struct TreeList exceptions, NeTree block, int line);
NeTree NeTree_make_tryexcept(NeTree try_tree, struct TreeList except_blocks, int line);
NeTree NeTree_make_kwparam(struct TreeList params, int code, int line);
NeTree NeTree_make_for_tree(struct TreeList params, NeTree block, int line, TreeType type);
bool NeTree_isexpr(NeTree tree);

#endif