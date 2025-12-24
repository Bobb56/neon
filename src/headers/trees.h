#ifndef TREES_H
#define TREES_H

#include <stdint.h>
#include <stdbool.h>
#include <stdint.h>
#include "neobj.h"
#include "constants.h"
#include "dynarrays.h"


/*
#include <fileioc.h>

typedef uint16_t TreeBufferIndex;

struct TreeListTemp {
    TreeBufferIndex* trees;
    unsigned int len;
};

typedef struct TreeBuffer {
    void* pointer;
    uint16_t size;
    uint8_t handle;
    char name[9];
    struct TreeListTemp remember;
} TreeBuffer;
*/


typedef unsigned int TreeBufferIndex;

struct TreeListTemp {
    TreeBufferIndex* trees;
    uint16_t len;
};

typedef struct TreeBuffer {
    TreeBufferIndex entry_point;
    int size;
    int n_blocks;
    int block_size;
    void* pointer;
    bool locked;
} TreeBuffer;



#define TREE_VOID                       ((TreeBufferIndex)-1)
#define GENERAL_INFO                    TreeType type; uint16_t line;
#define TREE_ISVOID(tbi)                (tbi == (TreeBufferIndex)-1)
#define TREE_AFFECT(left, right)        {TreeBufferIndex temp = right ; left = temp;}
#define TREELIST_AFFECT(left, right)    {struct TreeList temp = right ; left = temp;}
#define TREE_TYPE(tb, tbi)              ((struct {GENERAL_INFO}*)(((tb)->pointer)+tbi))->type
#define TREE_LINE(tb, tbi)              ((struct {GENERAL_INFO}*)(((tb)->pointer)+tbi))->line

#define treeUnOp(tb, tbi)               ((struct UnaryOp*)(((tb)->pointer) + tbi))
#define treeKW(tb, tbi)                 ((struct Keyword*)(((tb)->pointer) + tbi))
#define treeKWParam(tb, tbi)            ((struct KWParam*)(((tb)->pointer) + tbi))
#define treeBinOp(tb, tbi)              ((struct BinaryOp*)(((tb)->pointer) + tbi))
#define treeVar(tb, tbi)                ((struct Variable*)(((tb)->pointer) + tbi))
#define treeConst(tb, tbi)              ((struct ConstObj*)(((tb)->pointer) + tbi))
#define treeTE(tb, tbi)                 ((struct TryExcept*)(((tb)->pointer) + tbi))
#define treeLstIndx(tb, tbi)            ((struct ListIndex*)(((tb)->pointer) + tbi))
#define treeAttr(tb, tbi)               ((struct Attribute*)(((tb)->pointer) + tbi))
#define treeSntxTree(tb, tbi)           ((struct SyntaxTree*)(((tb)->pointer) + tbi))
#define treeFDef(tb, tbi)               ((struct FunctionDef*)(((tb)->pointer) + tbi))
#define treeExpt(tb, tbi)               ((struct ExceptBlock*)(((tb)->pointer) + tbi))
#define treeIEW(tb, tbi)                ((struct StatementIEW*)(((tb)->pointer) + tbi))
#define treeFor(tb, tbi)                ((struct StatementFor*)(((tb)->pointer) + tbi))
#define treeFCall(tb, tbi)              ((struct FunctionCall*)(((tb)->pointer) + tbi))
#define treeContLit(tb, tbi)            ((struct ContainerLit*)(((tb)->pointer) + tbi))
#define treeAttrLit(tb, tbi)            ((struct AttributeLit*)(((tb)->pointer) + tbi))
#define treeParCall(tb, tbi)            ((struct ParallelCall*)(((tb)->pointer) + tbi))
#define treelistGet(tb, tl)             ((TreeBufferIndex*)(((tb)->pointer) + (tl).indices))



#define FOR_NBARGS(tb,tbi)        (treeFor(tb,tbi)->params.length)
#define FOR_ARG(tb,tbi, n)        (treelistGet(tb, treeFor(tb,tbi)->params)[n])


typedef enum {
    TypeBinaryOp        = 1,
    TypeUnaryOp         = 2,
    TypeConst           = 3,
    TypeFor             = 4,
    TypeForeach         = 5,
    TypeWhile           = 6,
    TypeIf              = 7,
    TypeElif            = 8,
    TypeElse            = 9,
    TypeTryExcept       = 10,
    TypeFunctiondef     = 11,
    TypeVariable        = 12,
    TypeListindex       = 13,
    TypeFunctioncall    = 14,
    TypeAttribute       = 15,
    TypeAtomic          = 16,
    TypeConditionblock  = 17,
    TypeList            = 18,
    TypeKeyword         = 19,
    TypeKWParam         = 20,
    TypeSyntaxtree      = 21,
    TypeContainerLit    = 22,
    TypeAttributeLit    = 23,
    TypeExceptBlock     = 24,
    TypeParallelCall    = 25
} TreeType;



