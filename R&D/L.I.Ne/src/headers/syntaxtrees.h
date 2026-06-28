#ifndef SYNTAXTREES_H
#define SYNTAXTREES_H

#include <stdint.h>

#include "constants.h"
#include "objects.h"
#include "parser.h"
#include "trees.h"


char* get_tree_const(TreeBuffer* tb, TreeBufferIndex tree);
TreeBuffer createExpressionTree(char* string, bool free_after);
void affExpr(TreeBuffer* tb, TreeBufferIndex tree);
TreeBufferIndex createConditionBlockTree(TreeBuffer* tb, Ast** ast, toklist* tokens, intlist* lines, size_t offset);
TreeBufferIndex createStatementElseTree(TreeBuffer* tb, Ast** ast, toklist* tokens, intlist* lines, size_t offset);
TreeBufferIndex createStatementIEWTree(TreeBuffer* tb, Ast** ast, toklist* tokens, intlist* lines, size_t offset, TreeType type);
TreeBuffer createSyntaxTree(char* program, bool free_after);
TreeBufferIndex createFunctionTree(TreeBuffer* tb, Ast** ast, toklist* tokens, intlist* lines, size_t offset, bool isMethod);
TreeBufferIndex createSyntaxTreeAux(TreeBuffer* tb, Ast** ast, toklist* tokens, intlist* lines, size_t offset);
TreeBufferIndex createExpressionTreeAux(TreeBuffer* tb, Ast** ast, toklist* tokens, intlist* lines, size_t offset);


#endif