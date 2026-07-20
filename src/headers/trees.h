#ifndef TREES_H
#define TREES_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
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
    size_t size;
    size_t n_blocks;
    size_t block_size;
    void* pointer;
    bool locked;
    bool side_memory;
    int id;
} TreeBuffer;



#define TREE_VOID                       ((TreeBufferIndex)-1)
#define GENERAL_INFO                    TreeType type; uint16_t line;
#define TREE_ISVOID(tbi)                (tbi == (TreeBufferIndex)-1)
#define TREE_AFFECT(left, right)        {TreeBufferIndex temp = right ; left = temp;}
#define TREELIST_AFFECT(left, right)    {TreeBufferIndex temp = right ; left = temp;}
#define TREE_TYPE(tb, tbi)              ((struct {GENERAL_INFO}*)(((tb)->pointer)+tbi))->type
#define TREE_LINE(tb, tbi)              ((struct {GENERAL_INFO}*)(((tb)->pointer)+tbi))->line


#define BYTE(tb, tbi)                   (*(uint8_t*)((tb)->pointer + tbi))
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
#define treelistGet(tb, tl)             ((TreeBufferIndex*)(((tb)->pointer) + tl + 4))
#define treelistLength(tb, tl)          (*(uint16_t*)(((tb)->pointer) + tl + 2))


#define FOR_NBARGS(tb,tbi)        (treelistLength(tb, treeFor(tb, tbi)->params))
#define FOR_ARG(tb,tbi, n)        (treelistGet(tb, treeFor(tb,tbi)->params)[n])


typedef uint8_t TreeType;
#define TypeBinaryOp            1
#define TypeUnaryOp             2
#define TypeConst               3
#define TypeFor                 4
#define TypeForeach             5
#define TypeWhile               6
#define TypeIf                  7
#define TypeElif                8
#define TypeElse                9
#define TypeTryExcept           10
#define TypeFunctiondef         11
#define TypeVariable            12
#define TypeListindex           13
#define TypeFunctioncall        14
#define TypeAttribute           15
#define TypeAtomic              16
#define TypeConditionblock      17
#define TypeList                18
#define TypeKeyword             19
#define TypeKWParam             20
#define TypeSyntaxtree          21
#define TypeContainerLit        22
#define TypeAttributeLit        23
#define TypeExceptBlock         24
#define TypeParallelCall        25

#define TypeTreeList            30



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
    TreeBufferIndex attributes;
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
    TreeBufferIndex params;
};


struct FunctionCall {
    GENERAL_INFO
    TreeBufferIndex function;
    NeObj function_obj;
    TreeBufferIndex args;
};


struct TryExcept {
    GENERAL_INFO
    TreeBufferIndex try_tree;
    TreeBufferIndex except_blocks;
};

struct FunctionDef {
    GENERAL_INFO
    char* name;
    NeObj object;
    TreeBufferIndex args;
};

struct Keyword {
    GENERAL_INFO
    int code;
};

struct KWParam {
    GENERAL_INFO
    int code;
    TreeBufferIndex params;
};

struct SyntaxTree {
    GENERAL_INFO
    TreeBufferIndex treelist;
};

struct ExceptBlock {
    GENERAL_INFO
    TreeBufferIndex exceptions;
    TreeBufferIndex block;
};

struct ParallelCall {
    GENERAL_INFO
    TreeBuffer* expr_buffer;
    TreeBufferIndex expr;
};

size_t type_size(TreeType type);

#define TreeList_destroy(...)
#define NeTree_destroy(...)

char* printable_tree_type(TreeBuffer* tb, TreeBufferIndex tree);

void TreeBuffer_init(TreeBuffer*);
void TreeBuffer_iter(TreeBuffer* tb, void (*function)(TreeBuffer*, TreeBufferIndex, void* arg), void* arg);
void TreeBuffer_destroy(TreeBuffer* tb);
void TreeBuffer_delete_all(ptrlist* tree_buffers);
TreeBufferIndex TreeBuffer_alloc(TreeBuffer* tb, size_t size);

TreeBufferIndex NeTree_create(TreeBuffer* tb, TreeType type, int line);
void TreeListTemp_init(struct TreeListTemp* tree_list);
void TreeListTemp_append(struct TreeListTemp* tree_list, TreeBufferIndex tree);
void TreeListTemp_insert(struct TreeListTemp* tree_list, TreeBufferIndex tree, int index);
void TreeListTemp_destroy(TreeBuffer* tb, struct TreeListTemp* temp_list);
TreeBufferIndex TreeList_alloc(TreeBuffer* tb, uint16_t length);
TreeBufferIndex TreeListTemp_dump(TreeBuffer* tb, struct TreeListTemp* temp_list);



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