struct TreeList {
    TreeBufferIndex indices; /* Pointeur vers le début du tableau dans le TreeBuffer*/
    uint16_t length;
};

struct BinaryOp {
    GENERAL_INFO
    int op;
    TreeBufferIndex left;
    TreeBufferIndex right;
};

struct Variable {
    GENERAL_INFO
    Var var;
};

struct ListIndex {
    GENERAL_INFO
    TreeBufferIndex object;
    TreeBufferIndex index;
};

struct ContainerLit {
    GENERAL_INFO
    int container_type;
    struct TreeList attributes;
};

struct AttributeLit {
    GENERAL_INFO
    char* name;
    TreeBufferIndex expr;
};

struct Attribute {
    GENERAL_INFO
    TreeBufferIndex object;
    int index;
    int last_cont_type;
    char* name;
};

struct UnaryOp {
    GENERAL_INFO
    int op;
    TreeBufferIndex expr;
};

struct ConstObj {
    GENERAL_INFO
    NeObj obj;
};

struct StatementIEW {
    GENERAL_INFO
    TreeBufferIndex expression;
    TreeBufferIndex code;
};

struct StatementFor {
    GENERAL_INFO
    TreeBufferIndex block;
    struct TreeList params;
};


struct FunctionCall {
    GENERAL_INFO
    TreeBufferIndex function;
    NeObj function_obj;
    struct TreeList args;
};


struct TryExcept {
    GENERAL_INFO
    TreeBufferIndex try_tree;
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
    TreeBufferIndex block;
};

struct ParallelCall {
    GENERAL_INFO
    TreeBuffer* expr_buffer;
    TreeBufferIndex expr;
};

int TreeBuffer_init(TreeBuffer*);
void TreeBuffer_destroy(TreeBuffer* tb);
void TreeBuffer_delete_all(ptrlist* tree_buffers);
TreeBufferIndex TreeBuffer_alloc(TreeBuffer* tb, int size);
void TreeList_destroy(TreeBuffer* tb, struct TreeList* treelist);
void NeTree_destroy(TreeBuffer* tb, TreeBufferIndex tree);

TreeBufferIndex NeTree_create(TreeBuffer* tb, TreeType type, int line);
void TreeListTemp_init(struct TreeListTemp* tree_list);
void TreeListTemp_destroy(TreeBuffer* tb, struct TreeListTemp* list);
void TreeListTemp_append(struct TreeListTemp* tree_list, TreeBufferIndex tree);
void TreeListTemp_insert(struct TreeListTemp* tree_list, TreeBufferIndex tree, int index);
struct TreeList TreeListTemp_dump(TreeBuffer* tb, struct TreeListTemp* temp_list);



TreeBufferIndex NeTree_make_binaryOp(TreeBuffer* tb, int op, TreeBufferIndex left, TreeBufferIndex right, int line);
TreeBufferIndex NeTree_make_unaryOp(TreeBuffer* tb, int op, TreeBufferIndex expr, int line);
TreeBufferIndex NeTree_make_parallel_call(TreeBuffer* tb, TreeBuffer* expr_buffer, TreeBufferIndex expr, int line);
TreeBufferIndex NeTree_make_variable(TreeBuffer* tb, Var variable, int line);
TreeBufferIndex NeTree_make_const(TreeBuffer* tb, NeObj const_obj, int line);
TreeBufferIndex NeTree_make_IEWF_tree(TreeBuffer* tb, TreeBufferIndex expr, TreeBufferIndex block, TreeType type, int line);
TreeBufferIndex NeTree_make_fcall(TreeBuffer* tb, int line);
TreeBufferIndex NeTree_make_attribute(TreeBuffer* tb, TreeBufferIndex object, char* name, int line);
TreeBufferIndex NeTree_make_functiondef(TreeBuffer* tb, char* name, struct TreeListTemp args, NeObj object, int line);
TreeBufferIndex NeTree_make_except_block(TreeBuffer* tb, struct TreeListTemp exceptions, TreeBufferIndex block, int line);
TreeBufferIndex NeTree_make_tryexcept(TreeBuffer* tb, TreeBufferIndex try_tree, struct TreeListTemp except_blocks, int line);
TreeBufferIndex NeTree_make_kwparam(TreeBuffer* tb, struct TreeListTemp params, int code, int line);
TreeBufferIndex NeTree_make_for_tree(TreeBuffer* tb, struct TreeListTemp params, TreeBufferIndex block, int line, TreeType type);
bool NeTree_isexpr(TreeBuffer* tb, TreeBufferIndex tree);

#endif