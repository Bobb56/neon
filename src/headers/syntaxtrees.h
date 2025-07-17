#ifndef SYNTAXTREES_H
#define SYNTAXTREES_H

#include <stdint.h>

#include "constants.h"
#include "objects.h"
#include "parser.h"


struct Tree
{
    uint8_t type;
    int line;
    char* label1;
    int label2;
    struct Tree** sons;
    int nbSons;
    int capacity;
    NeObj data; // permet de stocker les données contenues dans les feuilles
    Var variable; // adresse de la variable relative au tableau ADRESSES : indice de la variable dans ADRESSES
};


Tree* tree_create(char* label1, int label2, uint8_t type);
void tree_appendSon(Tree* tree, Tree* son);
bool tree_isEmpty(Tree* tree);
void tree_destroy(Tree* tree);
void tree_removeSon(Tree* tree, int index);
Tree* tree_copy(Tree* tree);
char* blockFromStatement(char* statement);
char* expressionFromStatement(char* statement);
char* fonctionName(char* fonction, char* set);
char* fonctionArgs(char* fonction, char* set);
void createExpressionTree(Tree* tree, char* string);
void affExpr(Tree* tree);
void createConditionBlockTree(Tree* tree, Ast** ast, strlist* tokens, intlist* lines, int offset);
void createStatementElseTree(Tree* tree, Ast** ast, strlist* tokens, intlist* lines, int offset);
void createStatementIEWFTree(Tree* tree, Ast** ast, strlist* tokens, intlist* lines, int offset, int type);
void createSyntaxTree(Tree* tree, char* program);
void createFunctionTree(Tree* tree, Ast** ast, strlist* tokens, intlist* lines, int offset, bool isMethod);
void createSyntaxTreeAux(Tree* tree, Ast** ast, strlist* tokens, intlist* lines, int offset);



#endif