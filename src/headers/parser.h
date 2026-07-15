#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include <stdint.h>

#include "constants.h"
#include "dynarrays.h"

typedef struct Ast
{
    uint8_t type;
    unsigned fin;
    struct Ast* suiv;
} Ast;

bool is_sousop(char car); // Exported function for syntax highlighting
char* get_infinity(void);
char* get_nan(void);
char* get_pi(void);
char* get_none(void);
int get_operator_index(Token operator_string);
int get_type_operande(Token operator_string);
int get_type_operande_index(int index);
char* get_True(void);
bool operatorIs(int index, int mask);
char* get_True(void);
int get_blockword1Line_index(Token word);
int get_lkeywords_index(Token word);
bool is_binary(int);
bool is_unary(int);
char* get_operator_string(int operator_index);

bool isFull(char* string);
char* nomBlockLine(char* blockline); //a liberer apres
void finTokensSimples(char* string, bool* isPotentiallyNumber, bool* isPotentiallyString, bool* isPotentiallyWord, bool* isPotentiallyOp, bool* isPotentiallyLongComm, bool* isPotentiallyString2, bool* isPotentiallyComm, bool* isPotentiallyHexBin, char* char2, char* char1, bool* nouvTok, toklist* tokenAdd, intlist* typeTok, intlist* lines, int* debTok, int i, int* stepNumber, int* stepHexBin, int* nombrePoints);
void debutTokensSimples(int i, int* debTok, char* char1, bool* isPotentiallyString, bool* isPotentiallyNumber, bool* isPotentiallyWord, bool* isPotentiallyOp, bool* isPotentiallyString2, bool* isPotentiallyComm, bool* isPotentiallyLongComm, bool* isPotentiallyHexBin);
void annulerOuPoursuivre(bool* isPotentiallyFonc, int* foncStep, bool* isPotentiallyInst, int* instStep, bool* isPotentiallyListIndex, int* listIndexStep, toklist* tokenAdd, intlist* typeTok, int k);
void debutTokensComposes(size_t k, intlist* typeTok, toklist* tokenAdd, size_t* debTok2, size_t* lastDebTok, bool* nouvTokComp, bool* isPotentiallyList, bool* isPotentiallyPar, bool* isPotentiallyFonc, bool* isPotentiallyListIndex, bool* isPotentiallyBlock, bool* isPotentiallyInst, int* listIndexStep, int *foncStep, int *instStep, int *foncParDeb, int *indexParDeb, int* nbPar, int* nbCro);
void finsTypesComposes(size_t *debTok2, size_t* lastDebTok, size_t k, bool *nouvTokComp, bool* isPotentiallyPar, bool *isPotentiallyFonc, int *foncStep, toklist* tokenAdd, intlist typeTok, int *nbPar, bool *isPotentiallyInst, int *instStep, int *listIndexStep, bool *isPotentiallyList, bool *isPotentiallyListIndex, int *nbCro, bool *isPotentiallyBlock, int *nbAcc, int *foncParDeb, int *indexParDeb, Ast** ast, size_t offset, intlist* lines);
void verificationGrammaire(toklist* tokens, Ast** ast, bool* isPotentiallyExpr, int* debExpr);
void compConditionBlock(int cas, bool* isPotentiallyConBlock, int *conBlockStep, toklist* tokens, intlist* types, int *i, char* typeact, int* debConBlock);
void statements(intlist* types, toklist* tokens, Ast** ast, intlist* lines, size_t offset);
void cut(toklist* tokens, intlist* types, char* str, bool traiterStatements, intlist* lines, bool free_original_string);
void parse(toklist* tokenAdd, intlist typeTok, Ast** ast, intlist* lines, size_t offset);
void ast_aff(Ast** ast, int len);
bool toklist_ast_inList(toklist* list, Ast** ast, char* elt, int type, size_t offset);
bool ast_typeInAst(Ast** ast, size_t len, int type, size_t offset);
size_t ast_typeCountAst(Ast** ast, size_t len, int type, size_t offset);
int ast_minOp(Ast** ast, toklist* tokens, size_t offset);
size_t ast_length(Ast** ast, size_t token_length, size_t offset);
size_t ast_index(Ast** ast, size_t index, size_t offset);
size_t ast_prec(Ast** ast, size_t index, size_t offset);

#endif