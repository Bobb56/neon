#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include <stdint.h>

#include "constants.h"
#include "dynarrays.h"

typedef struct Ast
{
    uint8_t type;
    int fin;
    struct Ast* suiv;
} Ast;


char* nomBlockLine(char* blockline);//a liberer apres
void finTokensSimples(char* string, bool* isPotentiallyNumber, bool* isPotentiallyString, bool* isPotentiallyWord, bool* isPotentiallyOp, bool* isPotentiallyLongComm, bool* isPotentiallyString2, bool* isPotentiallyComm, bool* isPotentiallyHexBin, char* char2, char* char1, bool* nouvTok, strlist* tokenAdd, intlist* typeTok, intlist* lines, int* debTok, int i, int* stepNumber, int* stepHexBin, int len_string, int* nombrePoints);
void debutTokensSimples(int i, int* debTok, char* char1, bool* isPotentiallyString, bool* isPotentiallyNumber, bool* isPotentiallyWord, bool* isPotentiallyOp, bool* isPotentiallyString2, bool* isPotentiallyComm, bool* isPotentiallyLongComm, bool* isPotentiallyHexBin);
void annulerOuPoursuivre(bool* isPotentiallyPar, bool* isPotentiallyFonc, int* foncStep, bool* isPotentiallyInst, int* instStep, bool* isPotentiallyListIndex, int* listIndexStep, strlist* tokenAdd, intlist* typeTok, int k, int *foncParDeb, int *listeParDeb, int *indexParDeb, int* nbCro);
void debutTokensComposes(int k, intlist* typeTok, strlist* tokenAdd, int* debTok2, int* debTok3, int* lastDebTok, bool* nouvTokComp, bool* isPotentiallyList, bool* isPotentiallyPar, bool* isPotentiallyFonc, bool* isPotentiallyListIndex, bool* isPotentiallyBlock, bool* isPotentiallyInst, int* listIndexStep, int *foncStep, int *instStep, int *foncParDeb, int *listeParDeb, int *indexParDeb, int* nbPar, int* nbCro);
void finsTypesComposes(int *debTok2, int *debTok3, int* lastDebTok, int k, bool *nouvTokComp, bool* isPotentiallyPar, bool *isPotentiallyFonc, int *foncStep, strlist* tokenAdd, intlist typeTok, int *nbPar, bool *isPotentiallyInst, int *instStep, int *listIndexStep, bool *isPotentiallyList, bool *isPotentiallyListIndex, int *nbCro, bool *isPotentiallyBlock, int *nbAcc, int *foncParDeb, int *listeParDeb, int *indexParDeb, Ast** ast, int offset, intlist* lines);
void verificationGrammaire(strlist* tokens, Ast** ast, bool* isPotentiallyExpr, int* debExpr);
void compConditionBlock(int cas, bool* isPotentiallyConBlock, int *conBlockStep, strlist* tokens, intlist* types, int *i, char* typeact, int* debConBlock);
void statements(intlist* types, strlist* tokens, Ast** ast, intlist* lines, int offset);
void cut(strlist* tokens, intlist* types, char* str, bool traiterStatements, intlist* lines);
void parse(strlist* tokenAdd, intlist typeTok, Ast** ast, intlist* lines, int offset);
Ast** ast_create(intlist* typeTok);
void ast_push(Ast* ast);
int ast_pop(Ast* ast);
void ast_destroy(Ast ** ast, int length);
void ast_aff(Ast** ast, int len);
bool strlist_ast_inList(strlist* list, Ast** ast, char* elt, int type, int offset);
bool ast_typeInAst(Ast** ast, int len, int type, int offset);
int ast_typeCountAst(Ast** ast, int len, int type, int offset);
int ast_minOp(Ast** ast, strlist* tokens, int offset);
int ast_length(Ast** ast, int token_length, int offset);
int ast_minOp(Ast** ast, strlist* tokens, int offset);
int ast_length(Ast** ast, int token_length, int offset);
int ast_index(Ast** ast, int index, int offset);
int ast_prec(Ast** ast, int index, int offset);

#endif