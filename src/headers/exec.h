#ifndef EXEC_H
#define EXEC_H

#include "trees.h"


int exec_TryExcept(TreeBuffer* tb, TreeBufferIndex tree);
int exec_Atomic(TreeBuffer* tb, TreeBufferIndex tree);
int exec_KWParam(TreeBuffer* tb, TreeBufferIndex tree);
int exec_Keyword(TreeBuffer* tb, TreeBufferIndex tree);
int exec_Conditionblock(TreeBuffer* tb, TreeBufferIndex tree);
int exec_While(TreeBuffer* tb, TreeBufferIndex tree);
int exec_For(TreeBuffer* tb, TreeBufferIndex tree);
int exec_Foreach(TreeBuffer* tb, TreeBufferIndex tree);
int exec_Functiondef(TreeBuffer* tb, TreeBufferIndex tree);
int exec_Expression(TreeBuffer* tb, TreeBufferIndex tree);


#endif