#ifndef TREES_H
#define TREES_H

#include <stdint.h>
#include "objects.h"

#define GENERAL_INFO            uint8_t type; unsigned int line;
#define TREE_TYPE(tree)         (tree.general_info->type)
#define TREE_LINE(tree)         (tree.general_info->line)

typedef union NeTree NeTree;

typedef enum {
    TypeBinaryOp,
    TypeUnaryOp,
    TypeConst,
    TypeFor,
    TypeForeach,
    TypeWhile,
    TypeIf,
    TypeElif,
    TypeElse,
    TypeTryExcept,
    TypeTry,
    TypeExcept,
    TypeFunctiondef,
    TypeVariable,
    TypeListindex,
    TypeFunctioncall
} TreeType;


struct TreeList {
    NeTree** trees;
    int len;
};

struct BinaryOp {
    GENERAL_INFO
    int op;
    NeTree* left;
    NeTree* right;
};

struct UnaryOp {
    GENERAL_INFO
    int op;
    NeTree* expr;
};

struct ConstObj {
    GENERAL_INFO
    NeObj obj;
};

struct StatementIEWF {
    GENERAL_INFO
    NeTree* expression;
    NeTree* code;
};

struct FunctionCall {
    GENERAL_INFO
    char* name;
    NeTree* function;
    struct TreeList args;
};


typedef union NeTree {
    struct {GENERAL_INFO} *general_info;
    struct BinaryOp* binary_op;
    struct UnaryOp* unary_op;
    struct ConstObj* const_obj;
} NeTree;








#endif