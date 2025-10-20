#ifndef SYNTAXTREES_H
#define SYNTAXTREES_H

#include <stdint.h>

#include "constants.h"
#include "objects.h"
#include "parser.h"
#include "trees.h"


char* blockFromStatement(char* statement);
char* expressionFromStatement(char* statement);
char* fonctionName(char* fonction, char* set);
char* fonctionArgs(char* fonction, char* set);
NeTree createExpressionTree(char* string);
void affExpr(NeTree tree);
NeTree createConditionBlockTree(Ast** ast, toklist* tokens, intlist* lines, int offset);
NeTree createStatementElseTree(Ast** ast, toklist* tokens, intlist* lines, int offset);
NeTree createStatementIEWTree(Ast** ast, toklist* tokens, intlist* lines, int offset, TreeType type);
NeTree createSyntaxTree(char* program);
NeTree createFunctionTree(Ast** ast, toklist* tokens, intlist* lines, int offset, bool isMethod);
NeTree createSyntaxTreeAux(Ast** ast, toklist* tokens, intlist* lines, int offset);
NeTree createExpressionTreeAux(Ast** ast, toklist* tokens, intlist* lines, int offset);


#endif