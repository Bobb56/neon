#ifndef EVAL_H
#define EVAL_H

#include "neobj.h"
#include "trees.h"

NeObj eval_UnaryOp(TreeBuffer* tb, TreeBufferIndex tree);
NeObj eval_BinaryOp(TreeBuffer* tb, TreeBufferIndex tree);
NeObj eval_List(TreeBuffer* tb, TreeBufferIndex tree);
NeObj eval_ParallelCall(TreeBuffer* tb, TreeBufferIndex tree);
NeObj eval_FunctionCall(TreeBuffer* tb, TreeBufferIndex tree);
NeObj eval_ListIndex(TreeBuffer* tb, TreeBufferIndex tree);
NeObj eval_ContainerLit(TreeBuffer* tb, TreeBufferIndex tree);
NeObj eval_Attribute(TreeBuffer* tb, TreeBufferIndex tree);
NeObj eval_Variable(TreeBuffer* tb, TreeBufferIndex tree);
NeObj eval_Const(TreeBuffer* tb, TreeBufferIndex tree);

#endif