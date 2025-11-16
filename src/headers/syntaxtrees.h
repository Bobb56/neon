#ifndef SYNTAXTREES_H
#define SYNTAXTREES_H

#include <stdint.h>

#include "constants.h"
#include "objects.h"
#include "parser.h"
#include "trees.h"


TreeBufferIndex createExpressionTree(TreeBuffer* tb, char* string, bool free_after);
void affExpr(TreeBuffer* tb, TreeBufferIndex tree);
TreeBufferIndex createConditionBlockTree(TreeBuffer* tb, Ast** ast, toklist* tokens, intlist* lines, int offset);
TreeBufferIndex createStatementElseTree(TreeBuffer* tb, Ast** ast, toklist* tokens, intlist* lines, int offset);
TreeBufferIndex createStatementIEWTree(TreeBuffer* tb, Ast** ast, toklist* tokens, intlist* lines, int offset, TreeType type);
TreeBufferIndex createSyntaxTree(TreeBuffer* tb, char* program, bool free_after);
TreeBufferIndex createFunctionTree(TreeBuffer* tb, Ast** ast, toklist* tokens, intlist* lines, int offset, bool isMethod);
TreeBufferIndex createSyntaxTreeAux(TreeBuffer* tb, Ast** ast, toklist* tokens, intlist* lines, int offset);
TreeBufferIndex createExpressionTreeAux(TreeBuffer* tb, Ast** ast, toklist* tokens, intlist* lines, int offset);


#endif